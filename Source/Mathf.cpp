#include "Mathf.h"
#include <stdlib.h>
#include	<time.h>
#include <cmath>
#include "imgui.h"
//線形補完
float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}

float Mathf::RandomRange(float min, float max)
{
	//もし同じ数字ならリターン
	if (min - max == 0)return min;

	const float num = 1000;//割り切らせる数字

	float Max = max * num;
	float Min = min * num;
	int dis = static_cast<int>(abs(Max)) + static_cast<int>(abs(Min));
	float Rnd = static_cast<float>(rand() % static_cast<int>(dis));
	Rnd /= num;//整数/整数して

	return Rnd + min;

}
//正規化 Vec3
VECTOR3 Mathf::Vec3Normalize(const VECTOR3& Vec)
{
	//return{ 0,0,0 };
	XMVECTOR VEC;
	VEC = DirectX::XMLoadFloat3(&Vec);
	VEC = DirectX::XMVector3Normalize(VEC);
	VECTOR3 vec;
	DirectX::XMStoreFloat3(&vec, VEC);
	return vec;
}

//template<class Ty>
//[[nodiscard]]
////イージングテスト
//static float Mathf::EasingImgui(Ty time, Ty totaltime, Ty max , Ty min )
//{
//	float result = 0.0f;
//	ImGui::Begin("Easing");
//	static int mode = 0;
//
//	ImGui::RadioButton("InQuad",		&mode, 0);
//	ImGui::RadioButton("OutQuad",		&mode, 1);
//	ImGui::RadioButton("InOutQuad",		&mode, 2);
//	ImGui::RadioButton("InCubic",		&mode, 3);
//	ImGui::RadioButton("OutCubic",		&mode, 4);
//	ImGui::RadioButton("InOutCubic",	&mode, 5);
//	ImGui::RadioButton("InQuart",		&mode, 6);
//	ImGui::RadioButton("OutQuart",		&mode, 7);
//	ImGui::RadioButton("InQuint",		&mode, 8);
//	ImGui::RadioButton("OutQuint",		&mode, 9);
//	ImGui::RadioButton("InOutQuint",	&mode, 10);
//	ImGui::RadioButton("InSine",		&mode, 11);
//	ImGui::RadioButton("OutSine",		&mode, 12);
//	ImGui::RadioButton("InOutSine",		&mode, 13);
//	ImGui::RadioButton("InExp",			&mode, 14);
//	ImGui::RadioButton("OutExp",		&mode, 15);
//	ImGui::RadioButton("InOutExp",		&mode, 16);
//	ImGui::RadioButton("InCirc",		&mode, 17);
//	ImGui::RadioButton("OutCirc",		&mode, 18);
//	ImGui::RadioButton("InOutCirc",		&mode, 19);
//	ImGui::RadioButton("InBack",		&mode, 20);
//	ImGui::RadioButton("OutBack",		&mode, 21);
//	ImGui::RadioButton("InOutBack",		&mode, 22);
//	ImGui::RadioButton("OutBounce",		&mode, 23);
//	ImGui::RadioButton("InBounce",		&mode, 24);
//	ImGui::RadioButton("InOutBounce",	&mode, 25);
//	ImGui::RadioButton("Linear",		&mode, 26);
//
//
//	ImGui::End();
//
//	switch (mode)
//	{
//	case 0:
//		return InQuad(time,totaltime,max,min);
//		break;
//	case 1:
//		return OutQuad(time, totaltime, max, min);
//		break;
//	case 2:
//		return InOutQuad(time, totaltime, max, min);
//		break;
//	case 3:
//		return InCubic(time, totaltime, max, min);
//		break;
//	case 4:
//		return OutCubic(time, totaltime, max, min);
//		break;
//	case 5:
//		return InOutCubic(time, totaltime, max, min);
//		break;
//	case 6:
//		return InQuart(time, totaltime, max, min);
//		break;
//	case 7:
//		return OutQuart(time, totaltime, max, min);
//		break;
//	case 8:
//		return InQuint(time, totaltime, max, min);
//		break;
//	case 9:
//		return OutQuint(time, totaltime, max, min);
//		break;
//	case 10:
//		return InOutQuint(time, totaltime, max, min);
//		break;
//	case 11:
//		return InSine(time, totaltime, max, min);
//		break;
//	case 12:
//		return OutSine(time, totaltime, max, min);
//		break;
//	case 13:
//		return InOutSine(time, totaltime, max, min);
//		break;
//	case 14:
//		return InExp(time, totaltime, max, min);
//		break;
//	case 15:
//		return OutExp(time, totaltime, max, min);
//		break;
//	case 16:
//		return InOutExp(time, totaltime, max, min);
//		break;
//	case 17:
//		return InCirc(time, totaltime, max, min);
//		break;
//	case 18:
//		return OutCirc(time, totaltime, max, min);
//		break;
//	case 19:
//		return InOutCirc(time, totaltime, max, min);
//		break;
//	case 20:
//		return InBack(time, totaltime, max, min);
//		break;
//	case 21:
//		return OutBack(time, totaltime, max, min);
//		break;
//	case 22:
//		return InOutBack(time, totaltime, max, min);
//		break;
//	case 23:
//		return OutBounce(time, totaltime, max, min);
//		break;
//	case 24:
//		return InBounce(time, totaltime, max, min);
//		break;
//	case 25:
//		return InOutBounce(time, totaltime, max, min);
//		break;
//	case 26:
//		return Linear(time, totaltime, max, min);
//		break;
//
//	}
//
//	return result;
//}
//