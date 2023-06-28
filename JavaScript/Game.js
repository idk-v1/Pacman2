import { Pacman } from "./Pacman"
import { ChaserGhost } from "./ChaserGhost"
import { SignalGhost } from "./SignalGhost"

export class Game
{
	constructor(num, HUD, score, hScore, level)
	{
		this.map = new Array(), this.light = new Array();

		this.ghosts = new Array();

		this.portals = new Array();

		this.scoreTxt, this.hScoreTxt;

		this.pacman = new Pacman();

		this.scale = 0, this.oldScale = 0, this.xoff = 0, this.yoff = 0, this.yHUDOff = 0,
			this.lightScale = 3, this.lightRange = 10, this.maxLight = this.lightScale * this.lightRange,
			this.overTimer = 5 * 60, this.startTimer = 3 * 60, this.seenTimer = 0, this.portalTimer = 0,
			this.HUD, this.lives = 3, this.score = 0, this.hScore = { h: 0 }, this.bonusScore = 10 * 60, this.level = 0,
			this.dots = 0, this.maxDots = 0, this.dotProg = 0, this.inputTimer = 0, this.inputDir = -1;

		this.inputBuffer = 0.5;

		this.failed = false;

		this.texture;

		this.mapSize = { x: 28, y: 31 }, this.ghostSpawn = { x: 0, y: 0 }, this.pacmanSpawn = { x: 0, y: 0 };

		this.texture = new Image();
		this.texture.src = "../res/textures/Tilemap.png";
		this.loadMap(num);

		for (let i = 0; i < 3; i++)
			this.ghosts.push_back(new ChaserGhost());
		this.ghosts.push_back(new SignalGhost());

		this.HUD = HUD;
		this.lives = lives;
		this.score = score;
		this.hScore = hScore;
		this.level = level;

		ctx.font("1px ../res/fonts/emulogic.ttf");
	}


	start()
	{
		this.timer.restart();
		this.pacman.start(this.pacmanSpawn);

		if (this.failed)
			for (let i = 0; i < 4 - this.ghosts.size; i++)
				this.ghosts.push_back(new ChaserGhost());

		for (let ghost in this.ghosts)
			ghost.start(this.ghostSpawn);
	}


	update()
	{
		let seen = 0, randX, randY;
		let los = false, onPortal = false;


		if (sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: W) || sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: Up))
		{
			inputTimer = 60 * inputBuffer;
			inputDir = 0;
		}
		if (sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: D) || sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: Right))
		{
			inputTimer = 60 * inputBuffer;
			inputDir = 1;
		}
		if (sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: S) || sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: Down))
		{
			inputTimer = 60 * inputBuffer;
			inputDir = 2;
		}
		if (sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: A) || sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: Left))
		{
			inputTimer = 60 * inputBuffer;
			inputDir = 3;
		}

		// Changes light detail keys 1 - 9
		for (int i = 1; i <= 9; i++)
		if (sf:: Keyboard:: isKeyPressed(sf:: Keyboard:: Key(26 + i)))
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
				for (auto & ghost : ghosts)
				{
					ghost -> updateTarget(map, pacman, mapSize);
					ghost -> move(map, pacman, ghosts, mapSize, lives);
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
						for (auto & ghost : ghosts)
						ghost -> loseTarget();
						score += 40;
					}
					score += 10;
					dots--;
					dotProg += 100;
					mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 0].texCoords = { 0, 0 };
					mapVert[(pacman.getPos().x + pacman.getPos().y * mapSize.x) * 4 + 1].texCoords = { 32, 0 };
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
						mapVert[(randX + randY * mapSize.x) * 4 + 0].texCoords = { 224	   , 0 };
						mapVert[(randX + randY * mapSize.x) * 4 + 1].texCoords = { 224 + 32, 0 };
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
				for (auto & ghost : ghosts)
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
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].color = sf:: Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].color = sf:: Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].color = sf:: Color(0, 0, 0, 255);
				lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].color = sf:: Color(0, 0, 0, 255);
				light[y][x] = 0;
			}

			// Check if ghosts can see pacman
			for (auto & ghost : ghosts)
			{
				if (ghost -> hasLOS())
					los = true;
				if (ghost -> getTimer() > seen)
					seen = ghost -> getTimer();
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

			if (score > * hScore)
			* hScore = score;

			scoreTxt.setString("SCORE " + std:: to_string(score) + "+" + std:: to_string(bonusScore / 60 * 500));
			hScoreTxt.setString("HIGH " + std:: to_string(* hScore) + " LVL " + std:: to_string(level + 1));

			if (!dots && portalTimer < 60)
				portalTimer += 2;


			// Light pacman
			recLight(map, light, lightVert, (pacman.getPos().x + pacman.getProg().x / 100.f + 0.5f) * lightScale, (pacman.getPos().y + pacman.getProg().y / 100.f + 0.5f) * lightScale, maxLight * overTimer / (5.f * 60), false);

			// Light portals
			for (auto portal : portals)
			recLight(map, light, lightVert, portal.x * lightScale, portal.y * lightScale, maxLight * overTimer / (5.f * 60) / (2 - (portalTimer / 60.f)), false);
		}
	}


void Game:: draw(sf:: RenderWindow & window)
{
	int value;

	// Reset scale
	scale = std:: min(window.getSize().x / mapSize.x, window.getSize().y / (mapSize.y + HUD));
	xoff = (window.getSize().x - mapSize.x * scale) / 2;
	yoff = (window.getSize().y - (mapSize.y - HUD) * scale) / 2;
	yHUDOff = (window.getSize().y - (mapSize.y + HUD) * scale) / 2;

	// Change vertex array positions if window is resized
	if (oldScale != scale)
	{
		for (int y = 0; y < mapSize.y; y++)
		for (int x = 0; x < mapSize.x; x++)
		{
			mapVert[(x + y * mapSize.x) * 4 + 0].position = sf:: Vector2f((xoff + (x + 0) * scale), (yoff + (y + 0) * scale));
			mapVert[(x + y * mapSize.x) * 4 + 1].position = sf:: Vector2f((xoff + (x + 1) * scale), (yoff + (y + 0) * scale));
			mapVert[(x + y * mapSize.x) * 4 + 2].position = sf:: Vector2f((xoff + (x + 1) * scale), (yoff + (y + 1) * scale));
			mapVert[(x + y * mapSize.x) * 4 + 3].position = sf:: Vector2f((xoff + (x + 0) * scale), (yoff + (y + 1) * scale));
		}

		for (int y = 0; y < mapSize.y * lightScale; y++)
		for (int x = 0; x < mapSize.x * lightScale; x++)
		{
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].position = sf:: Vector2f((xoff + (x + 0) * scale / lightScale), (yoff + (y + 0) * scale / lightScale));
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].position = sf:: Vector2f((xoff + (x + 1) * scale / lightScale), (yoff + (y + 0) * scale / lightScale));
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].position = sf:: Vector2f((xoff + (x + 1) * scale / lightScale), (yoff + (y + 1) * scale / lightScale));
			lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].position = sf:: Vector2f((xoff + (x + 0) * scale / lightScale), (yoff + (y + 1) * scale / lightScale));
		}
	}

	// Draws map
	window.draw(mapVert, & texture);

	// Draws pacman if alive
	if (lives && !pacman.hasWon())
	{
		rect.setSize(sf:: Vector2f(scale, scale));

		// Blinks when invincible
		if (pacman.getDamageTimer() / 6 % 2)
			rect.setFillColor(sf:: Color(0xAAAA00FF));
		else
			rect.setFillColor(sf:: Color(0xFFFF00FF));

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
	for (auto & ghost : ghosts)
	{
		rect.setFillColor(ghost -> getColor());

		// Draws a duplicate ghost in the opposite portal
		if (ghost -> getPos().x == 0)
		{
			rect.setPosition(xoff + (0 + ghost -> getProg().x / 100.f) * scale, yoff + (ghost -> getPos().y + ghost -> getProg().y / 100.f) * scale);
			window.draw(rect);
			rect.setPosition(xoff + (mapSize.x + ghost -> getProg().x / 100.f) * scale, yoff + (ghost -> getPos().y + ghost -> getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		else if (ghost -> getPos().x == mapSize.x - 1)
		{
			rect.setPosition(xoff + (-1 + ghost -> getProg().x / 100.f) * scale, yoff + (ghost -> getPos().y + ghost -> getProg().y / 100.f) * scale);
			window.draw(rect);
			rect.setPosition(xoff + (mapSize.x - 1 + ghost -> getProg().x / 100.f) * scale, yoff + (ghost -> getPos().y + ghost -> getProg().y / 100.f) * scale);
			window.draw(rect);
		}
		// Draws ghost normally
		else
		{
			rect.setPosition(xoff + (ghost -> getPos().x + ghost -> getProg().x / 100.f) * scale, yoff + (ghost -> getPos().y + ghost -> getProg().y / 100.f) * scale);
			window.draw(rect);
		}
	}

	// Orange overlay
	rect.setFillColor(sf:: Color(0xFF880044));
	rect.setSize(sf:: Vector2f(mapSize.x * scale, mapSize.y * scale));
	rect.setPosition(xoff, yoff);
	window.draw(rect);

	// Draws lighting
	window.draw(lightVert);

	// Covers pacman going through portal out of bounds
	rect.setSize(sf:: Vector2f(xoff, window.getSize().y));
	rect.setPosition(0, 0);
	rect.setFillColor(sf:: Color());
	window.draw(rect);
	rect.setPosition(window.getSize().x - xoff, 0);
	window.draw(rect);

	// Draws remaining lives
	rect.setFillColor(sf:: Color(0xFFFF00FF));
	rect.setSize(sf:: Vector2f(scale / 3.f * 2, scale / 3.f * 2));
	for (int i = 0; i < lives - 1; i++)
	{
		rect.setPosition(xoff + (i * 2 + 1.f / 6 + 1) * scale, yHUDOff + scale / 6.f);
		window.draw(rect);
	}

	// Draws dot progress bar
	rect.setSize(sf:: Vector2f((mapSize.x * scale - scale / 3.f * 2) * ((float)(dots + dotProg / 100.f) / maxDots), scale));
	rect.setPosition(xoff + scale / 3.f, yHUDOff + scale);
	window.draw(rect);

	// Draws coloring for warning
	rect.setSize(sf:: Vector2f(mapSize.x * scale, mapSize.y * scale));
	rect.setPosition(xoff, yoff);
	rect.setFillColor(sf:: Color(127, 0, 0, seenTimer / 2));
	window.draw(rect);

	// Draws score
	scoreTxt.setCharacterSize(scale / 2.f);
	scoreTxt.setPosition(xoff + mapSize.x / 2 * scale - scoreTxt.getGlobalBounds().width, yHUDOff + scoreTxt.getGlobalBounds().height / 4);
	window.draw(scoreTxt);
	hScoreTxt.setCharacterSize(scale / 2.f);
	hScoreTxt.setPosition(xoff + mapSize.x * scale - hScoreTxt.getGlobalBounds().width - scale, yHUDOff + hScoreTxt.getGlobalBounds().height / 4);
	window.draw(hScoreTxt);

	oldScale = scale;
}

void Game:: drawDebug(sf:: RenderWindow & window)
{
	rect.setSize(sf:: Vector2f(scale, scale));

	// Draws pacman hitbox
	rect.setFillColor(sf:: Color(0xFFFF0088));
	rect.setPosition(xoff + pacman.getPos().x * scale, yoff + pacman.getPos().y * scale);
	window.draw(rect);

	for (auto & ghost : ghosts)
	{
		// Draws ghost hitboxes
		rect.setFillColor(ghost -> getColor());
		rect.setPosition(xoff + ghost -> getPos().x * scale, yoff + ghost -> getPos().y * scale);
		window.draw(rect);

		// Draws ghost targets
		rect.setOutlineColor(rect.getFillColor());
		rect.setFillColor(sf:: Color(0, 0, 0, 0));
		rect.setOutlineThickness(3);
		rect.setPosition(xoff + ghost -> getTarget().x * scale, yoff + ghost -> getTarget().y * scale);
		window.draw(rect);
		rect.setOutlineThickness(0);
	}
}

void Game:: setLightScale(int newScale)
{
	lightScale = newScale;
	lightVert.resize(mapSize.x * lightScale * mapSize.y * lightScale * 4);
	for (int y = 0; y < mapSize.y * lightScale; y++)
	for (int x = 0; x < mapSize.x * lightScale; x++)
	{
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].position = sf:: Vector2f(xoff + (x + 0) * scale / lightScale, yoff + (y + 0) * scale / lightScale);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].position = sf:: Vector2f(xoff + (x + 1) * scale / lightScale, yoff + (y + 0) * scale / lightScale);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].position = sf:: Vector2f(xoff + (x + 1) * scale / lightScale, yoff + (y + 1) * scale / lightScale);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].position = sf:: Vector2f(xoff + (x + 0) * scale / lightScale, yoff + (y + 1) * scale / lightScale);

		lightVert[(x + y * mapSize.x * lightScale) * 4 + 0].color = sf:: Color(0, 0, 0, 255);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 1].color = sf:: Color(0, 0, 0, 255);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 2].color = sf:: Color(0, 0, 0, 255);
		lightVert[(x + y * mapSize.x * lightScale) * 4 + 3].color = sf:: Color(0, 0, 0, 255);
	}

	light.resize(mapSize.y * lightScale);
	for (auto & y : light)
	{
		y.resize(mapSize.x * lightScale);
		for (auto & x : y)
		x = 0;
	}

	maxLight = lightScale * lightRange;

	recLight(map, light, lightVert, (pacman.getPos().x + pacman.getProg().x / 100.f + 0.5f) * lightScale, (pacman.getPos().y + pacman.getProg().y / 100.f + 0.5f) * lightScale, maxLight, false);

	for (auto & portal : portals)
	recLight(map, light, lightVert, portal.x * lightScale, portal.y * lightScale, maxLight / 2, false);
}

bool Game:: isOver()
{
	return !overTimer;
}

int Game:: getLives()
{
	return lives;
}

int Game:: getScore()
{
	return score;
}

loadMap(num)
{
	let value;
	let img;

	this.map.resize(this.mapSize.y);
	for (let y in this.map)
	y.resize(this.mapSize.x);
	this.light.resize(this.mapSize.y * this.lightScale);
	for (let y in this.light)
	{
		y.resize(this.mapSize.x * this.lightScale);
		for (let x in y)
		x = 0;
	}

	if (img.loadFromFile("../res/maps/map" + num + ".png"))
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
		this.failed = true;

		for (let y = 0; y < this.mapSize.y; y++)
			for (let x = 0; x < this.mapSize.x; x++)
			{
				if (x == 0 || y == 0 || x == this.mapSize.x - 1 || y == this.mapSize.y - 1)
					this.map[y][x] = 1;
				else
				{
					if ((x + 4) % 5 && (y + 6) % 7)
					{
						if ((x + 4) % 5 < 4 && (x + 4) % 5 > 1 && (y + 6) % 7 < 6 && (y + 6) % 7 > 1)
							this.map[y][x] = 2;
						else
							this.map[y][x] = 1;
					}
					else
						this.map[y][x] = 7;
				}
			}

		this.map[15][0] = 0;
		this.map[this.mapSize.y / 3][this.mapSize.x / 2 - 1] = 3;
		this.map[this.mapSize.y / 3 * 2 + 2][this.mapSize.x / 2 - 1] = 9;
	}

	for (let y = 0; y < this.mapSize.y; y++)
		for (let x = 0; x < this.mapSize.x / 2; x++)
		{
			value = this.map[y][x];

			if (x == 0 && !value)
				this.portals.push_back({ x: 0, y: y + 0.5 });
			if (y == 0 && !value)
				this.portals.push_back({ x: x + 0.5, y: 0 });

			// Sets map textures
			switch (value)
			{
				case 3:		// Ghost Spawn
					this.ghostSpawn = { x, y };
					break;
				case 7:		// Dot
					this.dots += 2;
					break;
				case 8:		// Powerup
					this.dots += 2;
					break;
				case 9:		// Pacman Spawn
					this.pacmanSpawn = { x, y };
					break;
			}

			this.map[y][this.mapSize.x - 1 - x] = this.map[y][x];
		}

	this.maxDots = this.dots;
}

recLight(map, light, x, y, value, onSolid)
{
	// Keeps starting light in bounds
	if (x < 0)
		x = 0;
	if (x >= this.mapSize.x * this.lightScale)
		x = this.mapSize.x * this.lightScale - 1;
	if (y < 0)
		y = 0;
	if (y >= this.mapSize.y * this.lightScale)
		y = this.mapSize.y * this.lightScale - 1;

	// Change if new light is greater
	if (light[y][x] < value)
	{
		// If tile is non-solid and last light was from a solid
		if (map[y / this.lightScale][x / this.lightScale] != 1 && onSolid);
		// Opposite of that. It's weird
		else
		{
			light[y][x] = value;

			// If tile is solid
			if (map[y / this.lightScale][x / this.lightScale] == 1)
				onSolid = true;

			// Recursion
			if (y - 1 < 0 && !onSolid)
				this.recLight(map, light, x, this.mapSize.y * this.lightScale - 1, value - 1, onSolid);
			else
				this.recLight(map, light, x, y - 1, value - 1, onSolid);

			if (x + 1 >= this.mapSize.x * this.lightScale && !onSolid)
				this.recLight(map, light, 0, y, value - 1, onSolid);
			else
				this.recLight(map, light, x + 1, y, value - 1, onSolid);

			if (y + 1 >= this.mapSize.y * this.lightScale && !onSolid)
				this.recLight(map, light, x, 0, value - 1, onSolid);
			else
				this.recLight(map, light, x, y + 1, value - 1, onSolid);

			if (x - 1 < 0 && !onSolid)
				this.recLight(map, light, this.mapSize.x * this.lightScale - 1, y, value - 1, onSolid);
			else
				this.recLight(map, light, x - 1, y, value - 1, onSolid);
		}
	}
}
}