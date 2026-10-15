#include "SceneBase.h"
#include "Lib.h"
#include "../GameCommon.h"
#include "SceneManager.h"
#include "imgui.h"

static	VECTOR4 rectPos;
static	VECTOR4 line;

void SceneTitle::Initialize()
{
	rectPos = { 100,100,100,100 };
	line = { 200,200,400,400 };
}

void SceneTitle::Update()
{
	if (TK_Lib::KeyInput::GetKeyState(KEY_RIGHT)>=1||
		TK_Lib::KeyInput::GetButtonDown(BTN::A)>=1
		)
	{
		GameCommon::GetInstance()->sceneManager->ChangeScene(new SceneGame);

		//GameCommon::GetInstance()->sceneManager->ChangeScene(new SceneLoad(new SceneGame));
	}

	ImGui::Begin("Input");

	for (int i = 0; i < static_cast<int> (BTN::END); i++)
	{
		ImGui::Text("%d=%d",i, TK_Lib::KeyInput::GetButtonDown(static_cast<BTN>(i)));
		
	}
	ImGui::Text("GetAxisLX=%f", TK_Lib::KeyInput::GetAxisLX());
	ImGui::Text("GetAxisLY=%f", TK_Lib::KeyInput::GetAxisLY());
	ImGui::Text("GetAxisRX=%f", TK_Lib::KeyInput::GetAxisRX());
	ImGui::Text("GetAxisRY=%f", TK_Lib::KeyInput::GetAxisRY());

	ImGui::End();


	

	ImGui::Begin("Debug");
	ImGui::SliderFloat4("rect", &rectPos.x,0,1000);
	ImGui::SliderFloat4("line", &line.x, 0, 1000);
	ImGui::End();
	

}

void SceneTitle::Render()
{
	TK_Lib::Render::Font(VECTOR2{ 0,0 }, L"Title");

	VECTOR4 color = { 1,0,0,1 };
	if (TK_Lib::Collision::RectVsLine({ rectPos.x,rectPos.y }, { rectPos.z,rectPos.w }, { line.x,line.y }, { line.z,line.w }))
	{
		color = { 0,0,1,1 };
	}

	TK_Lib::Debug2D::Box({ rectPos.x,rectPos.y}, { rectPos.z,rectPos.w },0, color);
	TK_Lib::Debug2D::Line({ line.x,line.y }, { line.z,line.w }, color);
}

void SceneTitle::End()
{
}
