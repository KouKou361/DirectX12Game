#pragma once
#include "../CharacterBase.h"
#include <string>

class NodeBase;
class BehaviorTree;
class BehaviorData;



class dtCrowd;
class EnemyBase :public CharacterBase
{
public:
	EnemyBase(SceneGame* sceneGame);
	~EnemyBase();

	//初期化
	virtual void Initialize();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Render();





protected:

	//behaviorTreeの設定
	virtual void SetBehaviorTree()=0;
	//ビヘイビアツリーの行動処理
	void ActionBehaviorTree();
	//Imguiデバッグ用
	void ImguiDebug();

	//平行移動
	void ParallelMove() override;
	//アニメーション設定
	virtual void SetAnimationData()override =0;


	NodeBase* actionNode = nullptr;
	BehaviorTree* behaviorTree = nullptr;
	BehaviorData* behaviordata = nullptr;
	std::string attackNodeName = "";
	int registerIndex;
public:
	//索敵範囲にTargetがいればTRUEを返す
	bool SearchTarget(const float& l);
	//索敵範囲に目標地点がいればTRUEを返す
	bool SearchPosition(const float& L, const VECTOR3& Position);

	void SetRegisterIndex(int registerIndex) { this->registerIndex = registerIndex; };




	inline float GetSearchLength () { return searchLength; }//索敵範囲
	inline float GetPursuitLength() { return pursuitLength; }//追跡範囲
	inline float GetAttackLength() { return attackLength; }//攻撃する範囲
	inline std::string AttackNode() { return attackNodeName; }

	bool moveFlg = false;



private:



	float searchLength = 30.0f;//索敵範囲
	float pursuitLength = 40.0f;//追跡範囲
	float attackLength = 5.0f;//攻撃する範囲



};

//スライム
class EnemySlime :public EnemyBase
{
public:
	EnemySlime(SceneGame* sceneGame);
	~EnemySlime();

	//初期化
	virtual void Initialize();

	//behaviorTreeの設定
	void SetBehaviorTree();
	//アニメーション設定
	void SetAnimationData()override;
};

//亀
class EnemyTurleShell :public EnemyBase
{
public:
	EnemyTurleShell(SceneGame* sceneGame);
	~EnemyTurleShell();

	//初期化
	virtual void Initialize();

	//behaviorTreeの設定
	void SetBehaviorTree();
	//アニメーション設定
	void SetAnimationData()override;
};

//宝箱
class EnemyChest :public EnemyBase
{
public:
	EnemyChest(SceneGame* sceneGame);
	~EnemyChest();

	//初期化
	virtual void Initialize();

	//behaviorTreeの設定
	void SetBehaviorTree();
	//アニメーション設定
	void SetAnimationData()override;
};

//一つ目ビホルダー
class EnemyBeholder :public EnemyBase
{
public:
	EnemyBeholder(SceneGame* sceneGame);
	~EnemyBeholder();

	//初期化
	virtual void Initialize();

	//behaviorTreeの設定
	void SetBehaviorTree();
	//アニメーション設定
	void SetAnimationData()override;
};
