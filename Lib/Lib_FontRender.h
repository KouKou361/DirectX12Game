#pragma once
#include "Lib_Shader.h"
class FontRenderer :public SpriteShader
{
public:

	FontRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS, const int createNum);
	void Render(Graphics* graphics, int index);
};
