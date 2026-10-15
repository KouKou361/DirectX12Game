#include "EnemyManager.h"
#include "EnemyBase.h"
//初期化
void EnemyManager::Initialize()
{
	enemyList.clear();
	enemyList.shrink_to_fit();

	removeList.clear();
	removeList.shrink_to_fit();

	enemyList.resize(128);

	for (EnemyBase* enemy : enemyList)
	{
		enemy = nullptr;
	}

	{
		EnemySlime* enm = new EnemySlime(sceneGame);
		enm->SetPos({ 30,0,0 });
		enm->Initialize();
		RegisterEnemy(enm);
	}

	{
		EnemyTurleShell* enm = new EnemyTurleShell(sceneGame);
		enm->SetPos({ 0,0,30 });
		enm->Initialize();
		RegisterEnemy(enm);
	}

	{
		EnemyBeholder* enm = new EnemyBeholder(sceneGame);
		enm->SetPos({ 30,0,30 });
		enm->Initialize();
		RegisterEnemy(enm);
	}

	{
		EnemyChest* enm = new EnemyChest(sceneGame);
		enm->SetPos({ 30,0,60 });
		enm->Initialize();
		RegisterEnemy(enm);
	}


}
//更新
void EnemyManager::Update()
{
	for (EnemyBase* enemy : enemyList)
	{
		if (enemy == nullptr)continue;
		enemy->Update();
	}

	//データ削除
	for (EnemyBase* removeEnemy : removeList)
	{
		if (removeEnemy == nullptr)continue;

		for (int i=0;i<enemyList.size();i++)
		{
			EnemyBase* enemy  =  enemyList.at(i);
			if (enemy == nullptr)continue;
			if (enemy == removeEnemy)
			{
				delete enemyList.at(i);
				enemyList.at(i) = nullptr;
				break;
			}
		}
		
		removeEnemy = nullptr;
	}

	removeList.clear();
}
//描画
void EnemyManager::Render()
{
	for (EnemyBase* enemy : enemyList)
	{
		if (enemy == nullptr)continue;
		enemy->Render();
	}

	
}


//終了
void EnemyManager::End()
{
	for (EnemyBase* enemy: enemyList)
	{
		if (enemy == nullptr)continue;
		delete enemy;
		enemy = nullptr;
	}

	for (EnemyBase* enemy : removeList)
	{
		if (enemy == nullptr)continue;
		delete enemy;
		enemy = nullptr;
	}

	enemyList.clear();
	enemyList.shrink_to_fit();

	removeList.clear();
	removeList.shrink_to_fit();
}

//敵生成
void EnemyManager::RegisterEnemy(EnemyBase* enemy)
{
	for (int i=0;i<enemyList.size();i++)
	{
		const EnemyBase* enm = enemyList.at(i);
		if (enm == nullptr)
		{
			enemyList.at(i) = enemy;
			enemyList.at(i)->SetRegisterIndex(i);
			return;
		}
	}

}
//敵削除
void EnemyManager::RemoveEnemy(EnemyBase* enemy)
{
	removeList.emplace_back(enemy);
}
