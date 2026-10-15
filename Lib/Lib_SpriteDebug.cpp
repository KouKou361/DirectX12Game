#include "Lib_SpriteDebug.h"

SpriteDebug::SpriteDebug(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);
}


SpriteDebug::~SpriteDebug()
{
	vertexResource->Unmap(0, nullptr);
}

