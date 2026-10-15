#include "CameraManager.h"
#include "CameraBase.h"
CameraManager::CameraManager(SceneGame* sceneGame)
{
	this->sceneGame = sceneGame;
}
void CameraManager::Initialize()
{
	cameraNormal = new CameraNormal(sceneGame);
	cameraTargetEnemy = new CameraTargetEnemy(sceneGame);
}
void CameraManager::Update()
{
	if (currentCamera != nullptr)
	{
		currentCamera->Update();
	}
}
void CameraManager::Render()
{
	if (currentCamera != nullptr)
	{
		currentCamera->Render();
	}
}

void CameraManager::End()
{
	if (currentCamera != nullptr)
	{
		currentCamera->End();
	}

	if (cameraNormal!=nullptr)
	{
		delete cameraNormal;
		cameraNormal = nullptr;
	}

	if (cameraTargetEnemy != nullptr)
	{
		delete cameraTargetEnemy;
		cameraTargetEnemy = nullptr;
	}

}

void CameraManager::ChangeCamera(CameraBase* nextCamera)
{
	VECTOR3 blendPos = {0,0,0};
	VECTOR3 blendTargetPos = { 0,0,0 };
	if (currentCamera != nullptr)
	{
		currentCamera->End();
		blendPos = currentCamera->pos;
		blendTargetPos = currentCamera->targetPos;
	}
	

	currentCamera = nextCamera;

	currentCamera->earsingBlendStartPos = blendPos;
	currentCamera->earsingBlendStartTargetPos = blendTargetPos;
	currentCamera->earsingBlendSecandTime = 0.5f;
	currentCamera->earsingBlendTime = 0.0f;

	currentCamera->Initialize();



}

void CameraManager::ChangeCamera(const CameraType& type)
{
	switch (type)
	{
	case CameraType::Normal:
		ChangeCamera(cameraNormal);
		break;
	case CameraType::TargetEnemy:
		ChangeCamera(cameraTargetEnemy);
		break;
	}
}

