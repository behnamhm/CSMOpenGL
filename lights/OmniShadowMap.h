#pragma once
#include "ShadowSystem.h"
class OmniShadowMap :
	public ShadowSystem
{
public:
	OmniShadowMap();

	bool Init(unsigned int width, unsigned int height);

	void Write();

	void Read(GLenum TextureUnit);

	~OmniShadowMap();
};

