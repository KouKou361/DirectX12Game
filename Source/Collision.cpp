#include "Collision.h"
#include "../Source/Character/Player.h"
#include "../Source/Character/Enemy/EnemyBase.h"
#include "../Source/Character/Enemy/EnemyManager.h"
#include "../Source/Scene/SceneBase.h"


void Collision::PlayerAttackVsEnemy(const VECTOR3 &nodePos,const float attackCollision)
{
	for (int i = 0; i < sceneGame->enemyManager->enemyList.size(); i++)
	{
		EnemyBase* enm = sceneGame->enemyManager->enemyList.at(i);

		if (enm == nullptr)continue;
		if (TK_Lib::Collision::CircleVsCircle(nodePos, attackCollision, enm->GetPos(), enm->GetDamageCollision()))
		{
			enm->OnDamage(1, 1.0f);
		}
	}
}

//ƒvƒŒƒCƒ„[‚ÌUŒ‚‚Æ“G‚Ì‚Ì“–‚½‚è”»’è
void Collision::EnemyAttackVsPlayer(const VECTOR3& nodePos, const float attackCollision)
{
	Player* player = sceneGame->player;
	if (player == nullptr)return;
	if (TK_Lib::Collision::CircleVsCircle(nodePos, attackCollision, player->GetPos(), player->GetDamageCollision()))
	{
		player->OnDamage(1, 1.0f);
	}
}

