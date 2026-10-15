#include "MeshInstance.h"
#include "Lib_ModelResource.h"
#include "Lib_Model.h"

void MeshInstanceDrawList::Register(Model* model)
{
	for (int i = 0; i < modelList.size(); i++)
	{
		vector<Model*> &models = modelList.at(i);
		//const string s = models.at(0)->GetResource()->GetModelPath();
		const string s2 = model->GetResource()->GetModelPath();

		if (strcmp(models.at(0)->GetResource()->GetModelPath().c_str(), model->GetResource()->GetModelPath().c_str())==0)
		{
			models.emplace_back(model);//ä˘Ç…Ç†ÇÈÇÃÇ≈í«â¡ÅB
			return;
		}

	}



	//êVÇµÇ≠í«â¡
	vector<Model*> models;
	models.emplace_back(model);
	modelList.emplace_back(models);
}
void MeshInstanceDrawList::Clear()
{
	for (int i = 0; i < modelList.size(); i++)
	{
		modelList.at(i).clear();
	}
	modelList.clear();
}