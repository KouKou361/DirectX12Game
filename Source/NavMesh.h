#pragma once

#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>
#include "Lib_Model.h"

// ナビメッシュ
class NavMesh
{
public:
	NavMesh();
	~NavMesh();

	// ビルド
	void Build(Model* model);

	// クリーン
	void Clean();

	// 更新処理
	void Update(float elapsedTime);

	// ナビメッシュ取得
	dtNavMesh* GetNavMesh() { return navMesh; }

	// ナビメッシュクエリ取得
	dtNavMeshQuery* GetNavMeshQuery() { return navQuery; }

	// 詳細ポリゴンメッシュ取得
	rcPolyMeshDetail* GetPolyMeshDetail() { return polyMeshDetail; }

	// 群衆取得
	dtCrowd* GetCrowd() { return crowd; }

	// GUI描画
	void OnGUI(Model* model);

	const int& AddAgent(
		const float &posX, const float& posY, const float& posZ, const dtCrowdAgentParams& param);

private:
	dtNavMesh*				navMesh = nullptr;
	dtNavMeshQuery*			navQuery = nullptr;
	rcPolyMesh*				polyMesh = nullptr;
	rcPolyMeshDetail*		polyMeshDetail = nullptr;
	rcContourSet*			contourSet = nullptr;
	rcCompactHeightfield*	compactHeightfield = nullptr;
	rcHeightfield*			heightfield = nullptr;
	rcConfig				config;
	rcContext				context;
	dtCrowd*				crowd = nullptr;

	float cellSize = 1.0f;
	//float cellSize = 2.0f;
	//float cellSize = 5.0f;
	float cellHeight = 0.7f;
	float agentHeight = 2.0f;
	float agentRadius = 0.1f;
	float agentMaxClimb = 3.5f;
	float agentMaxSlope = 45.0f;
	float regionMinSize = 16;
	float regionMergeSize = 40;
	float edgeMaxLen = 12.0f;
	float edgeMaxError = 1.3f;
	float vertsPerPoly = 6.0f;
	float detailSampleDist = 6.0f;
	float detailSampleMaxError = 1.0f;
};
