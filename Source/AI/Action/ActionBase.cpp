#include "ActionBase.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "../Source/Scene/SceneBase.h"

ActionBase::ActionBase(EnemyBase* enemy,SceneGame* sceneGame)
{
	owner = enemy;
	this->sceneGame = sceneGame;
}

//もし目標物が消えてしまった場合
ActionBase::State ActionBase::DeleteTarget()
{
	return State::FAILED;
}

//もしターゲットがいなくなっているかどうか
bool ActionBase::CheakTarget(const CharacterBase* chara)
{
	return false;
}
