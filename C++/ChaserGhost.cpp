#include "ChaserGhost.h"

ChaserGhost::ChaserGhost()
{
	color = sf::Color(0x8888FFFF);
}

void ChaserGhost::special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
	hasLineOfSight = false;

	for (auto& pacman : pacmen)
	{
		if (lineOfSight(map, pacman, ghosts, mapSize) && !pacman.getPower() && isFree)
		{
			setTimer(2 * 60);
			hasLineOfSight = true;
		}

		if (timer && !firstTarget)
		{
			hasTarget = true;
			target = pacman.getPos();
			speed = defSpeed + 2 + hasLineOfSight * 3;
		}
		else
			speed = defSpeed;
	}
}

void ChaserGhost::assignPacman(int index)
{
}
