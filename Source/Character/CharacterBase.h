#pragma once
#include "Lib.h"
#include <string>

class Model;
class SceneGame;
class dtCrowd;
class Animation;

class CharacterBase
{
public:
	CharacterBase(SceneGame* sceneGame);
	~CharacterBase();

	//初期化
	virtual void Initialize();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Render();
	//アニメーション設定
	virtual void SetAnimationData() =0 ;

	//目標地点の変更
	void ResetMoveTarget();
	//無敵時間の
	void InvincibilityTimeUpdate();
	//被ダメージ
	virtual bool OnDamage(const int &damage,const float & invincibilityTime);

	inline const VECTOR3& GetPos() { return pos; }
	inline void SetPos(const VECTOR3& pos) { this->pos = pos; }

	//目標キャラクター
	inline void SetTarget(CharacterBase* target) { this->target = target; }
	//目標キャラクター
	inline void SetTargetPos(const VECTOR3& pos) { this->targetPos = pos; }
	inline CharacterBase* GetTarget() { return target; }

	Animation* GetAnimation() { return anima; }
	Model* GetModel() { return modelData; }
	
	
	inline float GetDamageCollision() { return damageCollisionLength; }//被ダメージ範囲
	inline float GetAtttackCollision() { return attackCollisionLength; }//与ダメージ範囲

	inline int GetHP() { return hp; }
protected:


	//Imgui
	void DebugImgui();
	//回転する
	void Turn(const VECTOR3 & targetVec);
	//行列計算
	void UpdateMat();
	//垂直移動
	void VerticalMove();
	//平行移動
	virtual void ParallelMove()=0;
	//クォオタニオン
	void SetQuaternion(const VECTOR4& quaternion);
	inline const VECTOR4& GetQuaternion() { return quaternion; };

	inline VECTOR3 GetFront() { return front; }
	inline VECTOR3 GetRight() { return right; }
	inline VECTOR3 GetUp() { return up; }

	//ナビエージェントの追加
	void CreateNavAgent();

	VECTOR3 pos = {};
	VECTOR3 scale = {};
	VECTOR4 quaternion = {};
	XMFLOAT4X4 transform = {};

	VECTOR3 front = { 0,0,0 };
	VECTOR3 right = { 0,0,0 };
	VECTOR3 up = { 0,0,0 };
	
	VECTOR3 moveVec = { 0,0,0 };

	Model* modelData =nullptr;

	std::string name = "";

	float moveSpeed = 0;//移動の速さ
	float turnSpeed = XMConvertToRadians(10);

	float damageCollisionLength = 5.0f;
	float attackCollisionLength = 5.0f;
	float invincibilityTime = 3.0f;

	int navAgentIndex = -1;
	int hp = 10;

	SceneGame* sceneGame = nullptr;
	Animation* anima = nullptr;

	
	CharacterBase* target = nullptr;
	VECTOR3 targetPos = { 0,0,0 };
	
};

