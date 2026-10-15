#include "Lib_Collision.h"

bool Lib_Collision::RectVsLine(const Lib_CollisionRect& rect, const Lib_CollisionLine& line)
{
	VECTOR2 rectPos[4] =
	{
		{rect.pos1.x,rect.pos1.y},//右上
		{rect.pos1.x,rect.pos2.y+ rect.pos1.y},//右下
		{rect.pos2.x+rect.pos1.x,rect.pos1.y},//左上
		{rect.pos2.x+rect.pos1.x,rect.pos2.y+rect.pos1.y},//左下
	};

	VECTOR2 linePos[2] =
	{
		{line.start.x,line.start.y},//始
		{line.end.x,line.end.y},//終
	};

	XMVECTOR RectPos[4];
	for (int i=0;i<4;i++)
	{
		RectPos[i] = XMLoadFloat2(&rectPos[i]);
	}

	XMVECTOR LinePos[2];
	for (int i = 0; i < 2; i++)
	{
		LinePos[i] = XMLoadFloat2(&linePos[i]);
	}
	//
	XMVECTOR Line_V = XMVectorSubtract(LinePos[1], LinePos[0]);
	Line_V = XMVector2Normalize(Line_V);


	//
	XMVECTOR lineRect_v[4];
	for (int i = 0; i < 4; i++)
	{
		lineRect_v[i] = XMVectorSubtract(RectPos[i], LinePos[0]);
		lineRect_v[i] = XMVector2Normalize(lineRect_v[i]);
	}

	XMVECTOR reverceLineRect_v[4];
	for (int i = 0; i < 4; i++)
	{
		reverceLineRect_v[i] = XMVectorSubtract(RectPos[i], LinePos[1]);
		reverceLineRect_v[i] = XMVector2Normalize(reverceLineRect_v[i]);
	}

	bool result = false;
	float sign = 0;

	for (int i = 0; i < 4; i++)
	{


		XMVECTOR Cross = XMVector2Cross(Line_V, lineRect_v[i]);
		XMFLOAT2 cross;
		XMStoreFloat2(&cross,Cross);

		if (i == 0)
		{
			sign = cross.y;
		}
		else
		{
			float m = sign* cross.y;
			if (m <= 0)
			{

				XMVECTOR Dot = XMVector2Dot(Line_V, lineRect_v[i]);
				float dot;
				XMStoreFloat(&dot, Dot);
				if (dot < 0)continue;//線分外なので


				return true;
			}
		}
		//if (cross < 0)continue;//線分外なので

		//result = true;
		//break;
	}




	return result;
}

bool Lib_Collision::CircleVsCircle(const Lib_CollisionCircle& circle1, const Lib_CollisionCircle& circle2)
{
	XMVECTOR P1 = XMLoadFloat3(&circle1.pos);
	XMVECTOR P2 = XMLoadFloat3(&circle2.pos);

	XMVECTOR L = XMVectorSubtract(P1, P2);
	L = XMVector3Length(L);
	float l = 0;
	XMStoreFloat(&l,L);

	if (l >= circle1.radius + circle2.radius)return false;
	return true;
}
