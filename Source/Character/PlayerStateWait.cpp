#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "PlayerState.h"
#include "Player.h"

//初期化
void WaitState::Start()
{
	const int animationIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Idle);
	owner->GetModel()->PlayAnimation(animationIndex, true);
}
//更新
void WaitState::Run()
{
	if (TK_Lib::KeyInput::GetButtonDown(BTN::X) == 1)
	{
		//攻撃ステート以降
		owner->ChangeState(Player::State::ATTACK1);
	}

	if (TK_Lib::KeyInput::GetButtonDown(BTN::A) == 1)
	{
		//回避ステート以降
		owner->ChangeState(Player::State::ROLLING);
	}

	if (abs(owner->moveVec.x) > 0)
	{
		if (abs(owner->moveVec.z) > 0)
		{
			//走るステートに以降
			owner->ChangeState(Player::State::RUN);
		}
	}

}
//終了
void WaitState::End()
{

}

//Imgui
void WaitState::DebugImgui()
{

}