#include "Lib_Font.h"
#include <crtdbg.h>
#include <assert.h>
#include <memory>
#include "Lib_Graphics.h"
#include "Lib_Texture.h"
#include "Lib_Shader.h"


Font::Font(Graphics* graphics, const string fileName)
{
	// .fnt 読み込み
	{
		FILE* fp = nullptr;
		fopen_s(&fp, fileName.c_str(), "rb");
		if (fp == nullptr)
		{
			assert(!L"FNT File not found");
		}

		fseek(fp, 0, SEEK_END);
		long fntSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<char[]> fntData = std::make_unique<char[]>(fntSize);
		fread(fntData.get(), fntSize, 1, fp);
		fclose(fp);

		// (***.fnt)ヘッダ解析
		char* context = nullptr;	// 内部的に使用するので深く考えない
		const char* delimiter = " ,=\"\r\n";
		const char* delimiter2 = ",=\"\r\n";
		char* pToken = ::strtok_s(fntData.get(), delimiter, &context);
		if (::strcmp(pToken, "info") != 0)
		{
			assert(!L"FNT Format");
		}

		// face
		::strtok_s(nullptr, delimiter, &context);
		const char* face = ::strtok_s(nullptr, delimiter2, &context);
		// size
		::strtok_s(nullptr, delimiter, &context);
		int size = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// bold
		::strtok_s(nullptr, delimiter, &context);
		int bold = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// italic
		::strtok_s(nullptr, delimiter, &context);
		int italic = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// charset
		::strtok_s(nullptr, delimiter, &context);
		const char* charset = ::strtok_s(nullptr, delimiter, &context);
		// unicode
		if (::strcmp(charset, "unicode") == 0)
			charset = "";
		else
			::strtok_s(nullptr, delimiter, &context);
		int unicode = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// stretchH
		::strtok_s(nullptr, delimiter, &context);
		int stretchH = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// smooth
		::strtok_s(nullptr, delimiter, &context);
		int smooth = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// aa
		::strtok_s(nullptr, delimiter, &context);
		int aa = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// padding
		::strtok_s(nullptr, delimiter, &context);
		int padding1 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		int padding2 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		int padding3 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		int padding4 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// spacing
		::strtok_s(nullptr, delimiter, &context);
		int spacing1 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		int spacing2 = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// outline
		::strtok_s(nullptr, delimiter, &context);
		int outline = ::atoi(::strtok_s(nullptr, delimiter, &context));

		// common
		::strtok_s(nullptr, delimiter, &context);
		// lineHeight
		::strtok_s(nullptr, delimiter, &context);
		int lineHeight = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// base
		::strtok_s(nullptr, delimiter, &context);
		int base = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// scaleW
		::strtok_s(nullptr, delimiter, &context);
		int scaleW = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// scaleH
		::strtok_s(nullptr, delimiter, &context);
		int scaleH = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// pages
		::strtok_s(nullptr, delimiter, &context);
		int pages = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// packed
		::strtok_s(nullptr, delimiter, &context);
		int packed = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// alphaChnl
		::strtok_s(nullptr, delimiter, &context);
		int alphaChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// redChnl
		::strtok_s(nullptr, delimiter, &context);
		int redChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// greenChnl
		::strtok_s(nullptr, delimiter, &context);
		int greenChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
		// blueChnl
		::strtok_s(nullptr, delimiter, &context);
		int blueChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));

		// ディレクトリパス取得
		char dirname[256];
		::_splitpath_s(fileName.c_str(), nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);
		textures.resize(pages);
		for (int i = 0; i < pages; i++)
		{
			// page
			::strtok_s(nullptr, delimiter, &context);
			// id
			::strtok_s(nullptr, delimiter, &context);
			int id = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// file
			::strtok_s(nullptr, delimiter, &context);
			const char* file = ::strtok_s(nullptr, delimiter2, &context);
		
			// 相対パスの解決
			char fname[256];
			::_makepath_s(fname, 256, nullptr, dirname, file, nullptr);
		
			//// マルチバイト文字からワイド文字へ変換
			//size_t length;
			//wchar_t wfname[256];
			//::mbstowcs_s(&length, wfname, 256, fname, _TRUNCATE);
		
			// テクスチャ読み込み
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			textures.at(i) = new Texture(graphics, fname);
			//HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfname, resource.GetAddressOf(), shaderResourceViews.at(i).GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		
		// chars
		::strtok_s(nullptr, delimiter, &context);
		// count
		::strtok_s(nullptr, delimiter, &context);
		int count = ::atoi(::strtok_s(nullptr, delimiter, &context));
		
		// データ格納
		fontWidth = static_cast<float>(size);
		fontHeight = static_cast<float>(lineHeight);
		textureCount = pages;
		characterCount = count + 1;
		characterInfos.resize(characterCount);
		characterIndices.resize(0xFFFF);
		::memset(characterIndices.data(), 0, sizeof(WORD) * characterIndices.size());
		
		characterIndices.at(0x00) = CharacterInfo::EndCode;
		characterIndices.at(0x0a) = CharacterInfo::ReturnCode;
		characterIndices.at(0x09) = CharacterInfo::TabCode;
		characterIndices.at(0x20) = CharacterInfo::SpaceCode;
		
		// 文字情報解析
		int n = 1;
		for (int i = 0; i < count; i++)
		{
			// char
			::strtok_s(nullptr, delimiter, &context);
			// id
			::strtok_s(nullptr, delimiter, &context);
			int id = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// x
			::strtok_s(nullptr, delimiter, &context);
			int x = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// y
			::strtok_s(nullptr, delimiter, &context);
			int y = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// width
			::strtok_s(nullptr, delimiter, &context);
			int width = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// height
			::strtok_s(nullptr, delimiter, &context);
			int height = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// xoffset
			::strtok_s(nullptr, delimiter, &context);
			int xoffset = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// yoffset
			::strtok_s(nullptr, delimiter, &context);
			int yoffset = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// xadvance
			::strtok_s(nullptr, delimiter, &context);
			int xadvance = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// page
			::strtok_s(nullptr, delimiter, &context);
			int page = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// chnl
			::strtok_s(nullptr, delimiter, &context);
			int chnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
		
			// 文字コード格納
			if (id >= 0x10000) continue;
		
			CharacterInfo& info = characterInfos.at(n);
		
			characterIndices.at(id) = static_cast<WORD>(i + 1);
		
			// 文字情報格納
			info.left = static_cast<float>(x) / static_cast<float>(scaleW);
			info.top = static_cast<float>(y) / static_cast<float>(scaleH);
			info.right = static_cast<float>(x + width) / static_cast<float>(scaleW);
			info.bottom = static_cast<float>(y + height) / static_cast<float>(scaleH);
			info.xoffset = static_cast<float>(xoffset);
			info.yoffset = static_cast<float>(yoffset);
			info.xadvance = static_cast<float>(xadvance);
			info.width = static_cast<float>(width);
			info.height = static_cast<float>(height);
			info.page = page;
			info.ascii = (id < 0x100);
		
			n++;
		}
	}
}

Font::~Font()
{
	for (Texture* tex:textures)
	{
		delete tex;
		tex = nullptr;
	}
}

void Font::Draw(Graphics* graphics, SpriteShader* spriteShader, float x, float y, const wchar_t* string)
{
	size_t length = ::wcslen(string);

	float start_x = x;
	float start_y = y;
	float space = fontWidth;

	for (size_t i = 0; i < length; ++i)
	{
		// 文字値から文字情報が格納されているコードを取得
		WORD word = static_cast<WORD>(string[i]);
		WORD code = characterIndices.at(word);

		// 特殊制御用コードの処理
		if (code == CharacterInfo::EndCode)
		{
			break;
		}
		else if (code == CharacterInfo::ReturnCode)
		{
			x = start_x;
			y += fontHeight;
			continue;
		}
		else if (code == CharacterInfo::TabCode)
		{
			x += space * 4;
			continue;
		}
		else if (code == CharacterInfo::SpaceCode)
		{
			x += space;
			continue;
		}

		// 文字情報を取得し、頂点データを編集
		const CharacterInfo& info = characterInfos.at(code);

		float positionX = x + info.xoffset;// + 0.5f;
		float positionY = y + info.yoffset;// + 0.5f;

		;
		XMFLOAT2 size = textures.at(info.page)->GetSize();
		spriteShader->SetVertex(graphics, textures.at(info.page), { positionX,positionY },
			{ info.width ,info.height }, { info.left* size.x ,info.top * size.y ,info.right * size.x ,info.bottom * size.y }, {1,1,1,1},0);

		x += info.xadvance;
	//	// 0---1
	//	// |   |
	//	// 2---3
	//	currentVertex[0].position.x = positionX;
	//	currentVertex[0].position.y = positionY;
	//	currentVertex[0].position.z = 0.0f;
	//	currentVertex[0].texcoord.x = info.left;
	//	currentVertex[0].texcoord.y = info.top;
	//	currentVertex[0].color.x = 1.0f;
	//	currentVertex[0].color.y = 1.0f;
	//	currentVertex[0].color.z = 1.0f;
	//	currentVertex[0].color.w = 1.0f;
	//
	//	currentVertex[1].position.x = positionX + info.width;
	//	currentVertex[1].position.y = positionY;
	//	currentVertex[1].position.z = 0.0f;
	//	currentVertex[1].texcoord.x = info.right;
	//	currentVertex[1].texcoord.y = info.top;
	//	currentVertex[1].color.x = 1.0f;
	//	currentVertex[1].color.y = 1.0f;
	//	currentVertex[1].color.z = 1.0f;
	//	currentVertex[1].color.w = 1.0f;
	//
	//	currentVertex[2].position.x = positionX;
	//	currentVertex[2].position.y = positionY + info.height;
	//	currentVertex[2].position.z = 0.0f;
	//	currentVertex[2].texcoord.x = info.left;
	//	currentVertex[2].texcoord.y = info.bottom;
	//	currentVertex[2].color.x = 1.0f;
	//	currentVertex[2].color.y = 1.0f;
	//	currentVertex[2].color.z = 1.0f;
	//	currentVertex[2].color.w = 1.0f;
	//
	//	currentVertex[3].position.x = positionX + info.width;
	//	currentVertex[3].position.y = positionY + info.height;
	//	currentVertex[3].position.z = 0.0f;
	//	currentVertex[3].texcoord.x = info.right;
	//	currentVertex[3].texcoord.y = info.bottom;
	//	currentVertex[3].color.x = 1.0f;
	//	currentVertex[3].color.y = 1.0f;
	//	currentVertex[3].color.z = 1.0f;
	//	currentVertex[3].color.w = 1.0f;
	//
	//	// NDC座標変換
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		currentVertex[j].position.x = 2.0f * currentVertex[j].position.x / screenWidth - 1.0f;
	//		currentVertex[j].position.y = 1.0f - 2.0f * currentVertex[j].position.y / screenHeight;
	//	}
	//	currentVertex += 4;
	//
	//	x += info.xadvance;
	//
	//	// テクスチャが切り替わる度に描画する情報を設定
	//	if (currentPage != info.page)
	//	{
	//		currentPage = info.page;
	//
	//		Subset subset;
	//		subset.shaderResourceView = shaderResourceViews.at(info.page).Get();
	//		subset.startIndex = currentIndexCount;
	//		subset.indexCount = 0;
	//		subsets.emplace_back(subset);
	//	}
	//	currentIndexCount += 6;
	}
	
}
