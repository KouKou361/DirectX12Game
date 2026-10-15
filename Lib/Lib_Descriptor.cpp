#include "Lib_Descriptor.h"
#include "Lib_Graphics.h"
#include <assert.h>

void Descriptor::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_descriptor_handle, D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_descriptor_handle, UINT index)
{
	this->cpuHandle = d3d_cpu_descriptor_handle;
	this->gpuHandle = d3d_gpu_descriptor_handle;
	this->index = index;
	use = 0;
}

DescriptorHeap::DescriptorHeap(Graphics* graphics, UINT maxNum,D3D12_DESCRIPTOR_HEAP_TYPE descriptorType)
{
	ID3D12Device* device = graphics->GetDevice();
	descriptors.clear();
	descriptors.resize(maxNum);
	numDescriptor = 0;
	maxNumDescriptor = maxNum;

	// ディスクリプタヒープタイプ毎の設定
	LPCWSTR name = L"";
	D3D12_DESCRIPTOR_HEAP_FLAGS d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	switch (descriptorType)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		name = L"CBV_SRV_UAV_DescriptorHeap";
		d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		name = L"Sampler_DescriptorHeap";
		d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		name = L"RTV_DescriptorHeap";
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		name = L"DSV_DescriptorHeap";
		break;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	memset(&desc, 0, sizeof(desc));
	desc.Flags = d3d_descriptor_heap_flags;
	desc.NodeMask = 0;
	desc.NumDescriptors = maxNum;
	desc.Type = descriptorType;

	HRESULT hr = S_OK;
	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	descriptorHeap->SetName(name);
	if (hr != S_OK)assert(!L"テクスチャディスクリプタヒープのエラー");


	// ディスクリプタ初期化
	UINT d3d_descriptor_size = device->GetDescriptorHandleIncrementSize(descriptorType);
	D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	for (UINT index = 0; index < maxNum; ++index)
	{

		Descriptor* descriptor = new Descriptor();
		descriptor->Initialize(d3d_cpu_handle, d3d_gpu_handle, index);
		descriptors.at(index) = descriptor;

		d3d_cpu_handle.ptr += d3d_descriptor_size;
		d3d_gpu_handle.ptr += d3d_descriptor_size;
	}
}

DescriptorHeap::~DescriptorHeap()
{
	for (int i = 0; i < descriptors.size(); i++)
	{
		Descriptor* descriptor = descriptors.at(i);
		delete descriptor;
		descriptor = nullptr;
	}

	descriptors.clear();
	numDescriptor = 0;
	maxNumDescriptor = 0;
}
Descriptor* DescriptorHeap::GetDescriptor(UINT index)
{
	if (index >= descriptors.size())	assert(!"Descriptorがありません！");
	return descriptors.at(index);
}
Descriptor* DescriptorHeap::GetUnusedDescriptor()
{
	for (int i = 0; i < descriptors.size(); i++)
	{
		Descriptor* descriptor = descriptors.at(i);
		//使われていないものを発見
		if (descriptor->GetUse() == 0)
		{
			descriptor->SetUse(1);//使われている
			return descriptor;
		}
	}

	assert(!"もう未使用のDescriptorがありません！");
	return nullptr;
}
