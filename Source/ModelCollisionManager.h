#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>

class ModelResource;

class RayOut
{
public:
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 PolygonPos[3];//ポリゴンの位置
	float distance = 0.0f;//レイの視点から交点まで距離
	int materialIndex = -1;//衝突したポリゴンのマテリアル番号
};

//モデルの当たり判定用：仕切り
class ModelCollisionPartitioning
{
public:
	ModelCollisionPartitioning() {};
	~ModelCollisionPartitioning() {};
private:
	std::vector<DirectX::XMFLOAT3[3]> polygonData;
};

class ModelCollisionManager
{
public:
	ModelCollisionManager(const int& partitioningX, const int& partitioningY, DirectX::XMFLOAT2& onePartitioningLen);
	~ModelCollisionManager() {};

	void Register(const ModelResource* modelResource);

	void RayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray);

	void Render();
private:
	DirectX::XMFLOAT2 onePartitioningLen;
	std::vector <std::vector<ModelCollisionPartitioning>> modelCollisionPartitioningList;
	
};