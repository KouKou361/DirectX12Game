#pragma once
#include "Lib_Shader.h"
class Model;
class dtNavMesh;
class dtNavMeshQuery;
class dtMeshTile;
class GeometryDebugNav;
class Lib_ConstantBuffer;

class Lib_NavModelRenderer
{
public:
	Lib_NavModelRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	~Lib_NavModelRenderer();


	virtual void Begin(Graphics* graphics);
	virtual void Render(Graphics* graphics, GeometryDebugNav* GeometryDebug);
	virtual void End(Graphics* graphics);

	void Register(Graphics* graphics, dtNavMesh* navMesh, dtNavMeshQuery* navQuery, DirectX::XMFLOAT4X4 world, XMFLOAT3 targetPos, float targetRadius);
	void AllRender(Graphics* graphics);

private:
	void LoadShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	void NavRender(Graphics* graphics, dtNavMesh* navMesh, dtNavMeshQuery* navQuery, XMFLOAT3 targetPos, float targetRadius);
	void duDebugDrawNavMeshWithClosedList(Graphics* graphics, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags, XMFLOAT3 targetPos, float targetRadius);
	void drawMeshTile(Graphics* graphics, const dtNavMesh& mesh, const dtNavMeshQuery* query,const dtMeshTile* tile, unsigned char flags, XMFLOAT3 targetPos, float targetRadius);
	void SetVertex(Graphics* graphics, const float* pos);

	ComPtr<ID3D12PipelineState>		pipelineState = nullptr;
	ComPtr<ID3D12RootSignature>		rootSignature = nullptr;

	ComPtr<ID3DBlob> vsShader = nullptr;
	ComPtr<ID3DBlob> psShader = nullptr;

	Lib_ConstantBuffer* navWorldConstantBuffer = nullptr;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParm[2] = {};

	HRESULT hr = S_OK;

	vector<GeometryDebugNav*> navModelList;
	GeometryDebugNav* SetUnUsedRegisterNavList();

	struct  NavWorld
	{
		DirectX::XMFLOAT4X4 world;
	};

	NavWorld navWorld;



	std::vector<XMFLOAT3> polygonVertexList;

};
