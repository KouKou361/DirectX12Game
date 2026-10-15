#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "PlayerState.h"
#include "Player.h"

//‰Šú‰»
void DamageState ::Start()
{
	const int animationIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Damage);
	owner->GetModel()->PlayAnimation(animationIndex, false);
}
//XV
void DamageState::Run()
{
	owner->moveVec.x = 0;
	owner->moveVec.z = 0;

	if (owner->GetModel()->IsPlayAnimation())return;

	owner->ChangeState(Player::State::WAIT);
}
//I—¹
void DamageState::End()
{

}

//Imgui
void DamageState::DebugImgui()
{

}