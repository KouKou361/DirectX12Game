#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Lib_Vector.h"

class Graphics;
class Descriptor;
class Texture;

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

class GeometryVertex
{
public:
	XMFLOAT3 Position;	//原点位置
	XMFLOAT3 Size;		//拡大
	XMFLOAT3 Rotation;	//回転

	XMFLOAT3 SpeedVec;	//移動量
	XMFLOAT3 AcceleVec;	//加速度
	XMFLOAT3 RotationVec;//回転量
	XMFLOAT3 Scale;		//拡大量
	XMFLOAT4 AddColor;	//予備
	XMFLOAT3 Dummy2;	//予備


	XMFLOAT4 Tex;		//切り取り位置
	XMFLOAT4 Color;		//色
	float   time = 0;

};


//　ポイントスプライト配列。
class GeometryVertexVector
{
public:
	GeometryVertexVector(Graphics* graphics, Texture* texture, const int createCount);
	GeometryVertexVector() {};
	~GeometryVertexVector();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; };
	vector<GeometryVertex>& GetVertices() { return polygonVertexList; }
	void Apply();
	void Update(float time);
	Descriptor* GetDescriptor();

	bool renderUse = false;

protected:
	//頂点バッファ作成
	virtual void CreateVertex(Graphics* graphics);



	ComPtr<ID3D12Resource>			vertexResource;
	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
	vector<GeometryVertex>                  polygonVertexList;
	GeometryVertex* vertMap = nullptr;
	int								createCount;
	Texture* texture;


	HRESULT hr = S_OK;
};