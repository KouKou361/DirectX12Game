#include "SceneBase.h"
#include "Lib.h"
#include "../GameCommon.h"
#include "SceneManager.h"
#include "imgui.h"
#include <thread>


void SceneLoad::LoadingThread(SceneLoad* scene)
{

	if (scene->nextScene->initialized)return;
	scene->nextScene->Initialize();//ここが長い
	scene->nextScene->initialized = true;
}

void SceneLoad::Initialize()
{
	//スレッド開始
	std::thread thread(LoadingThread, this);

	//この関数が抜けてもLoadingThead()を実行させるために手放す
	thread.detach();//終了
}

void SceneLoad::Update()
{
	if (nextScene->initialized)
	{
		GameCommon::GetInstance()->sceneManager->ChangeScene(nextScene.release());
	}


}

void SceneLoad::Render()
{
	//TK_Lib::Render::Font(VECTOR2{ 0,0 }, L"Load");
}

void SceneLoad::End()
{
}
