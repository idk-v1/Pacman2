#pragma once
#include "Game.h"
class MultiplayerGame : public Game
{
public:
	MultiplayerGame();

	MultiplayerGame(int num, int HUD, int lives, int score, sf::Font& font, int* hScore, int level, int numPlayers);
};

