#include "GameCommon.h"
#include "Scene\SceneManager.h"

void GameCommon::Initialize()
{
	sceneManager = new SceneManager;
}

void GameCommon::End()
{
	if (sceneManager != nullptr)
	{
		delete sceneManager;
		sceneManager = nullptr;
	}
}
