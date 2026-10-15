#include "CameraBase.h"
#include "../Source/Mathf.h"

CameraBase::CameraBase(SceneGame* sceneGame)
{
	this->sceneGame = sceneGame;
}

void CameraBase::Initialize()
{
}

void CameraBase::Update()
{
	TK_Lib::Camera::SetLookAt(pos,targetPos,up);
}

void CameraBase::Render()
{
}

void CameraBase::End()
{
}

void CameraBase::EarsingBlendCameraPos()
{
	if (earsingBlendTime >= earsingBlendSecandTime)return;

	pos.x = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime,  earsingBlendEndPos.x, earsingBlendStartPos.x);
	pos.y = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime,  earsingBlendEndPos.y, earsingBlendStartPos.y);
	pos.z = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime,  earsingBlendEndPos.z, earsingBlendStartPos.z);

	targetPos.x = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime, earsingBlendEndTargetPos.x, earsingBlendStartTargetPos.x);
	targetPos.y = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime, earsingBlendEndTargetPos.y, earsingBlendStartTargetPos.y);
	targetPos.z = Mathf::Linear(earsingBlendTime, earsingBlendSecandTime, earsingBlendEndTargetPos.z, earsingBlendStartTargetPos.z);

	earsingBlendTime += TK_Lib::Window::GetElapsedTime();
}
