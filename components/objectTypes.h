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
	ShadowMap
};

enum class MeshType
{
	Floor,
	Cube
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

