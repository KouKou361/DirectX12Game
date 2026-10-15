#include "Lib_Graphics.h"
#include "d3dx12.h"
#include "Lib_DirectXFunction.h"
#include "Lib_ConstantBuffer.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "Lib_Vector.h"
#include "Lib_CameraLib.h"
#include <DirectXTex.h>

Graphics::Graphics(HWND hWnd, UINT windowWidth, UINT windowHeight)
{

	this->hWnd = hWnd;
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	ComPtr<ID3D12Debug> debug;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();

    EnableDebugLayer();      //デバッグレイヤー
	CreateDevice();          //デバイスを生成
	CreateFactory();         //ファクトリーの生成

	CreateCommandQueue();    //コマンドキューを生成
	CreateSwapChain();       //スワップチェインを生成
	CreateFence();

	CreateDescriptorRTV(10);								  //レンダーターゲットビュー用のディスクリプターヒープ作成
	CreateDescriptorDSV(10);								  //ディプスステンシルビュー用のディスクリプターヒープ作成
	CreateDescriptorSRV(10000);                               //シェーダーリソースビュー用のディスクリプターヒープ作成
	CreateDescriptorSMP(10);								  //サンプラー　　　　　　　用のディスクリプターヒープ作成
	CreateDescriptorImgui(1);								  //Imgui専用

	CreateBackBufferRTV();                               //バッファーのレンダーターゲットの作成
	CreateBackBufferDSV();                               //バッファーのデプスステンシルの作成



	CreateCommandList();     //コマンドリストとコマンドアロケーターを生成
	
	CreateWhileTexture();   //白色のテクスチャの作成
	CreateCameraConstantBuffer();//カメラの定数バッファ作成

	NormalShadowRtvResource = new RenderTargetViewResource();
	//CreateRTV(NormalShadowRtvResource, static_cast <UINT>(NormalShadowSize.x), static_cast <UINT>(NormalShadowSize.y));//レンダーターゲットの作成
	CreateRTV(NormalShadowRtvResource, static_cast <UINT>(windowWidth), static_cast <UINT>(windowHeight));//レンダーターゲットの作成

	cascadeShadowRtvResource.resize(CascadeShadowMapNum);
	for (int i = 0; i < CascadeShadowMapNum; i++)
	{
		cascadeShadowRtvResource.at(i) = new RenderTargetViewResource();
		CreateRTV(cascadeShadowRtvResource.at(i), static_cast <UINT>(shadowSize[i].x), static_cast <UINT>(shadowSize[i].y));
	}
}
	

Graphics::~Graphics()
{
	if (descriptorHeapSRV != nullptr)
	{
		delete descriptorHeapSRV;
		descriptorHeapSRV = nullptr;

	}
	if (descriptorHeapRTV != nullptr)
	{
		delete descriptorHeapRTV;
		descriptorHeapRTV = nullptr;
	}
	if (descriptorHeapSMP != nullptr)
	{
		delete descriptorHeapSMP;
		descriptorHeapSMP = nullptr;

	}
	if (descriptorHeapDSV != nullptr)
	{
		delete descriptorHeapDSV;
		descriptorHeapDSV = nullptr;
	}

	if (descriptorHeapImgui != nullptr)
	{
		delete descriptorHeapImgui;
		descriptorHeapImgui = nullptr;
	}


	if (NormalShadowRtvResource != nullptr)
	{
		delete NormalShadowRtvResource;
		NormalShadowRtvResource = nullptr;
	}


	for (int i = 0; i < CascadeShadowMapNum; i++)
	{
		if (cascadeShadowRtvResource.at(i) != nullptr)
		{
			delete cascadeShadowRtvResource.at(i);
			cascadeShadowRtvResource.at(i) = nullptr;
		}
	}

	


	

	
	for (auto adpt : adapters)
	{
		adpt->Release();
	}

	WaitSignal();
	if (fence != NULL)
	{
		CloseHandle(fenceEvent);
	}


	auto iter = textureResourceTable.begin();
	while (iter != textureResourceTable.end()) {
		iter->second->Release();
		++iter;
	}

	for (int idx = 0; idx < frameResources.size(); idx++)
	{
		FrameResource& frame_resource = frameResources.at(idx);
		if (frame_resource.sceneConstantBuffer != nullptr)
		{
			delete frame_resource.sceneConstantBuffer;
			frame_resource.sceneConstantBuffer = nullptr;
		}

	}
}


//デバッグレイヤー
void Graphics::EnableDebugLayer()
{
	HRESULT hr = S_OK;
	ID3D12Debug* debugLayout = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayout));

	debugLayout->EnableDebugLayer();
	debugLayout->Release();
}

//デバイスを生成
void Graphics::CreateDevice()
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	for (D3D_FEATURE_LEVEL level : levels)
	{
		hr = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(device.GetAddressOf()));
		if (hr == S_OK)break;
	}
	if (hr != S_OK)assert(!L"デバイス生成のエラー");
}

//ファクトリーの生成
void Graphics::CreateFactory()
{
	IDXGIAdapter* temAdaper = nullptr;//アダプター
	HRESULT hr = S_OK;

	hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf()));
	//hr = CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));


	if (hr != S_OK)assert(!L"ファクトリー生成のエラー");

	//アダプター
	for (int i = 0; factory->EnumAdapters(i, &temAdaper) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(temAdaper);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC desc = {};
		::memset(&desc, 0, sizeof(desc));
		adpt->GetDesc(&desc);

		wstring strDesc = desc.Description;
		//探したいアダプターの名前を確認
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			temAdaper = adpt;
			break;
		}
	}


}

//コマンドリストとコマンドアロケーター
void Graphics::CreateCommandList()
{
	HRESULT hr = S_OK;

	for (int idx = 0; idx < frameResources.size(); idx++)
	{
		FrameResource& frame_resource = frameResources.at(idx);

		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frame_resource.commandAllocator.GetAddressOf()));
		if (hr != S_OK)assert(!"コマンドアロケーター生成のエラー");

		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			frame_resource.commandAllocator.Get(), nullptr,
			IID_PPV_ARGS(frame_resource.commandList.GetAddressOf()));
		if (hr != S_OK)assert(!"コマンドリスト生成のエラー");


		frame_resource.commandList->Close();
		frame_resource.commandList->Reset(frame_resource.commandAllocator.Get(), nullptr);
	}



  //  commandAllocator->Reset();
}


//コマンドキューを生成
void Graphics::CreateCommandQueue()
{
	HRESULT hr = S_OK;
	D3D12_COMMAND_QUEUE_DESC desc = {};
	::memset(&desc, 0, sizeof(desc));

	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	if (hr != S_OK)assert(!"コマンドキュー生成のエラー");
}

//フェンス生成
void Graphics::CreateFence()
{
	device->CreateFence(fenceVal,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(fence.GetAddressOf()));
	//イベントハンドルの取得
	fenceEvent = CreateEvent(nullptr, false, false, nullptr);
}

//スワップチェインを生成
void Graphics::CreateSwapChain()
{
	buffer_count = 2;
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	::memset(&desc, 0, sizeof(desc));
	desc.Width = windowWidth;
	desc.Height = windowHeight;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = false;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	desc.BufferCount = buffer_count;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = factory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hWnd,
		&desc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)swapchain.GetAddressOf()
	);
	if (hr != S_OK)assert(!"スワップチェイン生成のエラー");
}
//バッファーのレンダーターゲットの作成
void Graphics::CreateBackBufferRTV()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC desc = {};
	::memset(&desc, 0, sizeof(desc));
	hr = swapchain->GetDesc(&desc);
	if (hr != S_OK)assert(!"ディスクリプタヒープ生成のエラー");
	frameResources.resize(buffer_count);

	for (int idx = 0; idx < frameResources.size(); idx++)
	{
		FrameResource& frame_resource = frameResources.at(idx);

		// ディスクリプタ取得
		frame_resource.rtv_descriptor = GetDescriptorHeapRTV()->GetUnusedDescriptor();

//		
		hr = swapchain->GetBuffer(idx, IID_PPV_ARGS(frame_resource.backBufferRtvResource.GetAddressOf()));
		frame_resource.backBufferRtvResource->SetName(L"BackBuffer");
		
		// レンダーターゲットビュー設定
		D3D12_RENDER_TARGET_VIEW_DESC d3d_rtv_desc;
		d3d_rtv_desc.Format = RenderTargetFormat;
		d3d_rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		d3d_rtv_desc.Texture2D.MipSlice = 0;
		d3d_rtv_desc.Texture2D.PlaneSlice = 0;
		
		device->CreateRenderTargetView(frame_resource.backBufferRtvResource.Get(), &d3d_rtv_desc, frame_resource.rtv_descriptor->GetCpuHandle());
	}


}

//バッファーのデプスステンシルの作成
void Graphics::CreateBackBufferDSV()
{
	HRESULT hr = S_OK;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES d3d_heap_props{};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;						// ヒープの種別(CPUからアクセスできない)
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUのページング設定(D3D12_HEAP_TYPE_CUSTUM以外では考えなくてよい)
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールがどこかを示す(D3D12_HEAP_TYPE_CUSTUM以外では考えなくてよい)
	d3d_heap_props.CreationNodeMask = 1;								// 単一アダプタなら0でよい
	d3d_heap_props.VisibleNodeMask = 1;									// 単一アダプタなら0でよい

	// リソースの設定
	D3D12_RESOURCE_DESC d3d_resource_desc{};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	d3d_resource_desc.Alignment = 0;									// 
	d3d_resource_desc.Width = windowWidth;								// テクスチャ幅
	d3d_resource_desc.Height = windowHeight;							// テクスチャ高さ
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;									// ミップマップレベル(0を使用すると最大ミップレベルになる)
	d3d_resource_desc.Format = DXGI_FORMAT_D32_FLOAT;					// フォーマットを指定
	d3d_resource_desc.SampleDesc.Count = 1;								// アンチエイリアシングのためのサンプリング回数
	d3d_resource_desc.SampleDesc.Quality = 0;							// アンチエイリアシングの品質指定
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;			// テクスチャレイアウトオプション
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; 	// リソースを操作するためのオプション

	// クリア値の設定
	D3D12_CLEAR_VALUE d3d_dsv_clear_value{};
	d3d_dsv_clear_value.Format = DepthStencilFormat;
	d3d_dsv_clear_value.DepthStencil.Depth = 1.0f;
	d3d_dsv_clear_value.DepthStencil.Stencil = 0;

	for (int idx = 0; idx < frameResources.size(); idx++)
	{
		FrameResource& frame_resource = frameResources.at(idx);
		// リソースの生成
		hr = device->CreateCommittedResource(
			&d3d_heap_props,												// ヒーププロパティの設定
			D3D12_HEAP_FLAG_NONE,											// ヒープオプション
			&d3d_resource_desc,												// リソースの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE,								// リソースの使用方法
			&d3d_dsv_clear_value,											// クリア設定
			IID_PPV_ARGS(&frame_resource.backBufferDsvResource)	// 格納するポインタのアドレス
		);
		if (hr != S_OK)assert(!"深度情報作成のエラー");

		frame_resource.backBufferDsvResource->SetName(L"DepthStencilView");

		// ディスクリプタ取得
		frame_resource.dsv_descriptor = GetDescriptorHeapDSV()->GetUnusedDescriptor();

		// デプスステンシルビューの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC d3d_dsv_desc{};
		d3d_dsv_desc.Format = DepthStencilFormat;							// リソースのフォーマットと同じにする
		d3d_dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;			// リソースのディメンションと同じにする
		d3d_dsv_desc.Flags = D3D12_DSV_FLAG_NONE;							// デプスステンシルビューオプション

		// デプスステンシルビュー生成
		device->CreateDepthStencilView(
			frame_resource.backBufferDsvResource.Get(),							// リソースのポインタ
			&d3d_dsv_desc,													// デプスステンシルの設定
			frame_resource.dsv_descriptor->GetCpuHandle()					// ディスクリプタを作成するアドレス
		);

	}

}

//レンダーターゲットの作成
void Graphics::CreateRTV(RenderTargetViewResource* rtvResource,const UINT Width, const UINT Height, const DXGI_FORMAT& rtvFormat)//レンダーターゲットの作成
{

	HRESULT hr = S_OK;

	rtvResource->rtv_descriptor = GetDescriptorHeapRTV()->GetUnusedDescriptor();
	rtvResource->srv_descriptor = GetDescriptorHeapSRV()->GetUnusedDescriptor();

	float clsClr[4] = { 0.0f,1.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE colorValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr);
	// レンダーターゲットビュー作成
	{
		D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto backbufferDesc = frameResources.at(0).backBufferRtvResource->GetDesc();

		backbufferDesc.Width = Width;
		backbufferDesc.Height = Height;

		device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&backbufferDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&colorValue,
			IID_PPV_ARGS(rtvResource->rtvResource.ReleaseAndGetAddressOf())
			);


		D3D12_RENDER_TARGET_VIEW_DESC d3d_rtv_desc = {};
		d3d_rtv_desc.Format = rtvFormat;
		d3d_rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		device->CreateRenderTargetView(rtvResource->rtvResource.Get(), &d3d_rtv_desc, rtvResource->rtv_descriptor->GetCpuHandle());
	}

	//シェーダーリソース作成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
		d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3d_srv_desc.Format = rtvFormat;
		d3d_srv_desc.Texture2D.MipLevels = 1;
		d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		device->CreateShaderResourceView(rtvResource->rtvResource.Get(), &d3d_srv_desc, rtvResource->srv_descriptor->GetCpuHandle());
	}



	//深度
	{
		rtvResource->dsv_descriptor = GetDescriptorHeapDSV()->GetUnusedDescriptor();

		// デプスステンシルビューの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC d3d_dsv_desc{};
		d3d_dsv_desc.Format = DepthStencilFormat;							// リソースのフォーマットと同じにする
		d3d_dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;			// リソースのディメンションと同じにする
		d3d_dsv_desc.Flags = D3D12_DSV_FLAG_NONE;							// デプスステンシルビューオプション

		HRESULT hr = S_OK;

		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;						// ヒープの種別(CPUからアクセスできない)
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUのページング設定(D3D12_HEAP_TYPE_CUSTUM以外では考えなくてよい)
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// メモリプールがどこかを示す(D3D12_HEAP_TYPE_CUSTUM以外では考えなくてよい)
		d3d_heap_props.CreationNodeMask = 1;								// 単一アダプタなら0でよい
		d3d_heap_props.VisibleNodeMask = 1;									// 単一アダプタなら0でよい

		// リソースの設定
		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
		d3d_resource_desc.Alignment = 0;									// 
		d3d_resource_desc.Width = Width;								// テクスチャ幅
		d3d_resource_desc.Height = Height;							// テクスチャ高さ
		//d3d_resource_desc.Width = windowWidth;								// テクスチャ幅
		//d3d_resource_desc.Height = windowHeight;							// テクスチャ高さ
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;									// ミップマップレベル(0を使用すると最大ミップレベルになる)
		d3d_resource_desc.Format = DXGI_FORMAT_D32_FLOAT;					// フォーマットを指定
		d3d_resource_desc.SampleDesc.Count = 1;								// アンチエイリアシングのためのサンプリング回数
		d3d_resource_desc.SampleDesc.Quality = 0;							// アンチエイリアシングの品質指定
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;			// テクスチャレイアウトオプション
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; 	// リソースを操作するためのオプション

		// クリア値の設定
		D3D12_CLEAR_VALUE d3d_dsv_clear_value{};
		d3d_dsv_clear_value.Format = DepthStencilFormat;
		d3d_dsv_clear_value.DepthStencil.Depth = 1.0f;
		d3d_dsv_clear_value.DepthStencil.Stencil = 0;	
	
		// リソースの生成
		hr = device->CreateCommittedResource(
			&d3d_heap_props,												// ヒーププロパティの設定
			D3D12_HEAP_FLAG_NONE,											// ヒープオプション
			&d3d_resource_desc,												// リソースの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE,								// リソースの使用方法
			&d3d_dsv_clear_value,											// クリア設定
			IID_PPV_ARGS(&rtvResource->dsvResource)	// 格納するポインタのアドレス
		);
		if (hr != S_OK)assert(!"深度情報作成のエラー");
	
		rtvResource->dsvResource->SetName(L"DepthStencilView");
	
		// デプスステンシルビュー生成
		device->CreateDepthStencilView(
			rtvResource->dsvResource.Get(),							// リソースのポインタ
			&d3d_dsv_desc,													// デプスステンシルの設定
			rtvResource->dsv_descriptor->GetCpuHandle()					// ディスクリプタを作成するアドレス
		);
	}


}

void Graphics::CreateDescriptorSRV(u_int maxNum)
{
	descriptorHeapSRV = new DescriptorHeap(this, maxNum, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Graphics::CreateDescriptorSMP(u_int maxNum)
{
	descriptorHeapSMP = new DescriptorHeap(this, maxNum, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

void Graphics::CreateDescriptorRTV(u_int maxNum)
{
	descriptorHeapRTV = new DescriptorHeap(this, maxNum, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Graphics::CreateDescriptorDSV(u_int maxNum)
{
	descriptorHeapDSV = new DescriptorHeap(this, maxNum, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Graphics::CreateDescriptorImgui(u_int maxNum)
{
	descriptorHeapImgui = new DescriptorHeap(this, maxNum, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}




// バッファコピー
HRESULT Graphics::CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource)
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);
	
	ID3D12CommandAllocator* commandAllocator = frame_resource.commandAllocator.Get();
	ID3D12GraphicsCommandList* commandList     =frame_resource.commandList.Get();

	D3D12_RESOURCE_BARRIER d3d_resource_barrier = {};
	d3d_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource = d3d_dst_resource;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	commandList->ResourceBarrier(1, &d3d_resource_barrier);

	commandList->CopyResource(d3d_dst_resource, d3d_src_resource);

	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	commandList->ResourceBarrier(1, &d3d_resource_barrier);

	commandList->Close();

	ID3D12CommandList* d3d_command_lists[] =
	{
		commandList
	};
	commandQueue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);

	WaitSignal();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, nullptr);

	return S_OK;
}


ID3D12Resource* Graphics::LoadTexture(string filename)
{

	//ここで一旦既に登録したデータならそのデータを返す

	auto it = textureResourceTable.find(filename);
	if (it != textureResourceTable.end())
		if (it != textureResourceTable.end())
		{
			//リソースを返す
			it->second->AddRef();
			return it->second;
		}

	HRESULT hr = S_OK;
	//WICテクスチャのロード
	TexMetadata metadata = {};
	ScratchImage scrachImg = {};
	

	auto wtexpath = GetWidiStringFromString(filename);
	auto ext = GetExtension(filename);
	//hr = loadLambdaTable[ext](wtexpath, &metadata, scrachImg);

	if (strcmp(ext.c_str(), "psd") == 0 || strcmp(ext.c_str(), "PSD") == 0) {
		filename = filename.substr(0,filename.length()-3);
		filename += "tga";
		ext = "tga";
	}


	if (strcmp(ext.c_str(), "tga") == 0 || strcmp(ext.c_str(), "TGA") == 0) {
	
		GetMetadataFromTGAFile(GetWidiStringFromString(filename).c_str(), metadata);

	
		hr = LoadFromTGAFile(GetWidiStringFromString(filename).c_str(), &metadata, scrachImg);

	}
	else
	{
		hr = LoadFromWICFile(
			GetWidiStringFromString(filename).c_str(),
			WIC_FLAGS_NONE,
			&metadata,
			scrachImg
		);
	}


	

	if (hr != S_OK)return nullptr;

	auto img = scrachImg.GetImage(0, 0, 0);

	//WriteToSubresourceで転送する用のヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	::memset(&texHeapProp, 0, sizeof(texHeapProp));

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC desc = {};
	::memset(&desc, 0, sizeof(desc));
	desc.Format = metadata.format;
	desc.Width = metadata.width;
	desc.Height = metadata.height;
	desc.DepthOrArraySize = metadata.arraySize;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = metadata.mipLevels;
	desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;


	//バッファ作成
	ID3D12Resource* texBuffer = nullptr;
	hr = device->CreateCommittedResource(
		&texHeapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuffer)
	);
	if (hr != S_OK)return nullptr;




	hr = texBuffer->WriteToSubresource(
		0, nullptr,
		img->pixels,
		img->rowPitch,
		img->slicePitch
	);

	//commandList->Close();
	//
	//ID3D12CommandList* d3d_command_lists[] =
	//{
	//	commandList.Get()
	//};
	//commandQueue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);
	//
	//WaitSignal();
	//
	//commandAllocator->Reset();
	//commandList->Reset(commandAllocator.Get(), nullptr);

	if (hr != S_OK)	return nullptr;
	texBuffer->SetName(L"aaaaa");

	//登録登録～
	textureResourceTable[filename] = texBuffer;


	return texBuffer;
}

void Graphics::CreateCameraConstantBuffer()
{

	for (int idx = 0; idx < frameResources.size(); idx++)
	{
		FrameResource& frame_resource = frameResources.at(idx);
		frame_resource.sceneConstantBuffer = new Lib_ConstantBuffer(this,sizeof(CbScene), &frame_resource.cb_scene_data);
	}
}

void Graphics::CreateWhileTexture()
{
	HRESULT hr = S_OK;
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	::memset(&texHeapProp, 0, sizeof(texHeapProp));

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC desc = {};
	::memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = 4;
	desc.Height = 4;
	desc.DepthOrArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//バッファ作成
	hr = device->CreateCommittedResource(
		&texHeapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(whiteTexture.GetAddressOf())
	);
	if (hr != S_OK)assert(!L"白テクスチャのエラー");

	std::vector<unsigned char>data(4 * 4 * 4);
	fill(data.begin(), data.end(), 0xff);

	hr = whiteTexture->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	if (hr != S_OK)assert(!L"白テクスチャのエラー");
}


void Graphics::WaitSignal()
{
	commandQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		fence->SetEventOnCompletion(fenceVal, fenceEvent);

		//イベントが発生するまで待つ。
		WaitForSingleObject(fenceEvent, INFINITE);

	}
	//if(fence.GetAddressOf()-)
}


//クロップ行列
XMFLOAT4X4 Graphics::CreateClop(const int Shadow_Index, const CameraLib& ModelView, const CameraLib& LightView, const XMMATRIX	viewMatrix, const XMMATRIX projectionMatrix)
{
	// カメラからパラメーターを取得
	DirectX::XMVECTOR	viewFront, viewRight, viewUp, viewPos;
	{
		DirectX::XMMATRIX   View = DirectX::XMLoadFloat4x4(&ModelView.GetView());
		DirectX::XMMATRIX	matrix = DirectX::XMMatrixInverse(nullptr, View);
		viewRight = DirectX::XMVector3Normalize(matrix.r[0]);
		viewUp = DirectX::XMVector3Normalize(matrix.r[1]);
		viewFront = DirectX::XMVector3Normalize(matrix.r[2]);
		viewPos = matrix.r[3];
	}
	//	シャドウマップの分割エリア定義
	float	splitAreaTable[] =
	{
		ModelView.GetNear(),	//	カメラのニアクリップ
		shadowArea[0],
		shadowArea[1],
		shadowArea[2]
	};

	float	nearDepth = splitAreaTable[Shadow_Index + 0];
	float	farDepth = splitAreaTable[Shadow_Index + 1];
	//	エリアを内包する視推台の8頂点を算出する
	DirectX::XMVECTOR	vertex[8];
	{
		//	エリアの近平面の中心からの上面までの距離を求める
		float	nearY = tanf(ModelView.GetFor() * 0.5f) * nearDepth;
		//	エリアの近平面の中心からの右面までの距離を求める
		float	nearX = nearY * ModelView.GetAspext();
		//	エリアの遠平面の中心からの上面までの距離を求める
		float	farY = tanf(ModelView.GetFor() * 0.5f) * farDepth;
		//	エリアの遠平面の中心からの右面までの距離を求める
		float	farX = farY * ModelView.GetAspext();

		//	エリアの近平面の中心座標を求める
		DirectX::XMVECTOR	nearPosition = DirectX::XMVectorAdd(viewPos, DirectX::XMVectorScale(viewFront, nearDepth));
		//	エリアの遠平面の中心座標を求める
		DirectX::XMVECTOR	farPosition = DirectX::XMVectorAdd(viewPos, DirectX::XMVectorScale(viewFront, farDepth));

		//	8頂点を求める
		{
			// 近平面の右上
			vertex[0] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, nearY), DirectX::XMVectorScale(viewRight, nearX)));
			// 近平面の左上
			vertex[1] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, nearY), DirectX::XMVectorScale(viewRight, -nearX)));
			// 近平面の右下
			vertex[2] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, -nearY), DirectX::XMVectorScale(viewRight, nearX)));
			// 近平面の左下
			vertex[3] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, -nearY), DirectX::XMVectorScale(viewRight, -nearX)));
			// 遠平面の右上
			vertex[4] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, farY), DirectX::XMVectorScale(viewRight, farX)));
			// 遠平面の左上
			vertex[5] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, farY), DirectX::XMVectorScale(viewRight, -farX)));
			// 遠平面の右下
			vertex[6] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, -farY), DirectX::XMVectorScale(viewRight, farX)));
			// 遠平面の左下
			vertex[7] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(viewUp, -farY), DirectX::XMVectorScale(viewRight, -farX)));
		}
	}

	//	8頂点をライトビュープロジェクション空間に変換して、最大値、最小値を求める
	DirectX::XMFLOAT3	vertexMin(FLT_MAX, FLT_MAX, FLT_MAX), vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto& it : vertex)
	{
		DirectX::XMFLOAT3	p;
		DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(it, viewMatrix * projectionMatrix));

		vertexMin.x = min(p.x, vertexMin.x);
		vertexMin.y = min(p.y, vertexMin.y);
		vertexMax.x = max(p.x, vertexMax.x);
		vertexMax.y = max(p.y, vertexMax.y);
	}

	//	クロップ行列を求める
	DirectX::XMFLOAT4X4	ClopMatrix;
	DirectX::XMMATRIX	clopMatrix = DirectX::XMMatrixIdentity();
	{
		float	xScale = 2.0f / (vertexMax.x - vertexMin.x);
		float	yScale = 2.0f / (vertexMax.y - vertexMin.y);
		float	xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
		float	yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;

		DirectX::XMStoreFloat4x4(&ClopMatrix, clopMatrix);
		ClopMatrix._11 = xScale;
		ClopMatrix._22 = yScale;
		ClopMatrix._41 = xOffset;
		ClopMatrix._42 = yOffset;
	}

	return ClopMatrix;
}


void Graphics::CreateShadowMapMatrix(const CameraLib& ModelView, const CameraLib& LightView)
{
	// ライトビュープロジェクション行列を算出
	DirectX::XMMATRIX	viewMatrix, projectionMatrix;
	{
		// ライト位置からのビューの設定
		VECTOR3 t(0, 0, 0);
		VECTOR3 p;

		t.x = ModelView.GetTarget().x;
		t.y = ModelView.GetTarget().y;
		t.z = ModelView.GetTarget().z;

		//p.x = LightView.GetPos().x;
		//p.y = LightView.GetPos().y;
		//p.z = LightView.GetPos().z;

		p.x = t.x - LightView.GetFront().x * 800.0f;
		p.y = t.y - LightView.GetFront().y * 800.0f;
		p.z = t.z - LightView.GetFront().z * 800.0f;

		// ライト用のカメラ
		CameraLib v;
		v.SetLookAt(p, t, VECTOR3(0, 1, 0));
		v.SetOrthongraphic(10000, 10000, 0.01f, 2000.0f);// 平行投影
		viewMatrix = XMLoadFloat4x4(&v.GetView());
		projectionMatrix = XMLoadFloat4x4(&v.GetProjection());
	}

	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);


	//　シャドウマップの作製
	for (int Shadow_Index = 0; Shadow_Index < CascadeShadowMapNum; Shadow_Index++)
	{
		//クロップ行列の作成
		XMMATRIX clopMatrix;
		{
			XMFLOAT4X4 clop;
			clop = CreateClop(Shadow_Index, ModelView, LightView, viewMatrix, projectionMatrix);
			clopMatrix = XMLoadFloat4x4(&clop);
		}


		//ライトビュープロジェクション行列を計算
		{
			DirectX::XMStoreFloat4x4(&frame_resource.cb_scene_data.cascadeShadowLightViewProjection[Shadow_Index], viewMatrix * projectionMatrix * clopMatrix);
			DirectX::XMStoreFloat4x4(&frame_resource.cb_scene_data.createCascadeShadowLightViewProjection[Shadow_Index], viewMatrix * projectionMatrix * clopMatrix);
		}
	}
}



//ビューポート生成
void Graphics::SetViewPort(const UINT& Width, const UINT& Height)
{

	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	viewPort = {};
	viewPort.Height = Height;
	viewPort.Width = Width;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	frame_resource.commandList->RSSetViewports(1, &viewPort);
}

//シザー矩形生成
void Graphics::SetScissorRect(const UINT& Width, const UINT& Height)
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + Width;
	scissorRect.bottom = scissorRect.top + Height;
	frame_resource.commandList->RSSetScissorRects(1, &scissorRect);
}
//ノーマルシャドウマップに切り替え準備
void Graphics::NormalShadowBeginRender()
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);
	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();

	//ノーマルシャドウマップにバリア切り替え
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		NormalShadowRtvResource->rtvResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);

	//シザーの設定
	SetScissorRect(windowWidth, windowHeight);

	//ビューポートの設定
	SetViewPort(windowWidth, windowHeight);

	//バックバッファに書き込み
	SetRTV(NormalShadowRtvResource->rtv_descriptor, NormalShadowRtvResource->dsv_descriptor, {0,1,0,1},false);
}

//カスケードシャドウマップに切り替える準備
void Graphics::CasCadeShadowBeginRender(const UINT shadowMapIndex)
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);
	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();

	RenderTargetViewResource* shadowMapRTVResource = cascadeShadowRtvResource.at(shadowMapIndex);

	//ノーマルシャドウマップにバリア切り替え
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		shadowMapRTVResource->rtvResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);


	//シザーの設定
	SetScissorRect(shadowSize[shadowMapIndex].x, shadowSize[shadowMapIndex].y);

	//ビューポートの設定
	SetViewPort(shadowSize[shadowMapIndex].x, shadowSize[shadowMapIndex].y);

	//バックバッファに書き込み
	SetRTV(shadowMapRTVResource->rtv_descriptor, shadowMapRTVResource->dsv_descriptor, { 0,1,0,1 }, false);
}

void Graphics::BackBufferBeginRender()
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);
	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();

	//バックバッファ設定
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			frame_resource.backBufferRtvResource.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		commandList->ResourceBarrier(1, &barrier);
	}

	//ノーマルシャドウマップ設定
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			NormalShadowRtvResource->rtvResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		commandList->ResourceBarrier(1, &barrier);
	}

	//カスケードシャドウマップ設定（０）
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			cascadeShadowRtvResource.at(0)->rtvResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		commandList->ResourceBarrier(1, &barrier);
	}

	//カスケードシャドウマップ設定（１）
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			cascadeShadowRtvResource.at(1)->rtvResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		commandList->ResourceBarrier(1, &barrier);
	}
	//カスケードシャドウマップ設定（２）
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			cascadeShadowRtvResource.at(2)->rtvResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		commandList->ResourceBarrier(1, &barrier);
	}
	
	//シザーの設定
	SetScissorRect(windowWidth, windowHeight);

	//ビューポートの設定
	SetViewPort(windowWidth, windowHeight);


	//バックバッファに書き込み
	SetRTV(frame_resource.rtv_descriptor, frame_resource.dsv_descriptor);

	ImGui::Begin("Hello World!");
	if (ImGui::TreeNode("NormalShadowMap")) {

		ImTextureID texture_id = (ImTextureID)(NormalShadowRtvResource->srv_descriptor->GetGpuHandle().ptr);
		ImGui::Image(texture_id, ImVec2{ 100,100 });

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("CascadeShadowMap")) {

		ImTextureID texture_id = (ImTextureID)(cascadeShadowRtvResource.at(0)->srv_descriptor->GetGpuHandle().ptr);
		ImGui::Image(texture_id, ImVec2{ 100,100 });

		texture_id = (ImTextureID)(cascadeShadowRtvResource.at(1)->srv_descriptor->GetGpuHandle().ptr);
		ImGui::Image(texture_id, ImVec2{ 100,100 });

		texture_id = (ImTextureID)(cascadeShadowRtvResource.at(2)->srv_descriptor->GetGpuHandle().ptr);
		ImGui::Image(texture_id, ImVec2{ 100,100 });

		ImGui::TreePop();
	}

	ImGui::End();
}

void Graphics::BeginRender()
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();
	
	// ディスクリプタヒープをあらかじめ設定
	ID3D12DescriptorHeap* descriptorHeaps[] =
	{
		GetDescriptorHeapSRV()->GetDescriptorHeap().Get(),
		GetDescriptorHeapSMP()->GetDescriptorHeap().Get(),
	};
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void Graphics::SetRTV(Descriptor* rtv,Descriptor* dsv,XMFLOAT4 color,BOOL rtvFlg)
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	ID3D12CommandAllocator* commandAllocator = frame_resource.commandAllocator.Get();
	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();

	//全てのレンダーターゲットビューとディプスステンシルをリセット
	FLOAT clear_color[4] = { color.x,color.y,color.z,color.w };

	D3D12_CPU_DESCRIPTOR_HANDLE d3d_rtv_handle = rtv->GetCpuHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE d3d_dsv_handle = dsv->GetCpuHandle();

	commandList->ClearRenderTargetView(d3d_rtv_handle, clear_color, 0, nullptr);
	commandList->ClearDepthStencilView(d3d_dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//バックバッファをセット
	commandList->OMSetRenderTargets(1, &d3d_rtv_handle, rtvFlg, &d3d_dsv_handle);

}


void Graphics::EndRender()
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	ID3D12CommandAllocator* commandAllocator = frame_resource.commandAllocator.Get();
	ID3D12GraphicsCommandList* commandList = frame_resource.commandList.Get();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		frame_resource.backBufferRtvResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList->ResourceBarrier(1, &barrier);
	commandList->Close();

	//コマンドリストに投げる。
	ID3D12CommandList* cmdList[] = { commandList };
	commandQueue->ExecuteCommandLists(1, cmdList);


	//描画が完成するまで待つ
	WaitSignal();

	//アロケーターとリストをリセット。
	commandAllocator->Reset();
	commandList->Reset(commandAllocator, nullptr);
	//	commandAllocator->Reset();
	//	commandList->Reset(commandAllocator.Get(), nullptr);

		//SetBarrierBefore(CurrentBackBuffer);

			//


		//フリップ！
	swapchain->Present(1, 0);


}

void Graphics::UpdateCamera(const CameraLib& modelCamera, const CameraLib& shadowCamera)
{
	UINT CurrentBackBuffer = swapchain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = frameResources.at(CurrentBackBuffer);

	
	XMFLOAT4X4 cameraViewProjection;
	{

		XMFLOAT4X4  view, projection = {};
		XMMATRIX ViewProjection, View, Projection;
		view = modelCamera.GetView();
		projection = modelCamera.GetProjection();
		View = XMLoadFloat4x4(&view);
		Projection = XMLoadFloat4x4(&projection);
		ViewProjection = View * Projection;
		XMStoreFloat4x4(&cameraViewProjection, ViewProjection);
	}

	XMFLOAT4X4 shadowViewProjection;
	{

		XMFLOAT4X4 viewProjection, view, projection = {};
		XMMATRIX ViewProjection, View, Projection;
		view = shadowCamera.GetView();
		projection = shadowCamera.GetProjection();
		View = XMLoadFloat4x4(&view);
		Projection = XMLoadFloat4x4(&projection);
		ViewProjection = View * Projection;
		XMStoreFloat4x4(&shadowViewProjection, ViewProjection);
	}
	

	XMFLOAT4 modelLightDirection;
	{
		XMFLOAT4 l = XMFLOAT4(shadowCamera.GetFront().x, shadowCamera.GetFront().y, shadowCamera.GetFront().z,1.0f);
		XMVECTOR L = XMLoadFloat4(&l);
		L = XMVector3Normalize(L);
		XMStoreFloat4(&modelLightDirection, L);
	}

	XMFLOAT4 cameraDirection;
	{
		XMFLOAT4 l = XMFLOAT4(modelCamera.GetFront().x, modelCamera.GetFront().y, modelCamera.GetFront().z, 1.0f);
		XMVECTOR L = XMLoadFloat4(&l);
		L = XMVector3Normalize(L);
		XMStoreFloat4(&cameraDirection, L);
	}

	



	frame_resource.cb_scene_data.cameraView = modelCamera.GetView();
	frame_resource.cb_scene_data.cameraProjection = modelCamera.GetProjection();
	frame_resource.cb_scene_data.cameraPos = DirectX::XMFLOAT4(modelCamera.GetPos().x, modelCamera.GetPos().y, modelCamera.GetPos().z, 1.0f);
	frame_resource.cb_scene_data.cameraDirection = DirectX::XMFLOAT4(cameraDirection.x, cameraDirection.y, cameraDirection.z, 1.0f);
	frame_resource.cb_scene_data.cameraViewProjection = cameraViewProjection;
	frame_resource.cb_scene_data.lightDirection = DirectX::XMFLOAT4(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z, 1.0f);
	frame_resource.cb_scene_data.lightColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	frame_resource.cb_scene_data.shadowLightViewProjection = shadowViewProjection;


	frameResources.at(GetCurrentBufferIndex()).sceneConstantBuffer->CopyToVRAM(this, &frame_resource.cb_scene_data);
}

Descriptor* Graphics::GetCameraDescriptor()
{
	return frameResources.at(GetCurrentBufferIndex()).sceneConstantBuffer->GetDescripter();
}


