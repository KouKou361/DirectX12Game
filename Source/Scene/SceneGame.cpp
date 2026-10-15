#include "SceneBase.h"
#include "Lib.h"
#include "Lib_Model.h"
#include "../Character/Player.h"
#include "../Camera/CameraManager.h"
#include "../Camera/CameraBase.h"
#include "../Character/Enemy/EnemyManager.h"
#include "../NavMesh.h"
#include "../Source/Collision.h"
#include "../Source/Mathf.h"
#include "imgui.h"

#include <DebugDraw.h>
#include <DetourDebugDraw.h>
#include <DetourNavMesh.h>
#include <DetourCrowd.h>
#include <Recast.h>


const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;

void SceneGame::Initialize()
{
	
#if 1
	{
		
		//stageModel = TK_Lib::Load::model("./Data/Model/Stage/Stage_Souko/Stage.mdl");
		stageModel = TK_Lib::Load::model("./Data/Model/Stage/Stage_Souko/GroundTree.mdl");
		stageModel->PlayAnimation(0, true);

		//navMesh = new NavMesh();
		//navMesh->Build(stageModel);

		XMMATRIX transform, scale;
		XMMATRIX mat = XMMatrixIdentity();
		scale = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		transform = XMMatrixTranslation(00.0f, 0.0f, 0.0f);
		//scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		//transform = XMMatrixTranslation(0.0f,0.0f, 0.0f);

		XMStoreFloat4x4(&StageMat, scale * transform);
		stageModel->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
		stageModel->ChangeStatic(StageMat);
	}


#else
	{
		stageModel = TK_Lib::Load::model("./Data/Model/Stage/DebugStage/DebugStage.mdl");
		stageModel->PlayAnimation(0, true);

		//navMesh = new NavMesh();
		//navMesh->Build(stageModel);

		XMMATRIX transform, scale;
		XMMATRIX mat = XMMatrixIdentity();
		scale = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		transform = XMMatrixTranslation(00.0f, 0.0f, 0.0f);
		//scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		//transform = XMMatrixTranslation(0.0f,0.0f, 0.0f);

		XMStoreFloat4x4(&StageMat, scale * transform);
		stageModel->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
		stageModel->ChangeStatic(StageMat);
	}


#endif
	navMesh = new NavMesh();
	if (navMesh != nullptr)
	{
		navMesh->Build(stageModel);
		TK_Lib::Collision::ModelRegister(stageModel);
	}
	

	player = new Player(this);
	player->Initialize();

	enemyManager = new EnemyManager(this);
	enemyManager->Initialize();

	collision = new Collision(this);



	XMMATRIX transform, scale;
	XMMATRIX mat = XMMatrixIdentity();
	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	transform = XMMatrixTranslation(0.0f, .0f, 0.0f);
	XMStoreFloat4x4(&StageMat, scale * transform);

	TK_Lib::Load::model("./Data/Model/tree.mdl");

//
//	scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
//	transform = XMMatrixTranslation(0.0f, -5, 0.0f);
//	XMStoreFloat4x4(&Mat, scale*transform);
//
	
	cameraManager = new CameraManager(this);
	cameraManager->Initialize();
	cameraManager->ChangeCamera(CameraType::Normal);

	{
		DirectX::XMFLOAT4X4 Mat;
		skyModel = TK_Lib::Load::model("./Data/Model/Sky/Sky.mdl");
		skyModel->PlayAnimation(0, true);

		XMMATRIX transform, scale;
		XMMATRIX mat = XMMatrixIdentity();
		scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		transform = XMMatrixTranslation(player->GetPos().x, player->GetPos().y, player->GetPos().z);

		XMStoreFloat4x4(&Mat, scale* transform);
		skyModel->UpdateTransform(Mat);
		skyModel->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
	}

	
}

void SceneGame::Update()
{
	player->Update();
	enemyManager->Update();
	cameraManager->Update();
	if (navMesh != nullptr)
	{
		navMesh->Update(TK_Lib::Window::GetElapsedTime());
	}

	DirectX::XMFLOAT4X4 Mat;
	XMMATRIX transform, scale;
	XMMATRIX mat = XMMatrixIdentity();
	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	transform = XMMatrixTranslation(player->GetPos().x, player->GetPos().y, player->GetPos().z);

	XMStoreFloat4x4(&Mat, scale * transform);
	skyModel->UpdateTransform(Mat);
	skyModel->UpdateAnimation(TK_Lib::Window::GetElapsedTime());

	DebugImgui();
}

void SceneGame::Render()
{
	TK_Lib::Render::ModelRender(stageModel, SpriteShaderType::PBR);
	TK_Lib::Render::ModelRender(skyModel,SpriteShaderType::NORMAL);
	
	player->Render();
	enemyManager->Render();
	cameraManager->Render();


	TK_Lib::Collision::ModelRender();
	if (navMesh != nullptr)
	{
		
		navMesh->OnGUI(stageModel);
		TK_Lib::Debug3D::Nav(navMesh->GetNavMesh(), navMesh->GetNavMeshQuery(), StageMat, player->GetPos(),200);
	}

}

//デバッグ用
void SceneGame::DebugImgui()
{
	bool flg = TK_Lib::Imgui::GetImguiFlg();
	ImGui::Begin("Debug");
	ImGui::Checkbox("DebugOn", &flg);
	ImGui::End();
	TK_Lib::Imgui::SetImguiFlg(flg);


	flg = TK_Lib::Imgui::GetImguiNavFlg();
	ImGui::Begin("Debug");
	ImGui::Checkbox("DebugNavOn", &flg);
	ImGui::End();
	TK_Lib::Imgui::SetImguiNavFlg(flg);
}


void SceneGame::End()
{
	if (stageModel != nullptr)
	{
		delete stageModel;
		stageModel = nullptr;
	}

	if (skyModel != nullptr)
	{
		delete skyModel;
		skyModel = nullptr;
	}

	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	if (collision != nullptr)
	{
		delete collision;
		collision = nullptr;
	}
	if (cameraManager != nullptr)
	{
		cameraManager->End();
		delete cameraManager;
		cameraManager = nullptr;
	}

	if (navMesh != nullptr)
	{
		navMesh->Clean();
		delete navMesh;
		navMesh = nullptr;
	}

	if (enemyManager != nullptr)
	{
		enemyManager->End();
		delete enemyManager;
		enemyManager = nullptr;
	}
}
