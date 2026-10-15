#include "ActionDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"

class EnemyBase;

//開始処理
void WalkAction::Start()
{
	owner->ResetMoveTarget();

	float angle = XMConvertToRadians(rand()%360);
	float targetX = owner->GetPos().x + sinf(angle) *100;
	float targetZ = owner->GetPos().z + cosf(angle) * 100;
	targetpos.x = targetX;
	targetpos.y = owner->GetPos().y;
	targetpos.z = targetZ;

	owner->SetTargetPos(targetpos);

	owner->moveFlg = TRUE;

	//アニメーションの取得
	const int animetionIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Run);
	owner->GetModel()->PlayAnimation(animetionIndex, TRUE);
}

//実行処理
ActionBase::State WalkAction::Run()
{

	if (owner->moveFlg == false)
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}

	//攻撃範囲に入っている。
	if (owner->SearchPosition(10,targetpos))
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}

	//ターゲットを発見した。
	if (owner->SearchPosition(owner->GetSearchLength(), owner->GetTarget()->GetPos()))
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}

	//行動の続行
	return ActionBase::State::RUN;
}

//終了処理
void WalkAction::End()
{
	owner->moveFlg = FALSE;
}

//Imguiデバッグ
void WalkAction::DebugImgui()
{

	ImGui::Begin("EnemyData");
	ImGui::Text("WalkAction");
	ImGui::Text("targetPos =%f,%f,%f,",targetpos.x, targetpos.y, targetpos.z);
	ImGui::End();

	//目的地
	TK_Lib::Debug3D::Circle(targetpos, 3.0f);

	//追跡範囲の描画
	TK_Lib::Debug3D::Cylinder(owner->GetPos(), 2.0f, owner->GetSearchLength(), { 1,1,0,1 });
}