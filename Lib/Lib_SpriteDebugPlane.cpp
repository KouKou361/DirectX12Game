#include "Lib_SpriteDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"
SpriteDebugBox::SpriteDebugBox(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	spriteDebugType = SpriteDebugType::SPRITE_DEBUG_BOX;
}
void SpriteDebugBox::SetVertex(Graphics* graphics, const XMFLOAT2& pos, const XMFLOAT2& scale,  const XMFLOAT4& color, const float& angle)
{
	const XMFLOAT2 halfPos = { (scale.x) / 2,(scale.y) / 2 };

	for (int i = 0; i < polygonVertexList.size(); i += 4)
	{
		//左上
		polygonVertexList.at(i+0).position = { -halfPos.x,halfPos.y,0 };
		polygonVertexList.at(i+0).color = color;
				
		//右上	
		polygonVertexList.at(i+1).position = { halfPos.x,halfPos.y,0 };
		polygonVertexList.at(i+1).color = color;
				
		//左下	
		polygonVertexList.at(i+2).position = { -halfPos.x,-halfPos.y,0 };
		polygonVertexList.at(i+2).color = color;
				
		//右下	
		polygonVertexList.at(i+3).position = { halfPos.x,-halfPos.y,0 };
		polygonVertexList.at(i+3).color = color;
	}


	//回転行列
	XMMATRIX RotationMat = XMMatrixIdentity();
	{
		XMFLOAT4X4 mat;
		mat._11 = cosf(angle); mat._12 = sinf(angle); mat._13 = 0; mat._14 = 0;
		mat._21 = -sin(angle); mat._22 = cosf(angle); mat._23 = 0; mat._24 = 0;
		mat._31 = 0;           mat._32 = 0;           mat._33 = 1; mat._34 = 0;
		mat._41 = 0;           mat._42 = 0;           mat._43 = 0; mat._44 = 1;
		RotationMat = XMLoadFloat4x4(&mat);
	}

	//移動行列
	XMMATRIX TransformMat = XMMatrixIdentity();
	{
		XMFLOAT4X4 mat;
		mat._11 = 1;     mat._12 = 0;     mat._13 = 0; mat._14 = 0;
		mat._21 = 0;     mat._22 = 1;     mat._23 = 0; mat._24 = 0;
		mat._31 = 0;     mat._32 = 0;     mat._33 = 1; mat._34 = 0;
		mat._41 = pos.x + halfPos.x; mat._42 = pos.y + halfPos.y; mat._43 = 0; mat._44 = 1;
		TransformMat = XMLoadFloat4x4(&mat);
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
	matMatrix = RotationMat * TransformMat * matMatrix;

	for (int i = 0; i < polygonVertexList.size(); i++)
	{
		XMVECTOR Pos = XMLoadFloat3(&polygonVertexList.at(i).position);
		Pos = XMVector3Transform(Pos, matMatrix);
		XMStoreFloat3(&polygonVertexList.at(i).position, Pos);
	}

	std::copy(std::begin(polygonVertexList), std::end(polygonVertexList), vertMap);
}

void SpriteDebugBox::CreateVertex(Graphics* graphics)
{

	polygonVertexList.resize(4);


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

void SpriteDebugBox::CreateIndex(Graphics* graphics)
{
	indices.resize(6);



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

	for (int i = 0; i < indices.size(); i += 6)
	{
		index[i + 0] = 0 + i;
		index[i + 1] = 1 + i;
		index[i + 2] = 2 + i;
		index[i + 3] = 2 + i;
		index[i + 4] = 1 + i;
		index[i + 5] = 3 + i;
	}

	indexResource->Unmap(0, nullptr);
}
