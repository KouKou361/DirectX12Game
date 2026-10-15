#include "Lib_Shader.h"
#include <d3dx12.h>
#include <algorithm>
#include <d3dcompiler.h>
#include "Lib_Graphics.h"
//shader基底クラス
Shader::Shader(const LPCWSTR shaderVS, const LPCWSTR shaderPS)
{

	HRESULT hr = S_OK;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダーの作成
	hr = D3DCompileFromFile(
		shaderVS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}


   //ピクセルシェーダーの作成
	hr = D3DCompileFromFile(
		shaderPS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}
}

Shader::Shader(const LPCWSTR shaderVS, const LPCWSTR shaderGS, const LPCWSTR shaderPS)
{
	HRESULT hr = S_OK;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダーの作成
	hr = D3DCompileFromFile(
		shaderVS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}

	//頂点シェーダーの作成
	hr = D3DCompileFromFile(
		shaderGS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "gs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&gsShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}


	//ピクセルシェーダーの作成
	hr = D3DCompileFromFile(
		shaderPS,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psShader, &errorBlob
	);
	if (hr != S_OK)
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))assert(!L"シェーダーの作成のエラー「ファイルが見つかりません」");
		else assert(!L"シェーダーの作成のエラー");
	}
}

Shader::~Shader()
{

}

