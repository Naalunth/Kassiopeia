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
#include <stack>


Kassiopeia::Map::Map()
	:isMapInitialized(false)
{
}


Kassiopeia::Map::~Map()
{
}

void Kassiopeia::Map::PrintMap()
{
	int lastline = 0;
	std::string toPrint;
	for (auto it : *this)
	{
		if (it.first.y > lastline)
		{
			std::cout << "\n";
			++lastline;
		}
		if (!it.second.isFree)
			toPrint = "#";
		else
		{
			if (it.second.isTurtle)
				toPrint = "K";
			else
				toPrint = " ";
		}
		std::cout << toPrint;
	}
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

void Kassiopeia::Map::SetMap(ivec2 p, const MapData & data)
{
	SetMap(p.x, p.y, data);
}

Kassiopeia::Map::MapData Kassiopeia::Map::GetMap(int x, int y) const
{
	return mapData[y * width + x];
}

Kassiopeia::Map::MapData Kassiopeia::Map::GetMap(ivec2 p) const
{
	return GetMap(p.x, p.y);
}

Kassiopeia::Map::MapData& Kassiopeia::Map::GetMapR(int x, int y)
{
	return mapData[y * width + x];
}

Kassiopeia::Map::MapData& Kassiopeia::Map::GetMapR(ivec2 p)
{
	return GetMapR(p.x, p.y);
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
std::vector<int> Kassiopeia::Map::partitionMap(bool considerPathsAsRegions)
{
	assert(isMapInitialized);

	std::vector<int> regionlabels(width * height, -1);
	auto getLabel = [this, &regionlabels](int x, int y)->int& {return regionlabels[y * width + x]; };
	disjoint_set<int> labelset{};

	//1st pass
	{
		auto isEqual = [this, &considerPathsAsRegions](int x, int y, const MapData& self)
		{
			return GetMap(x, y).isFree && (considerPathsAsRegions ? (self.internal_isPath == GetMap(x, y).internal_isPath) : true);
		};
		int labelcounter = 0;
		for (auto cell : *this)
		{
			if (!GetMap(cell.first.x, cell.first.y).isFree) continue;
			if (cell.first.x > 0 && isEqual(cell.first.x - 1, cell.first.y, cell.second))
			{
				if (cell.first.y > 0 && isEqual(cell.first.x, cell.first.y - 1, cell.second))
				{
					getLabel(cell.first.x, cell.first.y) = std::min(getLabel(cell.first.x - 1, cell.first.y), getLabel(cell.first.x, cell.first.y - 1));
					labelset.merge(getLabel(cell.first.x - 1, cell.first.y), getLabel(cell.first.x, cell.first.y - 1));
				}
				else
					getLabel(cell.first.x, cell.first.y) = getLabel(cell.first.x - 1, cell.first.y);
			}
			else if (cell.first.y > 0 && isEqual(cell.first.x, cell.first.y - 1, cell.second))
			{
				getLabel(cell.first.x, cell.first.y) = getLabel(cell.first.x, cell.first.y - 1);
			}
			else
			{
				getLabel(cell.first.x, cell.first.y) = labelcounter;
				labelset.insert(labelcounter++);
			}
		}
	}

	//2nd pass
	for (auto cell : *this)
	{
		int& label = getLabel(cell.first.x, cell.first.y);
		label = labelset.find(label);
	}

	return regionlabels;
}


int Kassiopeia::Map::numberOfRegions()
{
	std::vector<int> regionlabels = partitionMap();
	std::sort(regionlabels.begin(), regionlabels.end());
	return std::distance(regionlabels.begin(), std::unique(regionlabels.begin(), regionlabels.end()));
}

bool Kassiopeia::Map::isMapContinuous()
{
	return numberOfRegions() == 1;
}

std::string Kassiopeia::Map::FindFillingPath()
{
	struct SearchStruct
	{
		Direction direction;
	};
	std::vector<SearchStruct> searchstack;
	searchstack.push_back({ NONE });
	for (auto d : *this) d.second.isAlreadyVisited = false;
	ivec2 currentposition = std::find_if(begin(), end(), [](MapIterator::value_type d)->bool {return d.second.isTurtle; }).getPoint();
	

	bool foundsolution = false;
	while (!searchstack.empty() && !foundsolution)
	{
		int numDeadEnds = std::count_if(begin(), end(), [](MapIterator::value_type d)->bool {return d.second.internal_isDeadEnd; });
	}

	return std::string();
}

void Kassiopeia::Map::updateAllInternalMarkers()
{
	int walls;
	for (auto it : *this)
	{
		updatePoint({ it.first.x, it.first.y });
	}
}

void Kassiopeia::Map::updateAroundPoint(ivec2 p)
{
	updatePoint({ p.x, p.y });
	updatePoint({ p.x - 1, p.y });
	updatePoint({ p.x + 1, p.y });
	updatePoint({ p.x, p.y - 1 });
	updatePoint({ p.x, p.y + 1 });
}

void Kassiopeia::Map::updatePoint(ivec2 p)
{
	if (!GetMap(p).isFree) return;
	int walls = 0;
	auto check = [&](int x, int y)
	{
		if (!in_range(p.x, 0, width) || !in_range(p.y, 0, height) || !GetMap(p).isFree)
			++walls;
	};
	check(p.x - 1, p.y);
	check(p.x + 1, p.y);
	check(p.x, p.y - 1);
	check(p.x, p.y + 1);

	GetMapR(p).internal_isPath = (walls == 2);
	GetMapR(p).internal_isDeadEnd = (walls == 1);
}


inline Kassiopeia::Map::MapIterator::MapIterator(Map & m) : m(m) { reset(); }

inline Kassiopeia::Map::MapData Kassiopeia::Map::MapIterator::get() { return m.GetMap(p); }

inline Kassiopeia::Map::MapData & Kassiopeia::Map::MapIterator::getR() { return m.GetMapR(p); }

inline bool Kassiopeia::Map::MapIterator::good() const { return p.y < m.height && p.x < m.width; }

inline void Kassiopeia::Map::MapIterator::reset() { p = { 0,0 }; }
