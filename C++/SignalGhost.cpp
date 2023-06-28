#include "SignalGhost.h"

SignalGhost::SignalGhost()
{
	color = sf::Color(0xFF8888FF);
}

void SignalGhost::special(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize, int& lives)
{
	firstTarget = false;

	if (lineOfSight(map, pacman, ghosts, mapSize, lives) && !pacman.getPower() && isFree)
	{
		hasLineOfSight = true;
		for (auto& ghost : ghosts)
			ghost->setTimer(5 * 60);
	}
	else
		hasLineOfSight = false;

	if (!pacman.getPower())
	{
		hasTarget = true;
		target = pacman.getPos();
	}
}