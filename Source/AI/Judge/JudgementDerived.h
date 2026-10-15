#pragma once
#include "JudgementBase.h"

class EnemyBase;


//待機行動
class IdleJudgement :public JudgementBase
{
public:
	IdleJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

// 追跡行動
class PursuitJudgement : public JudgementBase
{
public:
	PursuitJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

// 攻撃行動
class AttackJudgement : public JudgementBase
{
public:
	AttackJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

//ダメージ行動
class DamageJudgement : public JudgementBase
{
public:
	DamageJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

//死亡行動
class DeadJudgement : public JudgementBase
{
public:
	DeadJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

//Boss専用
class BossRollAttackJudgement :public JudgementBase
{
public:
	BossRollAttackJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};

//Boss専用
class BossEntryJudgement :public JudgementBase
{
public:
	BossEntryJudgement(EnemyBase* enemy) :JudgementBase(enemy) {}
	//実行処理
	bool Judgement();
};





