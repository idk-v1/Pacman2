#pragma once
#include "Ghost.h"
class ChaserGhost : public Ghost
{
public:
	ChaserGhost();

	void special(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize, int& lives);

	int getType();
};