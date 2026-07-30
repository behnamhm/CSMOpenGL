#include "shadowSystem.h"
#include "scene/Scene.h"


ShadowSystem::ShadowSystem()
{
	FBO = 0;
	shadowMap = 0;
}

bool ShadowSystem::Init(unsigned int width, unsigned int height)
{
	shadowWidth = width; shadowHeight = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer error: %s\n", Status);
		return false;
	}

	return true;
}

void ShadowSystem::Write()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void ShadowSystem::Read(GLenum texUnit)
{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}


std::vector<glm::mat4> ShadowSystem::getLightSpaceMatrices(Scene& scene, std::vector<float>& shadowCascadeLevels,
    float& cameraNearPlane, float& cameraFarPlane)
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(scene, cameraNearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(scene, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(scene, shadowCascadeLevels[i - 1], cameraFarPlane));
        }
    }
    return ret;
}

glm::mat4 ShadowSystem::getLightSpaceMatrix(Scene& scene, const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(90.0f), (float)fb_width / (float)fb_height, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, scene.camera.calculateViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

std::vector<glm::vec4> ShadowSystem::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}


std::vector<glm::vec4> ShadowSystem::getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}



ShadowSystem::~ShadowSystem()
{
	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (shadowMap)
	{
		glDeleteTextures(1, &shadowMap);
	}
}
