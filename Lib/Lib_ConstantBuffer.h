#include "d3dx12.h"
#include <dxgi.h>
#include <wrl.h>
#include <DirectXMath.h>
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

class Graphics;
class Descriptor;

class Lib_ConstantBuffer
{
public:
	Lib_ConstantBuffer(Graphics* device,size_t size,void* srcData = nullptr);
	~Lib_ConstantBuffer();

	void Lock();//コンスタントバッファをロック
	void CopyToVRAM(Graphics* graphics, void* data);

	Descriptor* GetDescripter() { return constantBufferDescriptor; }

private:
	ComPtr<ID3D12Resource> constantBuffer = { nullptr };//定数バッファ。
	Descriptor* constantBufferDescriptor = nullptr;
	bool lockFlg = false;
	void* m_constBufferCPU = nullptr;
	int m_size = 0;									//定数バッファのサイズ。
};