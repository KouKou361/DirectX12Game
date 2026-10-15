#include <map>
#include <DetourNavMeshBuilder.h>
#include <Recast.h>
#include "NavMesh.h"
#include "imgui.h"


#include <windows.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

namespace DirectX
{
	bool operator <(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		if (lhs.x == rhs.x)
		{
			if (lhs.y == rhs.y)
			{
				return lhs.z < rhs.z;
			}
			return lhs.y < rhs.y;
		}
		return lhs.x < rhs.x;
	}
}

// コンストラクタ
NavMesh::NavMesh()
{
}

// デストラクタ
NavMesh::~NavMesh()
{
	Clean();
}

// ビルド
void NavMesh::Build(Model* model)
{
	Clean();

	// モデルから頂点抽出
	std::vector<DirectX::XMFLOAT3> polygonVertexList;
	std::vector<int> indices;
	DirectX::XMFLOAT3 boundsMin, boundsMax;
	{
		const ModelResource* resource = model->GetResource();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			const std::vector<Model::Node>& nodes = model->GetNodes();
			const Model::Node& node = nodes.at(mesh.nodeIndex);

			// 頂点座標データ抽出
			std::vector<DirectX::XMFLOAT3> positions;

			// 頂点データ出力
			if (mesh.nodeIndices.size() > 0)
			{
				// ボーン変換行列算出
				std::vector<DirectX::XMMATRIX> BoneTransforms;
				BoneTransforms.resize(mesh.offsetTransforms.size());
				for (size_t i = 0; i < mesh.offsetTransforms.size(); ++i)
				{
					DirectX::XMMATRIX O = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
					DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).world_transform);
					BoneTransforms[i] = DirectX::XMMatrixMultiply(O, W);
				}
				// ワールド空間頂点座標算出
				for (const ModelResource::Vertex& vertex : mesh.polygonVertexList)
				{
					const float* boneWeights = &vertex.boneWeight.x;
					const UINT* boneIndices = &vertex.boneIndex.x;
					DirectX::XMVECTOR V = DirectX::XMVectorZero();
					for (int i = 0; i < 4; ++i)
					{
						DirectX::XMMATRIX B = BoneTransforms.at(boneIndices[i]);
						DirectX::XMVECTOR P = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.position), B);
						V = DirectX::XMVectorAdd(V, DirectX::XMVectorScale(P, boneWeights[i]));
					}
					DirectX::XMFLOAT3 v;
					DirectX::XMStoreFloat3(&v, V);
					positions.emplace_back(v);
				}
			}
			else
			{
				// ワールド空間頂点座標算出
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).world_transform);
				for (const ModelResource::Vertex& vertex : mesh.polygonVertexList)
				{
					DirectX::XMVECTOR V = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.position), W);
					DirectX::XMFLOAT3 v;
					DirectX::XMStoreFloat3(&v, V);
					positions.emplace_back(v);
				}
			}

			// トライアングルリスト最適化（重複する頂点を削除）
			std::map<DirectX::XMFLOAT3, int> cache;
			for (size_t i = 0; i < mesh.indices.size(); ++i)
			{
				UINT index = mesh.indices[i];

				UINT n = static_cast<UINT>(polygonVertexList.size());
				const DirectX::XMFLOAT3& v = positions.at(index);

				auto it = cache.find(v);
				if (it == cache.end())
				{
					cache[v] = n;
					it = cache.find(v);

					polygonVertexList.emplace_back(v);
				}
				n = it->second;

				indices.emplace_back(n);
			}
		}
		// バウンディングボックス計算
		rcCalcBounds(&polygonVertexList[0].x, static_cast<int>(polygonVertexList.size()), &boundsMin.x, &boundsMax.x);
	}

	// ナビメッシュ
	{
		bool result;

		memset(&config, 0, sizeof(config));
		config.cs = cellSize;
		config.ch = cellHeight;
		config.walkableSlopeAngle = agentMaxSlope;
		config.walkableHeight = (int)ceilf(agentHeight / config.ch);
		config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
		config.walkableRadius = (int)ceilf(agentRadius / config.cs);
		config.maxEdgeLen = (int)(edgeMaxLen / cellSize);
		config.maxSimplificationError = edgeMaxError;
		config.minRegionArea = (int)rcSqr(regionMinSize);		// Note: area = size*size
		config.mergeRegionArea = (int)rcSqr(regionMergeSize);	// Note: area = size*size
		config.maxVertsPerPoly = (int)vertsPerPoly;
		config.detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
		config.detailSampleMaxError = cellHeight * detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(config.bmin, &boundsMin.x);
		rcVcopy(config.bmax, &boundsMax.x);
		rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);


		heightfield = rcAllocHeightfield();
		_ASSERT_EXPR_A(heightfield, "buildNavigation: Out of memory 'solid'.");

		result = rcCreateHeightfield(&context, *heightfield, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not create solid heightfield.");

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		int triCount = static_cast<int>(indices.size() / 3);
		std::unique_ptr<u_char[]> triareas = std::make_unique<u_char[]>(triCount);
		_ASSERT_EXPR_A(triareas, "buildNavigation: Out of memory 'm_triareas");

		// Find triangles which are walkable based on their slope and rasterize them.
		// If your input data is multiple meshes, you can transform them here, calculate
		// the are type for each of the meshes and rasterize them.
		memset(triareas.get(), 0, triCount * sizeof(u_char));
		float* verts = &polygonVertexList[0].x;
		int* tris = indices.data();
		int vertCount = static_cast<int>(polygonVertexList.size());
		rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, verts, vertCount, indices.data(), triCount, triareas.get());
		result = rcRasterizeTriangles(&context, verts, vertCount, tris, triareas.get(), triCount, *heightfield, config.walkableClimb);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not rasterize triangles.");


		rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightfield);
		rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightfield);
		rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightfield);

		compactHeightfield = rcAllocCompactHeightfield();
		_ASSERT_EXPR_A(compactHeightfield, "buildNavigation: Out of memory 'chf'.");

		result = rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb, *heightfield, *compactHeightfield);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not build compact data.");

		result = rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightfield);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not erode.");

		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		result = rcBuildDistanceField(&context, *compactHeightfield);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not build distance field.");

		// Partition the walkable surface into simple regions without holes.
		result = rcBuildRegions(&context, *compactHeightfield, 0, config.minRegionArea, config.mergeRegionArea);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not build watershed regions.");

		contourSet = rcAllocContourSet();
		_ASSERT_EXPR_A(contourSet, "buildNavigation: Out of memory 'cset'.");

		result = rcBuildContours(&context, *compactHeightfield, config.maxSimplificationError, config.maxEdgeLen, *contourSet);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not create contours.");

		polyMesh = rcAllocPolyMesh();
		_ASSERT_EXPR_A(polyMesh, "buildNavigation: Out of memory 'pmesh'.");

		result = rcBuildPolyMesh(&context, *contourSet, config.maxVertsPerPoly, *polyMesh);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not triangulate contours.");


		polyMeshDetail = rcAllocPolyMeshDetail();
		_ASSERT_EXPR_A(polyMeshDetail, "buildNavigation: Out of memory 'pmdtl'.");

		result = rcBuildPolyMeshDetail(&context, *polyMesh, *compactHeightfield, config.detailSampleDist, config.detailSampleMaxError, *polyMeshDetail);
		_ASSERT_EXPR_A(result, "buildNavigation: Could not build detail mesh.");

		for (int i = 0; i < polyMesh->npolys; ++i)
		{
			if (polyMesh->areas[i] == RC_WALKABLE_AREA)
			{
				polyMesh->flags[i] = 1;
			}
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = polyMesh->verts;
		params.vertCount = polyMesh->nverts;
		params.polys = polyMesh->polys;
		params.polyAreas = polyMesh->areas;
		params.polyFlags = polyMesh->flags;
		params.polyCount = polyMesh->npolys;
		params.nvp = polyMesh->nvp;
		params.detailMeshes = polyMeshDetail->meshes;
		params.detailVerts = polyMeshDetail->verts;
		params.detailVertsCount = polyMeshDetail->nverts;
		params.detailTris = polyMeshDetail->tris;
		params.detailTriCount = polyMeshDetail->ntris;
		params.offMeshConVerts = nullptr;
		params.offMeshConRad = nullptr;
		params.offMeshConDir = nullptr;
		params.offMeshConAreas = nullptr;
		params.offMeshConFlags = nullptr;
		params.offMeshConUserID = nullptr;
		params.offMeshConCount = 0;
		params.walkableHeight = agentHeight;
		params.walkableRadius = agentRadius;
		params.walkableClimb = agentMaxClimb;
		rcVcopy(params.bmin, polyMesh->bmin);
		rcVcopy(params.bmax, polyMesh->bmax);
		params.cs = config.cs;
		params.ch = config.ch;
		params.buildBvTree = true;

		unsigned char* navData = 0;
		int navDataSize = 0;
		result = dtCreateNavMeshData(&params, &navData, &navDataSize);
		_ASSERT_EXPR_A(result, "Could not build Detour navmesh");

		navMesh = dtAllocNavMesh();
		_ASSERT_EXPR_A(navMesh, "Could not create Detour navmesh");

		dtStatus status;

		status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh");

		navQuery = dtAllocNavMeshQuery();
		_ASSERT_EXPR_A(navMesh, "Could not create Detour navmesh query");

		status = navQuery->init(navMesh, 2048);
		_ASSERT_EXPR_A(dtStatusSucceed(status), "Could not init Detour navmesh query");

		crowd = dtAllocCrowd();
		_ASSERT_EXPR_A(crowd, "Could not create Detour crowd");

		result = crowd->init(128, agentRadius, navMesh);
		_ASSERT_EXPR_A(result, "Could not init Detour crowd");

	
	}
}
const int& NavMesh::AddAgent(
	const float& posX, const float& posY, const float& posZ, const dtCrowdAgentParams& param)
{
	

	float p[3] = { posX, posY, posZ };
	return crowd->addAgent(p, &param);
	
}

// クリーン
void NavMesh::Clean()
{
	rcFreeHeightField(heightfield);
	heightfield = nullptr;

	rcFreeCompactHeightfield(compactHeightfield);
	compactHeightfield = nullptr;

	rcFreeContourSet(contourSet);
	contourSet = nullptr;

	rcFreePolyMesh(polyMesh);
	polyMesh = nullptr;

	rcFreePolyMeshDetail(polyMeshDetail);
	polyMeshDetail = nullptr;

	dtFreeNavMesh(navMesh);
	navMesh = nullptr;

	dtFreeNavMeshQuery(navQuery);
	navQuery = nullptr;

	dtFreeCrowd(crowd);
	crowd = nullptr;
}

// 更新処理
void NavMesh::Update(float elapsedTime)
{
	//dtCrowdAgentDebugInfo debug;
	crowd->update(elapsedTime, nullptr);

}

// GUI描画
void NavMesh::OnGUI(Model* model)
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_Once);

	if (ImGui::Begin("NavMesh", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragFloat("cellSize", &cellSize, 0.01f, 0.01f, 20.0f, "%.3f");
		ImGui::DragFloat("cellHeight", &cellHeight, 0.01f, 0.01f, 20.0f, "%.3f");
		ImGui::DragFloat("agentHeight", &agentHeight, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentRadius", &agentRadius, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentMaxClimb", &agentMaxClimb, 0.01f, 0.01f, 10.0f, "%.3f");
		ImGui::DragFloat("agentMaxSlope", &agentMaxSlope, 0.0f, 1.0f, 90.0f, "%.3f");
		ImGui::DragFloat("regionMinSize", &regionMinSize, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("regionMergeSize", &regionMergeSize, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("edgeMaxLen", &edgeMaxLen, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("edgeMaxError", &edgeMaxError, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("vertsPerPoly", &vertsPerPoly, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("detailSampleDist", &detailSampleDist, 0.1f, 0.1f, 100.0f, "%.3f");
		ImGui::DragFloat("detailSampleMaxError", &detailSampleMaxError, 0.1f, 0.1f, 100.0f, "%.3f");
		if (ImGui::Button("Build"))
		{
			Build(model);
		}
	}
	ImGui::End();
}
