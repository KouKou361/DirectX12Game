#include "ActionDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "../Source/NavMesh.h"
#include "../Source/Scene/SceneBase.h"
#include "../Source/Character/Enemy/EnemyManager.h"

class EnemyBase;

//開始処理
void DeadAction::Start()
{
	owner->ResetMoveTarget();

	//アニメーションの取得
	const int animetionIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Die);
	owner->GetModel()->PlayAnimation(animetionIndex);

	owner->moveFlg = false;
}

//実行処理
ActionBase::State DeadAction::Run()
{
	if (owner->GetModel()->IsPlayAnimation() == FALSE)
	{
		sceneGame->enemyManager->RemoveEnemy(owner);
	}

	//行動の続行
	return ActionBase::State::RUN;
}

//終了処理
void DeadAction::End()
{
	owner->moveFlg = FALSE;
}

//Imguiデバッグ
void DeadAction::DebugImgui()
{
	ImGui::Begin("EnemyData");
	ImGui::Text("DeadAction");
	ImGui::End();
}