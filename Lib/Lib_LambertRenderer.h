#pragma once
#include "Lib_Shader.h"
class Model;
class LambertRenderer:public ModelShader
{
public:
	LambertRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~LambertRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);
};