#pragma once
class EnemyBase;

//行動処理判定基底クラス
class  JudgementBase
{
public:
	JudgementBase() {};
	JudgementBase(EnemyBase* enemy) :owner(enemy) {};

	//実行処理
	virtual bool Judgement() = 0;

protected:
	EnemyBase* owner=nullptr;
};

