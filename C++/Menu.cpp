#include "Menu.h"

Menu::Menu()
{
}

void Menu::draw(sf::RenderWindow& window)
{
	for (int i = 0; i < bElements.size(); i++)
	{
		if (i == selected)
		{
			bElements[i].setTextureRect(sf::IntRect(
				0,
				(animation + 1) * (bTextures[i].getSize().y / (bFrames[i] + 1)),
				bTextures[i].getSize().x,
				bTextures[i].getSize().y / (bFrames[i] + 1)));
		}

		window.draw(bElements[i]);
	}

	for (int i = 0; i < tElements.size(); i++)
	{
		window.draw(tElements[i]);
	}
}

void Menu::update(sf::RenderWindow& window, sf::Vector2i mousePos, int& menuState)
{
	scale = std::min(window.getSize().x / mapSize.x, window.getSize().y / (mapSize.y + 2));
	xoff = (window.getSize().x - mapSize.x * scale) / 2;
	yoff = (window.getSize().y - mapSize.y * scale) / 2;

	if (scale != lastScale)
	{
		for (int i = 0; i < bElements.size(); i++)
		{
			bElements[i].setPosition(
				xoff + scale * (bCoords[i].getPosition().x - bCoords[i].getSize().x / 2.f),
				yoff + scale * (bCoords[i].getPosition().y - bCoords[i].getSize().y / 2.f));
			bElements[i].setSize(sf::Vector2f(
				scale * bCoords[i].getSize().x,
				scale * bCoords[i].getSize().y));
			bElements[i].setTextureRect(sf::IntRect(
				0,
				0,
				bTextures[i].getSize().x,
				bTextures[i].getSize().y / (bFrames[i] + 1)));
		}

		for (int i = 0; i < tElements.size(); i++)
		{
			tElements[i].setCharacterSize(scale * tFontSize[i]);
			switch (tAlign[i])
			{
			case 'C':
				tElements[i].setPosition(
					xoff + scale * tCoords[i].x - tElements[i].getGlobalBounds().width / 2.f,
					yoff + scale * tCoords[i].y - tElements[i].getGlobalBounds().height / 2.f);
				break;
			case 'L':
				tElements[i].setPosition(
					xoff + scale * tCoords[i].x - tElements[i].getGlobalBounds().width,
					yoff + scale * tCoords[i].y - tElements[i].getGlobalBounds().height / 2.f);
				break;
			case 'R':
				tElements[i].setPosition(
					xoff + scale * tCoords[i].x,
					yoff + scale * tCoords[i].y - tElements[i].getGlobalBounds().height / 2.f);
			}
		}

		lastScale = scale;
	}

	lag += clock.restart().asMilliseconds();

	selected = -1;
	for (int i = 0; i < bElements.size(); i++)
		if (bElements[i].getGlobalBounds().contains(sf::Vector2f(mousePos)))
			selected = i;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && selected != -1)
		lastClick = true;
	else
	{
		if (selected != -1 && lastClick)
			menuState = bPages[selected];
		lastClick = false;
	}

	if (selected != lastSelected)
	{
		animation = 0;
		if (lastSelected != -1)
			bElements[lastSelected].setTextureRect(sf::IntRect(
				0,
				0,
				bTextures[lastSelected].getSize().x,
				bTextures[lastSelected].getSize().y / (bFrames[lastSelected] + 1)));
	}
	if (lastSelected != -1)
		while (lag >= 1000 / 20)
		{
			lag -= 1000 / 20;
			animation = (animation + 1) % bFrames[lastSelected];
		}

	lastSelected = selected;
}

void Menu::setElementPage(int index, int dest)
{
	bPages[index] = dest;
}

void Menu::setElementText(int index, std::string str)
{
	tElements[index].setString(str);
	switch (tAlign[index])
	{
	case 'C':
		tElements[index].setPosition(
			xoff + scale * tCoords[index].x - tElements[index].getGlobalBounds().width / 2.f,
			yoff + scale * tCoords[index].y - tElements[index].getGlobalBounds().height / 2.f);
		break;
	case 'L':
		tElements[index].setPosition(
			xoff + scale * tCoords[index].x - tElements[index].getGlobalBounds().width,
			yoff + scale * tCoords[index].y - tElements[index].getGlobalBounds().height / 2.f);
		break;
	case 'R':
		tElements[index].setPosition(
			xoff + scale * tCoords[index].x,
			yoff + scale * tCoords[index].y - tElements[index].getGlobalBounds().height / 2.f);
	}
}


void Menu::load(std::string path, sf::Font& font)
{
	int page, frame;
	char type, align;
	float fontSize;
	sf::RectangleShape rect;
	sf::Vector2f vec;
	sf::FloatRect coord;
	sf::Texture tex;
	sf::Text text;
	std::string img, data, str;

	std::ifstream file(path + "/layout.txt");
	if (file.is_open())
	{
		text.setFont(font);
		text.setFillColor(sf::Color(0xFFFFFFFF));
		bTextures.reserve(std::count(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>(), '\n') + 1);
		file.clear();
		file.seekg(0);
		while (std::getline(file, data))
		{
			std::stringstream dataStream(data);
			dataStream >> type;

			switch (type)
			{
			case 'B':
				dataStream >> coord.left >> coord.top >> coord.width >> coord.height >> frame >> page >> img;
				img = path + '/' + img;

				bPages.push_back(page);
				bFrames.push_back(frame);
				tex.loadFromFile(img);
				bTextures.push_back(tex);
				rect.setTexture(&bTextures.back());
				bElements.push_back(rect);
				bCoords.push_back(coord);
				break;
			case 'T':
				dataStream >> vec.x >> vec.y >> fontSize >> align >> str;
				for (int i = 0; i < str.size(); i++)
					if (str[str.size() - 1 - i] == '_')
						str[str.size() - 1 - i] = ' ';

				tCoords.push_back(vec);
				tFontSize.push_back(fontSize);
				tAlign.push_back(align);
				text.setString(str);
				tElements.push_back(text);
				break;
			}
		}
		file.close();
	}
}

void Menu::addTextElement(float x, float y, float fontSize, char align, std::string str, sf::Font& font)
{
	sf::Text text;
	text.setFont(font);
	text.setString(str);
	tCoords.push_back({ x, y });
	tFontSize.push_back(fontSize);
	tAlign.push_back(align);
	tElements.push_back(text);

	text.setCharacterSize(scale * fontSize);
	switch (align)
	{
	case 'C':
		text.setPosition(
			xoff + scale * x - text.getGlobalBounds().width / 2.f,
			yoff + scale * y - text.getGlobalBounds().height / 2.f);
		break;
	case 'L':
		text.setPosition(
			xoff + scale * x - text.getGlobalBounds().width,
			yoff + scale * y - text.getGlobalBounds().height / 2.f);
		break;
	case 'R':
		text.setPosition(
			xoff + scale * x,
			yoff + scale * y - text.getGlobalBounds().height / 2.f);
	}
}
