#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;
in vec3 vColor;


out vec4 colour;


struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};



uniform int pointLightCount;
uniform int spotLightCount;


uniform DirectionalLight directionalLight;
uniform PointLight pointLights[2];
uniform sampler2DArray shadowMap;
float farPlane = 500.0f;

uniform sampler2D theTexture;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D directionalShadowMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform samplerCube skybox;

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];
uniform int cascadeCount; 

uniform vec3 albedo = vec3(0.2, 0.2, 0.2);   
uniform float metallic;
uniform float roughness;
uniform float ao = 0.2;

 uniform mat4 view;

uniform vec3 eyePosition;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 


vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 


float ShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 dir = -directionalLight.direction;
	vec3 lightDir = normalize(dir);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}


vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor, vec3 lighPos, bool pointLight)
{
	vec3 viewDir = normalize(eyePosition - FragPos);
	vec3 lightDir = normalize(direction);
	vec3 halfVector = normalize(lightDir + viewDir);
	float roughnessMap = texture(roughnessMap, TexCoord).r * roughness;
	float metallicMap = texture(metallicMap, TexCoord).r * metallic;
	vec3 baseColor = albedo;




	// PBR	***************************************
	vec3 radiance = vec3(0.0f);
	vec3 F0 = vec3(0.04); 

    F0 = mix(F0, baseColor, metallicMap);
	vec3 Lo = vec3(0.0);
	if (pointLight)
	{ 
 		float distance = length(lighPos - FragPos);
		float attenuation = 1.0 / (distance * distance);
		radiance =  light.colour  * attenuation;
	}
	else
		{
		radiance =	light.colour ;  }

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normalize(Normal), halfVector, roughnessMap);   
    float G   = GeometrySmith(normalize(Normal), viewDir, lightDir, roughnessMap);      
    vec3 F    = fresnelSchlick(clamp(dot(halfVector, viewDir), 0.0, 1.0), F0);

	vec3 numerator = NDF * G * F; 

    float denominator = 4.0 * max(dot(normalize(Normal), viewDir), 0.0) * max(dot(normalize(Normal), lightDir), 0.0) + 0.0001; 
    vec3 directSpecular  = numerator / denominator;

	vec3 kS = F;

	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallicMap;

    // scale light by NdotL
    float NdotL = max(dot(normalize(Normal), lightDir), 0.0);   
	vec3 N = normalize(Normal);
	vec3 V = normalize(eyePosition - FragPos);
	vec3 R = reflect(-V, N);


	Lo += (kD * baseColor * light.diffuseIntensity / PI + directSpecular )
		  * radiance * NdotL;

	vec3 FRoughness = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessMap);
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuseIBL = irradiance * baseColor * kD;
	
	const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, normalize(R),  roughnessMap * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughnessMap)).rg;
    vec3 specularIBL  = prefilteredColor * (FRoughness * brdf.x + brdf.y);


    vec3 ambient = (diffuseIBL + specularIBL) * light.ambientIntensity * ao;

	return (vec4(ambient, 1.0f) + (1.0 - shadowFactor) * vec4(Lo, 1));
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = ShadowCalculation(FragPos);
	return CalcLightByDirection(directionalLight.base, -directionalLight.direction, shadowFactor, vec3(0, 0, 0), false);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
	vec3 direction = FragPos - pLight.position;
	float distance = length(direction);
	direction = normalize(direction);
	
	float shadowFactor = 0.0f;
	
	vec4 colour = CalcLightByDirection(pLight.base, -direction, shadowFactor, pLight.position, true);
	
	return (colour);
}


vec4 CalcPointLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{		
		totalColour += CalcPointLight(pointLights[i], i);
	}
	
	return totalColour;
}



void main()
{


	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLights();
	finalColour[3] = 1.0f;


	vec4 texColor = texture(theTexture, TexCoord);

    float gamma = 2.2;
	colour =  finalColour * texColor;

	vec3 hdr = colour.rgb;
	hdr = hdr / (hdr + vec3(1.0));
	colour.rgb = pow(hdr, vec3(1.0/gamma));

}