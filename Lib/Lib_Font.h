#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <vector>

using namespace std;

class Graphics;
class Texture;
class SpriteShader;



class Font
{
private:
	vector<Texture* >textures;

	struct CharacterInfo
	{
		static const WORD			NonCode = 0;			// コードなし
		static const WORD			EndCode = 0xFFFF;		// 終了コード
		static const WORD			ReturnCode = 0xFFFE;	// 改行コード
		static const WORD			TabCode = 0xFFFD;		// タブコード
		static const WORD			SpaceCode = 0xFFFC;		// 空白コード

		float						left;					// テクスチャ左座標
		float						top;					// テクスチャ上座標
		float						right;					// テクスチャ右座標
		float						bottom;					// テクスチャ下座標
		float						xoffset;				// オフセットX
		float						yoffset;				// オフセットY
		float						xadvance;				// プロポーショナル幅
		float						width;					// 画像幅
		float						height;					// 画像高さ
		int							page;					// テクスチャ番号
		bool						ascii;					// アスキー文字
	};

	float							fontWidth;
	float							fontHeight;
	int								textureCount;
	int								characterCount;
	std::vector<CharacterInfo>		characterInfos;
	std::vector<WORD>				characterIndices;

	
public:
	Font(Graphics* graphics, const string fileName);
	~Font();

	void Draw(Graphics* graphics, SpriteShader* spriteShader, float x, float y, const wchar_t* string);
};