#include "SceneBase.h"
#include "Lib.h"
#include "../GameCommon.h"
#include "SceneManager.h"

void SceneMenu::Initialize()
{
}

void SceneMenu::Update()
{
}

void SceneMenu::Render()
{
	TK_Lib::Render::Font(VECTOR2{ 0,0 }, L"Menu");
}

void SceneMenu::End()
{
}
