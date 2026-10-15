#include "SceneManager.h"
#include "SceneBase.h"
void SceneManager::Initialize()
{
	
}
void SceneManager::Update()
{
	if (currentScene != nullptr)
	{
		currentScene->Update();
	}
}
void SceneManager::Render()
{
	if (currentScene != nullptr)
	{
		currentScene->Render();
	}
}

void SceneManager::End()
{
	if (currentScene != nullptr)
	{
		currentScene->End();
		delete currentScene;
		currentScene = nullptr;
	}

}

void SceneManager::ChangeScene(SceneBase* nextScene)
{
	if (currentScene != nullptr)
	{
		currentScene->End();
		delete currentScene;
		currentScene = nullptr;
	}
	currentScene = nextScene;

	currentScene->Initialize();



}

