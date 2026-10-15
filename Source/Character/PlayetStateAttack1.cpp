#include "imgui.h"
#include "../Source/Animetion.h"
#include "Lib_Model.h"
#include "PlayerState.h"
#include "Player.h"
#include "../Source/Scene/SceneBase.h"
#include "../Source/Collision.h"


//‰Šú‰»
void AttackState1::Start()
{
	const int animationIndex = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Attack1);
	const int animationIndex2 = owner->GetAnimation()->GetIndex(owner->GetAnimation()->Run);
	//owner->GetModel()->PlayAnimation(animationIndex, false, animationIndex2,true);
	owner->GetModel()->PlayAnimation(animationIndex, false);

	nextAttackFlg = false;

	owner->moveSpeed = 10.0f;
	owner->turnSpeed = XMConvertToRadians(2);
	
}
//XV
void AttackState1::Run()
{

	//UŒ‚‚Ì“–‚½‚è”»’è
	AttackCollision();

	if (TK_Lib::KeyInput::GetButtonDown(BTN::X) == 1)
	{
		nextAttackFlg = TRUE;
	}



	if (owner->GetModel()->IsPlayAnimation())return;

	if (nextAttackFlg == TRUE)
	{
		//ŽŸ‚ÌUŒ‚‚ÉˆÚs
		owner->ChangeState(Player::State::ATTACK2);
	}
	else
	{
		owner->ChangeState(Player::State::WAIT);
	}
}
//I—¹
void AttackState1::End()
{
	owner->moveSpeed = 50.0f;
	owner->turnSpeed = XMConvertToRadians(10);
}
//Imgui
void AttackState1::DebugImgui()
{
	if (owner->modelData->GetAnimationSeconds() <= 0.55f)return;
	if (owner->modelData->GetAnimationSeconds() >= 0.75f)return;

	const Model::Node *node = owner->GetModel()->SearchNode("mixamorig:Sword_joint");
	TK_Lib::Debug3D::Circle({ node->world_transform._41,node->world_transform._42,node->world_transform._43}, 5.0f);
}

//UŒ‚
void AttackState1::AttackCollision()
{
	if (owner->modelData->GetAnimationSeconds() <= 0.55f)return;
	if (owner->modelData->GetAnimationSeconds() >= 0.75f)return;

	const Model::Node* node = owner->GetModel()->SearchNode("mixamorig:Sword_joint");
	sceneGame->collision->PlayerAttackVsEnemy({ node->world_transform._41,node->world_transform._42,node->world_transform._43 },owner->GetAtttackCollision());
}
