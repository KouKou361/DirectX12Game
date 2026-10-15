#include "Lib_CameraLib.h"

void CameraLib::SetLookAt(
	const VECTOR3& pos,
	const VECTOR3& target,
	const VECTOR3& up)
{
	using namespace DirectX;

	XMVECTOR Eye = XMLoadFloat3(&pos);
	XMVECTOR Focus = XMLoadFloat3(&target);
	XMVECTOR Up = XMLoadFloat3(&up);
	XMMATRIX View = XMMatrixLookAtLH(Eye, Focus, Up);
	XMStoreFloat4x4(&view, View);

	// ビュー行列を逆行列化(ワールド行列に変換)
	XMFLOAT4X4 world;
	XMMATRIX World = XMMatrixInverse(nullptr, View);

	XMStoreFloat4x4(&world, World);

	// カメラの方向を取り出す
	this->right = { world._11, world._12, world._13 };
	this->up = { world._21, world._22, world._23 };
	this->front = { world._31, world._32, world._33 };

	// 視点、注視点を保存
	this->pos = pos;
	this->target = target;



}

void CameraLib::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	fov = fovY;
	aspext = aspect;

	m_near = nearZ;
	m_far = farZ;

	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection); // プロジェクション行列作成
}
void CameraLib::SetOrthongraphic(float Width, float Height, float Near, float Far)
{
	m_near = Near;
	m_far = Far;
	DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
	XMStoreFloat4x4(&projection, Projection);
}


