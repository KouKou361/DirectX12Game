#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "PlayerState.h"
#include "Player.h"
#include "../Source/Mathf.h"


//‰Šú‰»
void RollingState::Start()
{
	const int animationIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Rolling);
	owner->GetModel()->PlayAnimation(animationIndex, false);
}
//XV
void RollingState::Run()
{
	float animationTIme = owner->GetModel()->GetAnimationSeconds();
	owner->moveSpeed = Mathf::OutQuint(animationTIme , owner->GetModel()->GetAnimationSecondsLength(), 150.0f, 50.0f);
	owner->moveSpeed -= Mathf::Linear(animationTIme, owner->GetModel()->GetAnimationSecondsLength(), 150.0f, 50.0f);

	if (owner->GetModel()->IsPlayAnimation())return;

	owner->ChangeState(Player::State::WAIT);
}
//I—¹
void RollingState::End()
{
	owner->moveSpeed = 50;
}

//Imgui
void RollingState::DebugImgui()
{

}