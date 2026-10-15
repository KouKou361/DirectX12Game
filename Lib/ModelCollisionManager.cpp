#include "ModelCollisionManager.h"
#include "Lib.h"
#include "Lib_Model.h"
#include "imgui.h"

ModelCollisionManager::ModelCollisionManager(const int& partitioningX, const int& partitioningY, const DirectX::XMFLOAT2& onePartitioningLen)
{
	this->onePartitioningLen = { onePartitioningLen.x ,onePartitioningLen.y};
	modelCollisionPartitioningList.resize(partitioningY);

	for (auto& modelCollisionPartitioning : modelCollisionPartitioningList)
	{
		modelCollisionPartitioning.resize(partitioningX);
	}

}
void ModelCollisionManager::Register(const Model* model)
{

	VECTOR3 worldOrigin = { 0,0,0 };
	worldOrigin.x = (modelCollisionPartitioningList.size() * onePartitioningLen.x)/2;
	worldOrigin.z = (modelCollisionPartitioningList.at(0).size() * onePartitioningLen.y) / 2;
	

	for (const ModelResource::Mesh & mesh : model->GetResource()->GetMeshes())
	{
		//メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		//三角形（面）との当たり判定
		const std::vector<ModelResource::Vertex>& vetices = mesh.polygonVertexList;
		const std::vector<UINT> indices = mesh.indices;

		//レイをワールド空間かわローカル空間に変換する
		XMMATRIX WorldTranceform = XMLoadFloat4x4(&node.world_transform);
		XMMATRIX InverseWorldTranceform = XMMatrixInverse(nullptr, WorldTranceform);
		//ローカル空間
		XMFLOAT3 size = { onePartitioningLen .x,0,onePartitioningLen .y};




		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{

				const UINT Index = subset.startIndex + i;
				const UINT NextIndex = Index + 1;
				const UINT Next2Index = NextIndex + 1;
				//三角形の頂点情報を算出する
				const ModelResource::Vertex& a = vetices.at(indices.at(Index));
				const ModelResource::Vertex& b = vetices.at(indices.at(NextIndex));
				const ModelResource::Vertex& c = vetices.at(indices.at(Next2Index));

				XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				XMVECTOR WorldPolygonPostion[3];
				WorldPolygonPostion[0] = XMVector3TransformCoord(A, WorldTranceform);
				WorldPolygonPostion[1] = XMVector3TransformCoord(B, WorldTranceform);
				WorldPolygonPostion[2] = XMVector3TransformCoord(C, WorldTranceform);

				VECTOR3 WorldPostion[3];
				XMStoreFloat3(&WorldPostion[0], WorldPolygonPostion[0]);
				XMStoreFloat3(&WorldPostion[1], WorldPolygonPostion[1]);
				XMStoreFloat3(&WorldPostion[2], WorldPolygonPostion[2]);


				int a_indexX = (WorldPostion[0].x + worldOrigin.x) / size.x;
				int a_indexZ = (WorldPostion[0].z + worldOrigin.z) / size.z;
											 
				int b_indexX = (WorldPostion[1].x + worldOrigin.x) / size.x;
				int b_indexZ = (WorldPostion[1].z + worldOrigin.z) / size.z;
											 
				int c_indexX = (WorldPostion[2].x + worldOrigin.x) / size.x;
				int c_indexZ = (WorldPostion[2].z + worldOrigin.z) / size.z;

				std::vector<DirectX::XMFLOAT3> polygonPos;
				polygonPos.push_back(WorldPostion[0]);
				polygonPos.push_back(WorldPostion[1]);
				polygonPos.push_back(WorldPostion[2]);

				int aSize = modelCollisionPartitioningList.at(a_indexZ).at(a_indexX).polygonPos.size();
				int bSize = modelCollisionPartitioningList.at(b_indexZ).at(b_indexX).polygonPos.size();
				int cSize = modelCollisionPartitioningList.at(c_indexZ).at(c_indexX).polygonPos.size();

				if (aSize + 1 > modelCollisionPartitioningList.at(a_indexZ).at(a_indexX).polygonPos.size())
				{
					modelCollisionPartitioningList.at(a_indexZ).at(a_indexX).polygonPos.push_back(polygonPos);
				}
				if (bSize + 1 > modelCollisionPartitioningList.at(b_indexZ).at(b_indexX).polygonPos.size())
				{
					modelCollisionPartitioningList.at(b_indexZ).at(b_indexX).polygonPos.push_back(polygonPos);
				}
				if (cSize + 1 > modelCollisionPartitioningList.at(c_indexZ).at(c_indexX).polygonPos.size())
				{
					modelCollisionPartitioningList.at(c_indexZ).at(c_indexX).polygonPos.push_back(polygonPos);
				}

			}
		}
	}

}

bool ModelCollisionManager::RayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray)
{
	//ローカル空間
	XMFLOAT3 size = { onePartitioningLen.x,0,onePartitioningLen.y };

	VECTOR3 worldOrigin = { 0,0,0 };
	worldOrigin.x = (modelCollisionPartitioningList.size() * onePartitioningLen.x) / 2;
	worldOrigin.z = (modelCollisionPartitioningList.at(0).size() * onePartitioningLen.y) / 2;

	const int startX = (start.x+worldOrigin.x) / size.x;
	const int startY = (start.z+worldOrigin.z) / size.z;

	const int endX = (end.x + worldOrigin.x) / size.x;
	const int endY = (end.z + worldOrigin.z) / size.z;



	RayPickModelList.clear();
	RayPickModelList.push_back({ static_cast<float>(startX), static_cast<float>(startY) });
	RayPickModelList.push_back({ static_cast<float>(endX), static_cast<float>(endY) });

	// = modelCollisionPartitioningList.at(y).at(x);

	p.x = startY;
	p.y = startX;


	for (int i = 0; i < RayPickModelList.size(); i++)
	{
		const int RayPickX = static_cast<int>(RayPickModelList.at(i).x);
		const int RayPickY = static_cast<int>(RayPickModelList.at(i).y);

		if (RayPickX < 0)continue;
		if (RayPickY < 0)continue;

		if (RayPickmodelCollisionPartitioning(start, end, ray, modelCollisionPartitioningList.at(RayPickY).at(RayPickX)))
		{
			return true;
		}
		
	}
	return false;


	
}

void ModelCollisionManager::Render()
{

	VECTOR3 WorldOrigin = { 0,0,0 };
	WorldOrigin.x = (modelCollisionPartitioningList.size() * onePartitioningLen.x) / 2;
	WorldOrigin.z = (modelCollisionPartitioningList.at(0).size() * onePartitioningLen.y) / 2;

	const int pX = static_cast<int>(p.y);
	const int pY = static_cast<int>(p.x);

	static bool chankRender = false;
	static bool stageRender = false;
	ImGui::Begin("Debug");
	ImGui::Text("chank =%d,%d", pX, pY);
	ImGui::Checkbox("chankRender", &chankRender);
	ImGui::Checkbox("stageRender", &stageRender);
	ImGui::End();

	if (stageRender)
	{

		for (int i = 0; i < RayPickModelList.size(); i++)
		{
			const int RayPickX = static_cast<int>(RayPickModelList.at(i).x);
			const int RayPickY = static_cast<int>(RayPickModelList.at(i).y);

			for (int i = 0; i < modelCollisionPartitioningList.at(RayPickY).at(RayPickX).polygonPos.size(); i++)
			{
				std::vector<DirectX::XMFLOAT3> poygon = modelCollisionPartitioningList.at(RayPickY).at(RayPickX).polygonPos.at(i);

				VECTOR3 p0 = { poygon.at(0).x,poygon.at(0).y + 1.0f ,poygon.at(0).z };
				VECTOR3 p1 = { poygon.at(1).x,poygon.at(1).y + 1.0f ,poygon.at(1).z };
				VECTOR3 p2 = { poygon.at(2).x,poygon.at(2).y + 1.0f ,poygon.at(2).z };
				TK_Lib::Debug3D::Line(p0, p1, { 0,1,0,1 });
				TK_Lib::Debug3D::Line(p1, p2, { 0,1,0,1 });
				TK_Lib::Debug3D::Line(p2, p0, { 0,1,0,1 });
			}
		}


		
	}
	
	if (chankRender)
	{
		
	for (int y=0;y< modelCollisionPartitioningList.size();y++)
	{
		for (int x = 0; x < modelCollisionPartitioningList.at(y).size(); x++)
		{
			VECTOR4 color = { 1,0,0,1 };
			if (y==p.y)
			{
				if (x == p.x)
				{
					color = { 0,0,1,1 };
				}
			}
			DirectX::XMFLOAT2 size = onePartitioningLen;
			const VECTOR3 pos1 = { y * size.x,-100,x * size.y };
			const VECTOR3 pos2 = { (y + 1) * size.x,100,(x + 1) * size.y };
			TK_Lib::Debug3D::Box(pos1 - WorldOrigin, pos2 - WorldOrigin, color);
		}
	}
	}



}


bool ModelCollisionManager::RayPickmodelCollisionPartitioning(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray, const ModelCollisionPartitioning& modelCollisionPartitioning)
{
	int materialIndex = -1;
	XMVECTOR HitPosition;
	XMVECTOR HitNormalize;
	XMVECTOR PolygonPostion[3];

	XMVECTOR Start = XMLoadFloat3(&start);
	XMVECTOR End = XMLoadFloat3(&end);
	XMVECTOR WorldRayVec = XMVectorSubtract(End, Start);
	XMVECTOR WorldRayLength = XMVector3Length(WorldRayVec);

	//ローカル空間
	XMVECTOR Vec = XMVectorSubtract(End, Start);
	XMVECTOR Dir = XMVector3Normalize(Vec);
	XMVECTOR Length = XMVector3Length(Vec);

	//レイの長さ
	float neart;
	XMStoreFloat(&neart, Length);

	bool hit = false;

	for (std::vector<DirectX::XMFLOAT3> polygonPos : modelCollisionPartitioning.polygonPos)
		{
			for (UINT i = 0; i < polygonPos.size(); i += 3)
			{
				//三角形の頂点情報を算出する
				const DirectX::XMFLOAT3 a = polygonPos.at(0);
				const DirectX::XMFLOAT3 b = polygonPos.at(1);
				const DirectX::XMFLOAT3 c = polygonPos.at(2);



				XMVECTOR A = DirectX::XMLoadFloat3(&a);
				XMVECTOR B = DirectX::XMLoadFloat3(&b);
				XMVECTOR C = DirectX::XMLoadFloat3(&c);

				//三角形の三辺ベクトルを算出
				XMVECTOR AB = XMVectorSubtract(B, A);
				XMVECTOR BC = XMVectorSubtract(C, B);
				XMVECTOR CA = XMVectorSubtract(A, C);
				//三角形の法線ベクトルの算出
				XMVECTOR Normal = XMVector3Normalize(XMVector3Cross(AB, BC));
				//XMVECTOR Normal = XMVector3Normalize(XMVector3Cross(BC,AB));
				//内積の値がプラスなら裏向き
				XMVECTOR Dot = XMVector3Dot(Dir, Normal);
				float dot;
				XMStoreFloat(&dot, Dot);
				if (dot >= 0)continue;//裏向き

				//レイと平面の交差点
				XMVECTOR d1 = XMVector3Dot(XMVectorSubtract(A, Start), Normal);
				//XMVECTOR d2 = XMVector3Dot(Normal, Dir);
				XMVECTOR T = XMVectorDivide(d1, Dot);
				float t;
				XMStoreFloat(&t, T);
				//交点までの距離が今までに計算した最近距離よりも大きいときはスキップ
				if (t<0.0f || t>neart)continue;
				XMVECTOR Position = XMVectorMultiply(Dir, T);
				Position = XMVectorAdd(Position, Start);
				XMFLOAT3 pos;
				XMStoreFloat3(&pos, Position);

				//交点が三角形の内側にあるか判定
				//1
				{
					XMVECTOR V1 = XMVectorSubtract(Position, A);
					XMVECTOR Cross = XMVector3Cross(AB, V1);
					//	XMVECTOR Cross = XMVector3Cross(V1,AB);
					XMVECTOR Dot1 = XMVector3Dot(Cross, Normal);
					float dot1;
					XMStoreFloat(&dot1, Dot1);
					if (dot1 < 0)continue;
				}

				//2
				{
					XMVECTOR V2 = XMVectorSubtract(Position, B);
					XMVECTOR Cross2 = XMVector3Cross(BC, V2);
					//XMVECTOR Cross2 = XMVector3Cross(V2, BC);
					XMVECTOR Dot2 = XMVector3Dot(Cross2, Normal);
					float dot2;
					XMStoreFloat(&dot2, Dot2);
					if (dot2 < 0)continue;

				}


				//3
				{
					XMVECTOR V3 = XMVectorSubtract(Position, C);
					XMVECTOR Cross3 = XMVector3Cross(CA, V3);
					//XMVECTOR Cross3 = XMVector3Cross(V3,CA );
					XMVECTOR Dot3 = XMVector3Dot(Cross3, Normal);
					float dot3;
					XMStoreFloat(&dot3, Dot3);
					if (dot3 < 0)continue;

				}

				//最短距離を更新する
				neart = t;
				//交点と法線を更新
				HitPosition = Position;
				HitNormalize = Normal;

				PolygonPostion[0] = A;
				PolygonPostion[1] = B;
				PolygonPostion[2] = C;

				hit = true;
			}

		}


	//保存
	if (hit ==TRUE)
	{
		XMVECTOR WorldPolygonPostion[3];
		XMVECTOR WorldCrossVec = XMVectorSubtract(HitPosition, Start);
		XMVECTOR WorldCrossLength = XMVector3Length(WorldCrossVec);

		float distance;
		XMStoreFloat(&distance, WorldCrossLength);

		ray.distance = distance;
		ray.materialIndex = materialIndex;
		XMStoreFloat3(&ray.Pos, HitPosition);
		XMStoreFloat3(&ray.Normal, XMVector3Normalize(HitNormalize));

		XMStoreFloat3(&ray.PolygonPos[0], WorldPolygonPostion[0]);
		XMStoreFloat3(&ray.PolygonPos[1], WorldPolygonPostion[1]);
		XMStoreFloat3(&ray.PolygonPos[2], WorldPolygonPostion[2]);
		hit = true;
	}

	return hit;
}
