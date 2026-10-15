#include "UI_Base.h"

UI_Base::UI_Base(SceneGame* scene)
{
	sceneGame = scene;
}

UI_Base::~UI_Base()
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children.at(i);
		children.at(i) =nullptr;
	}
	children.clear();
	children.shrink_to_fit();
}

//ワールド空間からスクリーン座標に変換
VECTOR2 UI_Base::WorldToScreen(VECTOR3 Pos)
{
	XMVECTOR WorldPosition;
	WorldPosition = XMLoadFloat3(&Pos);

	//座標変換
	XMMATRIX View = XMLoadFloat4x4(&TK_Lib::Camera::GetView());
	XMMATRIX Projection = XMLoadFloat4x4(&TK_Lib::Camera::GetProjection());
	XMMATRIX World = XMMatrixIdentity();

	VECTOR2 viewport = {1920,1080};

	VECTOR3 screenPos;
	//ワールド座標からスクリーン座標に変換する
	XMVECTOR ScreenPosition;
	ScreenPosition = XMVector3Project(WorldPosition, 0, 0, viewport.x, viewport.y, 0.0f, 1.0, Projection, View, World);
	XMStoreFloat3(&screenPos, ScreenPosition);

	return VECTOR2{ screenPos.x,screenPos.y };
}