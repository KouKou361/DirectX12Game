#pragma once
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <string>


using namespace DirectX;
using namespace std;

//アライメントに揃えたサイズを返す。
size_t AligmentedSize(size_t size, size_t aligment);

//マルチバイト文字からワイド文字に変換
wstring  GetWidiStringFromString(const string& str);

//モデルのパスとテクスチャのパスから合成パスを得る
string GetTeturePathFromModelAndTexPath(const string& modelPath, const char* texPath);

//ファイル名から拡張子を取得する
string GetExtension(const string& path);

//テクスチャのパスをセパレーター文字で分離する
pair<string, string> SplitFileName(const string& path, const char splitter = '*');