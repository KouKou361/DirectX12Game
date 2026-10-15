//pragma once
//include <wrl.h>
//include <d3d12.h>
//include <DirectXMath.h>
//include <vector>
//include <string>
//include "Lib_Vector.h"
//
//lass Graphics;
//lass Descriptor;
//
//sing namespace Microsoft::WRL;
//sing namespace std;
//sing namespace DirectX;
//
//lass Sprite
//
//ublic:
//	Sprite(Graphics* graphics,const string fileName=nullptr);
//	~Sprite();
//
//	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; };
//	D3D12_INDEX_BUFFER_VIEW	 GetIndexBufferView() { return  indexBufferView; };
//	Descriptor* GetDescriptor() { return textureDescriptor; }
//	//頂点の設定
//	void SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,
//		const XMFLOAT4& cut, const XMFLOAT4& color, const float& angle);
//
//rivate:
//	//頂点バッファ作成
//	void CreateVertex(Graphics* graphics);
//	//インデックスバッファ作成
//	void CreateIndex(Graphics* graphics);
//	//テクスチャ作成
//	void CreateTextureResource(Graphics* graphics, const string fileName);
//
//	struct Vertex
//	{
//		DirectX::XMFLOAT3	position;
//		DirectX::XMFLOAT4	color;
//		DirectX::XMFLOAT2	texcoord;
//	};
//
//
//
//	ComPtr<ID3D12Resource>			vertexResource;
//	ComPtr<ID3D12Resource>			indexResource;
//	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
//	D3D12_INDEX_BUFFER_VIEW			indexBufferView;
//	vector<Vertex>                  polygonVertexList;
//	vector<UINT>                    indices;
//	Vertex* vertMap = nullptr;
//
//	ID3D12Resource*			textureResource=nullptr;
//	Descriptor*             textureDescriptor = nullptr;
//	FLOAT textureWidth;
//	FLOAT textureHeight;
//	HRESULT hr = S_OK;
//;