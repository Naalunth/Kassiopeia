#include "util.h"

ivec2 ivec2::operator+(const ivec2 & other) const
{
	return ivec2{ x + other.x, y + other.y };
}
