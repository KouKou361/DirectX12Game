#include "Lib_ConstantBuffer.h"
#include "Lib_Graphics.h"
#include "Lib_Descriptor.h"

Lib_ConstantBuffer::Lib_ConstantBuffer(Graphics* graphics, size_t size, void* srcData)
{
	m_size = size;

	ID3D12Device* device = graphics->GetDevice();
	HRESULT hr = S_OK;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES d3d_heap_props{};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask = 1;
	d3d_heap_props.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC d3d_resource_desc{};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d_resource_desc.Alignment = 0;
	d3d_resource_desc.Width = (size + 255) & ~255;	// 256バイトアライメントにする
	d3d_resource_desc.Height = 1;
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;
	d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	d3d_resource_desc.SampleDesc.Count = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;


	// 定数バッファの生成
	hr = device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constantBuffer.GetAddressOf())
	);
	if (hr != S_OK)assert(!L"定数バッファ生成のエラー");
	constantBuffer->SetName(L"constantBuffer");

	// ディスクリプタ取得
	constantBufferDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();

	// 定数バッファビューの生成
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
	d3d_cbv_desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
	d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
	device->CreateConstantBufferView(
		&d3d_cbv_desc,
		constantBufferDescriptor->GetCpuHandle()
	);

	hr = constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_constBufferCPU));


	if (srcData != nullptr) {
		memcpy(m_constBufferCPU, srcData, m_size);
	}

	if (hr != S_OK)assert(!L"定数バッファMAPのエラー");


	lockFlg = false;
}

Lib_ConstantBuffer::~Lib_ConstantBuffer()
{
	return;

//
//	Lock();
//	if (constantBuffer != nullptr)
//	{
//		constantBuffer->Release();
//	}
//
}

//コンスタントバッファをロック
void Lib_ConstantBuffer::Lock()
{
	if (lockFlg == true)return;
	if (constantBuffer == nullptr)return;

	constantBuffer->Unmap(0, nullptr);
	lockFlg = true;
}

void Lib_ConstantBuffer::CopyToVRAM(Graphics* graphics,void* data)
{
	ID3D12Device* device = graphics->GetDevice();
	if (data != nullptr) {
		memcpy(m_constBufferCPU, data, m_size);
	}
	
}
