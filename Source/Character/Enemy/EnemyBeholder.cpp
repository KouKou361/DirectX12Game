#include "EnemyBase.h"
#include "Lib_Model.h"
#include "../Source/Mathf.h"
#include "../Source/AI/BehaviorData.h"
#include "../Source/AI/BehaviorTree.h"
#include "../Source/Animetion.h"

EnemyBeholder::EnemyBeholder(SceneGame* sceneGame) :EnemyBase(sceneGame)
{
	//behaviorTreeの設定
	SetBehaviorTree();
	SetAnimationData();
}

EnemyBeholder::~EnemyBeholder()
{
	CharacterBase::~CharacterBase();
}
//初期化
void EnemyBeholder::Initialize()
{
	modelData = TK_Lib::Load::model("./Data/Model/Beholder/Beholder.mdl");
	modelData->PlayAnimation(10, true);
	scale = { 0.05f,0.05f, 0.05f };
	moveSpeed = 130;
	XMVECTOR Qu = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	XMStoreFloat4(&quaternion, Qu);
	SetQuaternion(quaternion);
	name = "Beholder:" + to_string(registerIndex);

	CharacterBase::Initialize();

	
}

//behaviorTreeの設定
void EnemyBeholder::SetBehaviorTree()
{
	behaviorTree = new BehaviorTree();
	behaviordata = new BehaviorData();
	behaviorTree->AddNode("", "root", 0, BehaviorTree::SelectRule::PRIORITY, nullptr, nullptr);
}

//アニメション設定
void EnemyBeholder::SetAnimationData()
{
	anima = new Animation();
//
//	//アニメーション番号
//	string AnimeIndex[] =
//	{
//		anima->Idle,//"IdeleChest",
//		"IdeleNormal",
//		"IdeleBattle",
//		anima->Attack1,//"Attack01",
//		"Attack02",
//		"Taunting",
//		"Victory",
//		"WalkFWD",
//		"WalkBWD",
//		"WalkLeft",
//		"WalkRight",
//		anima->Run,//"Run",
//		"SeneseSomethingST",
//		"ScneneSomethingRPT",
//		anima->Damage,//"GetHit",
//		"Dizzy",
//		anima->Die,//"Die",
//		anima->End,//"End",
//	};
//	//アニメーションの登録
//	anima->AllAnimetionKey(&AnimeIndex[0]);
//
}