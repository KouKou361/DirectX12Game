#pragma once
#include "Lib_Shader.h"
class Model;
class Lib_ConstantBuffer;
class CameraLib;


struct CreateShadowMapConstantBuffer
{
public:
	float csb_Data;
	float dummy1;
	float dummy2;
	float dummy3;
};

class CreateCascadedShadowRenderer :public ModelShader
{
public:
	CreateCascadedShadowRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~CreateCascadedShadowRenderer();

	void AllRender(Graphics* graphics);

	void Begin(Graphics* graphics);
	void Begin(Graphics* graphics,const int shadowMapIndex);
	void Render(Graphics* graphics, Model* model);

private:
	//カスケードシャドウマップの数
	static const int NUM_SHADOW_MAP = 3;
	Lib_ConstantBuffer* constantBuffer[NUM_SHADOW_MAP];
	CreateShadowMapConstantBuffer csb_Data[NUM_SHADOW_MAP];
	
};


class CascadedShadowRenderer :public ModelShader
{
public:
	CascadedShadowRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~CascadedShadowRenderer();

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, Model* model);

private:
};