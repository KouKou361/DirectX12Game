#include "ActionDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "../Source/NavMesh.h"

class EnemyBase;

//開始処理
void PursuitAction::Start()
{
	owner->ResetMoveTarget();

	//アニメーションの取得
	const int animetionIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Run);
	owner->GetModel()->PlayAnimation(animetionIndex, TRUE);
	stalkerTime = 5.0f;

	
	owner->moveFlg = true;

	
}

//実行処理
ActionBase::State PursuitAction::Run()
{

	owner->SetTargetPos(owner->GetTarget()->GetPos());

	if (owner->moveFlg == false)
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}
	//攻撃範囲
	if (owner->SearchPosition(owner->GetAttackLength(), owner->GetTarget()->GetPos()))
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}


	//視認範囲外なら
	if (owner->SearchPosition(owner->GetPursuitLength(), owner->GetTarget()->GetPos()))
	{
		stalkerTime = 5.0f;
	
	}
	else
	{
		stalkerTime -= TK_Lib::Window::GetElapsedTime();
	}
	

	if (stalkerTime <= 0)
	{
		//行動終了！次の行動に移行
		return ActionBase::State::FAILED;
	}
	
	//行動の続行
	return ActionBase::State::RUN;
}

//終了処理
void PursuitAction::End()
{
	owner->moveFlg = FALSE;
}

//Imguiデバッグ
void PursuitAction::DebugImgui()
{
	ImGui::Begin("Action");
	ImGui::Text("StalkerAction");
	ImGui::Text("StalkerAction =%f", stalkerTime);
	ImGui::End();

	//目的地
	TK_Lib::Debug3D::Circle(owner->GetTarget()->GetPos(), 3.0f);
	//攻撃範囲の描画
	TK_Lib::Debug3D::Cylinder(owner->GetPos(), 2.0f, owner->GetAttackLength(), { 1,0,0,1 });
	//追跡範囲の描画
	TK_Lib::Debug3D::Cylinder(owner->GetPos(), 2.0f, owner->GetPursuitLength(), { 1,1,0,1 });
}