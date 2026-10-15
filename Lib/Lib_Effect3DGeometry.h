#pragma once
#include <vector>
using namespace std;

class Texture;
class GeometryVertexVector;
class Graphics;
class Effect3DGeometryRenderer;
class GeometryVertex;

class Lib_Effect3D
{
public:
	Lib_Effect3D(Graphics* graphics, Effect3DGeometryRenderer* effect3DGeometryRenderer, Texture* texture, const int createNum);
	Lib_Effect3D();
	~Lib_Effect3D();

	//開始。
	void Play(const float lifeTime,  const bool loopflg = 0 );
	//終了(新しくパーティクルを生成しない)
	void Stop();
	//終了(描画しない)
	void Close();

	void TimeUpdate(const float time);



	
	//適用
	void Apply();
	vector<GeometryVertex>& GetVertices();
		

		
private:
	GeometryVertexVector* geometryVertexVector=nullptr;
	Effect3DGeometryRenderer* effect3DGeometryRenderer = nullptr;

	bool loopflg=false;
	float lifeTime;
};