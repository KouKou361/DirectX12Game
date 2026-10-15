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

enum SpriteDebugType
{
	SPRITE_DEBUG_BOX,
	SPRITE_DEBUG_CIRCLE,
    SPRITE_DEBUG_LINE,
};

struct Vertex
{
	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	normal = { 0, 0, 0 };
	DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };
};


class SpriteDebug
{
public:
	SpriteDebug(Graphics* graphics);
	SpriteDebug() {};
	~SpriteDebug();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; };
	D3D12_INDEX_BUFFER_VIEW	 GetIndexBufferView() { return  indexBufferView; };
	//頂点の設定
	virtual void SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,
		 const XMFLOAT4& color, const float& angle) = 0;

	bool renderUse=false;

	SpriteDebugType GetSpriteType() { return spriteDebugType; }

protected:
	//頂点バッファ作成
	virtual void CreateVertex(Graphics* graphics){};
	//インデックスバッファ作成
	virtual void CreateIndex(Graphics* graphics){};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
	};



	ComPtr<ID3D12Resource>			vertexResource;
	ComPtr<ID3D12Resource>			indexResource;
	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			indexBufferView;
	vector<Vertex>                  polygonVertexList;
	vector<UINT>                    indices;
	Vertex* vertMap = nullptr;

	SpriteDebugType spriteDebugType;

	HRESULT hr = S_OK;
};

class SpriteDebugBox :public SpriteDebug
{

public:
	SpriteDebugBox(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,
		 const XMFLOAT4& color, const float& angle);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};

class SpriteDebugLine :public SpriteDebug
{

public:
	SpriteDebugLine(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,
		 const XMFLOAT4& color, const float& angle);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};

class SpriteDebugCircle :public SpriteDebug
{
private:
	//一つあたりの頂点の数
	UINT OneCircleNum = 24;

public:
	SpriteDebugCircle(Graphics* graphics);

	//頂点の設定
	void SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,
		 const XMFLOAT4& color, const float& angle);

protected:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

};