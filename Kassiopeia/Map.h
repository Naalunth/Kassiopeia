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
		};

		Map();
		~Map();

		//prints a nice little representation of the map to std::cout
		void PrintMap();

		void LoadMap(std::istream& in);

		void SetMap(int x, int y, const MapData& data);
		MapData GetMap(int x, int y) const;
		MapData& GetMapR(int x, int y);

	private:
		int width, height;
		std::vector<MapData> mapData_;
	};
}

