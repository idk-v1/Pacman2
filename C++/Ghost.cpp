#include "Ghost.h"

Ghost::Ghost()
{
	color = sf::Color(0xFF00FFFF);
}

void Ghost::start(sf::Vector2i pos)
{
	this->pos = pos;
	spawn = pos;
	prog.x = 50;
	prog.y = 0;
	dir = rand() % 2 * 2 + 1;
	hasTarget = false;
	timer = 0;
	speed = 3;
	isFree = false;
	hasLineOfSight = false;
}

void Ghost::move(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>ghosts, sf::Vector2i mapSize)
{
	for (int i = 0; i < speed / ((slowTimer != 0) + 1) * (1 + firstTarget * 0.5f); i++)
	{
		if (isFree)
		{
			special(map, pacmen, ghosts, mapSize);

			turn(map, mapSize);

			switch (this->dir)
			{
			case 0:
				if (canMove(pos.x, pos.y - 1, map, mapSize))
					prog.y--;
				break;
			case 1:
				if (canMove(pos.x + 1, pos.y, map, mapSize))
					prog.x++;
				break;
			case 2:
				if (canMove(pos.x, pos.y + 1, map, mapSize))
					prog.y++;
				break;
			case 3:
				if (canMove(pos.x - 1, pos.y, map, mapSize))
					prog.x--;
			}
		}
		else
		{
			if (canMove(pos.x, pos.y, map, mapSize))
				isFree = true;
			else
				prog.y--;
		}

		if (prog.x >= 100)
		{
			pos.x++;
			prog.x -= 100;
		}
		if (prog.x <= -100)
		{
			pos.x--;
			prog.x += 100;
		}
		if (prog.y >= 100)
		{
			pos.y++;
			prog.y -= 100;
		}
		if (prog.y <= -100)
		{
			pos.y--;
			prog.y += 100;
		}

		if (pos.x == -1)
			pos.x = mapSize.x - 1;
		if (pos.x == mapSize.x)
			pos.x = 0;
	}

	if (timer)
		timer--;
	if (slowTimer)
		slowTimer--;
}

sf::Vector2i Ghost::getPos()
{
	return pos;
}

sf::Vector2i Ghost::getProg()
{
	return prog;
}

sf::Vector2i Ghost::getTarget()
{
	return target;
}

int Ghost::getDir()
{
	return dir;
}

void Ghost::turn(std::vector<std::vector<char>>& map, sf::Vector2i mapSize)
{
	int minDist = 999, dir = -1, value;

	if (!prog.x && !prog.y)
	{
		if (this->dir != 2 && canMove(pos.x, pos.y - 1, map, mapSize))
		{
			value = sqrt(pow(pos.x - target.x, 2) + pow(pos.y - 1 - target.y, 2));
			if (value < minDist)
			{
				dir = 0;
				minDist = value;
			}
		}
		if (this->dir != 3 && canMove(pos.x + 1, pos.y, map, mapSize))
		{
			value = sqrt(pow(pos.x + 1 - target.x, 2) + pow(pos.y - target.y, 2));
			if (value < minDist)
			{
				dir = 1;
				minDist = value;
			}
		}
		if (this->dir != 0 && canMove(pos.x, pos.y + 1, map, mapSize))
		{
			value = sqrt(pow(pos.x - target.x, 2) + pow(pos.y + 1 - target.y, 2));
			if (value < minDist)
			{
				dir = 2;
				minDist = value;
			}
		}
		if (this->dir != 1 && canMove(pos.x - 1, pos.y, map, mapSize))
		{
			value = sqrt(pow(pos.x - 1 - target.x, 2) + pow(pos.y - target.y, 2));
			if (value < minDist)
			{
				dir = 3;
				minDist = value;
			}
		}
		if (dir != -1 && this->dir != dir)
			this->dir = dir;
	}
}

bool Ghost::lineOfSight(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
	if (!*pacman.lives)
		return false;
	if (pos.x == pacman.getPos().x && pos.y == pacman.getPos().y)
	{
		if (!pacman.getDamageTimer())
		{
			start(spawn);
			if (!pacman.getDamageTimer())
				pacman.damage();
			for (auto& ghost : ghosts)
			{
				ghost->slow();
				ghost->setTimer(0);
			}
		}
		return true;
	}
	else if (pos.x == pacman.getPos().x)
	{
		if (pos.y < pacman.getPos().y)
		{
			for (int i = pos.y; i <= pacman.getPos().y; i++)
				if (!canMove(pos.x, i, map, mapSize))
					return false;
			return true;
		}
		else if (pos.y > pacman.getPos().y)
		{
			for (int i = pacman.getPos().y; i <= pos.y; i++)
				if (!canMove(pos.x, i, map, mapSize))
					return false;
			return true;
		}
	}
	else if (pos.y == pacman.getPos().y)
	{
		if (pos.x < pacman.getPos().x)
		{
			for (int i = pos.x; i <= pacman.getPos().x; i++)
				if (!canMove(i, pos.y, map, mapSize))
					return false;
			return true;
		}
		else if (pos.x > pacman.getPos().x)
		{
			for (int i = pacman.getPos().x; i <= pos.x; i++)
				if (!canMove(i, pos.y, map, mapSize))
					return false;
			return true;
		}
	}
	return false;
}

void Ghost::special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize)
{
}

void Ghost::updateTarget(std::vector<std::vector<char>>& map, sf::Vector2i mapSize)
{
	if (target == pos)
	{
		firstTarget = false;
		hasTarget = false;
	}
	if (!hasTarget)
	{
		target.x = rand() % mapSize.x;
		target.y = rand() % mapSize.y;

		if (canMove(target.x, target.y, map, mapSize))
			hasTarget = true;
		else
			updateTarget(map, mapSize);
	}
}

void Ghost::setTimer(int value)
{
	if (timer < value)
		timer = value;
}

sf::Color Ghost::getColor()
{
	return color;
}

int Ghost::getTimer()
{
	return timer;
}

void Ghost::slow()
{
	slowTimer = 5 * 60;
}

bool Ghost::hasLOS()
{
	return hasLineOfSight;
}

void Ghost::loseTarget()
{
	hasTarget = false;
}

void Ghost::assignPacman(int index)
{
	targetPacman = index;
}

bool Ghost::canMove(int x, int y, std::vector<std::vector<char>>& map, sf::Vector2i mapSize)
{
	if (x >= 0 && y >= 0 && y < map.size() && x < map[0].size())
		return (map[y][x] == 0 || map[y][x] == 7 || map[y][x] == 8 || map[y][x] == 9);
	if (x == -1 || x == mapSize.x || y == -1 || y == mapSize.y)
		return true;
	return false;
}