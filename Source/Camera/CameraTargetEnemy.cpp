#include "CameraBase.h"
#include "CameraManager.h"
#include "../Character/Player.h"
#include "../Scene/SceneBase.h"
//初期化
void CameraTargetEnemy::Initialize()
{
	pos = { 0,12,0 };
	targetPos = { 0, 0, 0 };
	up = { 0, 1, 0 };
}
//更新
void CameraTargetEnemy::Update()
{
	if (sceneGame->player->GetTarget())
	{
		//目標がいない場合
		sceneGame->cameraManager->ChangeCamera(CameraType::Normal);
	}

	UpdateTarget();
	UpdatePos();


	CameraBase::Update();
}
//描画
void CameraTargetEnemy::Render()
{

}
//終了
void CameraTargetEnemy::End()
{

}

void CameraTargetEnemy::UpdatePos()
{
//	XMFLOAT2 inputDir;
//	inputDir.x = TK_Lib::KeyInput::GetAxisRX();
//	inputDir.y = TK_Lib::KeyInput::GetAxisRY();
//
//
//
//	const float time = TK_Lib::Window::GetElapsedTime();
//	if (inputDir.x <= -0.2f)  cangle -= time;
//	if (inputDir.x >= 0.2f)  cangle += time;
//	if (inputDir.y >= 0.2f)  pos.y += time * camera_up_speed;
//	if (inputDir.y <= -0.2f) pos.y -= time * camera_up_speed;
//
//	//if (GetKeyState('U') < 0)  cdist += time * camera_dist_speed;
//	//if (GetKeyState('O') < 0)  cdist -= time * camera_dist_speed;
//	pos.x = sinf(cangle) * cdist + targetPos.x;
//	//pos.y = targetPos.y + cdist;
//	pos.z = cosf(cangle) * cdist + targetPos.z;
//
//
//
//	{
//		static VECTOR3 lightPos = {};
//		const float lightTime = TK_Lib::Window::GetElapsedTime();
//		if (inputDir.x <= -0.2f)  cangle -= time;
//		if (inputDir.x >= 0.2f)  cangle += time;
//		if (inputDir.y >= 0.2f)  pos.y += time * camera_up_speed;
//		if (inputDir.y <= -0.2f) pos.y -= time * camera_up_speed;
//
//		//if (GetKeyState('U') < 0)  cdist += time * camera_dist_speed;
//		//if (GetKeyState('O') < 0)  cdist -= time * camera_dist_speed;
//		pos.x = sinf(cangle) * cdist + targetPos.x;
//		//pos.y = targetPos.y + cdist;
//		pos.z = cosf(cangle) * cdist + targetPos.z;
//	}
//
}

void CameraTargetEnemy::UpdateTarget()
{
	VECTOR3 playerPos = sceneGame->player->GetPos();
	VECTOR3 enemyPos = sceneGame->player->GetTarget()->GetPos();

	VECTOR3 resultPos = playerPos+ enemyPos;
	targetPos = resultPos / 2;
}
