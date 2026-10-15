#include "EnemyBase.h"
#include "Lib_Model.h"
#include "../Source/Mathf.h"
#include "../Source/AI/BehaviorData.h"
#include "../Source/AI/BehaviorTree.h"
#include "../Source/AI/Action/ActionDerived.h"
#include "../Source/AI/Judge/JudgementDerived.h"
#include "../Source/Animetion.h"

EnemySlime::EnemySlime(SceneGame* sceneGame) :EnemyBase(sceneGame)
{
	//behaviorTreeの設定
	SetBehaviorTree();
	SetAnimationData();
}

EnemySlime::~EnemySlime()
{
	CharacterBase::~CharacterBase();
}
//初期化
void EnemySlime::Initialize()
{
	modelData = TK_Lib::Load::model("./Data/Model/Slime/Slime.mdl");
	modelData->PlayAnimation(10, true);
	scale = { 0.05f,0.05f, 0.05f };
	moveSpeed = 130;
	XMVECTOR Qu = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	XMStoreFloat4(&quaternion, Qu);
	SetQuaternion(quaternion);

	attackNodeName = "EyeBall";

	name = "Slime:" + to_string(registerIndex);

	CharacterBase::Initialize();
}
//behaviorTreeの設定
void EnemySlime::SetBehaviorTree()
{
	behaviorTree = new BehaviorTree();
	behaviordata = new BehaviorData();
	behaviorTree->AddNode("", "root", 0, BehaviorTree::SelectRule::PRIORITY, nullptr, nullptr);

		//徘徊
		behaviorTree->AddNode("root", "Wander", 2, BehaviorTree::SelectRule::RANDOM, nullptr, nullptr);
			behaviorTree->AddNode("Wander", "Walk", 0, BehaviorTree::SelectRule::NONE, nullptr, new WalkAction(this,sceneGame));
			behaviorTree->AddNode("Wander", "Wait", 0, BehaviorTree::SelectRule::NONE, nullptr, new WaitAction(this, sceneGame));

		//戦い
		behaviorTree->AddNode("root", "Battle", 1, BehaviorTree::SelectRule::PRIORITY,new PursuitJudgement(this), nullptr);
			behaviorTree->AddNode("Battle", "Attack", 0, BehaviorTree::SelectRule::NONE, new AttackJudgement(this), new AttackAction(this, sceneGame));
			behaviorTree->AddNode("Battle", "Pursuit", 1, BehaviorTree::SelectRule::NONE, nullptr, new PursuitAction(this, sceneGame));

		//死亡
			behaviorTree->AddNode("root", "Die", 0, BehaviorTree::SelectRule::PRIORITY, new DeadJudgement(this), new DeadAction(this, sceneGame));
}

//アニメーション設定
void EnemySlime::SetAnimationData()
{
	anima = new Animation();

	//アニメーション番号
	string AnimeIndex[] =
	{
		anima->Idle,//"IdeleNormal",
		"IdeleBattle",
		anima->Attack1,//"Attack01",
		"Attack02",
		"WalkFWD",
		"WalkBWD",
		"WalkLeft",
		"WalkRight",
		anima->Run,//"RunFWD",
		"SeneseSomethingST",
		"ScneneSomethingRPT",
		"Taunt",
		"Victory",
		anima->Damage,//"GetHit",
		"Dizzy",
		anima->Die,//"Die",
		anima->End//"End",
	};

	//アニメーションの登録
	anima->AllAnimetionKey(&AnimeIndex[0]);
}

