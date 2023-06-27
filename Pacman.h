#include <vector>

#include <SFML/System/Vector2.hpp>

#pragma once
class Pacman
{
public:
	Pacman();

	void start(sf::Vector2i pos);

	void move(int dir, std::vector<std::vector<char>>& map, int dots, sf::Vector2i mapSize);

	sf::Vector2i getPos();

	sf::Vector2i getProg();

	int getDir();

	void damage();

	int getDamageTimer();

	void setPower();

	bool getPower();

	bool hasWon();

	void setDir(int dir);

private:

	bool canMove(int x, int y, std::vector<std::vector<char>>& map, int dots, sf::Vector2i mapSize);

	sf::Vector2i pos, prog;
	int speed = 7, dir = 3, damageTimer = 0;
	bool power = false, win = false;
};