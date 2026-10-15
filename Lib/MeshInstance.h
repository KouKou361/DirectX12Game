#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

class Model;

using ModelID = UINT64;
using MeshID = UINT64;

//1つのメッシュ情報
class MeshInstance
{
public:
	//vector<Model::Mesh> move;

};

//メッシュマネージャー
class MeshInstanceManager
{
public:
	MeshInstanceManager() {};
	~MeshInstanceManager() {};
	void Register(Model* model);
	void Clear();
private:
	vector<vector<Model*>> modelList;
};


//描画するメッシュのリスト。
class MeshInstanceDrawList
{
public:
	MeshInstanceDrawList() {};
	~MeshInstanceDrawList() {};
	void Register(Model* model);
	void Clear();

	vector<vector<Model*>> modelList;
private:

};

