#include "Pacman.h"
#include "ChaserGhost.h"
#include "SignalGhost.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>

#pragma once
class Game
{
public:
	Game(int num, int HUD, int lives, int score, sf::Font& font, int *hScore, int level);

	void start();

	void update();

	void draw(sf::RenderWindow& window);

	void drawDebug(sf::RenderWindow& window);

	void setLightScale(int newScale);

	bool isOver();

	int getLives();

	int getScore();

private:
	void loadMap(int num);

	void recLight(std::vector<std::vector<char>>& map, std::vector<std::vector<char>>& light, sf::VertexArray& vertLight, int x, int y, int value, bool onSolid);

	std::vector<std::vector<char>> map, light;

	std::vector<Ghost*> ghosts;

	std::vector<sf::Vector2f> portals;

	sf::Text scoreTxt, hScoreTxt;

	Pacman pacman;

	sf::Clock timer;

	int scale = 0, oldScale = 0, xoff = 0, yoff = 0, yHUDOff = 0,
		lightScale = 3, lightRange = 10, maxLight = lightScale * lightRange,
		lag = 0, overTimer = 5 * 60, startTimer = 3 * 60, seenTimer = 0, portalTimer = 0,
		HUD, lives = 3, score = 0, *hScore = NULL, bonusScore = 10 * 60, level = 0,
		dots = 0, maxDots = 0, dotProg = 0, inputTimer = 0, inputDir = -1;

	float inputBuffer = 0.5f;

	bool failed = false;

	sf::VertexArray mapVert, lightVert;
	sf::RectangleShape rect;
	sf::Texture texture;
	
	sf::Vector2i mapSize = { 28, 31 }, ghostSpawn, pacmanSpawn;
};