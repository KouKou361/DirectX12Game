#include "Lib_PointSpriteRenderer.h"
#include "Lib_Graphics.h"
#include "Lib_ModelResource.h"
#include "Lib_Model.h"
#include "Lib_ConstantBuffer.h"
#include "Lib_GeometryVertexVecter.h"

Effect3DGeometryRenderer::Effect3DGeometryRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS) :GeometryPointShader(graphics, shaderVS, shaderGS, shaderPS)
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
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	}

	// ディスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE d3d_descriptor_ranges[4] = {};
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
	}

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER d3d_root_parameters[4] = {};
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
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.NumParameters = _countof(d3d_root_parameters);
	rootSignatureDesc.pParameters = d3d_root_parameters;
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;

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
		{ "POSITION",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD1_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	
		{ "TEXCOORD2_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD3_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD4_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD5_",		0,DXGI_FORMAT_R32G32B32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD6_",		0,DXGI_FORMAT_R32G32B32A32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD7_",		0,DXGI_FORMAT_R32G32B32_FLOAT	,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },

		{ "TANGENT",		0,DXGI_FORMAT_R32G32B32A32_FLOAT	,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "COLOR",			0,DXGI_FORMAT_R32G32B32A32_FLOAT	,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },

		{ "TEXCOORD8_",		0,DXGI_FORMAT_R32_FLOAT		,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	memset(&desc, 0, sizeof(desc));
	//シェーダー
	desc.VS.pShaderBytecode = vsShader->GetBufferPointer();
	desc.VS.BytecodeLength = vsShader->GetBufferSize();
	desc.PS.pShaderBytecode = psShader->GetBufferPointer();
	desc.PS.BytecodeLength = psShader->GetBufferSize();
	desc.GS.pShaderBytecode = gsShader->GetBufferPointer();
	desc.GS.BytecodeLength = gsShader->GetBufferSize();
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

	desc.BlendState.AlphaToCoverageEnable = true;
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
	//ポイントで構成
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
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

void Effect3DGeometryRenderer::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//カメラのコンスタントバッファ設定
	commandList->SetGraphicsRootDescriptorTable(0, graphics->GetCameraDescriptor()->GetGpuHandle());

}

void Effect3DGeometryRenderer::Render(Graphics* graphics, GeometryVertexVector* geometryVertexVecter)
{

	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = geometryVertexVecter->GetVertexBufferView();

	auto heapHandle = geometryVertexVecter->GetDescriptor()->GetGpuHandle();
	commandList->SetGraphicsRootDescriptorTable(3, heapHandle);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->DrawIndexedInstanced(geometryVertexVecter->GetVertices().size(), 1, 0, 0, 0);
}

void Effect3DGeometryRenderer::End(Graphics* graphics)
{
	//geometryVertexVecterList.clear();
}
