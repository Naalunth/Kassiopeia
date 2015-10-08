#include "Kassiopeia.h"

#include "Map.h"

#include <fstream>
#include <iostream>
#include <string>


Kassiopeia::Kassiopeia::Kassiopeia()
{
}


Kassiopeia::Kassiopeia::~Kassiopeia()
{
}

void Kassiopeia::Kassiopeia::run()
{
	using std::cin;
	using std::cout;

	currentState = MAIN;

	auto mainState = [&]()
	{
		cout << "\n"
			<< "1: Load file\n"
			<< "0: Exit\n"
			<< "> ";
		int input;
		cin >> input;
		switch (input)
		{
		case 1:
			transitionState(MAP_LOADED);
			break;
		case 0:
			transitionState(CLOSING);
			break;
		}
	};

	auto mapLoadedState = [&]()
	{
		cout << "\n"
			<< "1: Print Map\n"
			<< "2: Show number of regions\n"
			<< "0: Back\n"
			<< "> ";
		int input;
		cin >> input;
		switch (input)
		{
		case 1:
			map.PrintMap();
			cout << std::endl;
			break;
		case 2:
			cout << "Number of regions: " << map.numberOfRegions() << std::endl;
			break;
		case 0:
			transitionState(MAIN);
			break;
		}
	};

	bool running = true;
	
	while (running)
	{
		switch (currentState)
		{
		case MAIN:
			mainState();
			break;
		case MAP_LOADED:
			mapLoadedState();
			break;
		case CLOSING:
			std::cout << "Bye! :D";
			running = false;
		}
	}
}

void Kassiopeia::Kassiopeia::transitionState(State next)
{
	switch (next)
	{
	case CLOSING:
		break;
	case MAIN:
		std::cout << "\n";
		break;
	case MAP_LOADED:
		if (currentState == MAIN)
		{
			std::cout << "Enter the filename\n"
				<< "> ";
			std::cout.flush();
			std::string input;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::getline(std::cin, input);
			try {
				std::ifstream s{ input };
				map.LoadMap(s);
			} catch (std::exception ex) {
				std::cerr << "Got an error while loading map: " << ex.what() << std::endl;
				currentState = MAIN;
				return;
			}
			std::cout << "\n";
		}
		break;
	}
	currentState = next;
}
