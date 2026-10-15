#pragma once
#include "Lib_Shader.h"
class Model;
class NoLightRenderer :public ModelShader
{
public:
	NoLightRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~NoLightRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);
};