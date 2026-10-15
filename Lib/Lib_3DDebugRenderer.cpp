#include "Lib_3DDebugRenderer.h"
#include "Lib_Graphics.h"
GeometryDebugRenderer::GeometryDebugRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS) :Shader(shaderVS, shaderPS)
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
	D3D12_DESCRIPTOR_RANGE descRange[1] = {};
	memset(&descRange, 0, sizeof(descRange));
	{
		descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//テクスチャタイプ
		descRange[0].NumDescriptors = 1;//テクスチャは１つ
		descRange[0].BaseShaderRegister = 0;//0番目に登録
		descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//ルートパラメータ(ピクセルシェイダー)
		memset(&rootParm[0], 0, sizeof(rootParm[0]));
		rootParm[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//ここでディスクリプタテーブルにする
		rootParm[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//ピクセルシェイダーに見えるようにする。
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
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
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
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
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
	

	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&linePipelineState));
	if (hr != S_OK)assert(!L"グラフィックスパイプライン生成のエラー");


	boxList.resize(50);
	for (int i = 0; i < boxList.size(); i++)
	{
		GeometryDebugBox* box = new GeometryDebugBox(graphics);
		boxList.at(i) = box;
	}

	lineList.resize(1000);
	for (int i = 0; i < lineList.size(); i++)
	{
		GeometryDebugLine* line = new GeometryDebugLine(graphics);
		lineList.at(i) = line;
	}

	cylinderList.resize(100);
	for (int i = 0; i < cylinderList.size(); i++)
	{
		GeometryDebugCylinder* cylinder = new GeometryDebugCylinder(graphics);
		cylinderList.at(i) = cylinder;
	}

	circleList.resize(10);
	for (int i = 0; i < circleList.size(); i++)
	{
		GeometryDebugCircle* circle = new GeometryDebugCircle(graphics);
		circleList.at(i) = circle;
	}
}

GeometryDebugRenderer::~GeometryDebugRenderer()
{
	for (int i = 0; i < boxList.size(); i++)
	{
		GeometryDebugBox* box = boxList.at(i);
		if (box != nullptr)
		{
			delete box;
			box = nullptr;
		}
	}
	for (int i = 0; i < lineList.size(); i++)
	{
		GeometryDebugLine* line = lineList.at(i);
		if (line != nullptr)
		{
			delete line;
			line = nullptr;
		}
	}



	for (int i = 0; i < circleList.size(); i++)
	{
		GeometryDebugCircle* circle = circleList.at(i);
		if (circle != nullptr)
		{
			delete circle;
			circle = nullptr;
		}
	}

	for (int i = 0; i < cylinderList.size(); i++)
	{

		GeometryDebugCylinder* cylinder = cylinderList.at(i);
		if (cylinder != nullptr)
		{
			delete cylinder;
			cylinder = nullptr;
		}
	}
}

void GeometryDebugRenderer::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//カメラのコンスタントバッファ設定
	commandList->SetGraphicsRootDescriptorTable(0, graphics->GetCameraDescriptor()->GetGpuHandle());
}

void GeometryDebugRenderer::Render(Graphics* graphics, GeometryDebug* GeometryDebug)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	//GeometryDebug->SetVertex(graphics, pos, scale,  color, angle);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = GeometryDebug->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW	 indexBufferView = GeometryDebug->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	switch (GeometryDebug->GetGeometryType())
	{
	case GeometryDebugType::Geometry_DEBUG_CIRCLE:

		commandList->DrawIndexedInstanced(GeometryDebug->GetIndicesSize(), 1, 0, 0, 0);

		break;
	case GeometryDebugType::Geometry_DEBUG_LINE:
		commandList->SetPipelineState(linePipelineState.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		commandList->DrawIndexedInstanced(2, 1, 0, 0, 0);

		commandList->SetPipelineState(pipelineState.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	case GeometryDebugType::Geometry_DEBUG_BOX:
		commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
		break;
	case GeometryDebugType::Geometry_DEBUG_CYLINDER:
		commandList->DrawIndexedInstanced(120, 1, 0, 0, 0);
		break;
	default:
		assert(!L"未登録の描画エラー");
		break;
	}

}

void GeometryDebugRenderer::End(Graphics* graphics)
{
	//renderList.clear();

	for (int i = 0; i < circleList.size(); i++)
	{
		GeometryDebugCircle* circle = circleList.at(i);
		circle->renderUse=false;
	}

	for (int i = 0; i < lineList.size(); i++)
	{
		GeometryDebugLine* line = lineList.at(i);
		line->renderUse = false;
	}

	for (int i = 0; i < cylinderList.size(); i++)
	{

		GeometryDebugCylinder* cylinder = cylinderList.at(i);
		cylinder->renderUse = false;
	}

	for (int i = 0; i < boxList.size(); i++)
	{
		GeometryDebugBox* box = boxList.at(i);
		box->renderUse = false;
	}
}

void GeometryDebugRenderer::AllRender(Graphics* graphics)
{
	Begin(graphics);
	for (int i = 0; i < circleList.size(); i++)
	{
		GeometryDebugCircle* circle = circleList.at(i);
		if (circle->renderUse == false)continue;
		Render(graphics, circle);
	}

	for (int i = 0; i < boxList.size(); i++)
	{
		GeometryDebugBox* box = boxList.at(i);
		if (box->renderUse == false)continue;
		Render(graphics, box);
	}


	for (int i = 0; i < lineList.size(); i++)
	{
		GeometryDebugLine* line = lineList.at(i);
		if (line->renderUse == false)continue;
		Render(graphics, line);
	}

	for (int i = 0; i < cylinderList.size(); i++)
	{
		GeometryDebugCylinder* cylinder = cylinderList.at(i);
		if (cylinder->renderUse == false)continue;
		Render(graphics, cylinder);
	}

	


	End(graphics);
}

void GeometryDebugRenderer::Register(Graphics* graphics, const GeometryDebugRender &GeometryDebugRender)
{
	GeometryDebug* GeometryDebug = nullptr;
	switch (GeometryDebugRender.type)
	{
	case Geometry_DEBUG_BOX    :
		GeometryDebug=SetUnUsedRegisterBoxList();
		GeometryDebug->SetVertex(graphics,GeometryDebugRender.pos, GeometryDebugRender.pos2, GeometryDebugRender.color);
		break;
	case Geometry_DEBUG_CIRCLE :
		GeometryDebug = SetUnUsedRegisterCircleList();
		GeometryDebug->SetVertex(graphics, GeometryDebugRender.pos, GeometryDebugRender.pos2, GeometryDebugRender.color);
		break;
	case Geometry_DEBUG_LINE	 :
		GeometryDebug = SetUnUsedRegisterLineList();
		GeometryDebug->SetVertex(graphics, GeometryDebugRender.pos, GeometryDebugRender.pos2, GeometryDebugRender.color);
		break;
	case Geometry_DEBUG_CYLINDER:
		GeometryDebug = SetUnUsedRegisterCylinderList();
		GeometryDebug->SetVertex(graphics, GeometryDebugRender.pos, GeometryDebugRender.pos2, GeometryDebugRender.color);
		break;
	default:
		assert(!L"未定のデバック用リストに追加しようとしました。");
		break;
	}

	GeometryDebug->renderUse = true;
	
}

GeometryDebugCircle* GeometryDebugRenderer::SetUnUsedRegisterCircleList()
{
	for (int i = 0; i < circleList.size(); i++)
	{
		GeometryDebugCircle* circle = circleList.at(i);
		if (circle->renderUse == true)continue;
		return circle;
	}

	assert(!L"デバック描画サイズがいっぱいです。");
}
GeometryDebugLine* GeometryDebugRenderer::SetUnUsedRegisterLineList()
{
	for (int i = 0; i < lineList.size(); i++)
	{
		GeometryDebugLine* line = lineList.at(i);
		if (line->renderUse == true)continue;
		return line;
	}

	assert(!L"デバック描画サイズがいっぱいです。");
}

GeometryDebugCylinder* GeometryDebugRenderer::SetUnUsedRegisterCylinderList()
{
	for (int i = 0; i < cylinderList.size(); i++)
	{
		GeometryDebugCylinder* cylinder = cylinderList.at(i);
		if (cylinder->renderUse == true)continue;
		return cylinder;
	}

	assert(!L"デバック描画サイズがいっぱいです。");
}

GeometryDebugBox* GeometryDebugRenderer::SetUnUsedRegisterBoxList()
{
	for (int i = 0; i < boxList.size(); i++)
	{
		GeometryDebugBox* box = boxList.at(i);
		if (box->renderUse == true)continue;
		return box;
	}

	assert(!L"デバック描画サイズがいっぱいです。");
}