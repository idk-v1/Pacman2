#include "Pacman.h"

Pacman::Pacman()
{
}

void Pacman::start(sf::Vector2i pos)
{
	this->pos = pos;
	prog.x = 50;
}

void Pacman::move(int dir, std::vector<std::vector<char>>& map, int dots, sf::Vector2i mapSize)
{

	for (int i = 0; i < speed * (1 + (power || !dots)); i++)
	{
		if (!prog.x && !prog.y)
		{
			switch (dir)
			{
			case 0:
				if (canMove(pos.x, pos.y - 1, map, dots, mapSize))
					this->dir = dir;
				break;
			case 1:
				if (canMove(pos.x + 1, pos.y, map, dots, mapSize))
					this->dir = dir;
				break;
			case 2:
				if (canMove(pos.x, pos.y + 1, map, dots, mapSize))
					this->dir = dir;
				break;
			case 3:
				if (canMove(pos.x - 1, pos.y, map, dots, mapSize))
					this->dir = dir;
			}
		}

		switch (this->dir)
		{
		case 0:
			if (canMove(pos.x, pos.y - 1, map, dots, mapSize))
				prog.y--;
			break;
		case 1:
			if (canMove(pos.x + 1, pos.y, map, dots, mapSize))
				prog.x++;
			break;
		case 2:
			if (canMove(pos.x, pos.y + 1, map, dots, mapSize))
				prog.y++;
			break;
		case 3:
			if (canMove(pos.x - 1, pos.y, map, dots, mapSize))
				prog.x--;
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
		{
			if (!dots)
				win = true;
			pos.x = mapSize.x - 1;
		}
		if (pos.x == mapSize.x)
		{
			if (!dots)
				win = true;
			pos.x = 0;
		}
		if (pos.y == -1)
		{
			if (!dots)
				win = true;
			pos.y = mapSize.y - 1;
		}
		if (pos.y == mapSize.y)
		{
			if (!dots)
				win = true;
			pos.y = 0;
		}
	}

	if (damageTimer)
	{
		damageTimer--;
		if (!damageTimer)
			power = false;
	}
}

sf::Vector2i Pacman::getPos()
{
	return pos;
}

sf::Vector2i Pacman::getProg()
{
	return prog;
}

int Pacman::getDir()
{
	return dir;
}

void Pacman::damage()
{
	if (!damageTimer)
		damageTimer = 3 * 60;
}

int Pacman::getDamageTimer()
{
	return damageTimer;
}

void Pacman::setPower()
{
	damageTimer = 5 * 60;
	power = true;
}

bool Pacman::getPower()
{
	return power;
}

bool Pacman::hasWon()
{
	return win;
}

void Pacman::setDir(int dir)
{
	if (dir == 1 || dir == 3)
		this->dir = dir;
}

bool Pacman::canMove(int x, int y, std::vector<std::vector<char>>& map, int dots, sf::Vector2i mapSize)
{
	if (x >= 0 && y >= 0 && y < map.size() && x < map[0].size())
		return (map[y][x] == 0 || map[y][x] == 7 || map[y][x] == 8 || map[y][x] == 9);
	if (x == -1 || x == mapSize.x || y == -1 || y == mapSize.y)
		return true;
	return false;
}