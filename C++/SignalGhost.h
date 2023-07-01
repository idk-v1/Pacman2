#pragma once
#include "Ghost.h"
class SignalGhost : public Ghost
{
public:
	SignalGhost();

	void special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize);

	void assignPacman(int index);

private:
	int targetPacman = 0;
};