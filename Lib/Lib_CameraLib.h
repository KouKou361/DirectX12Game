#pragma once
#include "Lib_Vector.h"
//このようなクラス名にしているのかというと
//ライブラリーを使っているソースに同じ名前になってしまうのを防ぐためである

//まずこんな名前にしたらクラス名がかぶったりしないだろう
class CameraLib
{
public:
	CameraLib() {}
	~CameraLib() {}
public:
	// 指定方向を向く
	void SetLookAt(
		const VECTOR3& pos,
		const VECTOR3& Target,
		const VECTOR3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	void SetOrthongraphic(float Width, float Height, float Near, float Far);


	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// 視点取得
	const VECTOR3& GetPos() const { return pos; }

	// 注視点取得
	const VECTOR3& GetTarget() const { return target; }

	// 上方向取得
	const VECTOR3& GetUp() const { return up; }

	// 前方向取得
	const VECTOR3& GetFront() const { return front; }

	// 右方向取得
	const VECTOR3& GetRight() const { return right; }

	// 右方向取得
	const float& GetNear() const { return m_near; }

	// 右方向取得
	const float& GetFar() const { return m_far; }


	// 最短距離
	const float& GetFor() const { return fov; }

	// 遠距離
	const float& GetAspext() const { return aspext; }
	//MEMO:SetView(XMMATRIX)とかあったらいいかも
	//SetProjection

private:

	DirectX::XMFLOAT4X4 view = { 0,0,0,0,  0,0,0,0, 0,0,0,0,  0,0,0,0 };
	DirectX::XMFLOAT4X4 projection = { 0,0,0,0,  0,0,0,0, 0,0,0,0,  0,0,0,0 };

	VECTOR3 pos = { 0,0,0 };
	VECTOR3 target = { 0,0,0 };

	VECTOR3 up = { 0,0,0 };
	VECTOR3 front = { 0,0,0 };
	VECTOR3 right = { 0,0,0 };

	float m_near = 0;
	float m_far = 0;
	float fov = 0;
	float aspext = 0;
};
