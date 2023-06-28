#include "ChaserGhost.h"

ChaserGhost::ChaserGhost()
{
	color = sf::Color(0x8888FFFF);
}

void ChaserGhost::special(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize, int& lives)
{
	if (lineOfSight(map, pacman, ghosts, mapSize, lives) && !pacman.getPower())
	{
		setTimer(2 * 60);
		hasLineOfSight = true;
	}
	else
		hasLineOfSight = false;

	if (timer && !firstTarget)
	{
		hasTarget = true;
		target = pacman.getPos();
		speed = defSpeed + 2 + hasLineOfSight * 3;
	}
	else
		speed = defSpeed;
}

int ChaserGhost::getType()
{
	return 0;
}