#include "Lib_ModelManager.h"
#include "Lib_ModelResource.h"

Lib_ModelManager::Lib_ModelManager()
{
	modelResourceTable.clear();
}

Lib_ModelManager::~Lib_ModelManager()
{
	Clear();
}
//モデルの全ての削除
void Lib_ModelManager::Clear()
{
	//modelResourceTable;
	auto iter = modelResourceTable.begin();
	while (iter != modelResourceTable.end()) {
		ModelResource* modelResource = iter->second;
		if (modelResource != nullptr)
		{
			delete modelResource;
			modelResource = nullptr;
		}
	
		++iter;
	}
}
//重複しているかどうか
ModelResource* Lib_ModelManager::IsDuplication(const string fileName)
{
	auto it = modelResourceTable.find(fileName);
	if (it != modelResourceTable.end())return it->second;

	return nullptr;
}
//登録
ModelResource* Lib_ModelManager::Resiger(Graphics* graphics,const string fileName)
{
	//モデルが既に存在している場合　それを返す。
	ModelResource* modelResource=IsDuplication(fileName);
	if (modelResource != nullptr)return modelResource;

	//モデルの作成
	modelResource = CreateModel(graphics, fileName);
	//モデルの登録
	modelResourceTable[fileName] = modelResource;

	return modelResource;

}
//モデルの生成
ModelResource* Lib_ModelManager::CreateModel(Graphics* graphics,const string fileName)
{
	ModelResource* modelResource = new ModelResource(graphics,fileName.c_str());
	return modelResource;
}
