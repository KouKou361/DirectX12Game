#include "UI_Base.h"
#include "../Scene/SceneBase.h"
#include "../Character/Player.h"
//‰Šú‰»
void UI_TargetEnemyCursor::Init()
{
	texture = TK_Lib::Load::sprite("./Data/Texture/LockOn.png");
	size = { 100,100 };
	cut = { 0,0,1400,1400 };
	color = { 1,1,1,1 };

}
//XVˆ—
void UI_TargetEnemyCursor::Update()
{

}
//•`‰æˆ—
void UI_TargetEnemyCursor::Render()
{
	if (sceneGame->player->GetTarget() == nullptr)return;

	pos = WorldToScreen(sceneGame->player->GetTarget()->GetPos());

	pos.x -= 50.0f;
	pos.y -= 50.0f;

	TK_Lib::Render::SpriteRender(texture,pos,size,cut,color,0);
}