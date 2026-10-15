#pragma once
class SceneManager;

class GameCommon
{
public:
	SceneManager* sceneManager;
	static GameCommon* GetInstance() {
		static GameCommon instance;
		return  &instance;
	};


	//‰Šú‰»
	void Initialize();

	//I—¹ˆ—
	void End();
};