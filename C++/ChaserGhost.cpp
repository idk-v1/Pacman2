#include "ChaserGhost.h"

ChaserGhost::ChaserGhost()
{
	color = sf::Color(0x8888FFFF);
}

void ChaserGhost::special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
	hasLineOfSight = false;

	for (int i = 0; i < pacmen.size(); i++)
	{
		if (lineOfSight(map, pacmen[i], ghosts, mapSize) && !pacmen[i].getPower() && isFree)
		{
			setTimer(2 * 60);
			targetPacman = i;
			hasLineOfSight = true;
		}

		if (timer)
		{
			firstTarget = false;
			hasTarget = true;
			if (targetPacman != -1)
			target = pacmen[targetPacman].getPos();
			speed = defSpeed + 2 + hasLineOfSight * 3;
		}
		else
		{
			speed = defSpeed;
			targetPacman = -1;
		}
	}
}

void ChaserGhost::assignPacman(int index)
{
}
