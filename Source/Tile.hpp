#pragma once
namespace GA
{
  // Tile-related data
  enum TileType { NONE, WALL, START, GOAL };

  struct PathPoint
  {
    PathPoint() : X(INT_MAX), Y(INT_MAX), Valid(true) {  }
    PathPoint(size_t x, size_t y, bool isValid) : X(x), Y(y), Valid(isValid) {  }
    size_t X;
    size_t Y;
    bool Valid = true;
  };

  struct Tile
  {
    TileType Type = NONE;
    bool IsPath = false;
  };
}
