#pragma once
#include "Lib_Shader.h"
class Model;
class Lib_NavMeshRenderer :public ModelShader
{
public:
	Lib_NavMeshRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~Lib_NavMeshRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);
};