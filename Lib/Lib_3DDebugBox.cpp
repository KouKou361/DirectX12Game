#include "Lib_3DDebug.h"
#include "d3dx12.h"
#include "Lib_Graphics.h"

GeometryDebugBox::GeometryDebugBox(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);

	geometryDebugType = GeometryDebugType::Geometry_DEBUG_BOX;
}

//頂点バッファ作成
void GeometryDebugBox::CreateVertex(Graphics* graphics)
{
	polygonVertexList.resize(36);


	ID3D12Device* device = graphics->GetDevice();

	CD3DX12_RESOURCE_DESC rscDesc = {};
	auto rscDescBuffer = rscDesc.Buffer(sizeof(polygonVertexList[0]) * polygonVertexList.size());
	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	hr = device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&rscDescBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexResource.GetAddressOf())
	);

	if (hr != S_OK)assert(!L"頂点バッファのエラー");


	//頂点バッファビュー
	memset(&vertexBufferView, 0, sizeof(vertexBufferView));
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(polygonVertexList[0]) * polygonVertexList.size();
	vertexBufferView.StrideInBytes = sizeof(polygonVertexList[0]);


	hr = vertexResource->Map(0, nullptr, (void**)&vertMap);
	if (hr != S_OK)assert(!L"頂点コピーのエラー");
}
//インデックスバッファ作成
void GeometryDebugBox::CreateIndex(Graphics* graphics)
{

	//インデックスバッファの作成

	u_int Indicies[] = {

		//前面
		1,0,2,
		1,2,3,
		//上面
		5,4,7,
		4,6,7,
		//後面
		10,8,9,
		11,10,9,
		//下面
		12,13,15,
		15,14,12,
		////右
		19,17,16,
		16,18,19,
		////右
		20,21,23,
		23,22,20,

	};
	u_int Index_count = (sizeof(Indicies)) / sizeof(Indicies[0]);


	indices.resize(Index_count);



	ID3D12Device* device = graphics->GetDevice();

	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC rsrDesc;
	auto desc = rsrDesc.Buffer(sizeof(UINT) * indices.size());

	hr = device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexResource.GetAddressOf())
	);

	if (hr != S_OK)assert(!L"インデックスバッファのエラー");


	//インデックスビューの作成
	memset(&indexBufferView, 0, sizeof(indexBufferView));
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = static_cast<UINT>(desc.Width);

	// インデックスデータ設定
	UINT* index;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&index));

	for (int i = 0; i < indices.size(); i++)
	{
		index[i] = Indicies[i];
	}

	indexResource->Unmap(0, nullptr);
}


//頂点の設定
void GeometryDebugBox::SetVertex(Graphics* graphics, const XMFLOAT3& pos, const XMFLOAT3& scale,
	const XMFLOAT4& color)
{


	Vertex vertices2[] = {
		//前
			{ DirectX::XMFLOAT3(pos.x, scale.y,pos.z), DirectX::XMFLOAT3(0,0,-1),color},
			{ DirectX::XMFLOAT3(scale.x,scale.y,pos.z), DirectX::XMFLOAT3(0,0,-1),color},
			{ DirectX::XMFLOAT3(pos.x, pos.y, pos.z), DirectX::XMFLOAT3(0,0,-1),color},
			{ DirectX::XMFLOAT3(scale.x,pos.y, pos.z), DirectX::XMFLOAT3(0,0,-1),color},
			//上							  								 
			{ DirectX::XMFLOAT3(pos.x, scale.y,scale.z),DirectX::XMFLOAT3(0,1,0) ,color},
			{ DirectX::XMFLOAT3(scale.x,scale.y,scale.z),DirectX::XMFLOAT3(0,1,0) ,color},
			{ DirectX::XMFLOAT3(pos.x, scale.y,pos.z), DirectX::XMFLOAT3(0,1,0) ,color},
			{ DirectX::XMFLOAT3(scale.x,scale.y,pos.z), DirectX::XMFLOAT3(0,1,0) ,color},
			//後							  								  
			{ DirectX::XMFLOAT3(pos.x, scale.y,scale.z),DirectX::XMFLOAT3(0,0,1) ,color},
			{ DirectX::XMFLOAT3(scale.x,scale.y,scale.z),DirectX::XMFLOAT3(0,0,1) ,color},
			{ DirectX::XMFLOAT3(pos.x, pos.y, scale.z),DirectX::XMFLOAT3(0,0,1) ,color},
			{ DirectX::XMFLOAT3(scale.x,pos.y, scale.z),DirectX::XMFLOAT3(0,0,1) ,color},
			//下														
			{ DirectX::XMFLOAT3(pos.x, pos.y, scale.z),DirectX::XMFLOAT3(0,-1,0),color },
			{ DirectX::XMFLOAT3(scale.x,pos.y, scale.z),DirectX::XMFLOAT3(0,-1,0),color },
			{ DirectX::XMFLOAT3(pos.x, pos.y, pos.z), DirectX::XMFLOAT3(0,-1,0),color },
			{ DirectX::XMFLOAT3(scale.x,pos.y, pos.z), DirectX::XMFLOAT3(0,-1,0),color },
			//右														
			{ DirectX::XMFLOAT3(scale.x,scale.y,scale.z),DirectX::XMFLOAT3(1,0,0) ,color},
			{ DirectX::XMFLOAT3(scale.x,pos.y, scale.z),DirectX::XMFLOAT3(1,0,0) ,color},
			{ DirectX::XMFLOAT3(scale.x,scale.y,pos.z), DirectX::XMFLOAT3(1,0,0) ,color},
			{ DirectX::XMFLOAT3(scale.x,pos.y, pos.z), DirectX::XMFLOAT3(1,0,0) ,color},
			//左													
			{ DirectX::XMFLOAT3(pos.x,scale.y,scale.z),DirectX::XMFLOAT3(-1,0,0),color },
			{ DirectX::XMFLOAT3(pos.x,pos.y, scale.z),DirectX::XMFLOAT3(-1,0,0),color },
			{ DirectX::XMFLOAT3(pos.x,scale.y,pos.z) ,DirectX::XMFLOAT3(-1,0,0),color },
			{ DirectX::XMFLOAT3(pos.x,pos.y, pos.z) ,DirectX::XMFLOAT3(-1,0,0),color },
	};

	std::copy(std::begin(vertices2), std::end(vertices2), vertMap);
}