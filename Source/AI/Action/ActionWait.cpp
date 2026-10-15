#include "ActionDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "Lib.h"
#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
using namespace std;

class EnemyBase;

//開始処理
void WaitAction::Start()
{
	waitTime = (rand() % 3) + 2;

	owner->ResetMoveTarget();

	//アニメーションの取得
	const int IdeiAnimetion = owner->GetAnimation()->GetIndex (owner->GetAnimation()->Idle);
	owner->GetModel()->PlayAnimation(IdeiAnimetion,TRUE);
}

//実行処理
ActionBase::State WaitAction::Run()
{

	waitTime -= TK_Lib::Window::GetElapsedTime();

	if (waitTime <= 0)
	{
		waitTime = 0;
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
void WaitAction::End()
{
	waitTime = 0;
}

//Imguiデバッグ
void WaitAction::DebugImgui()
{

	

	ImGui::Begin("EnemyData");
	ImGui::Text("WaitAction");
	ImGui::Text("waitTime = %f", waitTime);
	ImGui::End();

	//追跡範囲の描画
	TK_Lib::Debug3D::Cylinder(owner->GetPos(), 2.0f, owner->GetSearchLength(), { 1,1,0,1 });
}