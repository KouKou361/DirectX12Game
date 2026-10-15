

#include "Lib_Shader.h"
#include <d3dx12.h>

#include "Lib_Graphics.h"
#include "Lib_Texture.h"


SpriteShader::SpriteShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS, const int createNum) :createNum(createNum), Shader(shaderVS, shaderPS)
{
	CreateVertex(graphics);
	CreateIndex(graphics);
}
SpriteShader::~SpriteShader()
{
	vertexResource->Unmap(0, nullptr);
}
//頂点バッファ作成
void SpriteShader::CreateVertex(Graphics* graphics)
{

	ID3D12Device* device = graphics->GetDevice();

	CD3DX12_RESOURCE_DESC rscDesc = {};
	auto rscDescBuffer = rscDesc.Buffer(sizeof(Vertex) * 4 * createNum);
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
	vertexBufferView.SizeInBytes = static_cast<UINT>(rscDescBuffer.Width);
	vertexBufferView.StrideInBytes = sizeof(Vertex);


	hr = vertexResource->Map(0, nullptr, (void**)&vertMap);
	if (hr != S_OK)assert(!L"頂点コピーのエラー");
}
//インデックスバッファ作成
void SpriteShader::CreateIndex(Graphics* graphics)
{
	ID3D12Device* device = graphics->GetDevice();

	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC rsrDesc;
	auto desc = rsrDesc.Buffer(sizeof(UINT) * 6 * createNum);

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

	// 四角形 を 三角形 ２つに展開
	// 0---1      0---1  4
	// |   |  →  |／  ／|
	// 2---3      2  3---5
	for (UINT i = 0; i < createNum * 4; i += 4)
	{
		index[0] = i + 0;
		index[1] = i + 1;
		index[2] = i + 2;
		index[3] = i + 2;
		index[4] = i + 1;
		index[5] = i + 3;
		index += 6;
	}
	indexResource->Unmap(0, nullptr);

}

//頂点の設定
void SpriteShader::SetVertex(Graphics* graphics, Texture* texture, const XMFLOAT2& pos, const XMFLOAT2& scale, const XMFLOAT4& cut, const XMFLOAT4& color, const float& angle)
{
	if (renderNum >= createNum)assert(!L"最大数を達しました");

	const XMFLOAT2 halfPos = { (scale.x) / 2,(scale.y) / 2 };


	XMFLOAT4 Cut;
	Cut.x = cut.x / texture->GetSize().x;
	Cut.y = cut.y / texture->GetSize().y;
	Cut.z = cut.z / texture->GetSize().x;
	Cut.w = cut.w / texture->GetSize().y;

	Vertex polygonVertexList[4] = {};
	//左上
	polygonVertexList[0].position = { -halfPos.x,halfPos.y,0 };
	polygonVertexList[0].texcoord = { Cut.x,Cut.w };
	polygonVertexList[0].color = color;

	//右上  
	polygonVertexList[1].position = { halfPos.x,halfPos.y,0 };
	polygonVertexList[1].texcoord = { Cut.z,Cut.w };
	polygonVertexList[1].color = color;

	//左下  
	polygonVertexList[2].position = { -halfPos.x,-halfPos.y,0 };
	polygonVertexList[2].texcoord = { Cut.x,Cut.y };
	polygonVertexList[2].color = color;

	//右下  
	polygonVertexList[3].position = { halfPos.x,-halfPos.y,0 };
	polygonVertexList[3].texcoord = { Cut.z,Cut.y };
	polygonVertexList[3].color = color;

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
	Vertex* v = vertMap + (renderNum * 4);


	for (int i = 0; i < ARRAYSIZE(polygonVertexList); i++)
	{
		XMVECTOR Pos = XMLoadFloat3(&polygonVertexList[i].position);
		Pos = XMVector3Transform(Pos, matMatrix);
		XMStoreFloat3(&v[i].position, Pos);
		v[i].color = polygonVertexList[i].color;
		v[i].texcoord = polygonVertexList[i].texcoord;
	}

	renderNum++;

	renderTextureList.push_back(texture);
}


void SpriteShader::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
}


void SpriteShader::End(Graphics* graphics)
{
	renderNum = 0;
	renderTextureList.clear();
}

