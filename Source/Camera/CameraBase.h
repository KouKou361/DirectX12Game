#pragma once

#include <memory>
#include "Lib.h"
class Model;
class Player;
class SceneGame;
class CharacterBase;

class CameraBase
{
private:
	friend class CameraManager;

public:
	CameraBase(SceneGame* sceneGame=nullptr);
	~CameraBase() {};

	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//描画
	virtual void Render();
	//終了
	virtual void End();
	//ブレンド　エージング移動
	void EarsingBlendCameraPos();


	VECTOR3 earsingBlendStartPos = {};
	VECTOR3 earsingBlendEndPos = {};

	VECTOR3 earsingBlendStartTargetPos = {};
	VECTOR3 earsingBlendEndTargetPos = {};

	float earsingBlendSecandTime = 0.0f;
	float earsingBlendTime = 0.0f;

protected:



	VECTOR3 pos;
	VECTOR3 targetPos;
	VECTOR3 up;

	SceneGame* sceneGame = nullptr;
	CharacterBase* targetCharacter = nullptr;

};

//通常時のカメラ
class CameraNormal :public CameraBase
{
public:
	CameraNormal(SceneGame* sceneGame):CameraBase(sceneGame) {};
	~CameraNormal() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;

private:
	void UpdatePos();
	void UpdateTarget();
private:
	float cangle = 0.0f;
	float cdist = 30.0f;

	float camera_up_speed = 15.0f;
	float camera_dist_speed = 100.0f;

};

//敵ターゲットのカメラ
class CameraTargetEnemy :public CameraBase
{
public:
	CameraTargetEnemy(SceneGame* sceneGame) :CameraBase(sceneGame) {};
	~CameraTargetEnemy() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;

private:
	void UpdatePos();
	void UpdateTarget();
private:
	float cangle = 0.0f;
	float cdist = 30.0f;

	float camera_up_speed = 15.0f;
	float camera_dist_speed = 100.0f;
};

