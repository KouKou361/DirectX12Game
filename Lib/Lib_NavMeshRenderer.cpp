#include "Lib_NavMeshRenderer.h"
#include "Lib_Graphics.h"
#include "Lib_ModelResource.h"
#include "Lib_Model.h"
#include "Lib_ConstantBuffer.h"
#include "Lib_3DDebug.h"
#include "Lib.h"

#include <d3dx12.h>
#include <algorithm>
#include <d3dcompiler.h>

#include <DebugDraw.h>
#include <DetourDebugDraw.h>
#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>




void Lib_NavModelRenderer::SetVertex(Graphics* graphics, const float* pos)
{

	XMFLOAT3 vertex;

	vertex.x = pos[0];
	vertex.y = pos[1];
	vertex.z = pos[2];



	polygonVertexList.emplace_back(vertex);

	// クアッドの場合はトライアングルリストで表示するための残り２点を追加
	//if (quadCount >= 0)
	//{
	//	quadCount++;
	//	if (quadCount == 4)
	//	{
	//		Vertex* v = &vertex - 3;
	//		polygonVertexList.emplace_back(v[0]);
	//		
	//
	//		quadCount = 0;
	//		//primitive.vertexCount += 2;
	//	}
	//}
	//
	//polygonVertexList.emplace_back(vertex);

}

GeometryDebugNav* Lib_NavModelRenderer::SetUnUsedRegisterNavList()
{
	for (int i = 0; i < navModelList.size(); i++)
	{
		GeometryDebugNav* nav = navModelList.at(i);
		if (nav->renderUse == true)continue;
		return nav;
	}

	//assert(!L"デバック描画サイズがいっぱいです。");
	return nullptr;
}

void Lib_NavModelRenderer::drawMeshTile(Graphics* graphics, const dtNavMesh& mesh, const dtNavMeshQuery* query,
	const dtMeshTile* tile, unsigned char flags, XMFLOAT3 targetPos, float targetRadius)
{

	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);
	const unsigned int tileColor = duIntToCol(tileNum, 128);

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];


		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
			for (int k = 0; k < 3; ++k)
			{
				if (t[k] < p->vertCount)
				{
					SetVertex(graphics, &tile->verts[p->verts[t[k]] * 3]);
				}

				else
				{
					SetVertex(graphics,&tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3]);
				}

			}

			GeometryDebugNav* nav = SetUnUsedRegisterNavList();
			if (nav == nullptr)
			{
				polygonVertexList.clear();
				return;
			}


			VECTOR3 sumPos;
			for (int i = 0; i < polygonVertexList.size(); i++)
			{
				sumPos.x += polygonVertexList.at(i).x;
				sumPos.y += polygonVertexList.at(i).y;
				sumPos.z += polygonVertexList.at(i).z;
			}
			sumPos.x = sumPos.x / polygonVertexList.size();
			sumPos.y = sumPos.y / polygonVertexList.size();
			sumPos.z = sumPos.z / polygonVertexList.size();

			if (TK_Lib::Collision::CircleVsCircle({targetPos.x,targetPos.y,targetPos.z}, targetRadius, sumPos, 1) == FALSE)
			{
				polygonVertexList.clear();
				continue;
			}

			nav->SetVertex(graphics, polygonVertexList);
			nav->renderUse = TRUE;

			polygonVertexList.clear();
		}
		//for (int n = 0; n < polygonVertexList.size() - 1; n++)
		//{
		//	
		//	GeometryDebugNav* nav = SetUnUsedRegisterNavList();
		//	if (nav == nullptr)
		//	{
		//		polygonVertexList.clear();
		//		return;
		//	}
		//	nav->SetVertex(graphics, polygonVertexList);
		//	nav->renderUse = TRUE;
		//
		//	//TK_Lib::Debug3D::Line(
		//	//	{ polygonVertexList.at(n).position.x, polygonVertexList.at(n).position.y, polygonVertexList.at(n).position.z, },
		//	//	{ polygonVertexList.at(n+1).position.x, polygonVertexList.at(n + 1).position.y, polygonVertexList.at(n + 1).position.z, });
		//}
		//
		//GeometryDebugNav* nav = SetUnUsedRegisterNavList();
		//if (nav == nullptr)
		//{
		//	polygonVertexList.clear();
		//	return;
		//}
		//nav->SetVertex(graphics, polygonVertexList);
		//nav->renderUse = TRUE;
		//
		//polygonVertexList.clear();
	}

}



void Lib_NavModelRenderer::duDebugDrawNavMeshWithClosedList(Graphics* graphics, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags, XMFLOAT3 targetPos, float targetRadius)
{

	const dtNavMeshQuery* q = (flags & DU_DRAWNAVMESH_CLOSEDLIST) ? &query : 0;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTile(graphics,mesh, q, tile, flags, targetPos, targetRadius);
	}
}


void Lib_NavModelRenderer::NavRender(Graphics* graphics, dtNavMesh* navMesh, dtNavMeshQuery* navQuery, XMFLOAT3 targetPos, float targetRadius)
{
	if (navMesh != nullptr && navQuery != nullptr)
	{
		unsigned char navMeshDrawFlags = DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST;

		// ナビメッシュ描画
		duDebugDrawNavMeshWithClosedList(graphics ,*navMesh, *navQuery, navMeshDrawFlags, targetPos, targetRadius);
	}


}



Lib_NavModelRenderer::Lib_NavModelRenderer(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS) 
{
	navModelList.resize(3000);
	for (int i = 0; i < navModelList.size(); i++)
	{
		GeometryDebugNav* nav = new GeometryDebugNav(graphics);
		navModelList.at(i) = nav;
	}

	ID3D12Device* device = graphics->GetDevice();

	LoadShader(graphics, shaderVS,shaderPS);

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
	D3D12_DESCRIPTOR_RANGE d3d_descriptor_ranges[2] = {};
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
		
	}

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER d3d_root_parameters[2] = {};
	{
		d3d_root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// どのシェーダーから利用可能か
		d3d_root_parameters[0].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[0].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[0];	// ディスクリプタレンジのアドレス

		d3d_root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// パラメータ種別
		d3d_root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// どのシェーダーから利用可能か
		d3d_root_parameters[1].DescriptorTable.NumDescriptorRanges = 1;							// ディスクリプタレンジ数
		d3d_root_parameters[1].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[1];	// ディスクリプタレンジのアドレス
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

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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

	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
	if (hr != S_OK)assert(!L"グラフィックスパイプライン生成のエラー");


	
	//hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&linePipelineState));
	//if (hr != S_OK)assert(!L"グラフィックスパイプライン生成のエラー");

	navWorldConstantBuffer = new Lib_ConstantBuffer(graphics, sizeof(NavWorld), &navWorld);
	


}

Lib_NavModelRenderer::~Lib_NavModelRenderer()
{
	for (int i = 0; i < navModelList.size(); i++)
	{
		GeometryDebugNav* nav = navModelList.at(i);
		if (nav != nullptr)
		{
			delete nav;
			nav = nullptr;
		}
	}

	if (navWorldConstantBuffer != nullptr)
	{
		delete navWorldConstantBuffer;
		navWorldConstantBuffer = nullptr;
	}

	
}

void Lib_NavModelRenderer::LoadShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS)
{

	HRESULT hr = S_OK;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダーの作成
	hr = D3DCompileFromFile(
		shaderVS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}


	//ピクセルシェーダーの作成
	hr = D3DCompileFromFile(
		shaderPS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}
}

void Lib_NavModelRenderer::Begin(Graphics* graphics)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	// パイプライン設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//カメラのコンスタントバッファ設定
	commandList->SetGraphicsRootDescriptorTable(0, graphics->GetCameraDescriptor()->GetGpuHandle());

	
	commandList->SetGraphicsRootDescriptorTable(1, navWorldConstantBuffer->GetDescripter()->GetGpuHandle());

}

void Lib_NavModelRenderer::Render(Graphics* graphics, GeometryDebugNav* GeometryDebug)
{
	ID3D12GraphicsCommandList* commandList = graphics->GetCommandList();
	//GeometryDebug->SetVertex(graphics, pos, scale,  color, angle);
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = GeometryDebug->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW	 indexBufferView = GeometryDebug->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	commandList->SetPipelineState(pipelineState.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->DrawIndexedInstanced(GeometryDebug->GetVertexCopunt(), 1, 0, 0, 0);

}

void Lib_NavModelRenderer::End(Graphics* graphics)
{
	for (int i = 0; i < navModelList.size(); i++)
	{
		GeometryDebugNav* debugNav = navModelList.at(i);
		if (navModelList.at(i)->renderUse == false)break;
		debugNav->renderUse = false;
	}
	polygonVertexList.clear();

}
void Lib_NavModelRenderer::Register(Graphics* graphics,dtNavMesh* navMesh, dtNavMeshQuery* navQuery, DirectX::XMFLOAT4X4 world, XMFLOAT3 targetPos,float targetRadius)
{
	navWorld.world = world;
	navWorldConstantBuffer->CopyToVRAM(graphics, &navWorld);

	NavRender(graphics,navMesh,navQuery, targetPos, targetRadius);
}

void Lib_NavModelRenderer::AllRender(Graphics* graphics)
{
	Begin(graphics);

	for (int i = 0; i < navModelList.size(); i++)
	{
		if (navModelList.at(i)->renderUse == false)break;
		Render(graphics, navModelList.at(i));
	}

	End(graphics);


}

