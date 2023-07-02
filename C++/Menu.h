#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Mouse.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#pragma once
class Menu
{
public:
	Menu();

	void draw(sf::RenderWindow& window);

	void update(sf::RenderWindow& window, sf::Vector2i mousePos, int& menuState);

	void setElementPage(int index, int dest);

	void setElementText(int index, std::string str);

	void load(std::string path, sf::Font& font);

	void addTextElement(float x, float y, float fontSize, char align, std::string str, sf::Font& font);

	void clear();

	void rescale(sf::RenderWindow& window);

private:
	std::vector<sf::RectangleShape> bElements;
	std::vector<int> bPages, bFrames;
	std::vector<sf::FloatRect> bCoords;
	std::vector<sf::Texture> bTextures;

	std::vector<sf::Text> tElements;
	std::vector<sf::Vector2f> tCoords;
	std::vector<float> tFontSize;
	std::vector<char> tAlign;

	sf::Clock clock;

	sf::Vector2i mapSize = { 28, 31 };

	bool lastClick = false;

	int selected = -1, lastSelected = -1, animation = 0, scale = 0, lastScale = -1, xoff = 0, yoff = 0, lag = 0;
};

