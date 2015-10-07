#pragma once

#include <cstdint>
#include <vector>
#include <string>

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
			uint8_t internal_isMarked : 1;
		};

		struct MapIterator
		{
			MapIterator(Map& m);
			void operator++() { if (++x >= m.width) { x = 0; ++y; } }
			MapData operator*() { return get(); }
			bool operator!=(const MapIterator& other) { return !(x == other.x && y == other.y); }
			MapData get();
			MapData& getR();
			bool good();
			void reset();
			int x, y;
		private:
			Map& m;
		};

		Map();
		~Map();

		//prints a nice little representation of the map to std::cout
		void PrintMap();

		//loads a map from the specified istream, returns false on error
		//throws exceptions
		bool LoadMap(std::istream& in);
		bool LoadMap(std::istream&& in) = delete;

		void SetMap(int x, int y, const MapData& data);
		MapData GetMap(int x, int y) const;
		MapData& GetMapR(int x, int y);

		MapIterator begin();
		MapIterator end();

		int numberOfRegions();
		bool isMapContinuous();

		int width, height;
	private:
		std::vector<MapData> mapData;
		bool isMapInitialized;
	};
}

