#pragma once
class EnemyBase;
class CharacterBase;
class SceneGame;

//行動処理基底クラス
class  ActionBase
{
public:
	ActionBase(){}
	ActionBase(EnemyBase* enemy, SceneGame* sceneGame);

	
	virtual ~ActionBase() {}
	// 実行情報
	enum class State
	{
		RUN,//実行中
		FAILED,		// 実行失敗
		COMPLETE,	// 実行成功
	};

	//実行開始処理
	virtual void Start() = 0;

	//実行処理
	virtual ActionBase::State Run() = 0;

	//実行終了処理
	virtual void End() = 0;
	//Imgui
	virtual void DebugImgui()=0;
	//デバッグ用
	virtual void DebugRender() {};

	//もし目標物が消えてしまった場合
	virtual ActionBase::State DeleteTarget();

	//もしターゲットがいなくなっているかどうか
	bool CheakTarget(const CharacterBase* chara);



protected:
	EnemyBase* owner=nullptr;
	SceneGame* sceneGame = nullptr;
};
