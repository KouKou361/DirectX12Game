#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

class Graphics;
class Model;
class Texture;
class GeometryVertexVector;
class MeshInstanceDrawList;

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;


//shader基底クラス
class Shader
{
public:
	Shader(const LPCWSTR shaderVS, const LPCWSTR shaderPS);
	Shader(const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS);
	Shader() {};
	~Shader();

	virtual void Begin(Graphics* graphics) = 0;
	virtual void End(Graphics* graphics) = 0;
protected:
	ComPtr<ID3D12PipelineState>		pipelineState=nullptr;
	ComPtr<ID3D12RootSignature>		rootSignature=nullptr;

	ComPtr<ID3DBlob> vsShader = nullptr;
	ComPtr<ID3DBlob> gsShader = nullptr;
	ComPtr<ID3DBlob> psShader = nullptr;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParm[2] = {};

	HRESULT hr = S_OK;
};

//Sprite専用のShader
class SpriteShader:public Shader
{
private:
	//頂点バッファ作成
	void CreateVertex(Graphics* graphics);
	//インデックスバッファ作成
	void CreateIndex(Graphics* graphics);

	

	ComPtr<ID3D12Resource>			vertexResource;
	ComPtr<ID3D12Resource>			indexResource;

	int createNum = 0;

protected:

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			indexBufferView;
	Vertex* vertMap = nullptr;
	int renderNum = 0;

	vector<Texture* >renderTextureList;

	virtual void Begin(Graphics* graphics);
	virtual void Render(Graphics* graphics,int index) = 0;
	virtual void End(Graphics* graphics);
public:
	SpriteShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS,const int createNum);
	~SpriteShader();


	int GetCreateNum() { return createNum; }
	void SetVertex(Graphics* graphics,Texture* texture, const XMFLOAT2& pos, const XMFLOAT2& scale, const XMFLOAT4& cut,
		const XMFLOAT4& color, const float& angle);
	void AllRender(Graphics* graphics);
};

//Model専用のShader
class ModelShader :public Shader
{
protected:
	MeshInstanceDrawList* meshInstanceDrawList=nullptr;
public:
	ModelShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS) :Shader(shaderVS, shaderPS) { NewMeshInstanceDrawList(); };
	ModelShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS) :Shader(shaderVS, shaderGS, shaderPS) {};
	~ModelShader() {
		if (meshInstanceDrawList != nullptr)
		{
			delete meshInstanceDrawList;
			meshInstanceDrawList = nullptr;
		}
	};

	virtual void Begin(Graphics* graphics) = 0;
	virtual void Render(Graphics* graphics, Model* model) = 0;
	virtual void End(Graphics* graphics);

	void Register(Model* model);
	void AllRender(Graphics* graphics);
	void NewMeshInstanceDrawList();
};

//Geometry専用のShader
class GeometryPointShader :public Shader
{
protected:
	vector<GeometryVertexVector*> geometryVertexVecterList;
public:
	GeometryPointShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderPS) :Shader(shaderVS, shaderPS) {};
	GeometryPointShader(Graphics* graphics, const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS) :Shader(shaderVS, shaderGS, shaderPS) {};
	~GeometryPointShader() {};

	virtual void Begin(Graphics* graphics) = 0;
	virtual void Render(Graphics* graphics, GeometryVertexVector* geometryVertex) = 0;
	virtual void End(Graphics* graphics) = 0;

	void Register(GeometryVertexVector* geometryVertexVecter);
	void AllRender(Graphics* graphics) {};
	void AllRender(Graphics* graphics,float time);

};