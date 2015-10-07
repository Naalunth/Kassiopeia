#include "Map.h"

#include "disjoint_set.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <iterator>
#include <iostream>
#include <set>


Kassiopeia::Map::Map()
	:isMapInitialized(false)
{
}


Kassiopeia::Map::~Map()
{
}

void Kassiopeia::Map::PrintMap()
{
}

bool Kassiopeia::Map::LoadMap(std::istream& in)
{
	using std::string;
	using std::vector;
	using boost::lexical_cast;

	if (!in.good())
	{
		throw std::exception("input stream invalid");
	}

	try {
		//read the maps dimensions and construct a map of specified size
		{
			string head;
			vector<string> dimensions;
			std::getline(in, head);
			boost::split(dimensions, head, (int(__cdecl*)(int)) std::isspace, boost::token_compress_on);

			height = lexical_cast<int>(dimensions[0]);
			width = lexical_cast<int>(dimensions[1]);

			mapData.resize((size_t) height * width);
		}

		//read the maps data
		{
			string line;
			int lineCounter = 0;
			MapData tmp;
			while (std::getline(in, line).good())
			{
				for (int i = 0; i < width; i++)
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
				if (lineCounter >= height) break;
			}
		}

	} catch (const std::exception ex) {
		std::cerr << "Got an error while loading map: " << ex.what() << std::endl;
		return false;
	}
	isMapInitialized = true;
	return true;
}

void Kassiopeia::Map::SetMap(int x, int y, const MapData & data)
{
	mapData[y * width + x] = data;
}

Kassiopeia::Map::MapData Kassiopeia::Map::GetMap(int x, int y) const
{
	return mapData[y * width + x];
}

Kassiopeia::Map::MapData& Kassiopeia::Map::GetMapR(int x, int y)
{
	return mapData[y * width + x];
}

Kassiopeia::Map::MapIterator Kassiopeia::Map::begin()
{
	return MapIterator{ *this };
}

Kassiopeia::Map::MapIterator Kassiopeia::Map::end()
{
	MapIterator mi{ *this };
	mi.x = 0;
	mi.y = height;
	return mi;
}

//Two-pass variation stolen from https://en.wikipedia.org/wiki/Connected-component_labeling
int Kassiopeia::Map::numberOfRegions()
{
	assert(isMapInitialized);

	std::vector<int> regionlabels(width * height, -1);
	auto getLabel = [&regionlabels, this](int x, int y)->int& {return regionlabels[y * width + x]; };
	disjoint_set<int> labelset{};

	//1st pass
	{
		int labelcounter = 0;
		for (MapIterator it{ *this }; it.good(); ++it)
		{
			if (!it.get().isFree) continue;
			if (it.x > 0 && GetMap(it.x - 1, it.y).isFree)
			{
				if (it.y > 0 && GetMap(it.x, it.y - 1).isFree)
				{
					getLabel(it.x, it.y) = std::min(getLabel(it.x - 1, it.y), getLabel(it.x, it.y - 1));
					labelset.merge(getLabel(it.x - 1, it.y), getLabel(it.x, it.y - 1));
				}
				else
					getLabel(it.x, it.y) = getLabel(it.x - 1, it.y);
			}
			else if (it.y > 0 && GetMap(it.x, it.y - 1).isFree)
			{
				getLabel(it.x, it.y) = getLabel(it.x, it.y - 1);
			}
			else
			{
				getLabel(it.x, it.y) = labelcounter;
				labelset.add(labelcounter++);
			}
		}
	}

	//2nd pass
	for (MapIterator it{ *this }; it.good(); ++it)
	{
		int& label = getLabel(it.x, it.y);
		label = labelset.find(label);
	}



	{
		std::set<int> usedLabels;
		for (MapIterator it{ *this }; it.good(); ++it)
		{
			int label = getLabel(it.x, it.y);
			if (label == -1) continue;
			usedLabels.insert(label);
		}
		return usedLabels.size();
	}

	return -1;
}

bool Kassiopeia::Map::isMapContinuous()
{
	return numberOfRegions() == 1;
}

inline Kassiopeia::Map::MapIterator::MapIterator(Map & m) : m(m) { reset(); }

inline Kassiopeia::Map::MapData Kassiopeia::Map::MapIterator::get() { return m.GetMap(x, y); }

inline Kassiopeia::Map::MapData & Kassiopeia::Map::MapIterator::getR() { return m.GetMapR(x, y); }

inline bool Kassiopeia::Map::MapIterator::good() { return y < m.height && x < m.width; }

inline void Kassiopeia::Map::MapIterator::reset() { x = 0; y = 0; }

