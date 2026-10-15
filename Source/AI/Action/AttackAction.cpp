#include "ActionDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "../Source/NavMesh.h"
#include "../Source/Collision.h"
#include "../Source/Scene/SceneBase.h"

class EnemyBase;

//開始処理
void AttackAction::Start()
{
	owner->ResetMoveTarget();

	//アニメーションの取得
	const int animetionIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Attack1);
	owner->GetModel()->PlayAnimation(animetionIndex);

	owner->moveFlg = false;
}

//実行処理
ActionBase::State AttackAction::Run()
{
	//攻撃
	AttackCollision();

	if (owner->GetModel()->IsPlayAnimation()==FALSE)
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}

	//行動の続行
	return ActionBase::State::RUN;
}

//終了処理
void AttackAction::End()
{
	owner->moveFlg = FALSE;
}

//Imguiデバッグ
void AttackAction::DebugImgui()
{
	ImGui::Begin("EnemyData");
	ImGui::Text("AttackAction");
	ImGui::End();

	if (owner->GetModel()->GetAnimationSeconds() <= 0.25f)return;
	if (owner->GetModel()->GetAnimationSeconds() >= 0.30f)return;

	const Model::Node* node = owner->GetModel()->SearchNode(owner->AttackNode());
	TK_Lib::Debug3D::Circle({ node->world_transform._41,node->world_transform._42,node->world_transform._43 }, 5.0f);
}

//攻撃
void AttackAction::AttackCollision()
{
	if (owner->GetModel()->GetAnimationSeconds() <= 0.25f)return;
	if (owner->GetModel()->GetAnimationSeconds() >= 0.30f)return;

	const Model::Node* node = owner->GetModel()->SearchNode(owner->AttackNode());
	sceneGame->collision->EnemyAttackVsPlayer({ node->world_transform._41,node->world_transform._42,node->world_transform._43 }, owner->GetAtttackCollision());
}
