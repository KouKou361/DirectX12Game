#pragma once
#include "Lib_Shader.h"
class Effect3DGeometryRenderer:public GeometryPointShader
{
public:
	Effect3DGeometryRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS);
	~Effect3DGeometryRenderer() {};

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, GeometryVertexVector* geometryVertex);
	void End(Graphics* graphics);
};