#pragma once

#include <vector>
#include <string>
#include <wrl.h>
#include <DirectXMath.h>
#include <map>

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

class ModelResource;
class Graphics;
class Lib_ModelManager
{

public:
	Lib_ModelManager();
	~Lib_ModelManager();
	//登録
	ModelResource* Resiger(Graphics* graphics,const string fileName);
	//モデルの全ての削除
	void Clear();
	

private:
	//重複しているかどうか
	ModelResource* IsDuplication(const string fileName);

	//モデルの生成
	ModelResource* CreateModel(Graphics* graphics, const string fileName);

	map<string,ModelResource*> modelResourceTable;
};
