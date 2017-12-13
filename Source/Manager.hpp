#pragma once
#include "console-utils.hpp"
#include "Tile.hpp"
#include <string>



namespace GA
{
  class Manager
  {
  public:
    Manager(size_t width, size_t height);
    Manager(std::string file);
    bool Run();
    void PlaceRock(size_t x, size_t y, size_t side_size);

  private:
    // Private Functions
    void draw();
    unsigned char lookupASCII(TileType t);
    Tile parseFileChar(char c);
    RConsole::Color lookupColor(TileType t);
    Tile *getLocSafe(size_t x, size_t y);

    // Variables
    RConsole::Field2D<Tile> _tiles;
    size_t _width;
    size_t _height;
  };
}
