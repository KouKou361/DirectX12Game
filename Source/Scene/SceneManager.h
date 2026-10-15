#pragma once
class SceneBase;

class SceneManager
{
public:
	SceneManager() {};
	~SceneManager() {};

	//‰Šú‰»
	void Initialize();
	//XV
	void Update();
	//•`‰æ
	void Render();
	//I—¹
	void End();

	void ChangeScene(SceneBase* nextScene);
private:

	SceneBase* currentScene =nullptr;

};