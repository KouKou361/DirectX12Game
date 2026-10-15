#pragma once
#include "ActionBase.h"
#include "Lib.h"
class EnemyBase;

//待機
class WaitAction:public ActionBase
{
public:
	WaitAction(EnemyBase* enemy, SceneGame* sceneGame) : ActionBase(enemy, sceneGame) {}

	//開始処理
	void Start();

	//実行処理
	ActionBase::State Run();

	//終了処理
	void End();

	//Imguiデバッグ
	void DebugImgui();

private:
	float waitTime = 0;
};

//徘徊
class WalkAction :public ActionBase
{
public:
	WalkAction(EnemyBase* enemy, SceneGame* sceneGame) : ActionBase(enemy, sceneGame) {}

	//開始処理
	void Start();

	//実行処理
	ActionBase::State Run();

	//終了処理
	void End();

	//Imguiデバッグ
	void DebugImgui();

private:
	VECTOR3 targetpos;
};

//ストーカー
class PursuitAction :public ActionBase
{
public:
	PursuitAction(EnemyBase* enemy, SceneGame* sceneGame) : ActionBase(enemy, sceneGame) {}

	//開始処理
	void Start();

	//実行処理
	ActionBase::State Run();

	//終了処理
	void End();

	//Imguiデバッグ
	void DebugImgui();

private:
	float stalkerTime = 5.0f;
};


//ストーカー
class AttackAction :public ActionBase
{
public:
	AttackAction(EnemyBase* enemy, SceneGame* sceneGame) : ActionBase(enemy, sceneGame) {}

	//開始処理
	void Start();

	//実行処理
	ActionBase::State Run();

	//終了処理
	void End();

	//Imguiデバッグ
	void DebugImgui();
	
	//攻撃
	void AttackCollision();
private:
	float stalkerTime = 5.0f;
};


//死亡
class DeadAction :public ActionBase
{
public:
	DeadAction(EnemyBase* enemy, SceneGame* sceneGame) : ActionBase(enemy, sceneGame) {}

	//開始処理
	void Start();

	//実行処理
	ActionBase::State Run();

	//終了処理
	void End();

	//Imguiデバッグ
	void DebugImgui();

private:
	float stalkerTime = 5.0f;
};