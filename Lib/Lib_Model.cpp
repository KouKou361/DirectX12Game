#include "Lib_Misc.h"
#include "Lib_Graphics.h"
#include "Lib_Model.h"
#include "Lib_Graphics.h"
#include "Lib_ConstantBuffer.h"
#include "../Source/Mathf.h"

//// コンストラクタ
//Model::Model(Graphics* graphics,const char* filename)
//{
//	this->graphics = graphics;
//	// リソース読み込み
//	m_resource = new ModelResource(graphics, filename);
//
//	Create(graphics);
//}

Model::Model(Graphics* graphics, ModelResource* modelResource)
{
	this->graphics = graphics;
	// リソース読み込み
	m_resource = modelResource;

	Create(graphics);
}

Model::~Model()
{
	for (int i = 0; i < graphics->GetBufferCount(); i++)
	{
		for (Mesh& mesh : m_meshes)
		{
			const ModelResource::Mesh* res_mesh = mesh.mesh;
			FrameResource& frame_resource = mesh.frame_resources.at(i);
			if (frame_resource.constantBuffer != nullptr)
			{
				delete frame_resource.constantBuffer;
				frame_resource.constantBuffer = nullptr;
			}

		}
	}

}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");

	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : m_nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.local_transform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.world_transform, WorldTransform);
	}

//	Graphics& graphics = Graphics::Instance();
	for (Mesh& mesh : m_meshes)
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		FrameResource& frame_resource = mesh.frame_resources.at(graphics->GetCurrentBufferIndex());

		if (res_mesh->nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < res_mesh->nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_nodes.at(res_mesh->nodeIndices.at(i)).world_transform);
				DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&res_mesh->offsetTransforms.at(i));
				DirectX::XMMATRIX BoneTransform = OffsetTransform * WorldTransform;
				DirectX::XMStoreFloat4x4(&frame_resource.cbv_data[i], BoneTransform);
			
			}
		}
		else
		{
			frame_resource.cbv_data[0] = m_nodes.at(res_mesh->nodeIndex).world_transform;
		}

		frame_resource.constantBuffer->CopyToVRAM(graphics, frame_resource.cbv_data.data());
	}


}
void Model::ChangeStatic(const DirectX::XMFLOAT4X4& transform)
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");
	staticFlg = true;

	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : m_nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.local_transform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.world_transform, WorldTransform);
	}

	//	Graphics& graphics = Graphics::Instance();
	for (Mesh& mesh : m_meshes)
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;

		for (int i = 0; i < graphics->GetBufferCount(); i++)
		{
			FrameResource& frame_resource = mesh.frame_resources.at(i);
			
			if (res_mesh->nodeIndices.size() > 0)
			{
				for (size_t i = 0; i < res_mesh->nodeIndices.size(); ++i)
				{
					DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_nodes.at(res_mesh->nodeIndices.at(i)).world_transform);
					DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&res_mesh->offsetTransforms.at(i));
					DirectX::XMMATRIX BoneTransform = OffsetTransform * WorldTransform;
					DirectX::XMStoreFloat4x4(&frame_resource.cbv_data[i], BoneTransform);

				}
			}
			else
			{
				frame_resource.cbv_data[0] = m_nodes.at(res_mesh->nodeIndex).world_transform;
			}

			
		}
	
	}

	for (int i = 0; i < GetResource()->GetInstanceModelIndex().size(); i++)
	{

		const std::vector<int> &instanceList =GetResource()->GetInstanceModelIndex().at(i).data;
		int size = GetMeshes().size();
		Model::Mesh& mesh = GetMeshes().at(instanceList.at(0));
		
		for (int k = 0; k < graphics->GetBufferCount(); k++)
		{
			FrameResource& frame_resource = mesh.frame_resources.at(k);

			std::vector<DirectX::XMFLOAT4X4> &firstInstance = frame_resource.cbv_data;
			//frame_resource.cbv_data.resize(500);
			for (int j = 0; j < instanceList.size(); j++)
			{
				firstInstance[j] = GetMeshes().at(instanceList.at(j)).frame_resources.at(k).cbv_data[0];
			}
			//mesh.frame_resources.at(i).cbv_data[j] =
			frame_resource.constantBuffer->CopyToVRAM(graphics, frame_resource.cbv_data.data());
		}
	
		
	}
	
}

void Model::SetStaticTranceform(const DirectX::XMFLOAT4X4& transform)
{

}

// アニメーション再生
void Model::PlayAnimation(int animationIndex, bool loop, int lowerBobyAnimation , bool lowerLoop, float blendSconds)
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");

	bobyAnimationSetting.m_current_animation = animationIndex;
	bobyAnimationSetting.m_loop_animation = loop;
	bobyAnimationSetting.m_current_seconds = 0.0f;
	bobyAnimationSetting.m_end_animation = false;
	if (lowerBobyAnimation == -1)
	{
		lowerBobyAnimation = animationIndex;
		lowerLoop = loop;
	}

	lowerbobyAnimationSetting.m_current_animation = lowerBobyAnimation;
	lowerbobyAnimationSetting.m_loop_animation = lowerLoop;
	lowerbobyAnimationSetting.m_current_seconds = 0.0f;
	bobyAnimationSetting.m_end_animation = false;


	animationBlendSeconds = blendSconds;
	animationBlendTime = 0.0f;
	

}

// アニメーション停止
void Model::StopAnimation()
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");

	bobyAnimationSetting.m_current_animation = -1;
	bobyAnimationSetting.m_current_animation = -1;
}

void Model::UpperBobyUpdateAnimation(float elapsedTime, AnimationSetting& animationSetting)
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");

	if (animationSetting.m_current_animation < 0)
	{
		return;
	}

	if (m_resource->GetAnimations().empty())
	{
		return;
	}
	//ブレンド率
	float blendRate = 1.0f;
	if (animationBlendTime < blendRate)
	{
		blendRate = Mathf::Lerp(animationBlendTime, blendRate, animationBlendSeconds);
	}

	const ModelResource::Animation& animation = m_resource->GetAnimations().at(animationSetting.m_current_animation);

	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (animationSetting.m_current_seconds >= keyframe0.seconds && animationSetting.m_current_seconds < keyframe1.seconds)
		{
			float rate = (animationSetting.m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
			assert(m_nodes.size() == keyframe0.nodeKeys.size());
			assert(m_nodes.size() == keyframe1.nodeKeys.size());
			int nodeCount = static_cast<int>(m_nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ２つのキーフレーム間の補完計算
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);


				Node& node = m_nodes[nodeIndex];
				if (node.lowerBobyFlg == 1)continue;
				if (blendRate < 1.0f)
				{
					//モーションブレンド
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&node.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key1.translate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&node.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S1, S0, blendRate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R1, R0, blendRate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T1, T0, blendRate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}
				else
				{


					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}


				if (strcmp(originNodeName.c_str(), node.name) == 0)
				{
					node.translate.x = 0;
					node.translate.z = 0;
				}

				if (node.lowerBobyFlg == 1)
				{
					node.rotate = { 0,0,0,0 };
					node.translate.x = 0;
					node.translate.z = 0;
				}
			}
			break;
		}
	}

	// 最終フレーム処理
	if (animationSetting.m_end_animation)
	{
		animationSetting.m_end_animation = false;
		animationSetting.m_current_animation = -1;
		return;
	}

	// 時間経過
	animationSetting.m_current_seconds += elapsedTime;

	if (animationSetting.m_current_seconds >= animation.secondsLength)
	{
		if (animationSetting.m_loop_animation)
		{
			animationSetting.m_current_seconds -= animation.secondsLength;
		}
		else
		{
			animationSetting.m_current_seconds = animation.secondsLength;
			animationSetting.m_end_animation = true;
		}
	}
}

void Model::LowerBobyUpdateAnimation(float elapsedTime, AnimationSetting& animationSetting)
{
	if (staticFlg == true)assert(!"このモデルはスタティックなモデルです。");

	if (animationSetting.m_current_animation < 0)
	{
		return;
	}

	if (m_resource->GetAnimations().empty())
	{
		return;
	}
	//ブレンド率
	float blendRate = 1.0f;
	if (animationBlendTime < blendRate)
	{
		blendRate = Mathf::Lerp(animationBlendTime, blendRate, animationBlendSeconds);
	}

	const ModelResource::Animation& animation = m_resource->GetAnimations().at(animationSetting.m_current_animation);

	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (animationSetting.m_current_seconds >= keyframe0.seconds && animationSetting.m_current_seconds < keyframe1.seconds)
		{
			float rate = (animationSetting.m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
			assert(m_nodes.size() == keyframe0.nodeKeys.size());
			assert(m_nodes.size() == keyframe1.nodeKeys.size());
			int nodeCount = static_cast<int>(m_nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ２つのキーフレーム間の補完計算
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);


				Node& node = m_nodes[nodeIndex];
				if (node.lowerBobyFlg != 1)continue;
				if (blendRate < 1.0f)
				{
					//モーションブレンド
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&node.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key1.translate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&node.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S1, S0, blendRate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R1, R0, blendRate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T1, T0, blendRate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}
				else
				{


					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);
				}


				if (strcmp(originNodeName.c_str(), node.name) == 0)
				{
					node.translate.x = 0;
					node.translate.z = 0;
				}

			
			}
			break;
		}
	}

	// 最終フレーム処理
	if (animationSetting.m_end_animation)
	{
		animationSetting.m_end_animation = false;
		animationSetting.m_current_animation = -1;
		return;
	}

	// 時間経過
	animationSetting.m_current_seconds += elapsedTime;

	if (animationSetting.m_current_seconds >= animation.secondsLength)
	{
		if (animationSetting.m_loop_animation)
		{
			animationSetting.m_current_seconds -= animation.secondsLength;
		}
		else
		{
			animationSetting.m_current_seconds = animation.secondsLength;
			animationSetting.m_end_animation = true;
		}
	}
}

// アニメーション計算
void Model::UpdateAnimation(float elapsedTime)
{
	UpperBobyUpdateAnimation(elapsedTime, bobyAnimationSetting);
	LowerBobyUpdateAnimation(elapsedTime, lowerbobyAnimationSetting);

	animationBlendTime += elapsedTime;
}

void Model::SetOriginNode(const string& name)
{
	originNodeName = name;
}

void Model::SetLowerBody(const string& name)
{
	Node* node = SearchNode(name);
	node->lowerBobyFlg = 1;
	SetLowerBody(node);
}

void Model::SetLowerBody(Node* node)
{
	node->lowerBobyFlg = 1;
	for (int i = 0; i < node->children.size(); i++)
	{
		SetLowerBody(node->children.at(i));
	}
}

float& Model::GetAnimationSeconds()
{

	if (bobyAnimationSetting.m_current_animation < 0)
	{

		assert(L"bobyAnimationSetting.m_current_animationが０未満");
		return bobyAnimationSetting.m_current_seconds;
	}

	if (m_resource->GetAnimations().empty())
	{

		assert(L"アニメーションデータがありません。");
		return bobyAnimationSetting.m_current_seconds;
	}

	
	return bobyAnimationSetting.m_current_seconds;


}

float  Model::GetAnimationSecondsLength()
{
	if (bobyAnimationSetting.m_current_animation < 0)
	{

		assert(L"bobyAnimationSetting.m_current_animationが０未満");
		return bobyAnimationSetting.m_current_seconds;
	}

	if (m_resource->GetAnimations().empty())
	{

		assert(L"アニメーションデータがありません。");
		return bobyAnimationSetting.m_current_seconds;
	}
	const ModelResource::Animation& animation = m_resource->GetAnimations().at(bobyAnimationSetting.m_current_animation);
	return animation.secondsLength;
}

Model::Node* Model::SearchNode(const string& searchName)
{
	for (Node& node : m_nodes)
	{
		if (node.name == searchName)
			return &node;
		
	}
	return nullptr;
}

void Model::Create(Graphics* graphics)
{
	saveGraphics = graphics;
	ID3D12Device* d3d_device = graphics->GetDevice();

	HRESULT hr = S_OK;

	// ノード
	const std::vector<ModelResource::Node>& res_nodes = m_resource->GetNodes();

	m_nodes.resize(res_nodes.size());
	for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); ++nodeIndex)
	{
		auto&& src = res_nodes.at(nodeIndex);
		auto&& dst = m_nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &m_nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// メッシュ
	const std::vector<ModelResource::Mesh>& res_meshes = m_resource->GetMeshes();

	m_meshes.resize(res_meshes.size());

	for (size_t mesh_index = 0; mesh_index < m_meshes.size(); ++mesh_index)
	{
		auto&& src_mesh = res_meshes.at(mesh_index);
		auto&& dst_mesh = m_meshes.at(mesh_index);

		dst_mesh.mesh = &src_mesh;
		dst_mesh.frame_resources.resize(graphics->GetBufferCount());

		// フレームリソース
		for (FrameResource& frame_resource : dst_mesh.frame_resources)
		{
			UINT bone_num = static_cast<UINT>(src_mesh.nodeIndices.size());
			if (bone_num == 0) bone_num = 1;
			//frame_resource.cbv_data.resize(bone_num);
			frame_resource.cbv_data.resize(500);

			frame_resource.constantBuffer = new Lib_ConstantBuffer(graphics, sizeof(DirectX::XMFLOAT4X4) * 500,&(frame_resource.cbv_data.at(0)));
		}
	}

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}


void  Model::RayPick(const XMFLOAT3& start, const XMFLOAT3& end, RayOut& ray)
{
	XMVECTOR WorldStart = XMLoadFloat3(&start);
	XMVECTOR WorldEnd = XMLoadFloat3(&end);
	XMVECTOR WorldRayVec = XMVectorSubtract(WorldEnd, WorldStart);
	XMVECTOR WorldRayLength = XMVector3Length(WorldRayVec);
	//ワールド座標のレイの長さ
	XMStoreFloat(&ray.distance, WorldRayLength);

	bool hit = false;
	for (const ModelResource::Mesh& mesh : GetResource()->GetMeshes())
	{
		//メッシュノード取得
		const Node& node = GetNodes().at(mesh.nodeIndex);

		//レイをワールド空間かわローカル空間に変換する
		XMMATRIX WorldTranceform = XMLoadFloat4x4(&node.world_transform);
		XMMATRIX InverseWorldTranceform = XMMatrixInverse(nullptr, WorldTranceform);
		//ローカル空間
		XMVECTOR Start = XMVector3TransformCoord(WorldStart, InverseWorldTranceform);
		XMVECTOR End = XMVector3TransformCoord(WorldEnd, InverseWorldTranceform);
		XMVECTOR Vec = XMVectorSubtract(End, Start);
		XMVECTOR Dir = XMVector3Normalize(Vec);
		XMVECTOR Length = XMVector3Length(Vec);
		//レイの長さ
		float neart;
		XMStoreFloat(&neart, Length);

		//三角形（面）との当たり判定
		const std::vector<ModelResource::Vertex>& vetices = mesh.polygonVertexList;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		XMVECTOR HitPosition;
		XMVECTOR HitNormalize;
		XMVECTOR PolygonPostion[3];

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
				materialIndex = subset.materialIndex;
				PolygonPostion[0] = A;
				PolygonPostion[1] = B;
				PolygonPostion[2] = C;
			}

		}



		if (materialIndex >= 0)
		{
			//ローカル空間からワールド空間に変換する
			XMVECTOR WorldPosition = XMVector3TransformCoord(HitPosition, WorldTranceform);
			XMVECTOR WorldCrossVec = XMVectorSubtract(WorldPosition, WorldStart);
			XMVECTOR WorldCrossLength = XMVector3Length(WorldCrossVec);
			float distance;
			XMStoreFloat(&distance, WorldCrossLength);

			//保存
			if (ray.distance > distance)
			{
				XMVECTOR WorldNormal = XMVector3TransformNormal(HitNormalize, WorldTranceform);

				XMVECTOR WorldPolygonPostion[3];
				WorldPolygonPostion[0] = XMVector3TransformCoord(PolygonPostion[0], WorldTranceform);
				WorldPolygonPostion[1] = XMVector3TransformCoord(PolygonPostion[1], WorldTranceform);
				WorldPolygonPostion[2] = XMVector3TransformCoord(PolygonPostion[2], WorldTranceform);

				ray.distance = distance;
				ray.materialIndex = materialIndex;
				XMStoreFloat3(&ray.Pos, WorldPosition);
				XMStoreFloat3(&ray.Normal, XMVector3Normalize(WorldNormal));

				XMStoreFloat3(&ray.PolygonPos[0], WorldPolygonPostion[0]);
				XMStoreFloat3(&ray.PolygonPos[1], WorldPolygonPostion[1]);
				XMStoreFloat3(&ray.PolygonPos[2], WorldPolygonPostion[2]);
				hit = true;
			}
		}
	}

}
