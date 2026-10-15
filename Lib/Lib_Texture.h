#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Lib_Vector.h"

class Graphics;
class Descriptor;

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

class Texture
{
public:
	Texture(Graphics* graphics, const string fileName = nullptr);
	~Texture();

	Descriptor* GetDescriptor() { return textureDescriptor; }
	VECTOR2 GetSize() { return { textureWidth ,textureHeight }; }
	string GetFileName() { return fileName; }
private:
	//テクスチャ作成
	void CreateTextureResource(Graphics* graphics, const string fileName);

	ID3D12Resource* textureResource = nullptr;
	Descriptor* textureDescriptor = nullptr;
	FLOAT textureWidth;
	FLOAT textureHeight;
	HRESULT hr = S_OK;
	string fileName;
};