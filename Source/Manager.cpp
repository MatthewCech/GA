#include "Manager.hpp"
#include <fstream>
#include <string>
#include "RUtils/RException.hpp"


namespace GA
{
  Manager::Manager(std::string file) : _tiles(1, 1)
  {
    std::fstream fileObject(file, std::ios::in);
    if (fileObject.is_open())
    {
      try 
      {
        std::string buffer;
        std::getline(fileObject, buffer);
        _width = std::stoi(buffer);
        std::getline(fileObject, buffer);
        _height = std::stoi(buffer);
        _tiles = RConsole::Field2D<Tile>(_width, _height);
        size_t lineCount = 0;
        while (std::getline(fileObject, buffer))
        {
          for (int i = 0; i < buffer.size(); ++i)
            _tiles[i][lineCount] = parseFileChar(buffer[i]);

          ++lineCount;
        }
      }
      catch (...)
      {
        throw RUtils::RException("Error during file read! Confirm formatting is correct!");
      }
    }
    else
      throw RUtils::RException("Specified file not found!");
  }


  // Constructor - note: No defaults for this class!
  Manager::Manager(size_t width, size_t height)
    : _tiles(width, height)
    , _width(width)
    , _height(width)
  {
    RConsole::Canvas::FillCanvas(RConsole::RasterInfo(lookupASCII(NONE), RConsole::DARKGREY));
  }


  // Places obstacle in the tiles in code
  void Manager::PlaceRock(size_t x, size_t y, size_t side_size)
  {
    for (size_t i = x; i < x + side_size; ++i)
      for (size_t j = y; j < y + side_size; ++j)
        if (getLocSafe(i, j))
          getLocSafe(i, j)->Type = WALL;
  }

  // Runs the primary application
  bool Manager::Run()
  {
    draw();
    return true;
  }


  // Draw
  void Manager::draw()
  {
    _tiles.SetIndex(0);
    while (_tiles.GetIndex() < _tiles.Length())
    {
      const Tile &data = _tiles.Get();
      RConsole::Canvas::Draw(lookupASCII(data.Type), _tiles.GetIndex() % _width, _tiles.GetIndex() / _height, lookupColor(data.Type));
      _tiles.IncrementX();
    }
  }

  Tile parseFileChar(char c)
  {
    Tile tile;
    switch (c)
    {
    case '.':
    case ' ':
      tile.Type = NONE;
      break;
    case '#':
      tile.Type = WALL;
      break;
    case 'g':
    case 'G':
      tile.Type = WALL;
      break;
    case 's':
    case 'a':
    case 'S':
    case 'A':
      tile.Type = START;
      break;
    }
    return tile;
  }

  // Visuals lookup, purely for looking 
  unsigned char Manager::lookupASCII(TileType t)
  {
    switch (t)
    {
    case NONE:
    case WALL:
      return static_cast<unsigned char>(219);
    case GOAL:
      return 'G';
    case START:
      return 'S';
    default:
      return '?';
    }
  }

  // Allows color of a tile to be looked up.
  RConsole::Color Manager::lookupColor(TileType t)
  {
    switch (t)
    {
    case NONE:
      return RConsole::DARKGREY;
    case WALL:
      return RConsole::GREY;
    case GOAL:
      return RConsole::LIGHTBLUE;
    case START:
      return RConsole::GREEN;
    default:
      return RConsole::RED;
    }
  }


  // Safely gets the location of a tile in memory
  Tile *Manager::getLocSafe(size_t x, size_t y)
  {
    if (x >= _tiles.Width() || x < 0)
      return nullptr;

    if (y >= _tiles.Height() || y < 0)
      return nullptr;

    return &(_tiles[x][y]);
  }
}