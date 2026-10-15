#include "CameraBase.h"
#include "../Character/Player.h"
#include "../Scene/SceneBase.h"
#include "../Character/Enemy/EnemyManager.h"
#include "../Character/Enemy/EnemyBase.h"
#include "../Scene/SceneBase.h"
#include "../Character/Player.h"
//èâä˙âª
void CameraNormal::Initialize()
{

	earsingBlendEndPos = sceneGame->player->GetPos();
	earsingBlendEndTargetPos = sceneGame->player->GetPos();

	pos = { 0,12,0 };
	targetPos = { 0, 0, 0 };
	up = { 0, 1, 0 };

}
//çXêV
void CameraNormal::Update()
{
	UpdateTarget();
	UpdatePos();

	
	CameraBase::Update();
}
//ï`âÊ
void CameraNormal::Render()
{

}
//èIóπ
void CameraNormal::End()
{

}


void CameraNormal::UpdatePos()
{
	XMFLOAT2 inputDir;
	inputDir.x = TK_Lib::KeyInput::GetAxisRX();
	inputDir.y = TK_Lib::KeyInput::GetAxisRY();



	const float time = TK_Lib::Window::GetElapsedTime();
	if (inputDir.x <=-0.2f)  cangle -= time;
	if (inputDir.x >= 0.2f)  cangle += time;
	if (inputDir.y >= 0.2f)  pos.y += time * camera_up_speed;
	if (inputDir.y <= -0.2f) pos.y -= time * camera_up_speed;

	pos.x = sinf(cangle) * cdist + targetPos.x;
	pos.z = cosf(cangle) * cdist + targetPos.z;

	{
		static VECTOR3 lightPos = {};
		const float lightTime = TK_Lib::Window::GetElapsedTime();
		if (inputDir.x <= -0.2f)  cangle -= time;
		if (inputDir.x >= 0.2f)  cangle += time;
		if (inputDir.y >= 0.2f)  pos.y += time * camera_up_speed;
		if (inputDir.y <= -0.2f) pos.y -= time * camera_up_speed;

		//if (GetKeyState('U') < 0)  cdist += time * camera_dist_speed;
		//if (GetKeyState('O') < 0)  cdist -= time * camera_dist_speed;
		pos.x = sinf(cangle) * cdist + targetPos.x;
		//pos.y = targetPos.y + cdist;
		pos.z = cosf(cangle) * cdist + targetPos.z;
	}

}

void CameraNormal::UpdateTarget()
{
	Player* player = sceneGame->player;
	targetPos.x = player->GetPos().x;
	targetPos.y = player->GetPos().y;
	targetPos.z = player->GetPos().z;
}
