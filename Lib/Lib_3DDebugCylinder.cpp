#include "Lib_3DDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"

GeometryDebugCylinder::GeometryDebugCylinder(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	geometryDebugType = GeometryDebugType::Geometry_DEBUG_CYLINDER;
}

//頂点バッファ作成
void GeometryDebugCylinder::CreateVertex(Graphics* graphics)
{
	polygonVertexList.resize(OneCircleNum);


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
void GeometryDebugCylinder::CreateIndex(Graphics* graphics)
{

	//インデックスバッファの作成
	indices.resize(120);



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

	for (int i = 0; i < indices.size(); i++)
	{
		index[i] = i;
	}

	indexResource->Unmap(0, nullptr);
}


//頂点の設定
void GeometryDebugCylinder::SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
	const XMFLOAT4& color)
{

	std::vector<Vertex>vertices;
	vertices.resize(OneCircleNum);
	
	for (int i = 0; i < vertices.size(); i+=2)
	{
		float angle = XMConvertToRadians(i*4);
		{
			
			Vertex vertex;
			vertex.color = color;
			vertex.position = { pos.x +sinf(angle) * scale.x,pos.y, pos.z + cosf(angle)* scale.z };

			vertices.at(i) = vertex;
		}
		{
			Vertex vertex;
			vertex.color = color;
			vertex.position = { pos.x + sinf(angle) * scale.x,pos.y+ scale.y, pos.z + cosf(angle) * scale.z };
			vertices.at(i+1) = vertex;
		}
	}

	std::copy(std::begin(vertices), std::end(vertices), vertMap);
}