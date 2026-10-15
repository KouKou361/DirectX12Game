#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

class Descriptor;
class Graphics;
class Lib_ConstantBuffer;

struct CbLambertMaterial
{
	DirectX::XMFLOAT4	color = {};
};

class ModelResource
{
public:

	ModelResource(Graphics* graphics,const char* filename);
	ModelResource() {};
	~ModelResource();


	using NodeId = UINT64;



	struct Node
	{
		NodeId				id = 0;
		string			name ="";
		string			path = "";
		int					parentIndex = 0;
		XMFLOAT3	scale = {};
		XMFLOAT4	rotate = {};
		XMFLOAT3	translate = {};

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	//アルペド
	struct MaterialAlbedo
	{
		string			name="";
		string			textureFilename = "";
		XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };

		ID3D12Resource*	shaderResourceViewResource=nullptr;
		Descriptor* shaderResourceViewDescriptor = nullptr;

		Lib_ConstantBuffer* constantBuffer;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	//法線
	struct MaterialNormal
	{
		string			textureFilename="";

		ID3D12Resource* shaderResourceViewResource = nullptr;
		Descriptor* shaderResourceViewDescriptor = nullptr;
	};
	
	//メタリック
	struct MaterialMetallic
	{
		string			textureFilename = "";

		ID3D12Resource* shaderResourceViewResource = nullptr;
		Descriptor* shaderResourceViewDescriptor = nullptr;
	};

	//メタリック
	struct MaterialSmoothness
	{
		string			textureFilename = "";

		ID3D12Resource* shaderResourceViewResource = nullptr;
		Descriptor* shaderResourceViewDescriptor = nullptr;
	};

	

	struct Subset
	{
		UINT		startIndex = 0;
		UINT		indexCount = 0;
		int			materialIndex = 0;

		MaterialAlbedo* materialAlbedo = nullptr;
		MaterialNormal* materialNormal = nullptr;
		MaterialMetallic* materialMetallic = nullptr;
		MaterialSmoothness* materialSmoothness = nullptr;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position = { 0, 0, 0 };
		DirectX::XMFLOAT3	normal = { 0, 0, 0 };
		DirectX::XMFLOAT3	tangent = { 0, 0, 0 };
		DirectX::XMFLOAT2	texcoord = { 0, 0 };
		DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT4	boneWeight = { 1, 0, 0, 0 };
		DirectX::XMUINT4	boneIndex = { 0, 0, 0, 0 };

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Mesh
	{
		std::vector<Vertex>						polygonVertexList;
		std::vector<UINT>						indices;
		std::vector<Subset>						subsets;

		int										nodeIndex;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

		DirectX::XMFLOAT3						boundsMin;
		DirectX::XMFLOAT3						boundsMax;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_ib_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cb_resource;

		D3D12_VERTEX_BUFFER_VIEW				d3d_vbv;
		D3D12_INDEX_BUFFER_VIEW					d3d_ibv;
		Descriptor* cbv_descriptor = nullptr;

		int MeshKindID = 0;

		bool DeserializeDesFlg = 0;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};
	struct Animation
	{
		std::string					name;
		float						secondsLength;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct StaticModelIndex
	{
		std::vector<int> data;

		template<class Archive>
		void serialize(Archive& archive, int version);


	};

	// 各種データ取得
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }
	const std::vector<MaterialAlbedo>& GetMaterialsAlbedo() const { return materialsAlbedo; }
	const std::vector<MaterialNormal>& GetMaterialsNormal() const { return materialsNormal; }
	const std::vector<MaterialMetallic>& GetMaterialsMetallic() const { return materialsMetallic; }
	const std::vector<MaterialSmoothness>& GetMaterialsSmoothness() const { return materialsSmoothness; }
	void DeserializeModel();
	const std::string GetModelPath()const { return modelPath; }
	const std::vector<StaticModelIndex> &GetInstanceModelIndex()const { return InstanceModelIndex; }

protected:
	std::vector<Node>		nodes = {};
	std::vector<MaterialAlbedo>	materialsAlbedo = {};
	std::vector<MaterialNormal>	materialsNormal = {};
	std::vector<MaterialMetallic>	materialsMetallic = {};
	std::vector<MaterialSmoothness>	materialsSmoothness = {};
	std::vector<Mesh>		meshes = {};
	std::vector<Animation>	animations = {};

	// シリアライズ
	void Serialize(const char* filename);
	//デシリアライズ
	void Deserialize(const char* filename);
	//モデルの構築
	void BuildModel(Graphics* graphics, const char* filename);
	//マテリアルの構築
	void BuildMaterial(Graphics* graphics, const char* filename);
	//メッシュのリソース作成
	void CreateMeshResource(Graphics* graphics,Mesh& mesh);
	//メッシュのID設定
	void SetMeshKindID(Graphics* graphics, Mesh& mesh, const int index);

private:

	bool SerializeSmi(const char* filename);
	void DeserializeSmi(const char* filename);

	//引数と同じメッシュを探す。
	std::vector<int> GetIndexSearchSameMesh(const int meshIndex);
	std::vector<StaticModelIndex> InstanceModelIndex = {};
	std::string modelPath="";

};