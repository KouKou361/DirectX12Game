#pragma once
#include "Lib.h"
#include <string>
#include <vector>

class SceneGame;
class CharacterBase;

class UI_Base
{
public:
	UI_Base(SceneGame* scene);
	~UI_Base();

	//初期化
	virtual void Init() = 0;
	//更新処理
	virtual void Update() = 0;
	//描画処理
	virtual void Render() = 0;

protected:

	VECTOR2 WorldToScreen(const VECTOR3 Pos);


	UI_Base* parent = nullptr;//親
	std::vector<UI_Base*> children;//子供

	Texture* texture = nullptr;
	float value = 0;//汎用的な数字
	float value2 = 0;//汎用的な数字
	float angle = 0;

	VECTOR2 pos = { 0,0 };
	VECTOR2 size = { 0,0 };
	VECTOR4 cut = { 0,0,0,0 };
	VECTOR4 color = { 1,1,1,1 };

	std::string text = "";//テキスト
	SceneGame* scene = nullptr;
	CharacterBase* chara = nullptr;//キャラクター
	SceneGame* sceneGame = nullptr;
};

class UI_TargetEnemyCursor :public UI_Base
{
public:
	UI_TargetEnemyCursor(SceneGame* scene) :UI_Base(scene) {};
	//初期化
	void Init()override;
	//更新処理
	void Update()override;
	//描画処理
	void Render()override;
};