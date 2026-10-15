#include "Lib_3DDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"


GeometryDebugCircle::GeometryDebugCircle(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	geometryDebugType = GeometryDebugType::Geometry_DEBUG_CIRCLE;
}
//頂点バッファ作成
void GeometryDebugCircle::CreateVertex(Graphics* graphics)
{
	// インデックス数
	circle_Indices_num = circle_stacks * circle_slice * 2 * 3;

	polygonVertexList.resize(circle_Indices_num);


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
void GeometryDebugCircle::CreateIndex(Graphics* graphics)
{



	// インデックス設定
	indices.resize(circle_Indices_num);
	//	u_int* indices = new u_int[Circle_Indices_num];
	for (int y = 0; y < circle_stacks; y++) {
		for (int x = 0; x < circle_slice; x++) {
			int face = (y * circle_slice + x);
			int vertices_index = y * (circle_slice + 1) + x;
			indices[face * 6] = vertices_index + 1;
			indices[face * 6 + 1] = vertices_index;
			indices[face * 6 + 2] = vertices_index + (circle_slice + 1);

			indices[face * 6 + 3] = vertices_index + 1;
			indices[face * 6 + 4] = vertices_index + (circle_slice + 1);
			indices[face * 6 + 5] = vertices_index + (circle_slice + 1) + 1;
		}
	}

	indices.resize(circle_Indices_num);



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
		index[i] = indices[i];
	}

	indexResource->Unmap(0, nullptr);
}


//頂点の設定
void GeometryDebugCircle::SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
	const XMFLOAT4& color)
{
	//分かりやすくするために別の変数名にする
	float radius = scale.x;

	radius *= 2;
	//頂点の数

	u_int Circle_Vertex_num = (circle_slice + 1) * (circle_stacks + 1);
	//if (Circle_Vertex_num >= polygonVertexList.size())	assert(!" Primitive3D::circleの slice,stacksが多きすぎます");
	// 頂点定義
	for (u_int y = 0; y < circle_stacks + 1; y++) {
		for (u_int x = 0; x < circle_slice + 1; x++) {
			int index = y * (circle_slice + 1) + x;
			float h = 0.5f * cosf(y * XM_PI / circle_stacks);
			float w = 0.5f * sinf(y * XM_PI / circle_stacks);
			float rad_slices = x * XM_PI * 2.0f / circle_slice;

			polygonVertexList[index].position.x = w * sinf(rad_slices) * radius + pos.x;
			polygonVertexList[index].position.y = h * radius + pos.y;
			polygonVertexList[index].position.z = w * cosf(rad_slices) * radius + pos.z;

			polygonVertexList[index].normal.x = polygonVertexList[index].position.x * 2.0f;
			polygonVertexList[index].normal.y = polygonVertexList[index].position.y * 2.0f;
			polygonVertexList[index].normal.z = polygonVertexList[index].position.z * 2.0f;

			polygonVertexList[index].color = color;
		}
	}

	std::copy(std::begin(polygonVertexList), std::end(polygonVertexList), vertMap);
}
