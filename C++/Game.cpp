#include "Game.h"


Game::Game()
{
}

Game::Game(int num, int HUD, std::vector<int*>& lives, std::vector<int>& score, sf::Font& font, int *hScore, int level, int clientNum, int pacmenNum, int ghostNum, bool sharedCTRL)
{
	texture.loadFromFile("../res/textures/Tilemap.png");

	pacTex = new sf::Texture();
	pacTex->loadFromFile("../res/textures/Pacman.png");
	pacRect.setTexture(pacTex);

	loadMap(num);

	for (int i = 0; i < 3; i++)
		ghosts.push_back(new ChaserGhost);

	this->HUD = HUD;
	this->scores = score;
	this->hScore = hScore;
	this->level = level;
	this->lives = lives;
	this->sharedCTRL = sharedCTRL;
	client = clientNum;

	scoreTxt.setFont(font);
	scoreTxt2.setFont(font);
	hScoreTxt.setFont(font);

	timer.restart();

	// Creates a Pacman for each player
	for (int i = 0; i < pacmenNum; i++)
	{
		pacmen.push_back(Pacman());
		inputTimer.push_back(0);
		inputDir.push_back(3);
		pacmen[i].start(pacmanSpawn, lives[i]);
		pacmen[i].setScore(score[i]);
		ghosts.push_back(new SignalGhost());
		ghosts.back()->assignPacman(i);
	}

	for (auto& ghost : ghosts)
		ghost->start(ghostSpawn);

	// Debug mode is laggier. I assume it is keeping track of recursive variables
#if !defined(NDEBUG)
	setLightScale(4);
#else
	setLightScale(7);
#endif

	// Shuffles player outline colors so someone can't complain about not liking a certain color
	std::random_device rd;
	std::default_random_engine rnd(rd());
	std::shuffle(colors.begin(), colors.end(), rnd);
}


void Game::update()
{
	int seen, randX, randY;
	bool los, los2, onPortal, livesRemain, allWon, power;

	lag += timer.restart().asMilliseconds();

	// If game is in shared mode WASD controls player 1 and the arrow keys control player 2
	if (sharedCTRL)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 3;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			inputTimer[client + 1] = 60 * inputBuffer;
			inputDir[client + 1] = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			inputTimer[client + 1] = 60 * inputBuffer;
			inputDir[client + 1] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			inputTimer[client + 1] = 60 * inputBuffer;
			inputDir[client + 1] = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			inputTimer[client + 1] = 60 * inputBuffer;
			inputDir[client + 1] = 3;
		}
	}
	// Non shared mode both WASD and arrow keys work the same
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			inputTimer[client] = 60 * inputBuffer;
			inputDir[client] = 3;
		}
	}

	// Changes light detail keys 1 - 9
	for (int i = 1; i <= 9; i++)
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(26 + i)))
			if (lightScale != i)
				setLightScale(i);

	while (lag >= 1000 / 60)
	{
		lag -= 1000 / 60;

		seen = 0;
		los = false, los2 = false;
		onPortal = false;
		power = true;

		// Wait 3 seconds for game to start
		if (startTimer)
		{
			startTimer--;

			// changes direction without moving at start
			pacmen[client].setDir(inputDir[client]);
			if (sharedCTRL)
				pacmen[client + 1].setDir(inputDir[client + 1]);
		}
		else
		{
			// Update ghosts
			for (auto& ghost : ghosts)
			{
				ghost->updateTarget(map, mapSize);
				ghost->move(map, pacmen, ghosts, mapSize);
			}

			// input buffers for pacman
			for (int i = 0; i < pacmen.size(); i++)
			{
				if (inputTimer[i])
					inputTimer[i]--;
				else
					inputDir[i] = -1;
			}

			// Update pacman
			int count = 0;
			for (auto& pacman : pacmen)
			{
				// only move pacman if that pacman has not won or died
				if (!pacman.hasWon() && *pacman.lives)
				{
					pacman.move(inputDir[count], map, dots, mapSize);

					// Remove a dot if pacman is on one
					if (map[pacman.getPos().y][pacman.getPos().x] == 7 || map[pacman.getPos().y][pacman.getPos().x] == 8)
					{
						// Power up pacman if dot is powerup
						// only add score to pacman
						if (map[pacman.getPos().y][pacman.getPos().x] == 8)
						{
							pacman.setPower();
							for (auto& ghost : ghosts)
								ghost->loseTarget();
							pacman.addScore(40);
						}
						pacman.addScore(10);

						// remove dot for all players
						dots--;
						dotProg += 100;
						mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 0].texCoords = { 0,  0 };
						mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 1].texCoords = { 32,  0 };
						mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 2].texCoords = { 32, 32 };
						mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 3].texCoords = { 0, 32 };
						map[pacman.getPos().y][pacman.getPos().x] = 0;
					}
				}
				count++;
			}
		}

		// Replaces dots in built in map to make it unbeatable
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
		// scales speed
		if (dotProg > 0)
			dotProg -= 7 * std::ceil(dotProg / 100.f);
		if (dotProg < 0)
			dotProg++;

		// End game
		allWon = true;
		livesRemain = false;
		int count = 0;
		for (auto& pacman : pacmen)
		{
			// counts number of alive pacmen
			if (*pacman.lives)
				count++;
			// if a pacman is not dead and hasn't won
			if (!pacman.hasWon() && *pacman.lives)
				allWon = false;
			// if atleast one pacman is still alive
			if (*pacman.lives)
				livesRemain = true;
		}
		// if no pacmen were counted, game over
		if (!count)
			allWon = false;
		if (!livesRemain || allWon)
		{
			if (overTimer == 5 * 60)
				if (client != -1)
				{
					// set ending  bonus score to actual value for animation because it was stealing points otherwise
					pacmen[client].endBonus();
					if (sharedCTRL)
						pacmen[client + 1].endBonus();
				}
			if (overTimer)
				overTimer--;
			for (auto& ghost : ghosts)
				delete ghost;
			ghosts.clear();
			startTimer = 1;
			// adding bonus score to score animation
			if (client != -1)
			{
				if (pacmen[client].getBonusScore())
				{
					pacmen[client].subtractBonus(50);
					if (*pacmen[client].lives)
						pacmen[client].addScore(50);
				}
				if (sharedCTRL)
				{
					if (pacmen[client + 1].getBonusScore())
					{
						pacmen[client + 1].subtractBonus(50);
						if (*pacmen[client + 1].lives)
							pacmen[client + 1].addScore(50);
					}
				}
			}
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
			if (ghost->getTimer() > seen)
				seen = ghost->getTimer();

			if (client != -1 && ghost->hasLOS())
			{
				if (pacmen[client].getPos() == ghost->getTarget())
					los = true;
				if (sharedCTRL)
					if (pacmen[client + 1].getPos() == ghost->getTarget())
						los2 = true;
			}
		}

		if (client != -1)
		{
			if (los)
				pacmen[client].subtractBonus();
			if (sharedCTRL)
				if (los2)
					pacmen[client + 1].subtractBonus();

			// Warning animation
			// in shared mode the red will be shown if one pacman is seen even if the other is powerd up or not seen
			if (!pacmen[client].getPower())
				power = false;
			if (sharedCTRL)
				if (!pacmen[client + 1].getPower())
					power = false;
			if (seen && !power)
			{
				if (seenTimer < 127 && !los)
					seenTimer++;
				else if (seenTimer < 255 && los)
					seenTimer++;
			}
			else
				if (seenTimer > 0)
					seenTimer--;

			// set high score if greater
			// works for shared mode too because the highscore is only stored on the local machine
			if (pacmen[client].getScore() > *hScore)
				*hScore = pacmen[client].getScore();
			if (sharedCTRL)
				if (pacmen[client + 1].getScore() > *hScore)
				*hScore = pacmen[client + 1].getScore();

			// updates scores text
			scoreTxt.setString("SCORE " + std::to_string(pacmen[client].getScore()) + "+" + std::to_string((!livesRemain || allWon) ? pacmen[client].getBonusScore() : (pacmen[client].getBonusScore() / 60 * 500)));
			if (sharedCTRL)
				scoreTxt2.setString("SCORE " + std::to_string(pacmen[client + 1].getScore()) + "+" + std::to_string((!livesRemain || allWon) ? pacmen[client + 1].getBonusScore() : (pacmen[client + 1].getBonusScore() / 60 * 500)));
			hScoreTxt.setString("HIGH " + std::to_string(*hScore) + " LVL " + std::to_string(level + 1));
		}

		// portal fade in animation after all dots are eaten
		if (!dots && portalTimer < 60)
			portalTimer += 2;


		// Light pacman
		for (auto& pacman : pacmen)
			if (*pacman.lives)
				recLight(map, light, lightVert, (pacman.getPos().x + pacman.getProg().x / 100.f + 0.5f) * lightScale, (pacman.getPos().y + pacman.getProg().y / 100.f + 0.5f) * lightScale, maxLight, false);

		// Light portals
		for (auto portal : portals)
			recLight(map, light, lightVert, portal.x * lightScale, portal.y * lightScale, maxLight * overTimer / (5.f * 60) / (2 - (portalTimer / 60.f)), false);
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
	int count = 0;
	for (auto& pacman : pacmen)
	{
		if (*pacman.lives && !pacman.hasWon())
		{
			pacRect.setSize(sf::Vector2f(scale, scale));
			pacRect.setTextureRect(sf::IntRect(pacman.getDir() * 22, pacman.getAnimation() * 22, 22, 22));

			pacOutline.setRadius(scale / 2.f);
			pacOutline.setFillColor(sf::Color(0x00000000));
			pacOutline.setOutlineColor(colors[count]);
			pacOutline.setOutlineThickness(1);

			// Blinks when invincible
			if (pacman.getDamageTimer() / 6 % 2)
				pacRect.setFillColor(sf::Color(0xAAAA00FF));
			else
				pacRect.setFillColor(sf::Color(0xFFFF00FF));

			// Draws a duplicate pacman in the opposite portal
			if (pacman.getPos().x == 0)
			{
				pacRect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(pacRect);
				pacOutline.setPosition(pacRect.getPosition());
				window.draw(pacOutline);
				if (dots)
				{
					pacRect.setPosition(
						xoff + (mapSize.x + pacman.getProg().x / 100.f) * scale,
						yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
					window.draw(pacRect);
					pacOutline.setPosition(pacRect.getPosition());
					window.draw(pacOutline);
				}
			}
			else if (pacman.getPos().x == mapSize.x - 1)
			{
				if (dots)
				{
					pacRect.setPosition(
						xoff + (-1 + pacman.getProg().x / 100.f) * scale,
						yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
					window.draw(pacRect);
					pacOutline.setPosition(pacRect.getPosition());
					window.draw(pacOutline);
				}
				pacRect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(pacRect);
				pacOutline.setPosition(pacRect.getPosition());
				window.draw(pacOutline);
			}
			else if (pacman.getPos().y == 0)
			{
				pacRect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(pacRect);
				pacOutline.setPosition(pacRect.getPosition());
				window.draw(pacOutline);
				if (dots)
				{
					pacRect.setPosition(
						xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
						yoff + (mapSize.y + pacman.getProg().y / 100.f) * scale);
					window.draw(pacRect);
					pacOutline.setPosition(pacRect.getPosition());
					window.draw(pacOutline);
				}
			}
			else if (pacman.getPos().y == mapSize.y - 1)
			{
				if (dots)
				{
					pacRect.setPosition(
						xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
						yoff + (-1 + pacman.getProg().y / 100.f) * scale);
					window.draw(pacRect);
					pacOutline.setPosition(pacRect.getPosition());
					window.draw(pacOutline);
				}
				pacRect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(pacRect);
				pacOutline.setPosition(pacRect.getPosition());
				window.draw(pacOutline);
			}
			// Draws pacman normally
			else
			{
				pacRect.setPosition(
					xoff + (pacman.getPos().x + pacman.getProg().x / 100.f) * scale,
					yoff + (pacman.getPos().y + pacman.getProg().y / 100.f) * scale);
				window.draw(pacRect);
				pacOutline.setPosition(pacRect.getPosition());
				window.draw(pacOutline);
			}
		}
		count++;
	}

	// Draws ghosts
	rect.setSize(sf::Vector2f(scale, scale));
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

	// Orange overlay
	rect.setFillColor(sf::Color(0xFF880044));
	rect.setSize(sf::Vector2f(mapSize.x * scale, mapSize.y * scale));
	rect.setPosition(xoff, yoff);
	window.draw(rect);

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
	if (client != -1)
	{
		pacRect.setFillColor(sf::Color(0xFFFF00FF));
		pacRect.setSize(sf::Vector2f(scale / 3.f * 2, scale / 3.f * 2));
		pacRect.setTextureRect(sf::IntRect(22, 66, 22, 22));
		for (int i = 0; i < *pacmen[client].lives - 1; i++)
		{
			pacRect.setPosition(xoff + (i * 2 + 1.f / 6 + 1) * scale, yHUDOff + scale / 6.f);
			window.draw(pacRect);
		}
		if (sharedCTRL)
		{
			pacRect.setTextureRect(sf::IntRect(66, 66, 22, 22));
			for (int i = 0; i < *pacmen[client + 1].lives - 1; i++)
			{
				pacRect.setPosition(xoff + mapSize.x * scale - (i * 2 + 1.f / 6 + 1) * scale, yHUDOff + scale / 6.f);
				window.draw(pacRect);
			}
		}
	}

	// Draws dot progress bar
	rect.setFillColor(sf::Color(0xFFFF00FF));
	rect.setSize(sf::Vector2f((mapSize.x * scale - scale / 3.f * 2) * ((dots + dotProg / 100.f) / (float)maxDots), scale));
	rect.setPosition(xoff + scale / 3.f, yHUDOff + scale * 2);
	window.draw(rect);

	// Draws coloring for warning
	rect.setSize(sf::Vector2f(mapSize.x * scale, mapSize.y * scale));
	rect.setPosition(xoff, yoff);
	rect.setFillColor(sf::Color(127, 0, 0, seenTimer / 2));
	window.draw(rect);

	// Draws score
	if (client != -1)
	{
		scoreTxt.setFillColor(colors[0]);
		scoreTxt.setCharacterSize(scale / 2.f);
		scoreTxt.setPosition(xoff + scale, yHUDOff + scale + scoreTxt.getGlobalBounds().height / 4);
		window.draw(scoreTxt);
		if (sharedCTRL)
		{
			scoreTxt2.setFillColor(colors[1]);
			scoreTxt2.setCharacterSize(scale / 2.f);
			scoreTxt2.setPosition(xoff + mapSize.x * scale - scale - scoreTxt2.getGlobalBounds().width, yHUDOff + scale + scoreTxt2.getGlobalBounds().height / 4);
			window.draw(scoreTxt2);
		}
		scoreTxt.setFillColor(sf::Color(0xFFFFFFFF));
		hScoreTxt.setCharacterSize(scale / 2.f);
		hScoreTxt.setPosition(xoff + mapSize.x / 2 * scale - hScoreTxt.getGlobalBounds().width / 2, yHUDOff + scale + hScoreTxt.getGlobalBounds().height / 4);
		window.draw(hScoreTxt);
	}

	oldScale = scale;
}

void Game::drawDebug(sf::RenderWindow& window)
{
	rect.setSize(sf::Vector2f(scale, scale));

	// Draws pacman hitbox
	rect.setFillColor(sf::Color(0xFFFF0088));
	for (auto& pacman : pacmen)
	{
		if (*pacman.lives)
		rect.setPosition(xoff + pacman.getPos().x * scale, yoff + pacman.getPos().y * scale);
		window.draw(rect);
	}

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
}

bool Game::isOver()
{
	return !overTimer;
}

int Game::getScore(int index)
{
	return pacmen[index].getScore();
}

// DO NOT REMOVE OR REPLACE
// This is the deconstructor because otherwise it deletes the texture of pacman before the game starts
void Game::del()
{
	delete pacTex;
}

void Game::loadMap(int num)
{
	int value;
	sf::Vector2f coord;
	sf::Image img;
	sf::Color color;

	// reserves maps
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

	if (img.loadFromFile("../res/maps/map" + std::to_string(num) + ".png"))
	{
		// converts map image to tile values
		for (int y = 0; y < mapSize.y; y++)
			for (int x = 0; x < mapSize.x / 2; x++)
			{
				switch (img.getPixel(x, y).toInteger())
				{
				case 0xFFFFFFFF: // empty
					map[y][x] = 0;
					break;
				case 0x000000FF: // wall
					map[y][x] = 1;
					break;
				case 0x0000FFFF: // invalid
					map[y][x] = 2;
					break;
				case 0xFF0000FF: // ghost spawn
					map[y][x] = 3;
					break;
				case 0xFFFF00FF: // dot
					map[y][x] = 7;
					break;
				case 0xFF00FFFF: // powerup
					map[y][x] = 8;
					break;
				case 0x00FFFFFF: // pacman spawn
					map[y][x] = 9;
					break;
				}
			}
	}
	// loads built in map / final unbeatable level
	else
	{
		failed = true;

		for (int y = 0; y < mapSize.y; y++)
			for (int x = 0; x < mapSize.x; x++)
			{
				// border walls
				if (x == 0 || y == 0 || x == mapSize.x - 1 || y == mapSize.y - 1)
					map[y][x] = 1;
				else
				{
					// makes a lot of rectangle obstacles
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

		// sets spawn positions
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