#pragma once

#include <algorithm>

struct ivec2
{
	int x;
	int y;

	ivec2 operator+(const ivec2& other) const;
};

template<class T, class CompT = std::less<T> >
bool in_range(T val, T min, T max, CompT cmp = std::less<T>{})
{
	return !(cmp(val, min) || cmp(max, val));
}

enum Direction
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	NONE = 4
};

const ivec2 DirectionVectors[] = { {0,1}, {1,0}, {0,-1}, {-1,0}, {0,0} };
