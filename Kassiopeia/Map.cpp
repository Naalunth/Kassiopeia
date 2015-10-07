#include "Map.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <iterator>


Kassiopeia::Map::Map()
{
}


Kassiopeia::Map::~Map()
{
}

void Kassiopeia::Map::PrintMap()
{
}

void Kassiopeia::Map::LoadMap(std::istream& in)
{
	using std::string;
	using std::vector;
	using boost::lexical_cast;

	//read the maps dimensions and construct a map of specified size
	{
		string head;
		std::getline(in, head);
		vector<string> dimensions;
		boost::split(dimensions, head, std::isspace, boost::token_compress_on);

		height = lexical_cast<int>(dimensions[0]);
		width = lexical_cast<int>(dimensions[1]);

		mapData_.resize((size_t) height * width);
	}

	//read the maps data
	{
		string line;
		int lineCounter = 0;
		MapData tmp;
		while (std::getline(in, line).good())
		{
			for (int i = 0; i > width; i++)
			{
				switch (line.at(i))
				{
				case '#':
					tmp = { false, false, false };
					break;
				case ' ':
					tmp = { true, false, false };
					break;
				case 'K':
					tmp = { true, true, false };
					break;
				default:
					//error here: the map contained some symbols the program could not understand
					assert(false);
				}
				SetMap(i, lineCounter, tmp);
			}
			lineCounter++;
			//error here: the loaded map is larger than the specified size
			assert(lineCounter < height);
		}
	}

}

void Kassiopeia::Map::SetMap(int x, int y, const MapData & data)
{
	mapData_[y * width + x] = data;
}

Kassiopeia::Map::MapData Kassiopeia::Map::GetMap(int x, int y) const
{
	return mapData_[y * width + x];
}

Kassiopeia::Map::MapData& Kassiopeia::Map::GetMapR(int x, int y)
{
	return mapData_[y * width + x];
}
