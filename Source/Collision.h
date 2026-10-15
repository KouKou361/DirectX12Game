#pragma once
#include "Lib.h"
#include "Lib_Model.h"

class SceneGame;

class Collision
{
	
public:
	Collision(SceneGame* sceneGame) { this->sceneGame = sceneGame; }
	//プレイヤーの攻撃と敵のの当たり判定
	void PlayerAttackVsEnemy(const VECTOR3& nodePos, const float attackCollision);

	//プレイヤーの攻撃と敵のの当たり判定
	void EnemyAttackVsPlayer(const VECTOR3& nodePos, const float attackCollision);


private:
	SceneGame* sceneGame=nullptr;
};