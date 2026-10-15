#pragma once
#include "Lib_Shader.h"
class SpriteRenderer:public SpriteShader
{
public:

	SpriteRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS,const int createNum);


	void Render(Graphics* graphics, int index);
};
