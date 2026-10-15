#pragma once
class Player;
class BossStage;
class SceneGame;

//プレイヤーのステートマシン(基底クラス)
class PlayerState
{
public:
	PlayerState(Player* pl);
	~PlayerState() {};

	//初期化
	virtual void Start() = 0;
	//更新
	virtual void Run() = 0;
	//終了
	virtual void End() = 0;

	//Imgui
	virtual void DebugImgui() = 0;

	Player* owner=nullptr;
	SceneGame* sceneGame = nullptr;
	
};

//プレイヤーの攻撃1クラス
class AttackState1 :public PlayerState
{
private:
	//攻撃フラグ（一度に沢山の出てほしくないため）
	bool attackFlg = false;
	//攻撃アニメーションのタイミング
	const float AttackTimeRate = 0.5f;
	//次の攻撃フラグ
	bool nextAttackFlg = false;
public:
	AttackState1(Player* pl) :PlayerState(pl){};
	~AttackState1() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();

	//Imgui
	void DebugImgui();

private:
	//攻撃
	void AttackCollision();
};

//プレイヤーの攻撃2クラス
class AttackState2 :public PlayerState
{
private:
	//攻撃フラグ（一度に沢山の出てほしくないため）
	bool attackFlg = false;
	//攻撃アニメーションのタイミング
	const float AttackTimeRate = 0.5f;
	//次の攻撃フラグ
	bool nextAttackFlg = false;
public:
	AttackState2(Player* pl) :PlayerState(pl) {};
	~AttackState2() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();

	//Imgui
	void DebugImgui();

private:
	//攻撃
	void AttackCollision();
};

//プレイヤーの攻撃3クラス
class AttackState3 :public PlayerState
{
private:
	//攻撃フラグ（一度に沢山の出てほしくないため）
	bool attackFlg = false;
	//攻撃アニメーションのタイミング
	const float AttackTimeRate = 0.5f;
public:
	AttackState3(Player* pl) :PlayerState(pl) {};
	~AttackState3() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();

	//Imgui
	void DebugImgui();

private:
	//攻撃
	void AttackCollision();
};

//プレイヤーのダメージクラス
class DamageState :public PlayerState
{
public:
	DamageState(Player* pl) :PlayerState(pl) {};
	~DamageState() {};
private:
	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();
	//Imgui
	void DebugImgui();
};

//プレイヤーの死亡クラス
class DeadState :public PlayerState
{
public:
	DeadState(Player* pl) :PlayerState(pl) {};
	~DeadState() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();
	//Imgui
	void DebugImgui();
};


//プレイヤーの待機クラス
class WaitState :public PlayerState
{
public:
	WaitState(Player* pl) :PlayerState(pl) {};
	~WaitState() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();
	//Imgui
	void DebugImgui();
};

//プレイヤーの移動クラス
class RunState :public PlayerState
{
public:
	RunState(Player* pl) :PlayerState(pl) {};
	~RunState() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();
	//Imgui
	void DebugImgui();
};

//プレイヤーのローリングクラス
class RollingState :public PlayerState
{
public:
	RollingState(Player* pl) :PlayerState(pl) {};
	~RollingState() {};

	//初期化
	void Start();
	//更新
	void Run();
	//終了
	void End();
	//Imgui
	void DebugImgui();
};





