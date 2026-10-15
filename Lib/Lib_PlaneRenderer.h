#pragma once
#include "Lib_Shader.h"
class PlaneRenderer :public SpriteShader
{
public:

	PlaneRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS, const int createNum);


	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, int index);

	void SetVertex(Graphics* graphics, Texture* texture, XMFLOAT4X4 mat, XMFLOAT4 cut, XMFLOAT4 color);
};
