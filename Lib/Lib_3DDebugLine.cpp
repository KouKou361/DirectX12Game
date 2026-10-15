#include "Lib_3DDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"

GeometryDebugLine::GeometryDebugLine(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	geometryDebugType = GeometryDebugType::Geometry_DEBUG_LINE;
}

//頂点バッファ作成
void GeometryDebugLine::CreateVertex(Graphics* graphics)
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
//インデックスバッファ作成
void GeometryDebugLine::CreateIndex(Graphics* graphics)
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

//頂点の設定
void GeometryDebugLine::SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
	const XMFLOAT4& color)
{
	//左上
	polygonVertexList.at(0).position = { pos.x,pos.y,pos.z };
	polygonVertexList.at(0).color = color;

	//右上	
	polygonVertexList.at(1).position = { scale.x,scale.y,scale.z };
	polygonVertexList.at(1).color = color;

	std::copy(std::begin(polygonVertexList), std::end(polygonVertexList), vertMap);
}
