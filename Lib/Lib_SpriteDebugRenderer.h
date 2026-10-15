#pragma once
#include "Lib_Shader.h"
#include "Lib_Vector.h"
#include "Lib_SpriteDebug.h"
class SpriteDebug;
struct SpriteDebugRender
{
	VECTOR2 pos;
	VECTOR2 pos2;
	VECTOR4 color;
	float   angle;
	float   radius;
	SpriteDebugType type;

};

class SpriteDebugRenderer:public Shader
{
private:
	//線用のパイプライン
	ComPtr<ID3D12PipelineState>		linePipelineState = nullptr;

	//四角デバック用
	vector<SpriteDebugBox*>    boxList;
	//線デバック用
	vector<SpriteDebugLine*>   lineList;
	//円デバック用
	vector<SpriteDebugCircle*> circleList;

	SpriteDebugCircle* SetUnUsedRegisterCircleList();
	SpriteDebugLine* SetUnUsedRegisterLineList();
	SpriteDebugBox* SetUnUsedRegisterBoxList();
public:

	SpriteDebugRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~SpriteDebugRenderer();

	void Begin(Graphics* graphics);
	void Render(Graphics* graphics, SpriteDebug* sprite);
	void End(Graphics* graphics);

	void AllRender(Graphics* graphics);


	void Register(Graphics* graphics, const SpriteDebugRender &spriteDebugRender);

	
};
