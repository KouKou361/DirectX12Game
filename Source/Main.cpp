#include <windows.h>
#include <memory>
#include <wchar.h>
//#include "Lib_Sprite.h"
//#include "Lib_Model.h"
//#include "Lib_Texture.h"
//#include "Lib_Effect3DGeometry.h"
//#include "Lib_GeometryVertexVecter.h"
//#include "Lib.h"
//#include "imgui.h"
//#include "imgui_impl_dx12.h"
//
//#include "Audio/Lib_Audio.h"

#include "Lib.h"
#include "GameCommon.h"
#include "Scene\SceneManager.h"
#include "Scene\SceneBase.h"



const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;

//void CreateEffect1(Lib_Effect3D* effect)
//{
//	effect->GetVertices().at(0).Position = { 10,10,10 };
//
//	for (int i = 0; i < effect->GetVertices().size(); i++)
//	{
//		GeometryVertex* vertex = &effect->GetVertices().at(i);
//		const float x = static_cast<float>( rand() % 500 ) / 10 - 100;
//		const float y = static_cast<float>(rand() % 500) / 10;
//		const float z = static_cast<float>( rand() % 500 ) / 10 ;
//		vertex->Position = {x,y,z};
//		vertex->Size = { 5,5,5 };
//		vertex->Rotation = { 1,1,1 };
//		vertex->SpeedVec = { 1,1,1 };
//		vertex->AcceleVec = { 0,0,0 };
//		vertex->RotationVec = {-0.5f,-0.5f,-0.5f };
//		vertex->Scale = { 0,0,0 };
//		vertex->AddColor = { 0,0,0,0 };
//		vertex->Dummy2 = { 0,0,0 };
//		vertex->Tex = { 0,0,0,0 };
//		vertex->Color = { 1,1,1,1 };
//	}
//	effect->Apply();
//}

INT WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prev_instance, _In_ LPSTR cmd_line, _In_ int cmd_show)
{
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//もしメモリーリークした時にここに値をいれるといいよ
	//_CrtSetBreakAlloc(10828);
#endif

	TK_Lib::LibInit(L"TKライブラリ", SCREEN_WIDTH, SCREEN_HEIGHT);
	
	//カメラ
	TK_Lib::Camera::SetLookAt({ 0,10,-15 }, { 0,10,0 }, { 0,1,0 });
	TK_Lib::Camera::SetPerspectiveFov(XM_PIDIV2, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 1.0f, 10000.0f);
	
	TK_Lib::Light::SetLookAt({ -60,60,60 }, { 0,0,0 }, { 0,1,0 });
	TK_Lib::Light::SetLightOrthongraphic(350.0f, 350.0f, 0.1f, 1500.0f);
	
	
	SceneManager* sceneManager = GameCommon::GetInstance()->sceneManager;
	sceneManager = new SceneManager();
	GameCommon::GetInstance()->sceneManager = sceneManager;
	
	
	sceneManager->ChangeScene(new SceneTitle);
	sceneManager->Initialize();
	
	while (TK_Lib::LibLoop(true))
	{
		TK_Lib::Render::EndRender();
	
		sceneManager->Update();

		static float cangle = 0.0f;
		static float cdist = 15.0f;
		static VECTOR3 cpos = { -60,60,60 };
		//カメラ
		const float camera_up_speed = 15.0f;
		const float camera_dist_speed = 60.0f;
		const float time = TK_Lib::Window::GetElapsedTime();
		if (GetKeyState('J') < 0)  cangle += time;
		if (GetKeyState('L') < 0)  cangle -= time;
		if (GetKeyState('I') < 0)  cpos.y += time * camera_up_speed;
		if (GetKeyState('K') < 0)  cpos.y -= time * camera_up_speed;
		if (GetKeyState('U') < 0)  cdist += time * camera_dist_speed;
		if (GetKeyState('O') < 0)  cdist -= time * camera_dist_speed;
		cpos.x = sinf(cangle) * cdist;
		cpos.z = cosf(cangle) * cdist;



	//	TK_Lib::Camera::SetLookAt(cpos, VECTOR3(0, 10, 0), VECTOR3(0, 1, 0));

		TK_Lib::Light::SetLookAt(cpos, VECTOR3(0, 10, 0), VECTOR3(0, 1, 0));
	
		sceneManager->Render();
	
		TK_Lib::Render::BeginRender();
	}
	sceneManager->End();
	
	
	if (sceneManager != nullptr)
	{
		delete sceneManager;
		sceneManager = nullptr;
	}
	TK_Lib::Render::EndRender();
	TK_Lib::End();


	return 0;
}
//
//INT WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prev_instance, _In_ LPSTR cmd_line, _In_ int cmd_show)
//{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	//もしメモリーリークした時にここに値をいれるといいよ
//	//_CrtSetBreakAlloc(10792);
//#endif
//
//	TK_Lib::LibInit(L"TKライブラリ", SCREEN_WIDTH, SCREEN_HEIGHT);
//
//	//テクスチャ
//	Texture* sprite = nullptr;
//	Texture* sprite2 = nullptr;
//	Texture* sprite3 = nullptr;
//	Texture* sprite4 = nullptr;
//
//	{
//		sprite = TK_Lib::Load::sprite("./Data/EXP.png");
//		sprite2 = TK_Lib::Load::sprite("./Data/owner-sprites.png");
//		sprite3 = TK_Lib::Load::sprite("./Data/Texture/TestCard.png");
//		sprite4 = TK_Lib::Load::sprite("./Data/Texture/enemy.png");
//	}
//
//	//モデル
//	Model* model1 = nullptr;
//	Model* model2 = nullptr;
//	Model* model3 = nullptr;
//	Model* model4 = nullptr;
//
//	{
//		model1 = TK_Lib::Load::model("./Data/Model/Jammo/Jammo.mdl");
//		model1->PlayAnimation(1, true);
//
//		model2 = TK_Lib::Load::model("./Data/Model/Slime/Slime.mdl");
//		model2->PlayAnimation(1, true);
//
//		model3 = TK_Lib::Load::model("./Data/Model/Jammo/Jammo.mdl");
//		model3->PlayAnimation(2, true);
//
//		//model4 = TK_Lib::Load::model("./Data/Model/Stage/test2.mdl");
//		//model4 = TK_Lib::Load::model("./Data/Model/Stage/test.mdl");
//		//model4 = TK_Lib::Load::model("./Data/Model/Stage/Stage02/RedDungeon2.mdl");
//		//model4 = TK_Lib::Load::model("./Data/Model/Stage/Stage03/Blue.mdl");
//		model4 = TK_Lib::Load::model("./Data/Model/Stage/Stage01/GreenStage.mdl");
//		model4->PlayAnimation(0, true);
//
//		{
//
//			float s = 0.01f;
//			XMMATRIX transform, scale;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(s, s, s);
//			transform = XMMatrixTranslation(30.0f, 105.0f, 30.0f);
//			//transform = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale * transform);
//			model4->ChangeStatic(Mat);
//		}
//	}
//
//
//	//サウンド
//	Lib_AudioBase* audio;
//	Lib_AudioBase* audio2;
//
//	{
//		audio = TK_Lib::Load::Audio("./Data/Audio/BGM/TitleBGM.wav", "BGM");
//		audio2 = TK_Lib::Load::Audio("./Data/Audio/SE/GetCoin.wav", "SE");
//	}
//
//	//エフェクト
//	Lib_Effect3D* effect1;
//	Lib_Effect3D* effect2;
//	static float time = 0;
//	
//	{
//		effect1 = TK_Lib::Load::Effect(sprite, 100);
//		effect2 = TK_Lib::Load::Effect(sprite, 100);
//
//		
//
//		effect1->Play(10.0f, false);
//		CreateEffect1(effect1);
//
//		effect2->Play(3.0f);
//	}
//
//
//
//	//カメラ
//	TK_Lib::Camera::SetLookAt({ 0,10,-15 }, { 0,10,0 }, { 0,1,0 });
//	TK_Lib::Camera::SetPerspectiveFov(XM_PIDIV2, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 1.0f, 1000.0f);
//
//	TK_Lib::Light::SetLookAt({ -60,60,60 }, { 0,0,0 }, { 0,1,0 });
//	TK_Lib::Light::SetLightOrthongraphic(350.0f, 350.0f, 0.1f, 1500.0f);
//
//	while (TK_Lib::LibLoop(true))
//	{
//
//
//		TK_Lib::Render::EndRender();
//
//		
//		
//		ImGui::Begin("Hello World!");
//		ImGui::Text(u8"こんにちは。武友公作です。");
//
//		//ImTextureID texture_id = (ImTextureID)(sprite4->GetDescriptor()->GetGpuHandle().ptr);
//		//ImGui::Image(texture_id, ImVec2{ (float)sprite4->GetSize().x,(float)sprite4->GetSize().y });
//		if (ImGui::Button("BGMStart"))	TK_Lib::Audio::Play("BGM");
//		if (ImGui::Button("BGMStop"))	TK_Lib::Audio::Stop("BGM");
//		if (ImGui::Button("SEStart"))	TK_Lib::Audio::Play("SE");
//		if (ImGui::Button("SEStop"))	TK_Lib::Audio::Stop("SE");
//
//		if (ImGui::Button("Export"))
//		{
//			model4->DeserializeModel();
//		}
//
//		
//		//	ImGui::Begin("Hello World!");
//		
//
//		ImGui::End();
//
//
//		{
//			XMMATRIX transform, scale;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
//			transform = XMMatrixTranslation(0.0f, -5, 0.0f);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale*transform);
//			model1->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
//			model1->UpdateTransform(Mat);
//		}
//
//		{
//			XMMATRIX transform, scale;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
//			transform = XMMatrixTranslation(10.0f, 5, 10.0f);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale*transform);
//			model2->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
//			model2->UpdateTransform(Mat);
//		}
//
//	
//
//		{
//			XMMATRIX transform, scale;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
//			transform = XMMatrixTranslation(10.0f, 0, 0.0f);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale * transform);
//			model3->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
//			model3->UpdateTransform(Mat);
//		}
//
//		
//	
//
//		static float cangle = 0.0f;
//		static float cdist = 15.0f;
//		static VECTOR3 cpos = { 0,12,0 };
//		//カメラ
//		const float camera_up_speed = 15.0f;
//		const float camera_dist_speed = 10.0f;
//		const float time = TK_Lib::Window::GetElapsedTime();
//		if (GetKeyState('J') < 0)  cangle += time;
//		if (GetKeyState('L') < 0)  cangle -= time;
//		if (GetKeyState('I') < 0)  cpos.y += time * camera_up_speed;
//		if (GetKeyState('K') < 0)  cpos.y -= time * camera_up_speed;
//		if (GetKeyState('U') < 0)  cdist += time * camera_dist_speed;
//		if (GetKeyState('O') < 0)  cdist -= time * camera_dist_speed;
//		cpos.x = sinf(cangle) * cdist;
//		cpos.z = cosf(cangle) * cdist;
//
//
//
//		TK_Lib::Camera::SetLookAt(cpos, VECTOR3(0, 10, 0), VECTOR3(0, 1, 0));
//
//	//	TK_Lib::Light::SetLookAt(cpos, VECTOR3(0, 10, 0), VECTOR3(0, 1, 0));
//
//	//	TK_Lib::Render::SpriteRender(sprite, { 100.0f,400.0f }, { 100,100 }, { 0,0,960,540 }, { 1,1,1,1 }, 0);
//	//	TK_Lib::Render::SpriteRender(sprite2, { 200.0f,400.0f }, { 100,100 }, { 0,0,1960,1200 }, { 1,1,1,1 }, 0);
//	//	TK_Lib::Render::SpriteRender(sprite3, { 1700.0f,600.0f }, { 100.0f,161.8f }, { 0,0,1000,1618 }, { 1,0,0,1 }, 0);
//
//
//
//
//
//		TK_Lib::Render::Font({ 400,0 }, L"うン地ーKoんぐ");
//
//		TK_Lib::Render::Font({ 800,300 }, L"Fontシステム完成して嬉しいです！？？。、");
//		{
//			XMMATRIX transform, scale,rotation;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
//			transform = XMMatrixTranslation(-20.0f, 0, 0.0f);
//			rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale * rotation * transform);
//			TK_Lib::Render::PlaneRender(sprite, Mat, { 0,0,960,540 }, { 1,1,1,1 });
//		}
//
//		{
//			static float angle=0.0f, angle2 = 0.0f;
//			angle += XMConvertToRadians(1);
//			angle2 += XMConvertToRadians(1);
//			XMMATRIX transform, scale, rotation;
//			XMMATRIX mat = XMMatrixIdentity();
//			scale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
//			transform = XMMatrixTranslation(-20.0f, 0, 0.0f);
//			rotation = XMMatrixRotationRollPitchYaw(angle, 0, 0);
//			XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, scale * rotation * transform );
//			TK_Lib::Render::PlaneRender(sprite3, Mat, { 0,0,1000,1618 }, { 1,1,1,1 });
//
//		//	transform = XMMatrixTranslation(0.0f, 10.0f, 0.0f);
//		//	rotation = XMMatrixRotationRollPitchYaw(0, angle2, 0);
//		//	XMStoreFloat4x4(&Mat, scale * rotation * transform);
//		//	TK_Lib::Render::PlaneRender(sprite3, Mat, { 0,0,1000,1618 }, { 1,1,1,1 });
//
//
//			VECTOR4 cut[6] =
//			{
//				{ 0,     0,32*1,32 },
//				{ 32 * 1,0,32*2,32 },
//				{ 32 * 2,0,32*3,32 },
//				{ 32 * 3,0,32*4,32 },
//				{ 32 * 4,0,32*5,32 },
//				{ 32 * 5,0,32*6,32 },
//			};
//			static float time = 0.0f;
//			time += TK_Lib::Window::GetElapsedTime() * 13;
//			int index = int (time) ;
//			index = index % 6;
//			transform = XMMatrixTranslation(0.0f, 10.0f, 0.0f);
//			rotation = XMMatrixRotationRollPitchYaw(0, angle2, 0);
//			XMStoreFloat4x4(&Mat, scale * rotation * transform);
//			TK_Lib::Render::PlaneRender(sprite4, Mat, cut[index], { 1,1,1,1 });
//		}
//
//		effect1->TimeUpdate(TK_Lib::Window::GetElapsedTime());
//
//		TK_Lib::Render::ModelRender(model1);
//		TK_Lib::Render::ModelRender(model2);
//		TK_Lib::Render::ModelRender(model3);
//		TK_Lib::Render::ModelRender(model4);
//	
//		TK_Lib::Debug2D::Box({ 0,0 },     { 100,100 }, XMConvertToRadians(10),{1,0,0,1});
//		TK_Lib::Debug2D::Box({ 100,100 }, { 100,100 }, XMConvertToRadians(20),{1,1,0,1});
//		TK_Lib::Debug2D::Box({ 200,200 }, { 100,100 }, XMConvertToRadians(30),{1,0,1,1});
//		TK_Lib::Debug2D::Box({ 300,300 }, { 100,100 }, XMConvertToRadians(40),{1,1,0,1});
//		TK_Lib::Debug2D::Box({ 400,400 }, { 100,100 }, XMConvertToRadians(50),{0,1,1,1});
//		TK_Lib::Debug2D::Box({ 500,500 }, { 100,100 }, XMConvertToRadians(60),{0,0,1,1});
//		TK_Lib::Debug2D::Box({ 600,600 }, { 100,100 }, XMConvertToRadians(70),{1,1,1,1});
//	
//	
//		TK_Lib::Debug2D::Circle({ 0,0 },     10, { 1,0,0,1 });
//		TK_Lib::Debug2D::Circle({ 100,100 }, 20, { 1,1,0,1 });
//		TK_Lib::Debug2D::Circle({ 200,200 }, 30, { 1,0,1,1 });
//		TK_Lib::Debug2D::Circle({ 300,300 }, 40, { 1,1,0,1 });
//		TK_Lib::Debug2D::Circle({ 400,400 }, 50, { 0,1,1,1 });
//		TK_Lib::Debug2D::Circle({ 500,500 }, 60, { 0,0,1,1 });
//		TK_Lib::Debug2D::Circle({ 600,600 }, 70, { 1,1,1,1 });
//	
//		TK_Lib::Debug2D::Line({ 0,0 },     { 1000,0 },   { 1,0,0,1 });
//		TK_Lib::Debug2D::Line({ 100,100 }, { 1000,100 }, { 1,1,0,1 });
//		TK_Lib::Debug2D::Line({ 200,200 }, { 1000,200 }, { 1,0,1,1 });
//		TK_Lib::Debug2D::Line({ 300,300 }, { 1000,300 }, { 1,1,0,1 });
//		TK_Lib::Debug2D::Line({ 400,400 }, { 1000,400 }, { 0,1,1,1 });
//		TK_Lib::Debug2D::Line({ 500,500 }, { 1000,500 }, { 0,0,1,1 });
//		TK_Lib::Debug2D::Line({ 600,600 }, { 1000,600 }, { 1,1,1,1 });
//	
//		TK_Lib::Debug3D::Line({ 0,0,0 }, {10,0,10 }, { 1,0,1,1 });
//		TK_Lib::Debug3D::Box({ 10,10,10 }, { 20,20,20 }, { 1,0,1,1 });
//		TK_Lib::Debug3D::Circle ({ -10,10,10 }, 5, { 1,0,1,1 });
//
//		TK_Lib::Render::BeginRender();
//	}
//
//	TK_Lib::Render::EndRender();
//
//	if (sprite != nullptr)
//	{
//		delete sprite;
//		sprite = nullptr;
//	}
//	if (sprite2 != nullptr)
//	{
//		delete sprite2;
//		sprite = nullptr;
//	}
//
//	if (sprite3 != nullptr)
//	{
//		delete sprite3;
//		sprite3 = nullptr;
//	}
//
//	if (sprite4 != nullptr)
//	{
//		delete sprite4;
//		sprite4 = nullptr;
//	}
//
//	if (model1 != nullptr)
//	{
//		delete model1;
//		model1 = nullptr;
//	}
//	if (model2 != nullptr)
//	{
//		delete model2;
//		model2 = nullptr;
//	}
//	if (model3 != nullptr)
//	{
//		delete model3;
//		model3 = nullptr;
//	}
//	if (model4 != nullptr)
//	{
//		delete model4;
//		model4 = nullptr;
//	}
//
//	if (effect1 != nullptr)
//	{
//		delete effect1;
//		effect1 = nullptr;
//	}
//
//	if (effect2 != nullptr)
//	{
//		delete effect2;
//		effect2 = nullptr;
//	}
//
//
//
//
//
//	TK_Lib::End();
//
//
//
//	// 終了時の後処理
//	//WaitForPreviousFrame();
//	//CloseHandle(g_fenceEvent);
//
//	return 0;
//}

