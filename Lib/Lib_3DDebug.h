#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Lib_Vector.h"

class Graphics;
class Descriptor;



using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

enum GeometryDebugType
{
	Geometry_DEBUG_BOX,
	Geometry_DEBUG_CIRCLE,
	Geometry_DEBUG_LINE,
	Geometry_DEBUG_CYLINDER,
	Geometry_DEBUG_NAV,
};


class GeometryDebug
{
public:
	GeometryDebug(Graphics* graphics);
	GeometryDebug() {};
	~GeometryDebug();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; };
	D3D12_INDEX_BUFFER_VIEW	 GetIndexBufferView() { return  indexBufferView; };
	//頂点の設定
	virtual void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color) = 0;

	bool renderUse = false;

	GeometryDebugType GetGeometryType() { return geometryDebugType; }

	u_int GetIndicesSize() { return indices.size(); }
	u_int GetVertexSize() { return polygonVertexList.size(); }

protected:
	//頂点バッファ作成
	virtual void CreateVertex(Graphics* graphics) {};
	//インデックスバッファ作成
	virtual void CreateIndex(Graphics* graphics) {};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT4	color;
	};



	ComPtr<ID3D12Resource>			vertexResource;
	ComPtr<ID3D12Resource>			indexResource;
	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			indexBufferView;
	vector<Vertex>                  polygonVertexList;
	vector<UINT>                    indices;
	Vertex* vertMap = nullptr;

	GeometryDebugType geometryDebugType;

	HRESULT hr = S_OK;
};

class GeometryDebugBox :public GeometryDebug
{

public:
	GeometryDebugBox(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};

class GeometryDebugLine :public GeometryDebug
{

public:
	GeometryDebugLine(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};

class GeometryDebugCircle :public GeometryDebug
{
private:
	//一つあたりの頂点の数
	UINT OneCircleNum = 24;

	u_int circle_stacks=15;//縦列
	u_int circle_slice = 15;//横列
	u_int circle_Indices_num;//円を描画する時のインデックス数

public:
	GeometryDebugCircle(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};

class GeometryDebugCylinder :public GeometryDebug
{
private:
	//一つあたりの頂点の数
	UINT OneCircleNum = 120;

public:
	GeometryDebugCylinder(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};



class GeometryDebugNav :public GeometryDebug
{
public:
	GeometryDebugNav(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
		const XMFLOAT4& color);

	//頂点の設定
	void SetVertex(Graphics* graphics, std::vector<XMFLOAT3> posList);

	int GetVertexCopunt() { return vertexCopunt; }

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

	int vertexCopunt = 0;

};

