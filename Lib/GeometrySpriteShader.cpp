#include "Lib_Shader.h"
#include "Lib_GeometryVertexVecter.h"


void GeometryPointShader::Register(GeometryVertexVector* geometryVertexVecter)
{
	geometryVertexVecterList.push_back(geometryVertexVecter);
}

void GeometryPointShader::AllRender(Graphics* graphics, float time)
{
	Begin(graphics);

	for (int i = 0; i < geometryVertexVecterList.size(); i++)
	{
		GeometryVertexVector* geometryVertexVecter = geometryVertexVecterList.at(i);

		Render(
			graphics, geometryVertexVecter
		);

	}

	End(graphics);
}
