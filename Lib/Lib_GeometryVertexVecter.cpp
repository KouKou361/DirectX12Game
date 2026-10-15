#include "Lib_GeometryVertexVecter.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"
#include "Lib_Texture.h"
GeometryVertexVector::GeometryVertexVector(Graphics* graphics, Texture* texture, const int createCount)
{
	this->createCount = createCount;
	this->texture = texture;

	//頂点バッファ作成
	CreateVertex(graphics);


}

GeometryVertexVector::~GeometryVertexVector()
{
}

void GeometryVertexVector::Apply()
{
	std::copy(std::begin(polygonVertexList), std::end(polygonVertexList), vertMap);
}

void GeometryVertexVector::CreateVertex(Graphics* graphics)
{
	polygonVertexList.resize(createCount);


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

Descriptor* GeometryVertexVector::GetDescriptor()
{
	return texture->GetDescriptor();
}
