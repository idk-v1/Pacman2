#pragma once
#include "Ghost.h"
class ChaserGhost : public Ghost
{
public:
	ChaserGhost();

	void special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize);

	void assignPacman(int index);
};