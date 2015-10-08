#pragma once

#include "util.h"

#include <cstdint>
#include <vector>
#include <string>
#include <tuple>

namespace Kassiopeia
{
	class Map
	{
	public:
		struct MapData
		{
			uint8_t isFree : 1;
			uint8_t isTurtle : 1;
			uint8_t isAlreadyVisited : 1;
			uint8_t internal_isPath : 1;
			uint8_t internal_isDeadEnd : 1;
		};

		struct MapIterator : public std::iterator<std::forward_iterator_tag, std::pair<ivec2, MapData&>>
		{
			MapIterator(Map& m);
			void operator=(const MapIterator& other) { m = other.m; }
			void operator++() { if (++p.x >= m.width) { p.x = 0; ++p.y; } }
			value_type operator*() { return std::make_pair(p, getR()); }
			bool operator!=(const MapIterator& other) const { return !(p.x == other.p.x && p.y == other.p.y); }
			MapData get();
			MapData& getR();
			ivec2 getPoint() { return p; };
			bool good() const;
			void reset();
			ivec2 p;
		private:
			Map& m;
		};

		Map();
		~Map();

		//Prints a nice little representation of the map to std::cout
		void PrintMap();

		//Loads a map from the specified istream, returns false on error
		//throws exceptions
		bool LoadMap(std::istream& in);
		bool LoadMap(std::istream&& in) = delete;

		//Set the map data at the specified coordinates
		void SetMap(int x, int y, const MapData& data);
		//Set the map data at the specified coordinates
		void SetMap(ivec2 p, const MapData& data);

		//Get the map data at the specified coordinates
		MapData GetMap(int x, int y) const;
		//Get the map data at the specified coordinates
		MapData GetMap(ivec2 p) const;

		//Get a reference to the map data at the specified coordinates
		MapData& GetMapR(int x, int y);
		//Get a reference to the map data at the specified coordinates
		MapData& GetMapR(ivec2 p);

		MapIterator begin();
		MapIterator end();

		//Works out different partitions in the map
		std::vector<int> partitionMap(bool considerPathsAsWalls = false);

		//Counts how many continous regions the map has
		//Uses partitionMap()
		int numberOfRegions();

		//True if the map has only one continous region
		bool isMapContinuous();

		std::string FindFillingPath();

		int width, height;

	private:
		void updateAllInternalMarkers();
		void updateAroundPoint(ivec2 p);
		void updatePoint(ivec2 p);

		std::vector<MapData> mapData;
		bool isMapInitialized;
	};
}
