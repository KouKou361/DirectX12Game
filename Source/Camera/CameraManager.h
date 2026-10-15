#pragma once
class CameraBase;
class SceneGame;

class CameraNormal;
class CameraTargetEnemy;

enum CameraType
{
	Normal,
	TargetEnemy,
};
class CameraManager
{
public:
	CameraManager(SceneGame* sceneGame) ;
	~CameraManager() {};

	//‰Šú‰»
	void Initialize();
	//XV
	void Update();
	//•`‰æ
	void Render();
	//I—¹
	void End();

	void ChangeCamera(const CameraType &type);
	void ChangeCamera(CameraBase* nextCamera);
private:

	CameraBase* currentCamera = nullptr;
	SceneGame* sceneGame = nullptr;

	CameraNormal* cameraNormal = nullptr;
	CameraTargetEnemy* cameraTargetEnemy = nullptr;
};