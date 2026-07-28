#pragma once
#include <GL/glew.h>
#include <vector>
#include <stdint.h>

enum class ShaderType
{
	Main,
	Cubemap,
	Environment,
	Irradiance,
	Prefilter,
	BRDF,
	ShadowMap,
	DebugDepthQuad,
	DebugCascade
};

enum class MeshType
{
	Floor,
	Cube,
	Quad
};

enum class TextureType
{
	Checker,
	SpaceshipMetal,
	CheckerMetal,
	CheckerRoughness
};


enum class ModelType
{
	Spaceship
};

