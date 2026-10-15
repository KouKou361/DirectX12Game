#pragma once
#include "Lib_Shader.h"
class Model;

class CreateShadowRenderer :public ModelShader
{
public:
	CreateShadowRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~CreateShadowRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);
	
};


class ShadowRenderer :public ModelShader
{
public:
	ShadowRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~ShadowRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);
};