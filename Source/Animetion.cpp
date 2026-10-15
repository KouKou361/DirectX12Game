#include "Animetion.h"
#include "Lib.h"

//モデルアニメーション登録
void Animation::Register(const int Animeindex, const string AnimeKey)
{
	//登録
	animeData.insert(make_pair(AnimeKey, Animeindex));
}
//モデルアニメーション番号の取得
const int Animation::GetIndex(const string AnimeKey)
{
	return animeData.at(AnimeKey);
}

//モデルアニメーションのクリア
void Animation::Clear()
{
	animeData.clear();
}
void Animation::AllAnimetionKey(string* Nameregister)
{
	//アニメーションの登録
	for (int i = 0; End != *Nameregister; i++)
	{
		Register(i, *Nameregister);
		Nameregister++;
	}
}