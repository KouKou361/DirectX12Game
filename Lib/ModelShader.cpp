
#include "Lib_Shader.h"
#include <d3dx12.h>

#include "Lib_Graphics.h"
#include "MeshInstance.h"

void SpriteShader::AllRender(Graphics* graphics)
{
	Begin(graphics);

	for (int i = 0; i < renderNum; i++)
	{
		Render(graphics,i);

	}

	End(graphics);
}



void ModelShader::Register(Model* model)
{
	meshInstanceDrawList->Register(model);
}

void ModelShader::AllRender(Graphics* graphics)
{
	Begin(graphics);

	for (int i = 0; i < meshInstanceDrawList->modelList.size(); i++)
	{
		std::vector<Model*> model = meshInstanceDrawList->modelList.at(i);
	
		for (int i = 0; i < model.size(); i++)
		{
			Render(
				graphics, model.at(i)
			);
		}
		
	
	}
	
	End(graphics);
}

void ModelShader::NewMeshInstanceDrawList()
{
	meshInstanceDrawList = new MeshInstanceDrawList;
}

void ModelShader::End(Graphics* graphics)
{
	meshInstanceDrawList->Clear();
}