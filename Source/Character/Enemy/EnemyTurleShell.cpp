#include "EnemyBase.h"
#include "Lib_Model.h"
#include "../Source/Mathf.h"
#include "../Source/AI/BehaviorData.h"
#include "../Source/AI/BehaviorTree.h"
#include "../Source/Animetion.h"

EnemyTurleShell::EnemyTurleShell(SceneGame* sceneGame) :EnemyBase(sceneGame)
{
	//behaviorTreeの設定
	SetBehaviorTree();
	SetAnimationData();
}

EnemyTurleShell::~EnemyTurleShell()
{
	CharacterBase::~CharacterBase();
}
//初期化
void EnemyTurleShell::Initialize()
{
	modelData = TK_Lib::Load::model("./Data/Model/TurtleShell/TurtleShell.mdl");
	modelData->PlayAnimation(10, true);
	scale = { 0.05f,0.05f, 0.05f };
	moveSpeed = 130;
	XMVECTOR Qu = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	XMStoreFloat4(&quaternion, Qu);
	SetQuaternion(quaternion);
	name = "TurleShell:" + to_string(registerIndex);

	CharacterBase::Initialize();
}

//behaviorTreeの設定
void EnemyTurleShell::SetBehaviorTree()
{
	behaviorTree = new BehaviorTree();
	behaviordata = new BehaviorData();
	behaviorTree->AddNode("", "root", 0, BehaviorTree::SelectRule::PRIORITY, nullptr, nullptr);
}

//アニメーション設定
void EnemyTurleShell::SetAnimationData()
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
		"Defend",
		"DefendGetHit",
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