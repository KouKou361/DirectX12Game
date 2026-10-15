#include "Lib_SpriteDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"

SpriteDebugLine::SpriteDebugLine(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	spriteDebugType = SpriteDebugType::SPRITE_DEBUG_LINE;
}

void SpriteDebugLine::SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale, const XMFLOAT4& color, const float& angle)
{

	for (int i = 0; i < polygonVertexList.size(); i += 2)
	{
		//左上
		polygonVertexList.at(i + 0).position = { pos.x,pos.y,0 };
		polygonVertexList.at(i + 0).color = color;

		//右上	
		polygonVertexList.at(i + 1).position = { scale.x,scale.y,0 };
		polygonVertexList.at(i + 1).color = color;
	}
	//プロジェクション行列
	XMMATRIX matMatrix = XMMatrixIdentity();
	{
		const float WindowWidth = static_cast<float>(graphics->GetWindowWidth());
		const float WindowHeight = static_cast<float>(graphics->GetWindowHeight());
		matMatrix.r[0].m128_f32[0] = 2.0f / WindowWidth;
		matMatrix.r[1].m128_f32[1] = -2.0f / WindowHeight;

		matMatrix.r[3].m128_f32[0] = -1.0f;
		matMatrix.r[3].m128_f32[1] = 1.0f;
	}


	//行列作成
	matMatrix = matMatrix;

	for (int i = 0; i < polygonVertexList.size(); i++)
	{
		XMVECTOR Pos = XMLoadFloat3(&polygonVertexList.at(i).position);
		Pos = XMVector3Transform(Pos, matMatrix);
		XMStoreFloat3(&polygonVertexList.at(i).position, Pos);
	}

	std::copy(std::begin(polygonVertexList), std::end(polygonVertexList), vertMap);
}

void SpriteDebugLine::CreateVertex(Graphics* graphics)
{
	polygonVertexList.resize(2);


	ID3D12Device* device = graphics->GetDevice();

	CD3DX12_RESOURCE_DESC rscDesc = {};
	auto rscDescBuffer = rscDesc.Buffer(sizeof(polygonVertexList[0]) * polygonVertexList.size());
	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	hr = device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&rscDescBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexResource.GetAddressOf())
	);

	if (hr != S_OK)assert(!L"頂点バッファのエラー");


	//頂点バッファビュー
	memset(&vertexBufferView, 0, sizeof(vertexBufferView));
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(polygonVertexList[0]) * polygonVertexList.size();
	vertexBufferView.StrideInBytes = sizeof(polygonVertexList[0]);


	hr = vertexResource->Map(0, nullptr, (void**)&vertMap);
	if (hr != S_OK)assert(!L"頂点コピーのエラー");
}

void SpriteDebugLine::CreateIndex(Graphics* graphics)
{
	indices.resize(2);



	ID3D12Device* device = graphics->GetDevice();

	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC rsrDesc;
	auto desc = rsrDesc.Buffer(sizeof(UINT) * indices.size());

	hr = device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexResource.GetAddressOf())
	);

	if (hr != S_OK)assert(!L"インデックスバッファのエラー");


	//インデックスビューの作成
	memset(&indexBufferView, 0, sizeof(indexBufferView));
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = static_cast<UINT>(desc.Width);

	// インデックスデータ設定
	UINT* index;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&index));

	for (int i = 0; i < indices.size(); i += 2)
	{
		index[i + 0] = 0 + i;
		index[i + 1] = 1 + i;
	}

	indexResource->Unmap(0, nullptr);
}
