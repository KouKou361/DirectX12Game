#include "Lib_Effect3DGeometry.h"
#include "Lib_GeometryVertexVecter.h"
#include "Lib_Graphics.h"
#include "Lib_PointSpriteRenderer.h"

Lib_Effect3D::Lib_Effect3D(Graphics* graphics, Effect3DGeometryRenderer* effect3DGeometryRenderer,Texture* texture, const int createNum)
{
	geometryVertexVector = new GeometryVertexVector(graphics, texture, createNum);
	this->effect3DGeometryRenderer = effect3DGeometryRenderer;
}

Lib_Effect3D::Lib_Effect3D()
{
}

Lib_Effect3D::~Lib_Effect3D()
{
	if (geometryVertexVector != nullptr)
	{
		delete geometryVertexVector;
		geometryVertexVector = nullptr;
	}
}
//開始。
void Lib_Effect3D::Play(const float lifeTime, const bool loopflg)
{
	this->loopflg = loopflg;
	this->lifeTime = lifeTime;
	effect3DGeometryRenderer->Register(geometryVertexVector);
}
//終了(新しくパーティクルを生成しない)
void Lib_Effect3D::Stop()
{
}
//終了(描画しない)
void Lib_Effect3D::Close()
{
}

void Lib_Effect3D::TimeUpdate(const float time)
{
	for (int i = 0; i < GetVertices().size(); i++)
	{
		if (GetVertices().at(i).time < 0)continue;
		GetVertices().at(i).time += time;

		if (GetVertices().at(i).time <= lifeTime)continue;

		GetVertices().at(i).time = -1;
		if (loopflg)
		{
			GetVertices().at(i).time = 0;
		}
		else
		{
			//削除
			GetVertices().at(i).Color.w = 0.0f;
		}
		

			
	}

	Apply();
}


void Lib_Effect3D::Apply()
{

	geometryVertexVector->Apply();
}

vector<GeometryVertex>& Lib_Effect3D::GetVertices()
{
	return geometryVertexVector->GetVertices();
}
