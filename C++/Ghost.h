#include <vector>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Pacman.h"

#pragma once
class Ghost
{
public:
	Ghost();

	void start(sf::Vector2i pos);

	void move(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>ghosts, sf::Vector2i mapSize);

	sf::Vector2i getPos();

	sf::Vector2i getProg();

	sf::Vector2i getTarget();

	int getDir();

	void updateTarget(std::vector<std::vector<char>>& map, sf::Vector2i mapSize);

	void setTimer(int value);

	sf::Color getColor();

	int getTimer();

	void slow();

	bool hasLOS();

	void loseTarget();

	virtual void assignPacman(int index);

protected:

	void turn(std::vector<std::vector<char>>& map, sf::Vector2i mapSize);

	bool lineOfSight(std::vector<std::vector<char>>& map, Pacman& pacman, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize);

	virtual void special(std::vector<std::vector<char>>& map, std::vector<Pacman>& pacmen, std::vector<Ghost*>& ghosts, sf::Vector2i mapSize);

	bool canMove(int x, int y, std::vector<std::vector<char>>& map, sf::Vector2i mapSize);

	sf::Color color;

	sf::Vector2i pos, prog, target = { -10, -10 }, spawn;
	int speed = 3, defSpeed = speed, dir = 3, health = 1, timer = 0, slowTimer = 0, targetPacman = -1;
	bool hasTarget = false, hasLineOfSight = false, isFree = false, firstTarget = true;
};