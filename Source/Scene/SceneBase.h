#pragma once
#include <memory>
#include <DirectXMath.h>

class Model;
class Player;
class EnemyManager;
class CameraManager;
class NavMesh;
class dtNavMesh;
class dtNavMeshQuery;
class Collision;

class SceneBase
{
private:
	friend class SceneLoad;
	friend class SceneManager;

public:
	SceneBase() {};
	~SceneBase() {};

	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//描画
	virtual void Render();
	//終了
	virtual void End();

protected:
	bool initialized = false;
};

//タイトル
class SceneTitle :public SceneBase
{
public:
	SceneTitle() {};
	~SceneTitle() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;

private:

};

//ロード
class SceneLoad :public SceneBase
{
public:
	SceneLoad(SceneBase* nextScene) {
		this->nextScene.reset(nextScene);
	};
	~SceneLoad() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;

private:
	static void LoadingThread(SceneLoad* scene);

	std::unique_ptr<SceneBase> nextScene = nullptr;

};
//メニュー
class SceneMenu :public SceneBase
{
public:
	SceneMenu() {};
	~SceneMenu() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;
};
//チュートリアル
class SceneTuto :public SceneBase
{
public:
	SceneTuto() {};
	~SceneTuto() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;
};
//ゲーム
class SceneGame :public SceneBase
{
public:
	SceneGame() {};
	~SceneGame() {};


	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Render() override;
	//終了
	void End() override;
	//デバッグ用
	void DebugImgui();


public:
	Model* stageModel=nullptr;
	Model* skyModel = nullptr;
	Player* player=nullptr;
	CameraManager* cameraManager = nullptr;
	EnemyManager* enemyManager = nullptr;
	NavMesh* navMesh = nullptr;
	DirectX::XMFLOAT4X4 StageMat;
	Collision* collision = nullptr;

};