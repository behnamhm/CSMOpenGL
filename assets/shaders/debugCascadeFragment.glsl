#version 410 core

in vec3 FragPosWorld;

out vec4 FragColor;

uniform mat4 view;

uniform int cascadeCount;
uniform float cascadePlaneDistances[4];

void main()
{
    float depth = abs((view * vec4(FragPosWorld, 1.0)).z);

    if(depth < cascadePlaneDistances[0])
        FragColor = vec4(1.0, 0.0, 0.0, 0.5);
    else if(depth < cascadePlaneDistances[1])
        FragColor = vec4(0.0, 1.0, 0.0, 0.5);
    else if(depth < cascadePlaneDistances[2])
        FragColor = vec4(0.0, 0.0, 1.0, 0.5);
    else
        FragColor = vec4(1.0, 1.0, 0.0, 0.5);
}