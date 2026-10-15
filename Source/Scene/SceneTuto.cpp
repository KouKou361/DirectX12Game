#include "SceneBase.h"
#include "Lib.h"
#include "../GameCommon.h"
#include "SceneManager.h"


void SceneTuto::Initialize()
{
}

void SceneTuto::Update()
{
}

void SceneTuto::Render()
{
	TK_Lib::Render::Font(VECTOR2{ 0,0 }, L"Tuto");
}

void SceneTuto::End()
{
}
