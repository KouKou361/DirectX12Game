#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <map>
#include <DirectXMath.h>
#include "Lib_Descriptor.h"
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

// フォーマット
static const DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;


struct CbScene
{
	DirectX::XMFLOAT4X4	cameraView;
	DirectX::XMFLOAT4X4 cameraProjection;
	DirectX::XMFLOAT4	cameraPos;
	DirectX::XMFLOAT4   cameraDirection;

	DirectX::XMFLOAT4X4	cameraViewProjection;

	DirectX::XMFLOAT4	lightDirection;
	DirectX::XMFLOAT4	lightColor;

	DirectX::XMFLOAT4X4	shadowLightViewProjection;
	DirectX::XMFLOAT4X4	createCascadeShadowLightViewProjection[3];
	DirectX::XMFLOAT4X4	cascadeShadowLightViewProjection[3];
};

class Lib_ConstantBuffer;
class CameraLib;
class Graphics
{
private:

	struct FrameResource
	{
		ComPtr<ID3D12CommandAllocator>		commandAllocator;
		ComPtr<ID3D12GraphicsCommandList>	commandList;
		ComPtr<ID3D12Resource>				backBufferRtvResource;
		ComPtr<ID3D12Resource>				backBufferDsvResource;
		Descriptor* rtv_descriptor = nullptr;
		Descriptor* dsv_descriptor = nullptr;

		Lib_ConstantBuffer* sceneConstantBuffer = nullptr;
		CbScene cb_scene_data = {};
	};
	//レンダーターゲット専用(テクスチャに書き込みしたい用)
	class RenderTargetViewResource
	{
	public:
		RenderTargetViewResource() {};

		Descriptor* rtv_descriptor = nullptr;
		Descriptor* srv_descriptor = nullptr;
		Descriptor* dsv_descriptor = nullptr;
		ComPtr<ID3D12Resource> rtvResource;
		ComPtr<ID3D12Resource> srvResource;
		ComPtr<ID3D12Resource> dsvResource;
	};

public:
	Graphics(HWND hWnd,UINT windowWidth,UINT windowHeight);
	~Graphics();

	//描画処理の準備
	void BeginRender();
	//ノーマルシャドウマップに切り替え準備
	void NormalShadowBeginRender();
	//カスケードシャドウマップに切り替える準備
	void CasCadeShadowBeginRender(const UINT shadowMapIndex);
	//バックバッファに切り替える準備
	void BackBufferBeginRender();
	//描画処理するため時間を待たせる。
	void WaitSignal();
	//描画処理終了
	void EndRender(); 
	XMFLOAT4X4 CreateClop(const int Shadow_Index, const CameraLib& ModelView, const CameraLib& LightView, const XMMATRIX	viewMatrix, const XMMATRIX projectionMatrix);
	void CreateShadowMapMatrix(const CameraLib& ModelView, const CameraLib& LightView);

	//レンダーターゲットの切り替え
	void SetRTV(Descriptor* rtv, Descriptor* dsv, XMFLOAT4 color = XMFLOAT4{ 0,1,0,1 }, BOOL rtvFlg = true);
	//コンスタントバッファの作成
	void UpdateCamera(const CameraLib& modelCamera, const CameraLib& shadowCamera);

	ID3D12Resource* LoadTexture(string filename);       //テクスチャの読み込み
	ID3D12Resource* GetWhileTexture() { return whiteTexture.Get(); };                        //白色のテクスチャの取得
	HRESULT CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource);
	
	ID3D12Device* GetDevice() { return device.Get(); }
	



	DescriptorHeap* GetDescriptorHeapSRV(){return descriptorHeapSRV;}
	DescriptorHeap* GetDescriptorHeapRTV(){return descriptorHeapRTV;}
	DescriptorHeap* GetDescriptorHeapSMP(){return descriptorHeapSMP;}
	DescriptorHeap* GetDescriptorHeapDSV(){return descriptorHeapDSV;}
	DescriptorHeap* GetDescriptorHeapImgui() { return descriptorHeapImgui; }

	UINT GetWindowWidth() { return windowWidth; };
	UINT GetWindowHeight() { return windowHeight; };

	// バッファ数取得
	UINT GetBufferCount() const { return buffer_count; }

	// バッファインデックス取得
	UINT GetCurrentBufferIndex() const { return swapchain->GetCurrentBackBufferIndex(); }

	ID3D12GraphicsCommandList* GetCommandList() { return frameResources.at(GetCurrentBufferIndex()).commandList.Get(); };
	ID3D12CommandAllocator* GetCommandAllocator() { return frameResources.at(GetCurrentBufferIndex()).commandAllocator.Get(); };
	Descriptor* GetCameraDescriptor();
	Descriptor* GetShadowMapDescriptor() { return NormalShadowRtvResource->srv_descriptor; }
	Descriptor* GetCascadeShadowMapDescriptor(const int index) { 
		return cascadeShadowRtvResource.at(index)->srv_descriptor;
	}
	//ID3D12Resource* GetSceneConstantBuffer() { return constBuffer.Get(); }
	//DescriptorHeapSRV* GetShaderResourceHeap() { return shaderResourceHeapSRV; };
	//Descriptor* GetConstDescriptor() { return sceneDescriptor; }
	//ID3D12Resource* GetWhiteTexture() { return whiteTexture.Get(); }
	//ID3D12Resource* GetBlackTexture() { return blackTexture.Get(); }
	//ID3D12Resource* GetGradTexture() { return gradTexture.Get(); }
private:

	void EnableDebugLayer();                                  //デバッグレイヤー
	void CreateDevice();                                      //デバイスを生成
	void CreateFactory();                                     //ファクトリーの生成
	void CreateSwapChain();                                   //スワップチェインを生成
	void CreateCommandList();                                 //コマンドリストを生成
	void CreateCommandQueue();                                //コマンドキューを生成
	void CreateFence();                                       //フェンス生成
	void CreateBackBufferRTV();                               //バッファーのレンダーターゲットの作成
	void CreateBackBufferDSV();                               //バッファーのデプスステンシルの作成

	void CreateRTV(RenderTargetViewResource* rtvResource, const UINT Width, const UINT Height, const DXGI_FORMAT& rtvFormat = RenderTargetFormat);//レンダーターゲットの作成

	void CreateDescriptorSRV(u_int maxNum);                   //シェーダーリソースビュー用のディスクリプターヒープ作成
	void CreateDescriptorSMP(u_int maxNum);					  //サンプラー　　　　　　　用のディスクリプターヒープ作成
	void CreateDescriptorRTV(u_int maxNum);					  //レンダーターゲットビュー用のディスクリプターヒープ作成
	void CreateDescriptorDSV(u_int maxNum);					  //ディプスステンシルビュー用のディスクリプターヒープ作成
	void CreateDescriptorImgui(u_int maxNum);                   //シェーダーリソースビュー用のディスクリプターヒープ作成

	void CreateCameraConstantBuffer();                        //カメラのコンスタントバッファ設定
	void CreateWhileTexture();

	void SetViewPort(const UINT &Width, const UINT& Height);//ビューポート生成
	void SetScissorRect(const UINT& Width, const UINT& Height);//シザー矩形生成
private:													  
	ComPtr<ID3D12Device> device = nullptr;                    //デバイス
	ComPtr<IDXGIFactory4> factory = nullptr;                  //ファクトリー
	ComPtr<IDXGISwapChain3> swapchain = nullptr;              //スワップチェーン
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;        //コマンドキュー
	ComPtr<ID3D12Fence> fence = nullptr;                      //フェンス
	
	//白色のテクスチャ
	ComPtr<ID3D12Resource> whiteTexture = nullptr;

	HANDLE fenceEvent = NULL;
	//ビューポート
	D3D12_VIEWPORT viewPort;
	//シザー矩形
	D3D12_RECT scissorRect;

	DescriptorHeap* descriptorHeapSRV=nullptr;//シェーダーリソースビュー用のディスクリプター
	DescriptorHeap* descriptorHeapRTV=nullptr;//サンプラー　　　　　　　用のディスクリプター
	DescriptorHeap* descriptorHeapSMP=nullptr;//レンダーターゲットビュー用のディスクリプター
	DescriptorHeap* descriptorHeapDSV=nullptr;//ディプスステンシルビュー用のディスクリプター
	DescriptorHeap* descriptorHeapImgui = nullptr;//ディプスステンシルビュー用のディスクリプター

	vector<IDXGIAdapter*> adapters;                           //アダプターの列挙用
	
	//フレームリソース
	vector<FrameResource> frameResources;

	//シャドウマップのサイズ
	const XMFLOAT2 NormalShadowSize = { 1024, 1024 };
	//ノーマルシャドウマップ用
	RenderTargetViewResource* NormalShadowRtvResource=nullptr;

	//カスケードシャドウマップの数
	static const int CascadeShadowMapNum = 3;
	//シャドウマップのサイズ
	const XMFLOAT2 shadowSize[CascadeShadowMapNum] = { {2028, 2028}, { 1024,1024 }, { 512,512 } };
	//シャドウクリップエリア
	const float shadowArea[CascadeShadowMapNum] = { 100,300,600 };
	//カスケードシャドウマップ用
	std::vector<RenderTargetViewResource*> cascadeShadowRtvResource;


	HWND hWnd = {};
	UINT windowWidth = 0;
	UINT windowHeight = 0;
	UINT fenceVal = 0;
	UINT buffer_count = 0;

	//テクスチャリソース
	map<string, ID3D12Resource*> textureResourceTable;
};