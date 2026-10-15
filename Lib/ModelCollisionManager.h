#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Lib.h"

class Model;


class ModelCollisionPartitioning
{
public:
	ModelCollisionPartitioning() {};
	~ModelCollisionPartitioning() {};

	std::vector< std::vector<DirectX::XMFLOAT3>> polygonPos;
};

class ModelCollisionManager
{
public:
	ModelCollisionManager() {};
	ModelCollisionManager(const int& partitioningX, const int& partitioningY, const DirectX::XMFLOAT2& onePartitioningLen);
	void Register(const Model* model);
	bool RayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray);
	void Render();

private:

	bool RayPickmodelCollisionPartitioning(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray,const ModelCollisionPartitioning& modelCollisionPartitioning);
	VECTOR2 p;
	VECTOR2 onePartitioningLen;
	std::vector<std::vector<ModelCollisionPartitioning>>modelCollisionPartitioningList;

	std::vector <VECTOR2>RayPickModelList;
};