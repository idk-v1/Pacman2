#include "SignalGhost.h"

SignalGhost::SignalGhost()
{
	color = sf::Color(0xFF8888FF);
}

void SignalGhost::special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
	hasLineOfSight = false;
	firstTarget = false;

	for (auto& pacman : pacmen)
	{
		if (lineOfSight(map, pacman, ghosts, mapSize) && !pacman.getPower() && isFree)
		{
			hasLineOfSight = true;
			for (auto& ghost : ghosts)
				ghost->setTimer(5 * 60);
		}

		if (!pacman.getPower())
		{
			hasTarget = true;
			target = pacman.getPos();
		}
	}
}

void SignalGhost::assignPacman(int index)
{
	targetPacman = index;
}
