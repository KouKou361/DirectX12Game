#pragma once

#include "Lib_Shader.h"
#include "Lib_Vector.h"
#include "Lib_3DDebug.h"

struct GeometryDebugRender
{
	VECTOR3 pos;
	VECTOR3 pos2;
	VECTOR4 color;
	float   angle;
	float   radius;
	GeometryDebugType type;

};


class GeometryDebugRenderer :public Shader
{
private:
	//線用のパイプライン
	ComPtr<ID3D12PipelineState>		linePipelineState = nullptr;

	//四角デバック用
	vector<GeometryDebugBox*>    boxList;
	//線デバック用
	vector<GeometryDebugLine*>   lineList;
	//円デバック用
	vector<GeometryDebugCircle*> circleList;
	//円柱デバッグ用
	vector<GeometryDebugCylinder*> cylinderList;

	GeometryDebugCircle* SetUnUsedRegisterCircleList();
	GeometryDebugLine* SetUnUsedRegisterLineList();
	GeometryDebugBox* SetUnUsedRegisterBoxList();
	GeometryDebugCylinder* SetUnUsedRegisterCylinderList();
public:

	GeometryDebugRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~GeometryDebugRenderer();

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, GeometryDebug* Geometry);
	void End(Graphics* graphics);

	void AllRender(Graphics* graphics);


	void Register(Graphics* graphics, const GeometryDebugRender& GeometryDebugRender);
};