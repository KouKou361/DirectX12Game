#pragma once
#include "CharacterBase.h"

class dtCrowd;
class PlayerState;

class PlayerState;
class AttackState1;
class AttackState2;
class AttackState3;
class DamageState;
class DeadState;
class WaitState;
class RunState;
class RollingState;
class UI_TargetEnemyCursor;


class Player:public CharacterBase
{
private:
	friend class PlayerState;
	friend class AttackState1;
	friend class AttackState2;
	friend class AttackState3;
	friend class DamageState;
	friend class DeadState;
	friend class WaitState;
	friend class RunState;
	friend class RollingState;

	enum State
	{
		NONE,
		WAIT,
		RUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		DEAD,
		DAMAGE,
		JUMP,
		ROLLING,
	};
public:
	Player(SceneGame* sceneGame);
	~Player();
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Render();
	//アニメーション設定
	void SetAnimationData()override;
	//行動変更
	void ChangeState(const State& state);
	//被ダメージ
	bool OnDamage(const int& damage, const float& invincibilityTime) override;

	void LockOnTarget();

private:
	//移動取得
	void Input();

	//平行移動
	void ParallelMove()override;





private:
	PlayerState* currentState = nullptr;


	State state = NONE;

	AttackState1* attackState1 =nullptr;
	AttackState2* attackState2 = nullptr;
	AttackState3* attackState3 = nullptr;

	DamageState* damageState = nullptr;
	DeadState* deadState = nullptr;
	WaitState* waitState = nullptr;
	RunState* runState = nullptr;
	RollingState* rollingState = nullptr;
	UI_TargetEnemyCursor* uiTargetEnemyCursor = nullptr;

};