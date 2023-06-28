#pragma once
#include "Ghost.h"
class SignalGhost : public Ghost
{
public:
	SignalGhost();

	void special(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize, int& lives);
};