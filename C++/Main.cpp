// https://shaunlebron.github.io/pacman-mazegen/tetris/many.htm

#include "Game.h"
#include "Menu.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

int getRandMap(std::vector<int>& maps);

void save(int score);

void clientUpdate(sf::UdpSocket& sock, sf::IpAddress& hostIP, sf::Packet& receive, unsigned short port, bool gameStarted, bool& connected);

bool clientConnect(sf::UdpSocket& sock, sf::IpAddress& hostIP, unsigned short& port);

void hostUpdate(sf::UdpSocket& sock, std::vector<sf::IpAddress>& clientsIP, std::vector<sf::Packet>& receive, unsigned short port, bool gameStarted, bool& newConnection);

bool hostConnect(sf::UdpSocket& sock, unsigned short& port);


int main()
{
	srand(time(NULL));

	int HUD = 3, level = 0, score = 0, hScore = 0, hScoreOrigin = 0, scale = std::min(sf::VideoMode::getDesktopMode().width / 28, (sf::VideoMode::getDesktopMode().height - 200) / (31 + HUD));
	sf::RenderWindow window(sf::VideoMode(scale * 28, scale * (31 + HUD)), "Pacman 2");
	window.setFramerateLimit(165);

	Menu mMenu, gameover, multiplayer, clientjoined, hostCTRL;

	int menuState = 0, lastMenuState = 0;
	bool changeMenu = false;

	bool gameStarted = false;

	int numMaps = 0;
	for (auto& entry : std::filesystem::directory_iterator("../res/maps"))
		if (entry.is_regular_file())
			numMaps++;

	std::vector<int> maps;
	for (int i = 1; i < numMaps; i++)
		maps.push_back(i);

	std::vector<int> lives, scores;
	std::vector<int*> livesPtr;

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

	mMenu.load("../res/menu/mainmenu", font);
	gameover.load("../res/menu/gameover", font);
	multiplayer.load("../res/menu/multiplayer", font);
	clientjoined.load("../res/menu/clientjoined", font);
	hostCTRL.load("../res/menu/hostctrl", font);

	Game game;

	int numPacmen = 2, numGhosts = 0;

	int numPlayers = 1, connection = 0;
	unsigned short port;
	bool connected = false;
	std::vector<sf::IpAddress> clients;
	sf::UdpSocket sock;
	sf::IpAddress hostIP, ip;
	sf::Packet hostData;
	std::vector<sf::Packet> clientData;

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
				numPacmen = 1;
				numGhosts = 0;

				scores.reserve(numPacmen);
				lives.reserve(numPacmen);
				livesPtr.reserve(numPacmen);
				for (int i = 0; i < numPacmen; i++)
				{
					scores.push_back(0);
					lives.push_back(3);
					livesPtr.push_back(&lives[i]);
				}
				gameover.setElementPage(0, 1);
				level = 0;
				score = 0;
				game = Game(0, HUD, livesPtr, scores, font, &hScore, level, 0, numPacmen, numGhosts, false);
				gameStarted = true;
			}
			game.update();
			game.draw(window);
#if !defined(NDEBUG)
			game.drawDebug(window);
#endif

			if (game.isOver())
			{
				bool livesRemaining = false;
				for (auto& life : lives)
					if (life)
						livesRemaining = true;
				for (int i = 0; i < numPacmen; i++)
					scores[i] = game.getScore(i);
				if (livesRemaining)
					game = Game(getRandMap(maps), HUD, livesPtr, scores, font, &hScore, ++level, 0, numPacmen, numGhosts, false);
				else
				{
					gameStarted = false;
					lives.clear();
					livesPtr.clear();
					menuState = 2;
					gameover.update(window, sf::Mouse::getPosition(), menuState);
					gameover.setElementText(5, std::to_string(level + 1));
					gameover.setElementText(4, std::to_string(hScore));
					for (int i = 0; i < numPacmen; i++)
					{
						gameover.addTextElement(20, 16.5 + i, 0.75, 'L', std::to_string(scores[i]), font);
						gameover.addTextElement(8, 16.5 + i, 0.75, 'R', "Pacman " + std::to_string(i + 1), font);
					}
					gameover.rescale(window);
					if (hScore > hScoreOrigin)
						save(hScore);
					scores.clear();
				}
			}
			break;
		case 2: // Game Over
			gameover.update(window, sf::Mouse::getPosition(window), menuState);
			gameover.draw(window);
			break;
		case 3: // Multiplayer host/join
			multiplayer.update(window, sf::Mouse::getPosition(window), menuState);
			multiplayer.draw(window);
			break;
		case 4: // join enter ip
			if (changeMenu || !connection)
			{
				if (clientConnect(sock, hostIP, port))
					connection = 1;
				clientjoined.setElementText(1, "IP: " + sf::IpAddress::getLocalAddress().toString());
			}
			else
			{
				clientUpdate(sock, hostIP, hostData, port, gameStarted, connected);
				if (connected)
				{
					clientjoined.update(window, sf::Mouse::getPosition(window), menuState);
					clientjoined.draw(window);
				}
			}
			break;
		case 5: // host
			if (changeMenu || !connection)
			{
				if (hostConnect(sock, port))
					connection = 2;
				hostCTRL.setElementText(0, sf::IpAddress::getLocalAddress().toString());
			}
			else
			{
				bool newConnection = false;
				hostUpdate(sock, clients, clientData, port, gameStarted, newConnection);
				if (newConnection)
				{
					hostCTRL.addTextElement(8, 7 + clients.size(), 0.75, 'R', clients.back().toString(), font);
					hostCTRL.rescale(window); 
				}
				hostCTRL.update(window, sf::Mouse::getPosition(window), menuState);
				hostCTRL.draw(window);
			}

			break;
		case 6: // Shared screen multiplayer
			if (changeMenu)
			{
				numPacmen = 2;
				numGhosts = 0;

				scores.reserve(numPacmen);
				lives.reserve(numPacmen);
				livesPtr.reserve(numPacmen);
				for (int i = 0; i < numPacmen; i++)
				{
					scores.push_back(0);
					lives.push_back(3);
					livesPtr.push_back(&lives[i]);
				}
				gameover.setElementPage(0, 6);
				level = 0;
				score = 0;
				game = Game(0, HUD, livesPtr, scores, font, &hScore, level, 0, numPacmen, numGhosts, true);
				gameStarted = true;
			}
			game.update();
			game.draw(window);
#if !defined(NDEBUG)
			game.drawDebug(window);
#endif

			if (game.isOver())
			{
				bool livesRemaining = false;
				for (auto& life : lives)
					if (life)
						livesRemaining = true;
				for (int i = 0; i < numPacmen; i++)
					scores[i] = game.getScore(i);
				if (livesRemaining)
					game = Game(getRandMap(maps), HUD, livesPtr, scores, font, &hScore, ++level, 0, numPacmen, numGhosts, true);
				else
				{
					gameStarted = false;
					lives.clear();
					livesPtr.clear();
					menuState = 2;
					gameover.setElementText(5, std::to_string(level + 1));
					gameover.setElementText(4, std::to_string(hScore));
					for (int i = 0; i < numPacmen; i++)
					{
						gameover.addTextElement(20, 16.5 + i, 0.75, 'L', std::to_string(scores[i]), font);
						gameover.addTextElement(8, 16.5 + i, 0.75, 'R', "Pacman " + std::to_string(i + 1), font);
					}
					gameover.rescale(window);
					if (hScore > hScoreOrigin)
						save(hScore);
					scores.clear();
				}
			}
			break;
		case 7: // Network Multiplayer
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

void clientUpdate(sf::UdpSocket& sock, sf::IpAddress& hostIP, sf::Packet& receive, unsigned short port, bool gameStarted, bool& connected)
{
	if (!connected)
	{
		sf::Packet send;
		send << sf::IpAddress::getLocalAddress().toString();
		sock.send(send, hostIP, 50001);

		receive.clear();
		if (sock.receive(receive, hostIP, port) != sf::UdpSocket::NotReady)
		{
			int num;
			receive >> num;
			if (num == 1)
			{
				connected = true;
				std::cout << "Connected to Host\n";
			}
		}
	}
}

bool clientConnect(sf::UdpSocket& sock, sf::IpAddress& hostIP, unsigned short& port)
{
	std::string ipString;

	port = 50002;
	if (sock.bind(port) == sf::UdpSocket::Done)
	{
		sock.setBlocking(false);
		std::cout << "Started\n";
	}
	else
	{
		std::cout << "Unable to bind to port " << port << '\n';
		return false;
	}

	std::cout << "Enter IP Address of host:\n";
	std::cin >> ipString;
	hostIP = sf::IpAddress(ipString);
	if (hostIP == sf::IpAddress::None)
	{
		std::cout << "Invalid IP Address\n";
		return false;
	}
	else
		return true;
}

void hostUpdate(sf::UdpSocket& sock, std::vector<sf::IpAddress>& clientsIP, std::vector<sf::Packet>& receive, unsigned short port, bool gameStarted, bool& newConnection)
{
	if (!gameStarted)
	{
		sf::Packet clientIP;
		sf::IpAddress any;
		if (sock.receive(clientIP, any, port) == sf::UdpSocket::Done)
		{
			bool newAddress = true;
			std::string data;
			clientIP >> data;
			for (auto& ip : clientsIP)
				if (data == ip.toString())
					newAddress = false;
			if (newAddress)
			{
				newConnection = true;
				clientsIP.push_back(sf::IpAddress(data));
				sf::Packet send;
				send << 1;
				sock.send(send, clientsIP.back(), 50002);
				std::cout << data << " Joined!\n";
			}
		}
	}
}

bool hostConnect(sf::UdpSocket& sock, unsigned short& port)
{
	port = 50001;
	if (sock.bind(port) == sf::UdpSocket::Done)
	{
		sock.setBlocking(false);
		std::cout << "Started\n" << sf::IpAddress::getLocalAddress().toString() << '\n';
		return true;
	}
	std::cout << "Unable to bind to port " << port << '\n';
	return false;
}
