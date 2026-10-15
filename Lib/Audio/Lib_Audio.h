#pragma once
#include <dsound.h>
#include <string>
#include <map>
using namespace std;

class Lib_AudioBase
{
public:
	Lib_AudioBase();
	~Lib_AudioBase();

	//サウンドの作成
	void CreateAudio(IDirectSound8* pDS8, const std::string fileName);

	void Play(bool loop);
	void Stop();
	void SetVolume(const LONG& volume);
	void SetPan(const LONG& pan);
	 
private:

	IDirectSoundBuffer8* pDSBuffer;

};
class Lib_AudioManager
{
public:
	Lib_AudioManager(const HWND &hWbd);
	~Lib_AudioManager();

	//重複しているかどうか
	Lib_AudioBase* SearchAudionKey(const char* key);

	//サウンド作成
	Lib_AudioBase* CreateAudio(std::string fileName);

	//登録サウンド
	Lib_AudioBase* Resiger(const char* filePath, const char* key);
private:

	//サウンドの全ての削除
	void Clear();



	void Update();

	// サウンドデバイス作成
	IDirectSound8* pDS8;

	map<string, Lib_AudioBase*> AudioTable;
};


