#pragma once
#include "Lib_Shader.h"
class Model;
class Lib_ConstantBuffer;
class CameraLib;


struct CreatePBRRendererBuffer
{
public:
	float csb_Data;
	float dummy1;
	float dummy2;
	float dummy3;
};

class PBRRenderer :public ModelShader
{
public:
	PBRRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~PBRRenderer();

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);

private:
};