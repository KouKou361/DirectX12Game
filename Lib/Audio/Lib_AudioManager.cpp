#include "Lib_Audio.h"

Lib_AudioManager::Lib_AudioManager(const HWND &hWbd)
{
	DirectSoundCreate8(NULL, &pDS8, NULL);
	AudioTable.clear();
	pDS8->SetCooperativeLevel(hWbd, DSSCL_PRIORITY);
}

Lib_AudioManager::~Lib_AudioManager()
{
	Clear();
	pDS8->Release();
}

Lib_AudioBase* Lib_AudioManager::CreateAudio(std::string fileName)
{
	Lib_AudioBase* audio = new Lib_AudioBase();
	audio->CreateAudio(pDS8, fileName);
	return audio;
}

void Lib_AudioManager::Update()
{

}

//サウンドの全ての削除
void Lib_AudioManager::Clear()
{
	//modelResourceTable;
	auto iter = AudioTable.begin();
	while (iter != AudioTable.end()) {
		Lib_AudioBase* audioBase = iter->second;
		if (audioBase != nullptr)
		{
			delete audioBase;
			audioBase = nullptr;
		}

		++iter;
	}
}

//重複しているかどうか
Lib_AudioBase* Lib_AudioManager::SearchAudionKey(const char* key)
{
	auto it = AudioTable.find(key);
	if (it != AudioTable.end())return it->second;

	return nullptr;
}

//登録サウンド
Lib_AudioBase* Lib_AudioManager::Resiger(const char* filePath, const char* key)
{
	//モデルが既に存在している場合　それを返す。
	Lib_AudioBase* audio = SearchAudionKey(key);
	if (audio != nullptr)return audio;

	//モデルの作成
	audio = CreateAudio(filePath);
	//モデルの登録
	AudioTable[key] = audio;

	return audio;
}
