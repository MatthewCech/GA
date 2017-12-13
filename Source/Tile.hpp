#pragma once
namespace GA
{
  // Tile-related data
  enum TileType { NONE, WALL, START, GOAL };

  struct Tile
  {
    TileType Type = NONE;

  };
}
