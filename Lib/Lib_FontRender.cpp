#include "Lib_FontRender.h"
#include "Lib_Graphics.h"
#include "Lib_Sprite.h"
#include "Lib_Descriptor.h"
#include "Lib_Texture.h"
FontRenderer::FontRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS, const int createNum) :SpriteShader(graphics, shaderVS, shaderPS, createNum)
{
	ID3D12Device* device = graphics->GetDevice();



	ID3D10Blob* rootSigBlob = nullptr;

	//サンプラー
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	{
		memset(&samplerDesc, 0, sizeof(samplerDesc));
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MinLOD = D3D12_FLOAT32_MAX;
		samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	}

	//ディスクリプタテーブル
	D3D12_DESCRIPTOR_RANGE descRange[1] = {};
	memset(&descRange, 0, sizeof(descRange));
	{
		descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//テクスチャタイプ
		descRange[0].NumDescriptors = 1;//テクスチャは１つ
		descRange[0].BaseShaderRegister = 0;//0番目に登録
		descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//ルートパラメータ(ピクセルシェイダー)
		memset(&rootParm[0], 0, sizeof(rootParm[0]));
		rootParm[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//ここでディスクリプタテーブルにする
		rootParm[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェイダーに見えるようにする。
		rootParm[0].DescriptorTable.pDescriptorRanges = &descRange[0];
		rootParm[0].DescriptorTable.NumDescriptorRanges = 1;
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.pParameters = rootParm;//ルートパラメーター設定！
	rootSignatureDesc.NumParameters = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* errorBlob = nullptr;

	//バイナリコードの作成
	hr = D3D12SerializeRootSignature(&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);
	if (hr != S_OK)assert(!L"バイナリコード生成のエラー");


	//ルートシグネテャの作成
	hr = device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	if (hr != S_OK)assert(!L"ルートシグネチャ生成のエラー");

	rootSigBlob->Release();


	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {

		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT   ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	memset(&desc, 0, sizeof(desc));
	//シェーダー
	desc.VS.pShaderBytecode = vsShader->GetBufferPointer();
	desc.VS.BytecodeLength = vsShader->GetBufferSize();
	desc.PS.pShaderBytecode = psShader->GetBufferPointer();
	desc.PS.BytecodeLength = psShader->GetBufferSize();
	//サンプルマスク
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//ラスタライザ
	desc.RasterizerState.MultisampleEnable = false;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.DepthClipEnable = true;
	//ブレンドステート
	desc.BlendState.AlphaToCoverageEnable = false;
	desc.BlendState.IndependentBlendEnable = false;
	desc.BlendState.RenderTarget[0].BlendEnable = true;
	desc.BlendState.RenderTarget[0].LogicOpEnable = false;
	desc.BlendState.RenderTarget[0].SrcBlend        = D3D12_BLEND_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].DestBlend       = D3D12_BLEND_INV_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].BlendOp         = D3D12_BLEND_OP_ADD;
	desc.BlendState.RenderTarget[0].SrcBlendAlpha   = D3D12_BLEND_ONE;
	desc.BlendState.RenderTarget[0].DestBlendAlpha  = D3D12_BLEND_ZERO;
	desc.BlendState.RenderTarget[0].BlendOpAlpha    = D3D12_BLEND_OP_ADD;
	desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = _countof(inputLayout);

	desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	//三角形で構成
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//レンダーターゲット
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//アンチエイリアシング
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.pRootSignature = rootSignature.Get();


	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	if (hr != S_OK)assert(!L"グラフィックスパイプライン生成のエラー");


}

void FontRenderer::Render(Graphics* graphics, int index)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();

	// ディスクリプタテーブル


	auto heapHandle = renderTextureList.at(index)->GetDescriptor()->GetGpuHandle();
	commandList->SetGraphicsRootDescriptorTable(0, heapHandle);

	commandList->DrawIndexedInstanced(6, 1, index * 6, 0, 0);
}
