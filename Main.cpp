// https://shaunlebron.github.io/pacman-mazegen/tetris/many.htm

#include "Game.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>

#include <vector>
#include <filesystem>
#include <fstream>

int getRandMap(std::vector<int>& maps);

int main()
{
	srand(time(NULL));

	int HUD = 2, level = 0, score = 0, hScore = 0, hScoreOrigin = 0, lives = 3, scale = std::min(sf::VideoMode::getDesktopMode().width / 28, (sf::VideoMode::getDesktopMode().height - 200) / (31 + HUD));
	sf::RenderWindow window(sf::VideoMode(scale * 28, scale * (31 + HUD)), "Pacman 2");
	window.setFramerateLimit(165);

	int numMaps = 0;
	for (auto& entry : std::filesystem::directory_iterator("res/maps"))
		if (entry.is_regular_file())
			numMaps++;

	std::vector<int> maps;
	for (int i = 1; i < numMaps; i++)
		maps.push_back(i);

	sf::Clock timer;
	sf::Font font;
	std::ofstream fileOut;
	std::ifstream fileIn;
	fileIn.open("res/highscore.txt");
	if (fileIn.is_open())
	{
		fileIn >> hScoreOrigin;
		hScore = hScoreOrigin;
		fileIn.close();
	}

	font.loadFromFile("res/fonts/emulogic.ttf");


	Game game(0, HUD, lives, 0, font, &hScore, 0);
#if !defined(NDEBUG)
	game.setLightScale(4);
#else
	game.setLightScale(7);
#endif
	game.start();

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
				{
					fileOut.open("res/highscore.txt");
					fileOut << game.getScore();
					fileOut.close();
				}
			}
		}

		game.update();
		window.setView(sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)));
		window.clear();
		game.draw(window);
#if !defined(NDEBUG)
		game.drawDebug(window);
#endif
		window.display();

		if (game.isOver())
		{
			score = game.getScore();
			lives = game.getLives();
			if (lives)
				game = Game(getRandMap(maps), HUD, lives, score, font, &hScore, ++level);
			else
			{
				score = 0;
				level = 0;
				maps.clear();
				for (int i = 1; i < numMaps; i++)
					maps.push_back(i);
				game = Game(0, HUD, 3, score, font, &hScore, 0);
			}
#if !defined(NDEBUG)
			game.setLightScale(4);
#else
			game.setLightScale(7);
#endif
			game.start();
		}
	}
}

int getRandMap(std::vector<int>& maps)
{
	int value = rand() % maps.size(), sel = maps[value];
	maps.erase(maps.begin() + value);
	return sel;
}
