#include "Lib_3DDebug.h"

GeometryDebug::GeometryDebug(Graphics* graphics)
{
	//頂点バッファ作成
	CreateVertex(graphics);
	//インデックスバッファ作成
	CreateIndex(graphics);
}


GeometryDebug::~GeometryDebug()
{
	vertexResource->Unmap(0, nullptr);
}

