#include "Lib_Texture.h"
#include "Lib_Graphics.h"

Texture::Texture(Graphics* graphics, const string fileName)
{
	//テクスチャ作成
	CreateTextureResource(graphics, fileName);

	this->fileName = fileName;
}

Texture::~Texture()
{

}

//テクスチャ作成
void Texture::CreateTextureResource(Graphics* graphics, const string fileName)
{
	ID3D12Device* device = graphics->GetDevice();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	textureResource = graphics->LoadTexture(fileName);
	textureDescriptor = graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();
	srvDesc.Format = textureResource->GetDesc().Format;

	device->CreateShaderResourceView(textureResource,
		&srvDesc,
		textureDescriptor->GetCpuHandle());

	textureWidth = textureResource->GetDesc().Width;
	textureHeight = textureResource->GetDesc().Height;
}