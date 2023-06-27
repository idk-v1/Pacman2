#include "Game.h"


Game::Game(int num, int HUD, int lives, int score, sf::Font& font, int *hScore, int level)
{
	texture.loadFromFile("res/textures/tilemap.png");
	loadMap(num);

	for (int i = 0; i < 3; i++)
		ghosts.push_back(new ChaserGhost);
	ghosts.push_back(new SignalGhost());

	this->HUD = HUD;
	this->lives = lives;
	this->score = score;
	this->hScore = hScore;
	this->level = level;

	scoreTxt.setFont(font);
	hScoreTxt.setFont(font);
}


void Game::start()
{
	timer.restart();
	pacman.start(pacmanSpawn);

	if (failed)
		for (int i = 0; i < 4 - ghosts.size(); i++)
			ghosts.push_back(new ChaserGhost);

	for (auto& ghost : ghosts)
		ghost->start(ghostSpawn);
}


void Game::update()
{
	int seen = 0, randX, randY;
	bool los = false, onPortal = false;

	lag += timer.restart().asMilliseconds();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		inputTimer = 60 * inputBuffer;
		inputDir = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		inputTimer = 60 * inputBuffer;
		inputDir = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		inputTimer = 60 * inputBuffer;
		inputDir = 2;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		inputTimer = 60 * inputBuffer;
		inputDir = 3;
	}

	// Changes light detail keys 1 - 9
	for (int i = 1; i <= 9; i++)
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(26 + i)))
			if (lightScale != i)
				setLightScale(i);

	while (lag >= 1000 / 60)
	{
		lag -= 1000 / 60;

		// Wait 3 seconds
		if (startTimer)
		{
			startTimer--;
			pacman.setDir(inputDir);
		}
		else
		{
			// Update ghosts
			for (auto& ghost : ghosts)
			{
				ghost->updateTarget(map, pacman, mapSize);
				ghost->move(map, pacman, ghosts, mapSize, lives);
			}

			if (inputTimer)
				inputTimer--;
			else
				inputDir = -1;

			// Update pacman
			pacman.move(inputDir, map, dots, mapSize);

			// Remove a dot if pacman is on one
			if (map[pacman.getPos().y][pacman.getPos().x] == 7 || map[pacman.getPos().y][pacman.getPos().x] == 8)
			{
				// Power up pacman if dot is powerup
				if (map[pacman.getPos().y][pacman.getPos().x] == 8)
				{
					pacman.setPower();
					for (auto& ghost : ghosts)
						ghost->loseTarget();
					score += 40;
				}
				score += 10;
				dots--;
				dotProg += 100;
				mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 0].texCoords = { 0,  0 };
				mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 1].texCoords = { 32,  0 };
				mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 2].texCoords = { 32, 32 };
				mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 3].texCoords = { 0, 32 };
				map[pacman.getPos().y][pacman.getPos().x] = 0;
			}
		}

		// Replaces dots in final map
		if (failed)
		{
			if (dots <= maxDots / 3)
			{
				randX = rand() % mapSize.x;
				randY = rand() % mapSize.y;

				if (map[randY][randX] == 0)
				{
					dots++;
					dotProg -= 100;
					mapVert[(randX + randY * mapSize.x) * 4 + 0].texCoords = { 224	   ,  0 };
					mapVert[(randX + randY * mapSize.x) * 4 + 1].texCoords = { 224 + 32,  0 };
					mapVert[(randX + randY * mapSize.x) * 4 + 2].texCoords = { 224 + 32, 32 };
					mapVert[(randX + randY * mapSize.x) * 4 + 3].texCoords = { 224	   , 32 };
					map[randY][randX] = 7;
				}
			}
		}

		// Dot bar animation
		if (dotProg > 0)
			dotProg -= 7 * (1 + pacman.getPower());
		if (dotProg < 0)
			dotProg++;

		// End game
		if (!lives || pacman.hasWon())
		{
			if (overTimer)
				overTimer--;
			for (auto& ghost : ghosts)
				delete ghost;
			ghosts.clear();
			startTimer = 1;
			if (lives)
				score += bonusScore / 60 * 500;
			bonusScore = 0;
		}

		// Resets light
		for (int y = 0; y < mapSize.y * lightScale; y++)
			for (int x = 0; x < mapSize.x * lightScale; x++)
			{
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].color = sf::Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].color = sf::Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].color = sf::Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].color = sf::Color(0, 0, 0, 255);
				light[y][x] = 0;
			}

		// Check if ghosts can see pacman
		for (auto& ghost : ghosts)
		{
			if (ghost->hasLOS())
				los = true;
			if (ghost->getTimer() > seen)
				seen = ghost->getTimer();
		}

		if (bonusScore && los)
			bonusScore--;

		// Warning animation
		if (seen && !pacman.getPower())
		{
			if (seenTimer < 127 && !los)
				seenTimer++;
			else if (seenTimer < 255 && los)
				seenTimer++;
		}
		else
			if (seenTimer > 0)
				seenTimer--;

		if (score > *hScore)
			*hScore = score;

		scoreTxt.setString("SCORE " + std::to_string(score) + "+" + std::to_string(bonusScore / 60 * 500));
		hScoreTxt.setString("HIGH " + std::to_string(*hScore) + " LVL " + std::to_string(level + 1));

		if (!dots && portalTimer < 60)
			portalTimer += 2;


		// Light pacman
		recLight(map, light, lightVert, (pacman.getPos().x + pacman.getProg().x / 100.f + 0.5f) * lightScale, (pacman.getPos().y + pacman.getProg().y / 100.f + 0.5f) * lightScale, maxLight * overTimer / (5.f * 60), false);

		// Light portals
		if (!pacman.hasWon() && lives)
			for (auto portal : portals)
				recLight(map, light, lightVert, portal.x * lightScale, portal.y * lightScale, maxLight / (2 - (portalTimer / 60.f)), false);
	}
}


void Game::draw(sf::RenderWindow& window)
{
	int value;

	// Reset scale
	scale = std::min(window.getSize().x / mapSize.x, window.getSize().y / (mapSize.y + HUD));
	xoff = (window.getSize().x - mapSize.x * scale) / 2;
	yoff = (window.getSize().y - (mapSize.y - HUD) * scale) / 2;
	yHUDOff = (window.getSize().y - (mapSize.y + HUD) * scale) / 2;

	// Change vertex array positions if window is resized
	if (oldScale != scale)
	{
		for (int y = 0; y < mapSize.y; y++)
			for (int x = 0; x < mapSize.x; x++)
			{
				mapVert[(x + y * mapSize.x) * 4 + 0].position = sf::Vector2f((xoff + (x + 0) * scale), (yoff + (y + 0) * scale));
				mapVert[(x + y * mapSize.x) * 4 + 1].position = sf::Vector2f((xoff + (x + 1) * scale), (yoff + (y + 0) * scale));
				mapVert[(x + y * mapSize.x) * 4 + 2].position = sf::Vector2f((xoff + (x + 1) * scale), (yoff + (y + 1) * scale));
				mapVert[(x + y * mapSize.x) * 4 + 3].position = sf::Vector2f((xoff + (x + 0) * scale), (yoff + (y + 1) * scale));
			}

		for (int y = 0; y < mapSize.y * lightScale; y++)
			for (int x = 0; x < mapSize.x * lightScale; x++)
			{
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].position = sf::Vector2f((xoff + (x + 0) * scale / lightScale), (yoff + (y + 0) * scale / lightScale));
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].position = sf::Vector2f((xoff + (x + 1) * scale / lightScale), (yoff + (y + 0) * scale / lightScale));
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].position = sf::Vector2f((xoff + (x + 1) * scale / lightScale), (yoff + (y + 1) * scale / lightScale));
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].position = sf::Vector2f((xoff + (x + 0) * scale / lightScale), (yoff + (y + 1) * scale / lightScale));
			}
	}

	// Draws map
	window.draw(mapVert, &texture);

	// Draws pacman if alive
	if (lives && !pacman.hasWon())
	{
		rect.setSize(sf::Vector2f(scale, scale));

		// Blinks when invincible
		if (pacman.getDamageTimer() / 6 % 2)
			rect.setFillColor(sf::Color(0xAAAA00FF));
		else
			rect.setFillColor(sf::Color(0xFFFF00FF));

		// Draws a duplicate pacman in the opposite portal
		if (pacman.getPos().x == 0)
		{
			rect.setPosition(
				xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
				yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
			window.draw(rect);
			if (dots)
			{
				rect.setPosition(
					xoff + (mapSize.x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(rect);
			}
		}
		else if (pacman.getPos().x == mapSize.x - 1)
		{
			if (dots)
			{
				rect.setPosition(
					xoff + (-1 + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(rect);
			}
			rect.setPosition(
				xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale, 
				yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		else if (pacman.getPos().y == 0)
		{
			rect.setPosition(
				xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
				yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
			window.draw(rect);
			if (dots)
			{
				rect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (mapSize.y + pacman.getProg().y / 100.f) * scale);
				window.draw(rect);
			}
		}
		else if (pacman.getPos().y == mapSize.y - 1)
		{
			if (dots)
			{
				rect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (-1 + pacman.getProg().y / 100.f) * scale);
				window.draw(rect);
			}
			rect.setPosition(
				xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
				yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		// Draws pacman normally
		else
		{
			rect.setPosition(
				xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale, 
				yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
			window.draw(rect);
		}
	}

	// Draws ghosts
	for (auto& ghost : ghosts)
	{
		rect.setFillColor(ghost->getColor());

		// Draws a duplicate ghost in the opposite portal
		if (ghost->getPos().x == 0)
		{
			rect.setPosition(xoff + (0 + ghost->getProg().x / 100.f) * scale, yoff + (ghost->getPos().y + ghost->getProg().y / 100.f) * scale);
			window.draw(rect);
			rect.setPosition(xoff + (mapSize.x + ghost->getProg().x / 100.f) * scale, yoff + (ghost->getPos().y + ghost->getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		else if (ghost->getPos().x == mapSize.x - 1)
		{
			rect.setPosition(xoff + (-1 + ghost->getProg().x / 100.f) * scale, yoff + (ghost->getPos().y + ghost->getProg().y / 100.f) * scale);
			window.draw(rect);
			rect.setPosition(xoff + (mapSize.x - 1 + ghost->getProg().x / 100.f) * scale, yoff + (ghost->getPos().y + ghost->getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		// Draws ghost normally
		else
		{
			rect.setPosition(xoff + (ghost->getPos().x + ghost->getProg().x / 100.f) * scale, yoff + (ghost->getPos().y + ghost->getProg().y / 100.f) * scale);
			window.draw(rect);
		}
	}

	// Draws lighting
	window.draw(lightVert);

	// Covers pacman going through portal out of bounds
	rect.setSize(sf::Vector2f(xoff, window.getSize().y));
	rect.setPosition(0, 0);
	rect.setFillColor(sf::Color());
	window.draw(rect);
	rect.setPosition(window.getSize().x - xoff, 0);
	window.draw(rect);

	// Draws remaining lives
	rect.setFillColor(sf::Color(0xFFFF00FF));
	rect.setSize(sf::Vector2f(scale / 3.f * 2, scale / 3.f * 2));
	for (int i = 0; i < lives - 1; i++)
	{
		rect.setPosition(xoff + (i * 2 + 1.f / 6 + 1) * scale, yHUDOff + scale / 6.f);
		window.draw(rect);
	}

	// Draws dot progress bar
	rect.setSize(sf::Vector2f((mapSize.x * scale - scale / 3.f * 2) * ((float)(dots + dotProg / 100.f) / maxDots), scale));
	rect.setPosition(xoff + scale / 3.f, yHUDOff + scale);
	window.draw(rect);

	// Draws coloring for warning
	rect.setSize(sf::Vector2f(mapSize.x * scale, mapSize.y * scale));
	rect.setPosition(xoff, yoff);
	rect.setFillColor(sf::Color(127, 0, 0, seenTimer / 2));
	window.draw(rect);

	// Draws score
	scoreTxt.setCharacterSize(scale / 2.f);
	scoreTxt.setPosition(xoff + mapSize.x * scale / 2 - scoreTxt.getGlobalBounds().width / 2, yHUDOff + scoreTxt.getGlobalBounds().height / 4);
	window.draw(scoreTxt);
	hScoreTxt.setCharacterSize(scale / 2.f);
	hScoreTxt.setPosition(xoff + mapSize.x * scale - hScoreTxt.getGlobalBounds().width - scale, yHUDOff + hScoreTxt.getGlobalBounds().height / 4);
	window.draw(hScoreTxt);

	oldScale = scale;
}

void Game::drawDebug(sf::RenderWindow& window)
{
	rect.setSize(sf::Vector2f(scale, scale));

	// Draws pacman hitbox
	rect.setFillColor(sf::Color(0xFFFF0088));
	rect.setPosition(xoff + pacman.getPos().x * scale, yoff + pacman.getPos().y * scale);
	window.draw(rect);

	for (auto& ghost : ghosts)
	{
		// Draws ghost hitboxes
		rect.setFillColor(ghost->getColor());
		rect.setPosition(xoff + ghost->getPos().x * scale, yoff + ghost->getPos().y * scale);
		window.draw(rect);

		// Draws ghost targets
		rect.setOutlineColor(rect.getFillColor());
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineThickness(3);
		rect.setPosition(xoff + ghost->getTarget().x * scale, yoff + ghost->getTarget().y * scale);
		window.draw(rect);
		rect.setOutlineThickness(0);
	}
}

void Game::setLightScale(int newScale)
{
	lightScale = newScale;
	lightVert.resize(mapSize.x * lightScale * mapSize.y * lightScale * 4);
	for (int y = 0; y < mapSize.y * lightScale; y++)
		for (int x = 0; x < mapSize.x * lightScale; x++)
		{
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].position = sf::Vector2f(xoff + (x + 0) * scale / lightScale, yoff + (y + 0) * scale / lightScale);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].position = sf::Vector2f(xoff + (x + 1) * scale / lightScale, yoff + (y + 0) * scale / lightScale);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].position = sf::Vector2f(xoff + (x + 1) * scale / lightScale, yoff + (y + 1) * scale / lightScale);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].position = sf::Vector2f(xoff + (x + 0) * scale / lightScale, yoff + (y + 1) * scale / lightScale);

			lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].color = sf::Color(0, 0, 0, 255);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].color = sf::Color(0, 0, 0, 255);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].color = sf::Color(0, 0, 0, 255);
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].color = sf::Color(0, 0, 0, 255);
		}

	light.resize(mapSize.y * lightScale);
	for (auto& y : light)
	{
		y.resize(mapSize.x * lightScale);
		for (auto& x : y)
			x = 0;
	}

	maxLight = lightScale * lightRange;

	recLight(map, light, lightVert, (pacman.getPos().x + pacman.getProg().x / 100.f + 0.5f) * lightScale, (pacman.getPos().y + pacman.getProg().y / 100.f + 0.5f) * lightScale, maxLight, false);

	for (auto& portal : portals)
		recLight(map, light, lightVert, portal.x * lightScale, portal.y * lightScale, maxLight / 2, false);
}

bool Game::isOver()
{
	return !overTimer;
}

int Game::getLives()
{
	return lives;
}

int Game::getScore()
{
	return score;
}

void Game::loadMap(int num)
{
	int value;
	sf::Vector2f coord;
	sf::Image img;
	sf::Color color;

	map.resize(mapSize.y);
	for (auto& y : map)
		y.resize(mapSize.x);
	light.resize(mapSize.y * lightScale);
	for (auto& y : light)
	{
		y.resize(mapSize.x * lightScale);
		for (auto& x : y)
			x = 0;
	}

	mapVert.setPrimitiveType(sf::Quads);
	mapVert.resize(mapSize.x * mapSize.y * 4);
	lightVert.setPrimitiveType(sf::Quads);
	lightVert.resize(mapSize.x * lightScale * mapSize.y * lightScale * 4);

	if (img.loadFromFile("res/maps/map" + std::to_string(num) + ".png"))
	{
		for (int y = 0; y < mapSize.y; y++)
			for (int x = 0; x < mapSize.x / 2; x++)
			{
				switch (img.getPixel(x, y).toInteger())
				{
				case 0xFFFFFFFF:
					map[y][x] = 0;
					break;
				case 0x000000FF:
					map[y][x] = 1;
					break;
				case 0x0000FFFF:
					map[y][x] = 2;
					break;
				case 0xFF0000FF:
					map[y][x] = 3;
					break;
				case 0xFFFF00FF:
					map[y][x] = 7;
					break;
				case 0xFF00FFFF:
					map[y][x] = 8;
					break;
				case 0x00FFFFFF:
					map[y][x] = 9;
					break;
				}
			}
	}
	else
	{
		failed = true;

		for (int y = 0; y < mapSize.y; y++)
			for (int x = 0; x < mapSize.x; x++)
			{
				if (x == 0 || y == 0 || x == mapSize.x - 1 || y == mapSize.y - 1)
					map[y][x] = 1;
				else
				{
					if ((x + 4) % 5 && (y + 6) % 7)
					{
						if ((x + 4) % 5 < 4 && (x + 4) % 5 > 1 && (y + 6) % 7 < 6 && (y + 6) % 7 > 1)
							map[y][x] = 2;
						else
							map[y][x] = 1;
					}
					else
						map[y][x] = 7;
				}
			}

		map[15][0] = 0;
		map[mapSize.y / 3][mapSize.x / 2 - 1] = 3;
		map[mapSize.y / 3.f * 2 + 2][mapSize.x / 2 - 1] = 9;
	}

	for (int y = 0; y < mapSize.y; y++)
		for (int x = 0; x < mapSize.x / 2; x++)
		{
			value = map[y][x];

			if (x == 0 && !value)
				portals.push_back({ 0, y + 0.5f });
			if (y == 0 && !value)
				portals.push_back({ x + 0.5f, 0 });

			// Sets map textures
			switch (value)
			{
			case 3:		// Ghost Spawn
				ghostSpawn = { x, y };
				break;
			case 7:		// Dot
				dots += 2;
				break;
			case 8:		// Powerup
				dots += 2;
				break;
			case 9:		// Pacman Spawn
				pacmanSpawn = { x, y };
				break;
			}
			coord = { value * 32.f, 0 };

			mapVert[(x + y * mapSize.x) * 4 + 0].texCoords = { coord.x,		 coord.y };
			mapVert[(x + y * mapSize.x) * 4 + 1].texCoords = { coord.x + 32, coord.y };
			mapVert[(x + y * mapSize.x) * 4 + 2].texCoords = { coord.x + 32, coord.y + 32 };
			mapVert[(x + y * mapSize.x) * 4 + 3].texCoords = { coord.x,		 coord.y + 32 };

			mapVert[(mapSize.x - 1 - x + y * mapSize.x) * 4 + 0].texCoords = { coord.x,		 coord.y };
			mapVert[(mapSize.x - 1 - x + y * mapSize.x) * 4 + 1].texCoords = { coord.x + 32, coord.y };
			mapVert[(mapSize.x - 1 - x + y * mapSize.x) * 4 + 2].texCoords = { coord.x + 32, coord.y + 32 };
			mapVert[(mapSize.x - 1 - x + y * mapSize.x) * 4 + 3].texCoords = { coord.x,		 coord.y + 32 };

			map[y][mapSize.x - 1 - x] = map[y][x];
		}

	maxDots = dots;
}

void Game::recLight(std::vector<std::vector<char>>& map, std::vector<std::vector<char>>& light, sf::VertexArray& vertLight, int x, int y, int value, bool onSolid)
{
	// Keeps starting light in bounds
	if (x < 0)
		x = 0;
	if (x >= mapSize.x * lightScale)
		x = mapSize.x * lightScale - 1;
	if (y < 0)
		y = 0;
	if (y >= mapSize.y * lightScale)
		y = mapSize.y * lightScale - 1;

	// Change if new light is greater
	if (light[y][x] < value)
	{
		// If tile is non-solid and last light was from a solid
		if (map[y / lightScale][x / lightScale] != 1 && onSolid);
		// Opposite of that. It's weird
		else
		{
			light[y][x] = value;
			vertLight[(x + y * mapSize.x * lightScale) * 4 + 0].color.a = 255 - 255 * (value / (float)maxLight);
			vertLight[(x + y * mapSize.x * lightScale) * 4 + 1].color.a = 255 - 255 * (value / (float)maxLight);
			vertLight[(x + y * mapSize.x * lightScale) * 4 + 2].color.a = 255 - 255 * (value / (float)maxLight);
			vertLight[(x + y * mapSize.x * lightScale) * 4 + 3].color.a = 255 - 255 * (value / (float)maxLight);

			// If tile is solid
			if (map[y / lightScale][x / lightScale] == 1)
				onSolid = true;

			// Recursion
			if (y - 1 < 0 && !onSolid)
				recLight(map, light, vertLight, x, mapSize.y * lightScale - 1, value - 1, onSolid);
			else
				recLight(map, light, vertLight, x, y - 1, value - 1, onSolid);

			if (x + 1 >= mapSize.x * lightScale && !onSolid)
				recLight(map, light, vertLight, 0, y, value - 1, onSolid);
			else
				recLight(map, light, vertLight, x + 1, y, value - 1, onSolid);

			if (y + 1 >= mapSize.y * lightScale && !onSolid)
				recLight(map, light, vertLight, x, 0, value - 1, onSolid);
			else
				recLight(map, light, vertLight, x, y + 1, value - 1, onSolid);

			if (x - 1 < 0 && !onSolid)
				recLight(map, light, vertLight, mapSize.x * lightScale - 1, y, value - 1, onSolid);
			else
				recLight(map, light, vertLight, x - 1, y, value - 1, onSolid);
		}
	}
}