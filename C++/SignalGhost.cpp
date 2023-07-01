#include "SignalGhost.h"

SignalGhost::SignalGhost()
{
	color = sf::Color(0xFF8888FF);
}

void SignalGhost::special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
	hasLineOfSight = false;
	firstTarget = false;

	for (int i = 0; i < pacmen.size(); i++)
	{
		if (lineOfSight(map, pacmen[i], ghosts, mapSize) && !pacmen[i].getPower() && isFree)
		{
			hasLineOfSight = true;
			for (auto& ghost : ghosts)
			{
				ghost->setTimer(5 * 60);
				ghost->assignPacman(i);
			}
		}

		if (!pacmen[targetPacman].getPower())
		{
			hasTarget = true;
			target = pacmen[targetPacman].getPos();
			if (!*pacmen[targetPacman].lives)
				targetPacman = (targetPacman + 1) % pacmen.size();
		}
	}
}

void SignalGhost::assignPacman(int index)
{
	if (firstTarget)
		targetPacman = index;
}
