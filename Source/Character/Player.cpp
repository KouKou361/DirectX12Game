#include "Player.h"
#include "Lib_Model.h"
#include "../Mathf.h"
#include "../Scene/SceneBase.h"
#include "../NavMesh.h"
#include "../Source/Animetion.h"
#include "PlayerState.h"
#include "Enemy\EnemyBase.h"
#include "Enemy/EnemyManager.h"
#include "Lib.h"
#include "../Source/UI/UI_Base.h"
#include "../Source/Camera/CameraManager.h"

Player::Player(SceneGame* sceneGame) :CharacterBase(sceneGame)
{

}

Player::~Player()
{
	if (attackState1 != nullptr)
	{
		delete attackState1;
		attackState1 = nullptr;
	}
	if (attackState2 != nullptr)
	{
		delete attackState2;
		attackState2 = nullptr;
	}
	if (attackState3 != nullptr)
	{
		delete attackState3;
		attackState3 = nullptr;
	}
	if (damageState != nullptr)
	{
		delete damageState;
		damageState = nullptr;
	}
	if (deadState != nullptr)
	{
		delete deadState;
		deadState = nullptr;
	}
	if (waitState != nullptr)
	{
		delete waitState;
		waitState = nullptr;
	}
	if (runState != nullptr)
	{
		delete runState;
		runState = nullptr;
	}

	if (rollingState != nullptr)
	{
		delete rollingState;
		rollingState = nullptr;
	}

	if (uiTargetEnemyCursor != nullptr)
	{
		delete uiTargetEnemyCursor;
		uiTargetEnemyCursor = nullptr;
	}

	CharacterBase::~CharacterBase();
}
//初期化
void Player::Initialize()
{
	SetAnimationData();

	modelData = TK_Lib::Load::model("./Data/Model/Player/Player.mdl");
	modelData->SetOriginNode("mixamorig:Hips");
	modelData->SetLowerBody("mixamorig:LeftUpLeg");
	modelData->SetLowerBody("mixamorig:RightUpLeg");

	pos = { 0,0,0 };
	scale = { 0.05f,0.05f, 0.05f };
	moveSpeed = 50;
	XMVECTOR Qu = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	XMStoreFloat4(&quaternion, Qu);
	SetQuaternion(quaternion);

	
	attackState1 = new AttackState1(this);
	attackState2 = new AttackState2(this);
	attackState3 = new AttackState3(this);
	damageState = new DamageState(this);
	deadState = new DeadState(this);
	waitState = new WaitState(this);
	runState = new RunState(this);
	rollingState = new RollingState(this);
	
	ChangeState(Player::State::WAIT);

	uiTargetEnemyCursor = new UI_TargetEnemyCursor(sceneGame);
	uiTargetEnemyCursor->Init();

	name = "Player";



	CharacterBase::Initialize();
}
//更新処理
void Player::Update()
{
	//移動取得
	Input();

	//ロックオン
	LockOnTarget();

	if (currentState != nullptr)
	{
		currentState->Run();
		currentState->DebugImgui();
	}

	uiTargetEnemyCursor->Update();

	CharacterBase::Update();
}
//描画処理
void Player::Render()
{
	uiTargetEnemyCursor->Render();
	CharacterBase::Render();
}

//移動取得
void Player::Input()
{
	float ax = TK_Lib::KeyInput::GetAxisLX();
	float az = TK_Lib::KeyInput::GetAxisLY();

	VECTOR3 CameraFront = TK_Lib::Camera::GetFront();
	//XZ平面に変換する
	{
		CameraFront.y = 0.0f;
		XMVECTOR Front = DirectX::XMLoadFloat3(&CameraFront);
		Front = XMVector3Normalize(Front);
		DirectX::XMStoreFloat3(&CameraFront, Front);
	}

	VECTOR3 CameraRight = TK_Lib::Camera::GetRight();
	//XZ平面に変換する
	{
		CameraRight.y = 0.0f;
		XMVECTOR right = DirectX::XMLoadFloat3(&CameraRight);
		right = XMVector3Normalize(right);
		DirectX::XMStoreFloat3(&CameraRight, right);
	}

	moveVec.x = (CameraFront.x * az + CameraRight.x * ax);
	moveVec.z = (CameraFront.z * az + CameraRight.z * ax);
	moveVec.y = 0.0f;

	//正規化
	moveVec = Mathf::Vec3Normalize(moveVec);

}

//平行移動
void Player::ParallelMove()
{
	if (navAgentIndex <= -1)return;
	if (sceneGame->navMesh == nullptr)return;

	const dtCrowdAgent* ag = sceneGame->navMesh->GetCrowd()->getAgent(navAgentIndex);
	if (!ag->active) return;

	moveVec.x *= moveSpeed;
	moveVec.z *= moveSpeed;

	sceneGame->navMesh->GetCrowd()->requestMoveVelocity(navAgentIndex,&moveVec.x);

	const float* pos = ag->npos;
	SetPos({ pos[0],pos[1],pos[2] });
}

//アニメーション設定
void Player::SetAnimationData()
{
	anima = new Animation();

#if 0

	//アニメーション番号
	string AnimeIndex[] =
	{
		anima->Attack1,//"IdeleChest",
		anima->Die,//"Die",
		"Falling",
		anima->Attack1,//"Attack01",
		anima->Damage,//"GetHit",
		anima->Idle,
		"Jump",
		"Jump_Flip",
		"Landing",
		"Revive",

		anima->Run,//"Run",
		"Walk",
		anima->End,//"End",
	};
#else
	//アニメーション番号
	string AnimeIndex[] =
	{
		anima->Idle,
		anima->Rolling,
		"Sheild",
		anima->Die,
		anima->Attack3,
		anima->Attack2,
		anima->Attack1,
		anima->Run,
		anima->Damage,
		anima->End
	};
#endif
	//アニメーションの登録
	anima->AllAnimetionKey(&AnimeIndex[0]);

}

//行動変更
void Player::ChangeState(const State& state)
{
	this->state = state;

	if (currentState != nullptr)
	{
		currentState->End();
	}
	
	switch (state)
	{
	case Player::State::NONE:
			break;
	case Player::State::WAIT:
		currentState = waitState;
		break;
	case Player::State::RUN:
		currentState = runState;
		break;
	case Player::State::ATTACK1:
		currentState = attackState1;
		break;
	case Player::State::ATTACK2:
		currentState = attackState2;
		break;
	case Player::State::ATTACK3:
		currentState = attackState3;
		break;
	case Player::State::DEAD:
		currentState = deadState;
		break;
	case Player::State::DAMAGE:
		currentState = damageState;
		break;
	case Player::State::JUMP:
	//	currentState = jumpState;
		break;
	case Player::State::ROLLING:
		currentState = rollingState;
		break;
	default:
		break;
	}

	if (currentState != nullptr)
	{
		currentState->Start();
	}
}

//被ダメージ
bool Player::OnDamage(const int& damage, const float& invincibilityTime)
{
	if (CharacterBase::OnDamage(damage, invincibilityTime))
	{
		ChangeState(Player::State::DAMAGE);
		return true;
	}
	return false;
	
}

//ロックオン
void Player::LockOnTarget()
{
	VECTOR3 camerafrontVec = TK_Lib::Camera::GetFront();
	XMVECTOR v1;
	v1 = XMLoadFloat3(&camerafrontVec);
	v1 = XMVector3Normalize(v1);
	VECTOR3 cameraPos = TK_Lib::Camera::GetPos();

	constexpr float ViewAngle = 45;
	float nearDot = FLT_MAX;
	EnemyBase* saveEnm = nullptr;

	for (int i = 0; i < sceneGame->enemyManager->enemyList.size(); i++)
	{
		EnemyBase* enm = sceneGame->enemyManager->enemyList.at(i);
		if (enm == nullptr)continue;
		VECTOR3 enmPos = enm->GetPos();
		VECTOR3 v = enmPos - cameraPos;

		XMVECTOR v2;
		v2 = XMLoadFloat3(&v);
		v2 = XMVector3Normalize(v2);

		XMVECTOR Dot = XMVector3Dot(v2, v1);
		float dot = 0.0f;
		XMStoreFloat(&dot, Dot);
		dot = acosf(dot);
		dot = XMConvertToDegrees(dot);

		//ターゲット視野角に入ってないならcontinue
		if (dot >= ViewAngle)
		{
			continue;
		}
		if (dot >= nearDot)
		{
			continue;
		}

		saveEnm = enm;
		nearDot = dot;

	}
	
	SetTarget(saveEnm);

	if (saveEnm != nullptr)
	{
		if (TK_Lib::KeyInput::GetButtonDown(BTN::RB))
		{
			sceneGame->cameraManager->ChangeCamera(CameraType::TargetEnemy);
		}
	}
}