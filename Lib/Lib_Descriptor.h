#pragma once
#include <d3d12.h>
#include <vector>
#include <wrl.h>

using namespace std;
using namespace Microsoft::WRL;
class Graphics;

class Descriptor
{
private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	UINT                        index = -1;
	UINT                        use = -1;//0:使われていない,1:使われている 

public:
	Descriptor() {};
	~Descriptor() {};

	// 初期化
	void Initialize(D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_descriptor_handle, D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_descriptor_handle, UINT index);
	// GPUハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return cpuHandle; }
	// GPUハンドル取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return gpuHandle; }

	// ハンドル取得
	UINT GetIndex() const { return index; }
	// 使用中かどうか取得
	UINT GetUse() const { return use; }
	void  SetUse(UINT id) { use = id; }
};


class DescriptorHeap
{
private:
	vector<Descriptor*> descriptors;
	UINT maxNumDescriptor;//保存できるディスクリプターの最大数
	UINT numDescriptor = 0;//使用しているディスクリプターの数
	ComPtr<ID3D12DescriptorHeap>descriptorHeap;
public:
	DescriptorHeap(Graphics* graphics, UINT maxNum,D3D12_DESCRIPTOR_HEAP_TYPE descriptorType);
	~DescriptorHeap();

	Descriptor* GetDescriptor(UINT maxNum);
	Descriptor* GetUnusedDescriptor();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return  descriptorHeap; }
};
