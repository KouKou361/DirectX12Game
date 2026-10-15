#pragma once
#include <vector>
class EnemyBase;
class SceneGame;
class EnemyManager
{
public:
	EnemyManager(SceneGame* sceneGame) { this->sceneGame = sceneGame; };
	~EnemyManager() {};

	//‰Šú‰»
	void Initialize();
	//XV
	void Update();
	//•`‰æ
	void Render();
	//I—¹
	void End();
	//“G¶¬
	void RegisterEnemy(EnemyBase* enemy);
	//“Gíœ
	void RemoveEnemy(EnemyBase* enemy);

	std::vector<EnemyBase*> enemyList;

private:


	std::vector<EnemyBase*> removeList;

	SceneGame* sceneGame = nullptr;
};