#include "EnemyBase.h"
#include "Lib_Model.h"
#include "../Source/Mathf.h"
#include "../Source/AI/BehaviorData.h"
#include "../Source/AI/BehaviorTree.h"
#include "../Source/AI/NodeBase.h"

#include "../Source/Character/Player.h"
#include "../Source/Scene/SceneBase.h"

#include "../Source/Scene/SceneBase.h"
#include "../Source/NavMesh.h"

#include "imgui.h"


EnemyBase::EnemyBase(SceneGame* sceneGame) :CharacterBase(sceneGame)
{
	SetTarget(sceneGame->player);

}

EnemyBase::~EnemyBase()
{
	CharacterBase::~CharacterBase();

	if (behaviorTree != nullptr)
	{
		delete behaviorTree;
		behaviorTree = nullptr;
	}

	if (behaviordata != nullptr)
	{
		delete behaviordata;
		behaviordata = nullptr;
	}

}
//初期化
void EnemyBase::Initialize()
{
	modelData = TK_Lib::Load::model("./Data/Model/Slime/Slime.mdl");
	modelData->PlayAnimation(10, true);
	scale = { 0.05f,0.05f, 0.05f };
	moveSpeed = 130;
	XMVECTOR Qu = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	XMStoreFloat4(&quaternion, Qu);
	SetQuaternion(quaternion);
	CharacterBase::Initialize();
}
//更新処理
void EnemyBase::Update()
{
	//ビヘイビアツリーの行動処理
	ActionBehaviorTree();

	CharacterBase::Update();
}
//描画処理
void EnemyBase::Render()
{
	ImguiDebug();

	CharacterBase::Render();
}

//平行移動
void EnemyBase::ParallelMove()
{
	if (moveFlg == false)return;
	if (navAgentIndex <= -1)return;
	if (sceneGame->navMesh == nullptr)return;

	const dtCrowdAgent* ag = sceneGame->navMesh->GetCrowd()->getAgent(navAgentIndex);
	if (!ag->active) return;

	dtCrowd* crowd = sceneGame->navMesh->GetCrowd();

	const dtQueryFilter* filter = crowd->getFilter(0);
	const float* halfExtents = crowd->getQueryExtents();
	dtPolyRef polyRef;
	// 指定ポイントへ移動
	dtNavMeshQuery* navQuery = sceneGame->navMesh->GetNavMeshQuery();

	navQuery->findNearestPoly(&targetPos.x, halfExtents, filter, &polyRef, 0);

	if (sceneGame->navMesh->GetCrowd()->requestMoveTarget(navAgentIndex, polyRef, &targetPos.x))
	{
		const float* pos = ag->npos;
		SetPos({ pos[0],pos[1],pos[2] });

		const float* vel = ag->nvel;
		moveVec = { vel[0],vel[1] ,vel[2] };
		
	}
	else
	{
		//もしいけないと判断した場合
		moveFlg = false;
	}
	


}
//ビヘイビアツリーの行動処理
void EnemyBase::ActionBehaviorTree()
{

	//もし行動決まっていないなら新しく思考する。
	if (actionNode == nullptr)
	{

		actionNode = behaviorTree->ActiveNodeInference(this, behaviordata);
		//if(ActionNode)ActionNode->ImguiDebug();
	}
	//行動を開始
	if (actionNode != nullptr)
	{
		actionNode->ImguiDebug();
		actionNode = behaviorTree->Run(this, actionNode, behaviordata);
		
	}


}

//Imguiデバッグ用
void EnemyBase::ImguiDebug()
{
	ImGui::Begin("Action");
	ImGui::Text("StalkerAction");
	ImGui::Text("hp =%d", hp);
	ImGui::Text("invincibilityTime =%f", invincibilityTime);
	ImGui::End();
}


//索敵範囲にTargetがいればTRUEを返す
bool EnemyBase::SearchTarget(const float& l)
{
	CharacterBase* Target = GetTarget();
	//もし目標がいなかったらエラー
	if (Target == nullptr)
	{
		assert(!L"No Target!");
		return false;
	}
	//索敵範囲にいるかどうかを判断
	return SearchPosition(l, Target->GetPos());

}

//索敵範囲に目標地点がいればTRUEを返す
bool EnemyBase::SearchPosition(const float& L, const VECTOR3& Position)
{
	//目標までの距離を算出
	float TargetL;
	{
		VECTOR3 vec = pos - Position;
		XMVECTOR Vec = XMLoadFloat3(&vec);
		Vec = XMVector3Length(Vec);
		XMStoreFloat(&TargetL, Vec);
	}
	//目標までの距離と比べる
	return TargetL <= L;

}