#include "ModelCollisionManager.h"
#include "Lib.h"

ModelCollisionManager::ModelCollisionManager(const int& partitioningX, const int& partitioningY, DirectX::XMFLOAT2& onePartitioningLen)
{
	this->onePartitioningLen = onePartitioningLen;
	modelCollisionPartitioningList.resize(partitioningY);

	for (auto& modelCollisionPartitioning : modelCollisionPartitioningList)
	{
		modelCollisionPartitioning.resize(partitioningX);
	}

}
void ModelCollisionManager::Register(const ModelResource* modelResource)
{

}

void ModelCollisionManager::RayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray)
{

}

void ModelCollisionManager::Render()
{
	for (int y=0;y< modelCollisionPartitioningList.size();y++)
	{
		for (int x = 0; x < modelCollisionPartitioningList.at(y).size(); x++)
		{
			DirectX::XMFLOAT2 size = onePartitioningLen;
			TK_Lib::Debug3D::Box({y* size.x,-100,x * size.y }, { y+1 * size.x,100,x+1 * size.y });
		}
	}
}
