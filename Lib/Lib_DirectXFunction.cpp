#include "Lib_DirectXFunction.h"

//アライメントに揃えたサイズを返す。
size_t AligmentedSize(size_t size, size_t aligment)
{
	return size + aligment - size % aligment;
}

//マルチバイト文字からワイド文字に変換
wstring GetWidiStringFromString(const std::string& str)
{
	//呼び出し一回目（文字列数をえる）
	auto num1 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		nullptr,
		0
	);

	wstring wstr;
	wstr.resize(num1);

	//呼び出し二回目(確保済みのwstrに変数文字列をコピー)
	auto num2 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		&wstr[0],
		num1
	);
	assert(num1 == num2);//エラーチェック
	return wstr;
}

//モデルのパスとテクスチャのパスから合成パスを得る
string GetTeturePathFromModelAndTexPath(const string& modelPath, const char* texPath)
{
	auto folderPath = modelPath.substr(0, modelPath.rfind('/'));
	return folderPath+ '/' + texPath;
}

//ファイル名から拡張子を取得する
string GetExtension(const string& path)
{
	int idx = path.rfind('.');
	return path.substr(idx + 1, path.length() - idx - 1);
}

//テクスチャのパスをセパレーター文字で分離する
pair<string, string> SplitFileName(const string& path, const char splitter)
{
	int idx = path.find(splitter);
	pair<string, string> ret;
	ret.first = path.substr(0, idx);
	ret.second = path.substr(idx+1,path.length()-idx-1);
	return ret;
}
