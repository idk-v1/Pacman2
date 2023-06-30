// https://shaunlebron.github.io/pacman-mazegen/tetris/many.htm

#include "Game.h"
#include "Menu.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>

#include <vector>
#include <filesystem>
#include <fstream>

int getRandMap(std::vector<int>& maps);

void save(int score);

int main()
{
	srand(time(NULL));

	int HUD = 2, level = 0, score = 0, hScore = 0, hScoreOrigin = 0, lives = 3, scale = std::min(sf::VideoMode::getDesktopMode().width / 28, (sf::VideoMode::getDesktopMode().height - 200) / (31 + HUD));
	sf::RenderWindow window(sf::VideoMode(scale * 28, scale * (31 + HUD)), "Pacman 2");
	window.setFramerateLimit(165);

	Menu mMenu, gameover;

	int menuState = 0, lastMenuState = 0;
	bool changeMenu = false;

	int numMaps = 0;
	for (auto& entry : std::filesystem::directory_iterator("../res/maps"))
		if (entry.is_regular_file())
			numMaps++;

	std::vector<int> maps;
	for (int i = 1; i < numMaps; i++)
		maps.push_back(i);

	std::ifstream fileIn;
	fileIn.open("../res/highscore.txt");
	if (fileIn.is_open())
	{
		fileIn >> hScoreOrigin;
		hScore = hScoreOrigin;
		fileIn.close();
	}

	sf::Font font;
	font.loadFromFile("../res/fonts/emulogic.ttf");

	mMenu.load("../res/mainmenu", font);
	gameover.load("../res/gameover", font);

	Game game;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				if (hScore > hScoreOrigin)
					save(hScore);
			}
		}

		window.setView(sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)));
		window.clear();

		switch (menuState)
		{
		case 0: // Main menu
			mMenu.update(window, sf::Mouse::getPosition(window), menuState);
			mMenu.draw(window);
			break;
		case 1: // Singleplayer
			if (changeMenu)
			{
				gameover.setElementPage(0, 1);
				lives = 3;
				level = 0;
				score = 0;
				game = Game(0, HUD, 3, 0, font, &hScore, 0);
				game.start();
			}
			game.update();
			game.draw(window);
#if !defined(NDEBUG)
			game.drawDebug(window);
#endif

			if (game.isOver())
			{
				score = game.getScore();
				lives = game.getLives();
				if (lives)
				{
					game = Game(getRandMap(maps), HUD, lives, score, font, &hScore, ++level);
					game.start();
				}
				else
				{
					menuState = 2;
					gameover.setElementText(4, std::to_string(level + 1));
					gameover.setElementText(5, std::to_string(hScore));
					gameover.setElementText(6, std::to_string(score));
					if (hScore > hScoreOrigin)
						save(hScore);
				}
			}
			break;
		case 2: // gameover
			gameover.update(window, sf::Mouse::getPosition(window), menuState);
			gameover.draw(window);
			break;
		}
		changeMenu = menuState != lastMenuState;
		lastMenuState = menuState;

		window.display();
	}
}

int getRandMap(std::vector<int>& maps)
{
	int value = rand() % maps.size(), sel = maps[value];
	maps.erase(maps.begin() + value);
	return sel;
}

void save(int score)
{
	std::ofstream file("../res/highscore.txt");
	file << score;
	file.close();
}
