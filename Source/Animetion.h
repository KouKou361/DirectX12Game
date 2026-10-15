#pragma once
#include <map>
#include <string>
using namespace std;
class Animation
{
public:
	const string Idle = "Idle";
	const string Run = "Run";
	const string Attack1 = "Attack1";
	const string Attack2 = "Attack2";
	const string Attack3 = "Attack3";
	const string Damage = "Damage";
	const string Die = "Die";
	const string End = "End";
	const string Walk = "Walk";
	const string Rolling = "Rolling";

	const string BossOpen = "Open";
	const string BossOpenGoToRoll = "OpenGoToRoll";
	const string BossCloseStopRoll = "CloseStopRoll";
	const string BossWalk = "Walk";
	const string BossClose_RollLoop = "Close_RollLoop";
	const string BossClose = "Close";
private:
	map<string, int> animeData;

public:
	Animation() { Clear(); };
	~Animation() { Clear(); };


	//モデルアニメーション番号の取得
	const int GetIndex(const string animeKey);
	//モデルアニメーションのクリア
	void Clear();
	//アニメションの登録(これでまとめて一括読み込み)
	void AllAnimetionKey(string* Nameregister);

	
private:
	
	//モデルアニメーション登録
	void Register(const int animeindex, const string animeKey);
};
