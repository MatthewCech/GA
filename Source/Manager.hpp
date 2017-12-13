#pragma once
#include "console-utils.hpp"
#include "Tile.hpp"


namespace GA
{
  class Manager
  {
  public:
    Manager(size_t width, size_t height);
    bool Run();
    void PlaceRock(size_t x, size_t y, size_t side_size);

  private:
    // Private Functions
    void draw();
    unsigned char lookupASCII(TileType t);
    RConsole::Color lookupColor(TileType t);
    Tile *getLocSafe(size_t x, size_t y);

    // Variables
    RConsole::Field2D<Tile> _tiles;
    const size_t _width;
    const size_t _height;
  };
}
