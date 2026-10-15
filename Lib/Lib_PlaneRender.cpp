#include "Lib_PlaneRenderer.h"
#include "Lib_Graphics.h"
#include "Lib_Descriptor.h"
#include "Lib_Texture.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"


PlaneRenderer::PlaneRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS, const int createNum) :SpriteShader(graphics, shaderVS, shaderPS, createNum)
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

	//ディスクリプタテーブル
	D3D12_DESCRIPTOR_RANGE descRange[2] = {};
	memset(&descRange, 0, sizeof(descRange));
	{
		descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//テクスチャタイプ
		descRange[0].NumDescriptors = 1;//テクスチャは１つ
		descRange[0].BaseShaderRegister = 0;//0番目に登録
		descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		// b0
		descRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// レンジ種別
		descRange[1].NumDescriptors = 1;							// ディスクリプタ数
		descRange[1].BaseShaderRegister = 0;						// 先頭レジスタ番号
		descRange[1].RegisterSpace = 0;								// つじつまを合わせるためのスペース
		descRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//ルートパラメータ(ピクセルシェイダー)
		memset(&rootParm[0], 0, sizeof(rootParm[0]));
		rootParm[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//ここでディスクリプタテーブルにする
		rootParm[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェイダーに見えるようにする。
		rootParm[0].DescriptorTable.pDescriptorRanges = &descRange[0];
		rootParm[0].DescriptorTable.NumDescriptorRanges = 1;


		rootParm[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		rootParm[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// どのシェーダーから利用可能か
		rootParm[1].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		rootParm[1].DescriptorTable.pDescriptorRanges = &descRange[1];	// ディスクリプタレンジのアドレス
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.pParameters = rootParm;//ルートパラメーター設定！
	rootSignatureDesc.NumParameters = 2;
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

	desc.DepthStencilState.DepthEnable = true;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.StencilEnable = false;

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

void PlaneRenderer::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	//カメラのコンスタントバッファ設定
	commandList->SetGraphicsRootDescriptorTable(1, graphics->GetCameraDescriptor()->GetGpuHandle());

}


//頂点の設定
void PlaneRenderer::SetVertex(Graphics* graphics, Texture* texture, XMFLOAT4X4 mat, XMFLOAT4 cut, XMFLOAT4 color)
{
	if (renderNum >= GetCreateNum())assert(!L"最大数を達しました");
	XMFLOAT4 Cut;
	Cut.x = cut.x / texture->GetSize().x;
	Cut.y = cut.y / texture->GetSize().y;
	Cut.z = cut.z / texture->GetSize().x;
	Cut.w = cut.w / texture->GetSize().y;

	const  XMFLOAT2 halfPos = { 0.5f,0.5f };
	Vertex polygonVertexList[4] = {};
	//左上
	polygonVertexList[0].position = { -halfPos.x,-halfPos.y,0 };
	polygonVertexList[0].texcoord = { Cut.x,Cut.w };
	polygonVertexList[0].color = color;

	//右上  
	polygonVertexList[1].position = { halfPos.x,-halfPos.y,0 };
	polygonVertexList[1].texcoord = { Cut.z,Cut.w };
	polygonVertexList[1].color = color;

	//左下  
	polygonVertexList[2].position = { -halfPos.x,halfPos.y,0 };
	polygonVertexList[2].texcoord = { Cut.x,Cut.y };
	polygonVertexList[2].color = color;

	//右下  
	polygonVertexList[3].position = { halfPos.x,halfPos.y,0 };
	polygonVertexList[3].texcoord = { Cut.z,Cut.y };
	polygonVertexList[3].color = color;


	//行列作成
	Vertex* v = vertMap + (renderNum * 4);


	for (int i = 0; i < ARRAYSIZE(polygonVertexList); i++)
	{
		XMVECTOR Pos = XMLoadFloat3(&polygonVertexList[i].position);
		XMMATRIX Mat = XMLoadFloat4x4(&mat);
		Pos = XMVector3Transform(Pos, Mat);
		XMStoreFloat3(&v[i].position, Pos);
		v[i].color = polygonVertexList[i].color;
		v[i].texcoord = polygonVertexList[i].texcoord;
	}

	renderNum++;

	renderTextureList.push_back(texture);
}

void PlaneRenderer::Render(Graphics* graphics, int index)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();

	// ディスクリプタテーブル



	auto heapHandle = renderTextureList.at(index)->GetDescriptor()->GetGpuHandle();
	commandList->SetGraphicsRootDescriptorTable(0, heapHandle);


	commandList->DrawIndexedInstanced(6, 1, index * 6, 0, 0);

	//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

