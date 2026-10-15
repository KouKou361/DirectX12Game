#include "Lib_ShadowRenderer.h"
#include "Lib_Graphics.h"
#include "Lib_ModelResource.h"
#include "Lib_Model.h"
#include "Lib_ConstantBuffer.h"

ShadowRenderer::ShadowRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS) :ModelShader(graphics, shaderVS, shaderPS)
{
	ID3D12Device* device = graphics->GetDevice();



	ID3D10Blob* rootSigBlob = nullptr;

	//サンプラー
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	{
		memset(&samplerDesc[0], 0, sizeof(samplerDesc[0]));
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//折り返し
		samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[0].MinLOD = 0.0f;
		samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[0].ShaderRegister = 0;

		memset(&samplerDesc[1], 0, sizeof(samplerDesc[1]));

		samplerDesc[1] = samplerDesc[0];
		samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//折り返し
		samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//折り返し
		samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//折り返し
		samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[1].MaxAnisotropy = 1;
		samplerDesc[1].ShaderRegister = 1;
	}

	// ディスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE d3d_descriptor_ranges[5] = {};
	{
		// b0
		d3d_descriptor_ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// レンジ種別
		d3d_descriptor_ranges[0].NumDescriptors = 1;							// ディスクリプタ数
		d3d_descriptor_ranges[0].BaseShaderRegister = 0;						// 先頭レジスタ番号
		d3d_descriptor_ranges[0].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		d3d_descriptor_ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		// b1
		d3d_descriptor_ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// レンジ種別
		d3d_descriptor_ranges[1].NumDescriptors = 1;							// ディスクリプタ数
		d3d_descriptor_ranges[1].BaseShaderRegister = 1;						// 先頭レジスタ番号
		d3d_descriptor_ranges[1].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		d3d_descriptor_ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		// b2
		d3d_descriptor_ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// レンジ種別
		d3d_descriptor_ranges[2].NumDescriptors = 1;							// ディスクリプタ数
		d3d_descriptor_ranges[2].BaseShaderRegister = 2;						// 先頭レジスタ番号
		d3d_descriptor_ranges[2].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		d3d_descriptor_ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		// t0
		d3d_descriptor_ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// レンジ種別
		d3d_descriptor_ranges[3].NumDescriptors = 1;							// ディスクリプタ数
		d3d_descriptor_ranges[3].BaseShaderRegister = 0;						// 先頭レジスタ番号
		d3d_descriptor_ranges[3].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		d3d_descriptor_ranges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// t1 シャドウマップ用
		d3d_descriptor_ranges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// レンジ種別
		d3d_descriptor_ranges[4].NumDescriptors = 1;							// ディスクリプタ数
		d3d_descriptor_ranges[4].BaseShaderRegister = 1;						// 先頭レジスタ番号
		d3d_descriptor_ranges[4].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		d3d_descriptor_ranges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER d3d_root_parameters[5] = {};
	{
		d3d_root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				// どのシェーダーから利用可能か
		d3d_root_parameters[0].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[0].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[0];	// ディスクリプタレンジのアドレス

		d3d_root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// どのシェーダーから利用可能か
		d3d_root_parameters[1].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[1].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[1];	// ディスクリプタレンジのアドレス

		d3d_root_parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// どのシェーダーから利用可能か
		d3d_root_parameters[2].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[2].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[2];	// ディスクリプタレンジのアドレス

		d3d_root_parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// どのシェーダーから利用可能か
		d3d_root_parameters[3].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[3].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[3];	// ディスクリプタレンジのアドレス

		d3d_root_parameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// どのシェーダーから利用可能か
		d3d_root_parameters[4].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[4].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[4];	// ディスクリプタレンジのアドレス
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.NumParameters = _countof(d3d_root_parameters);
	rootSignatureDesc.pParameters = d3d_root_parameters;
	rootSignatureDesc.NumStaticSamplers = _countof(samplerDesc);
	rootSignatureDesc.pStaticSamplers = samplerDesc;

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

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.DepthClipEnable = true;
	//深度情報
	//desc.DepthStencilState.DepthEnable = true;
	//desc.DepthStencilState.DepthWriteMask=D3D12_DEPTH_WRITE_MASK_ALL;
	//desc.DepthStencilState.DepthFunc=D3D12_COMPARISON_FUNC_LESS;

	desc.DepthStencilState.DepthEnable = true;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.StencilEnable = false;
	//ブレンドステート
	//desc.BlendState.AlphaToCoverageEnable = true;
	//desc.BlendState.IndependentBlendEnable = false;
	//D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	//{
	//	memset(&renderTargetBlendDesc, 0, sizeof(renderTargetBlendDesc));
	//	renderTargetBlendDesc.BlendEnable = false;
	//	renderTargetBlendDesc.LogicOpEnable = false;
	//	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//}
	//desc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.BlendState.IndependentBlendEnable = false;
	desc.BlendState.RenderTarget[0].BlendEnable = true;
	desc.BlendState.RenderTarget[0].LogicOpEnable = false;
	desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = _countof(inputLayout);

	desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	//三角形で構成
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//レンダーターゲット
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.DSVFormat = DepthStencilFormat;
	//アンチエイリアシング
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.pRootSignature = rootSignature.Get();


	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	if (hr != S_OK)assert(!L"グラフィックスパイプライン生成のエラー");


}

void ShadowRenderer::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//カメラのコンスタントバッファ設定
	commandList->SetGraphicsRootDescriptorTable(0, graphics->GetCameraDescriptor()->GetGpuHandle());

	// メッシュ定数バッファ設定
	commandList->SetGraphicsRootDescriptorTable(4, graphics->GetShadowMapDescriptor()->GetGpuHandle());
}

void ShadowRenderer::Render(Graphics* graphics, Model* model)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();


	for (Model::Mesh& mesh : model->GetMeshes())
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		Model::FrameResource& frame_resource = mesh.frame_resources.at(graphics->GetCurrentBufferIndex());

		// メッシュ定数バッファ設定
		commandList->SetGraphicsRootDescriptorTable(1, frame_resource.constantBuffer->GetDescripter()->GetGpuHandle());

		// 頂点バッファ設定
		commandList->IASetVertexBuffers(0, 1, &res_mesh->d3d_vbv);
		commandList->IASetIndexBuffer(&res_mesh->d3d_ibv);

			// サブセット
		for (const ModelResource::Subset& subset : res_mesh->subsets)
		{
			const ModelResource::MaterialAlbedo* materialAlbedo = subset.materialAlbedo;

			// マテリアル定数バッファ設定
			commandList->SetGraphicsRootDescriptorTable(2, materialAlbedo->constantBuffer->GetDescripter()->GetGpuHandle());

			// シェーダーリソースビュー設定
			commandList->SetGraphicsRootDescriptorTable(3, materialAlbedo->shaderResourceViewDescriptor->GetGpuHandle());

			// 描画
			commandList->DrawIndexedInstanced(subset.indexCount, 1, subset.startIndex, 0, 0);
		}
	}
}

