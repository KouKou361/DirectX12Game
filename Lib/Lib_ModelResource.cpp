#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>


#include <stdlib.h>
#include <fstream>
#include <functional>
#include <assert.h>

#include "Lib_ModelResource.h"
#include "Lib_Graphics.h"
#include "Lib_ConstantBuffer.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::MaterialAlbedo, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::Vertex, 1)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource::StaticModelIndex, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)

// シリアライズ
namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(id),
		CEREAL_NVP(name),
		CEREAL_NVP(path),
		CEREAL_NVP(parentIndex),
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::MaterialAlbedo::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(textureFilename),
		CEREAL_NVP(color)
	);
}

template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(startIndex),
		CEREAL_NVP(indexCount),
		CEREAL_NVP(materialIndex)
	);
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(normal),
		CEREAL_NVP(tangent),
		CEREAL_NVP(texcoord),
		CEREAL_NVP(color),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex)
	);
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(polygonVertexList),
		CEREAL_NVP(indices),
		CEREAL_NVP(subsets),
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(nodeIndices),
		CEREAL_NVP(offsetTransforms),
		CEREAL_NVP(boundsMin),
		CEREAL_NVP(boundsMax)
	);
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(scale),
		CEREAL_NVP(rotate),
		CEREAL_NVP(translate)
	);
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(nodeKeys)
	);
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(secondsLength),
		CEREAL_NVP(keyframes)
	);
}

template<class Archive>
void ModelResource::StaticModelIndex::serialize(Archive& archive, int version)
{
	archive(
		CEREAL_NVP(data)
	);
}




// シリアライズ
void ModelResource::Serialize(const char* filename)
{
	std::ofstream ostream(filename, std::ios::binary);
	if (ostream.is_open())
	{
		cereal::BinaryOutputArchive archive(ostream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materialsAlbedo),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			assert(!L"モデルのロードが出来ないねええええええええええええええええ");
			ostream.close();
			return;
		}
	}
	ostream.close();
}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);
	if (istream.is_open())
	{
		cereal::BinaryInputArchive archive(istream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materialsAlbedo),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			assert(!L"モデルのロードが出来ないねええええええええええええええええ");
			return;
		}
	}
	else
	{
		assert(!L"モデルのロードが出来ないねええええええええええええええええ");
		return;
	}
}


ModelResource::ModelResource(Graphics* graphics,const char* filename)
{
	// ディレクトリパス取得
	char drive[32], dir[256], dirname[256];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	
	// デシリアライズ
	Deserialize(filename);
	//モデルの構築
	BuildModel(graphics, dirname);


	modelPath = filename;

	DeserializeModel();

}
ModelResource::~ModelResource()
{
	for (auto& materialAlbedo : materialsAlbedo)
	{
		if (materialAlbedo.constantBuffer != nullptr)
		{
			delete materialAlbedo.constantBuffer;
			materialAlbedo.constantBuffer = nullptr;
		}
	}
	materialsAlbedo.clear();
	materialsNormal.clear();
	materialsMetallic.clear();


};

//モデルの構築
void ModelResource::BuildModel(Graphics* graphics, const char* dirname)
{

	ID3D12Device* device = graphics->GetDevice();


	HRESULT hr = S_OK;


	//マテリアルの構築
	BuildMaterial(graphics,dirname);

	

	// メッシュ
	for (Mesh& mesh : meshes)
	{
		// サブセット
		for (Subset& subset : mesh.subsets)
		{
			subset.materialAlbedo = &materialsAlbedo.at(subset.materialIndex);
			subset.materialNormal = &materialsNormal.at(subset.materialIndex);
			subset.materialMetallic = &materialsMetallic.at(subset.materialIndex);
			subset.materialSmoothness = &materialsSmoothness.at(subset.materialIndex);
		}

		

		//メッシュのリソース作成
		CreateMeshResource(graphics, mesh);
	}
}

//マテリアルの構築
void ModelResource::BuildMaterial(Graphics* graphics, const char* dirname)
{
	ID3D12Device* device = graphics->GetDevice();


	HRESULT hr = S_OK;

	// アルベドマテリアル
	for (MaterialAlbedo& materialAlbedo : materialsAlbedo)
	{
		// 相対パスの解決
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, materialAlbedo.textureFilename.c_str(), nullptr);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// テクスチャ読み込み
		materialAlbedo.shaderResourceViewResource = graphics->LoadTexture(filename);
		if (materialAlbedo.shaderResourceViewResource == nullptr)
		{
			// 読み込み失敗したらダミーテクスチャを作る
			materialAlbedo.shaderResourceViewResource = graphics->GetWhileTexture();
		}

		// シェーダーリソースビューの生成
		{
			D3D12_RESOURCE_DESC desc = materialAlbedo.shaderResourceViewResource->GetDesc();

			// シェーダーリソースビューの設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = desc.Format;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			// ディスクリプタ取得
			materialAlbedo.shaderResourceViewDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();

			// シェーダリソースビューを生成.
			device->CreateShaderResourceView(
				materialAlbedo.shaderResourceViewResource,
				&srvDesc,
				materialAlbedo.shaderResourceViewDescriptor->GetCpuHandle()
			);
		}
		// 定数バッファの生成
		// データ設定
		CbLambertMaterial cb_data;
		materialAlbedo.constantBuffer = new Lib_ConstantBuffer(graphics, sizeof(CbLambertMaterial), &cb_data);

		cb_data.color = materialAlbedo.color;
		materialAlbedo.constantBuffer->CopyToVRAM(graphics, &cb_data);
		materialAlbedo.constantBuffer->Lock();
		
	}

	materialsNormal.resize(materialsAlbedo.size());
	// 法線マテリアル
	for (int i=0;i< materialsNormal.size();i++)
	{
		
		MaterialNormal& materialNormal = materialsNormal.at(i);
		MaterialAlbedo& materialAlbedo = materialsAlbedo.at(i);

	
		string textureFileName = materialAlbedo.textureFilename;
		int textureSize = textureFileName.size();

		if (textureSize != 0)
		{
			materialNormal.textureFilename = textureFileName.insert(textureSize - 4, "_normal");
		}
		
		//	"Textures/PBRSlimeNormal.png";
		// 相対パスの解決
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, materialNormal.textureFilename.c_str(), nullptr);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// テクスチャ読み込み
		materialNormal.shaderResourceViewResource = graphics->LoadTexture(filename);
		if (materialNormal.shaderResourceViewResource == nullptr)
		{
			// 読み込み失敗したらダミーテクスチャを作る
			materialNormal.shaderResourceViewResource = graphics->GetWhileTexture();
		}

		// シェーダーリソースビューの生成
		{
			D3D12_RESOURCE_DESC desc = materialNormal.shaderResourceViewResource->GetDesc();

			// シェーダーリソースビューの設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = desc.Format;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			// ディスクリプタ取得
			materialNormal.shaderResourceViewDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();

			// シェーダリソースビューを生成.
			device->CreateShaderResourceView(
				materialNormal.shaderResourceViewResource,
				&srvDesc,
				materialNormal.shaderResourceViewDescriptor->GetCpuHandle()
			);
		}
	}


	materialsMetallic.resize(materialsAlbedo.size());
	// メタリックマテリアル
	for (int i = 0; i < materialsMetallic.size(); i++)
	{

		MaterialMetallic& materialMetallic = materialsMetallic.at(i);
		MaterialAlbedo& materialAlbedo = materialsAlbedo.at(i);

	
		string textureFileName = materialAlbedo.textureFilename;
		int textureSize = textureFileName.size();
		
		

		if (textureSize != 0)
		{
			materialMetallic.textureFilename = textureFileName.insert(textureSize - 4, "_metallic");
		}
		// 相対パスの解決
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, materialMetallic.textureFilename.c_str(), nullptr);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// テクスチャ読み込み
		materialMetallic.shaderResourceViewResource = graphics->LoadTexture(filename);
		if (materialMetallic.shaderResourceViewResource == nullptr)
		{
			// 読み込み失敗したらダミーテクスチャを作る
			materialMetallic.shaderResourceViewResource = graphics->GetWhileTexture();
		}

		// シェーダーリソースビューの生成
		{
			D3D12_RESOURCE_DESC desc = materialMetallic.shaderResourceViewResource->GetDesc();

			// シェーダーリソースビューの設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = desc.Format;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			// ディスクリプタ取得
			materialMetallic.shaderResourceViewDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();

			// シェーダリソースビューを生成.
			device->CreateShaderResourceView(
				materialMetallic.shaderResourceViewResource,
				&srvDesc,
				materialMetallic.shaderResourceViewDescriptor->GetCpuHandle()
			);
		}
	}

	materialsSmoothness.resize(materialsAlbedo.size());
	// メタリックマテリアル
	for (int i = 0; i < materialsSmoothness.size(); i++)
	{

		MaterialSmoothness& materialSmoothness = materialsSmoothness.at(i);
		MaterialAlbedo& materialAlbedo = materialsAlbedo.at(i);

		string textureFileName = materialAlbedo.textureFilename;
		int textureSize = textureFileName.size();
		if (textureSize != 0)
		{
			materialSmoothness.textureFilename = textureFileName.insert(textureSize - 4, "_smoothness");
		}


		// 相対パスの解決
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, materialSmoothness.textureFilename.c_str(), nullptr);

		// マルチバイト文字からワイド文字へ変換
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// テクスチャ読み込み
		materialSmoothness.shaderResourceViewResource = graphics->LoadTexture(filename);
		if (materialSmoothness.shaderResourceViewResource == nullptr)
		{
			// 読み込み失敗したらダミーテクスチャを作る
			materialSmoothness.shaderResourceViewResource = graphics->GetWhileTexture();
		}

		// シェーダーリソースビューの生成
		{
			D3D12_RESOURCE_DESC desc = materialSmoothness.shaderResourceViewResource->GetDesc();

			// シェーダーリソースビューの設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = desc.Format;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			// ディスクリプタ取得
			materialSmoothness.shaderResourceViewDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();

			// シェーダリソースビューを生成.
			device->CreateShaderResourceView(
				materialSmoothness.shaderResourceViewResource,
				&srvDesc,
				materialSmoothness.shaderResourceViewDescriptor->GetCpuHandle()
			);
		}
	}
}


// シリアライズ
bool ModelResource::SerializeSmi(const char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);
	if (ifs.is_open())
	{
		cereal::BinaryInputArchive archive(ifs);

		try
		{
			archive(
				InstanceModelIndex
			);
		}
		catch (...)
		{

		}
	}
	ifs.close();

	if (InstanceModelIndex.size() == 0)
	{
		return false;
	}
	return true;
}

void ModelResource::DeserializeSmi(const char* filename)
{

	std::ofstream ostream(filename, std::ios::binary);
	if (ostream.is_open())
	{
		cereal::BinaryOutputArchive archive(ostream);

		try
		{
			archive(
				CEREAL_NVP(InstanceModelIndex)
			);
		}
		catch (...)
		{
			assert(!L"モデルのロードが出来ないねええええええええええええええええ");
			ostream.close();
			return;
		}
	}
	ostream.close();


}

void ModelResource::DeserializeModel()
{
	string fileName = modelPath;

	fileName = fileName.substr(0, fileName.size() - 3);
	fileName += "smi";

	if (SerializeSmi(fileName.c_str()))
	{
		return;
	}


	for (int i = 0; i < meshes.size(); i++)
	{
		std::vector<int> list = GetIndexSearchSameMesh(i);
		StaticModelIndex res;
		res.data = list;
		if (res.data.size() == 0)continue;
		InstanceModelIndex.push_back(res);
	
	}
	
	DeserializeSmi(fileName.c_str());
}


std::vector<int> ModelResource::GetIndexSearchSameMesh(const int meshIndex)
{
	
	std::vector<int> result;
	result.clear();

	const Mesh &serchMesh = meshes.at(meshIndex);
	if (serchMesh.DeserializeDesFlg == true)return result;

	for (int i = meshIndex; i < meshes.size(); i++)
	{
		Mesh& mesh = meshes.at(i);
		if (mesh.DeserializeDesFlg == true)continue;
		if (serchMesh.polygonVertexList.size() != mesh.polygonVertexList.size())continue;
		if (serchMesh.indices.size() != mesh.indices.size())continue;


		//もしポリゴンがめちゃ多い場合は、速度が遅くなります。
		//その場合を考慮して1000より下の場合のみ受けます。
		if (serchMesh.polygonVertexList.size() <= 5000)
		{
			bool ret = true;

			for (int j = 0; j < serchMesh.polygonVertexList.size(); j++)
			{
				for (int k = 0; k < mesh.polygonVertexList.size(); k++)
				{
					if (serchMesh.polygonVertexList.at(j).position.x != mesh.polygonVertexList.at(j).position.x)ret = false;
					if (serchMesh.polygonVertexList.at(j).position.y != mesh.polygonVertexList.at(j).position.y)ret = false;
					if (serchMesh.polygonVertexList.at(j).position.z != mesh.polygonVertexList.at(j).position.z)ret = false;

					if (serchMesh.polygonVertexList.at(j).texcoord.x != mesh.polygonVertexList.at(j).texcoord.x)ret = false;
					if (serchMesh.polygonVertexList.at(j).texcoord.y != mesh.polygonVertexList.at(j).texcoord.y)ret = false;

					if (ret == false)continue;
				}
				if (ret == false)continue;
			}



			if (ret == false)continue;
		}
	

		mesh.DeserializeDesFlg = true;
		result.push_back(i);
	}

	return result;
}

//メッシュのリソース作成
void ModelResource::CreateMeshResource(Graphics* graphics,Mesh& mesh)
{
	ID3D12Device* device = graphics->GetDevice();


	HRESULT hr = S_OK;

	// 頂点バッファ
	{
		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = sizeof(Vertex) * mesh.polygonVertexList.size();
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// アップロード用頂点バッファ生成
		ComPtr<ID3D12Resource>	d3d_upload_resource;
		hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(d3d_upload_resource.GetAddressOf())
		);
		if (hr != S_OK)
		{
			assert(!" アップロード用頂点バッファの生成のエラー");
		}

		// アップロード用頂点バッファにデータ内容をコピー
		void* vb = nullptr;
		hr = d3d_upload_resource->Map(0, nullptr, &vb);

		if (hr != S_OK)
		{
			assert(!" データ内容コピーのエラー");
		}

		::memcpy(vb, mesh.polygonVertexList.data(), sizeof(Vertex) * mesh.polygonVertexList.size());
		d3d_upload_resource->Unmap(0, nullptr);

		// 描画用頂点バッファ生成
		d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(mesh.d3d_vb_resource.GetAddressOf())
		);
		if (hr != S_OK)
		{
			assert(!" データ内容コピーのエラー");
		}
		mesh.d3d_vb_resource->SetName(L"ModelResourceVertexBuffer");

		// 頂点バッファビュー設定
		mesh.d3d_vbv.BufferLocation = mesh.d3d_vb_resource->GetGPUVirtualAddress();
		mesh.d3d_vbv.SizeInBytes = static_cast<UINT>(sizeof(Vertex) * mesh.polygonVertexList.size());
		mesh.d3d_vbv.StrideInBytes = sizeof(Vertex);

		// アップロード用バッファを描画用バッファにコピー
		graphics->CopyBuffer(d3d_upload_resource.Get(), mesh.d3d_vb_resource.Get());
	}

	// インデックスバッファ
	{
		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = sizeof(UINT) * mesh.indices.size();
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// アップロード用頂点バッファ生成
		Microsoft::WRL::ComPtr<ID3D12Resource> d3d_upload_resource;
		hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(d3d_upload_resource.GetAddressOf())
		);

		if (hr != S_OK)
		{
			assert(!"インデックスバッファの生成のエラー");
		}

		// アップロード用インデックスバッファにデータ内容をコピー
		void* ib = nullptr;
		hr = d3d_upload_resource->Map(0, nullptr, &ib);
		if (hr != S_OK)
		{
			assert(!" アップロード用インデックスバッファにデータ内容をコピーのエラー");
		}

		::memcpy(ib, mesh.indices.data(), sizeof(UINT) * mesh.indices.size());
		d3d_upload_resource->Unmap(0, nullptr);

		// 描画用インデックスバッファ生成
		d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(mesh.d3d_ib_resource.GetAddressOf())
		);
		if (hr != S_OK)
		{
			assert(!" 描画用インデックスバッファ生成のエラー");
		}

		mesh.d3d_vb_resource->SetName(L"ModelResourceIndexBuffer");

		// インデックスバッファビュー設定
		mesh.d3d_ibv.BufferLocation = mesh.d3d_ib_resource->GetGPUVirtualAddress();
		mesh.d3d_ibv.SizeInBytes = static_cast<UINT>(sizeof(UINT) * mesh.indices.size());
		mesh.d3d_ibv.Format = DXGI_FORMAT_R32_UINT;

		// アップロード用バッファを描画用バッファにコピー
		graphics->CopyBuffer(d3d_upload_resource.Get(), mesh.d3d_ib_resource.Get());
	}
}

//メッシュのID設定
void ModelResource::SetMeshKindID(Graphics* graphics, Mesh& mesh, const int index)
{
	ID3D12Device* device = graphics->GetDevice();

	HRESULT hr = S_OK;

	//自分よりも階層が低い
	for (int i = 0; i < index; i++)
	{
		//if (mesh.polygonVertexList.size() != size2) {
		//	return false;
		//}
	}
}