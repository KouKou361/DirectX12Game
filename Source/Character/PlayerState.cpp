#include "PlayerState.h"
#include "Player.h"
PlayerState::PlayerState(Player* pl)
{
	owner = pl;
	sceneGame = pl->sceneGame;

}
