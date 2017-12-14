#include "Manager.hpp"
#include <fstream>
#include <string>
#include "RUtils/RException.hpp"
#include <cstdlib>

#define REM_P(p) (points[p.X][p.Y] = PathPoint())
#define P_AT(x, y) (x > 0 && x < _width && y > 0 && y < _height && points[x][y].X == INT_MAX)
#define P_LEFT  (P_AT(p.X-1, p.Y) ? points[p.X - 1][p.Y].X != INT_MAX : false)
#define P_RIGHT (P_AT(p.X+1, p.Y) ? points[p.X + 1][p.Y].X != INT_MAX : false)
#define P_UP    (P_AT(p.X, p.Y-1) ? points[p.X][p.Y - 1].X != INT_MAX : false)
#define P_DOWN  (P_AT(p.X, p.Y+1) ? points[p.X][p.Y + 1].X != INT_MAX : false)
#define ADD_U  { if(P_AT(p.X, p.Y-1)) { points[p.X][p.Y - 1] = PathPoint(p.X, p.Y - 1, _tiles[p.X][p.Y - 1].Type == WALL ? false : true); REM_P(p);}}
#define ADD_UL  { if(P_AT(p.X-1, p.Y-1)) { points[p.X - 1][p.Y - 1] = PathPoint(p.X - 1, p.Y - 1, _tiles[p.X - 1][p.Y - 1].Type == WALL ? false : true); REM_P(p); }}
#define ADD_L  { if(P_AT(p.X-1, p.Y)) { points[p.X-1][p.Y] = PathPoint(p.X-1, p.Y, _tiles[p.X-1][p.Y].Type == WALL ? false : true); REM_P(p);}}
#define ADD_UR  { if(P_AT(p.X+1, p.Y-1)) { points[p.X + 1][p.Y - 1] = PathPoint(p.X + 1, p.Y - 1, _tiles[p.X + 1][p.Y - 1].Type == WALL ? false : true); REM_P(p); }}
#define ADD_D  { if(P_AT(p.X, p.Y+1)) { points[p.X][p.Y + 1] = PathPoint(p.X, p.Y + 1, _tiles[p.X][p.Y + 1].Type == WALL ? false : true); REM_P(p); }}
#define ADD_DL  { if(P_AT(p.X-1, p.Y+1)) { points[p.X - 1][p.Y + 1] = PathPoint(p.X - 1, p.Y + 1, _tiles[p.X - 1][p.Y + 1].Type == WALL ? false : true); REM_P(p); }}
#define ADD_R  { if(P_AT(p.X+1, p.Y)) { points[p.X+1][p.Y] = PathPoint(p.X+1, p.Y, _tiles[p.X+1][p.Y].Type == WALL ? false : true); REM_P(p);}}
#define ADD_DR  { if(P_AT(p.X+1, p.Y+1)) { points[p.X + 1][p.Y + 1] = PathPoint(p.X + 1, p.Y + 1, _tiles[p.X + 1][p.Y + 1].Type == WALL ? false : true); REM_P(p); }}


#define NUMBER_OF_CHILDREN_TO_TRY 5
#define DELAY_MS 0
#define INCLUDE_PARENT true


namespace GA
{
  // Constructor that builds this from a file.
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
        while (std::getline(fileObject, buffer))
          for (size_t i = 0; i < buffer.size(); ++i)
          {
            _tiles.Set(parseFileChar(buffer[i]));
            _tiles.IncrementX();
          }
      }
      catch (...)
      {
        throw RUtils::RException("Error during file read! Confirm formatting is correct!");
      }
    }
    else
      throw RUtils::RException("Specified file not found!");

    init();
  }


  // Constructor - note: No defaults for this class!
  Manager::Manager(size_t width, size_t height)
    : _tiles(width, height)
    , _width(width)
    , _height(width)
  {
    RConsole::Canvas::FillCanvas(RConsole::RasterInfo(lookupASCII({}), RConsole::DARKGREY));
    init();
  }


  // Places obstacle in the tiles in code
  void Manager::PlaceRock(size_t x, size_t y, size_t side_size)
  {
    for (size_t i = x; i < x + side_size; ++i)
      for (size_t j = y; j < y + side_size; ++j)
        if (getLocSafe(i, j))
          getLocSafe(i, j)->Type = WALL;
  }




    /////////////////////////////
   // GA Private member funcs //
  /////////////////////////////
  void Manager::init()
  {
    _allPathsEver.push_back(getSquarePath());
    _allPathsEver.push_back(_allPathsEver[0]);
    applyPathData(_allPathsEver[1]);
  }

  void Manager::stepOnce(bool includeParent)
  {
    std::vector<Path> mutated;
    if (includeParent)
      mutated.push_back(_allPathsEver[_allPathsEver.size() - 1]);

    for(int i = 0; i < NUMBER_OF_CHILDREN_TO_TRY; ++i)
      mutated.push_back(mutate(_allPathsEver[_allPathsEver.size() - 1], .1f));

    mutated[std::rand() % mutated.size()].Cost += WALL_COST * 5;

    int best = 0;
    for (size_t i = 0; i < mutated.size(); ++i)
      if (mutated[i].Cost < mutated[best].Cost)
        best = i;

    _allPathsEver.push_back(mutated[best]);
  }

  Path Manager::mutate(const Path &path, float percentage)
  {
    // Make this 2D so we can handle it.
    std::vector<std::vector<PathPoint>> points(_width);
    for (size_t i = 0; i < _width; ++i)
      points[i] = std::vector<PathPoint>(_height);

    for (PathPoint p : path.ControlPoints)
      points[p.X][p.Y] = p;

    for (int i = path.ControlPoints.size() - 1; i >= 0; --i)
    {
      float val = static_cast<float>(std::rand()) / RAND_MAX;
      if (val < percentage)
      {
        int val = std::rand() % 8;
        PathPoint p = path.ControlPoints[i];
        switch (val)
        {
          case 0: ADD_UL break;
          case 1: ADD_U break;
          case 2: ADD_UR break;
          case 3: ADD_DL break;
          case 4: ADD_D break;
          case 5: ADD_DR break;
          case 6: ADD_L break;
          case 7: ADD_R break;
        }
      }
    }

    std::vector<PathPoint> outPts;
    for (size_t i = 0; i < _width; ++i)
    {
      for (size_t j = 0; j < _height; ++j)
        if (points[i][j].X != INT_MAX)
          outPts.push_back(points[i][j]);
    }

    return Path(outPts);
  }

  Path Manager::getSquarePath()
  {
    _tiles.SetIndex(0);

    std::vector<PathPoint> path;
    PathPoint start;
    PathPoint goal;

    while (_tiles.GetIndex() < _tiles.Length())
    {
      if (_tiles.Get().Type == START)
        start = PathPoint(_tiles.GetIndex() % _width, _tiles.GetIndex() / _width, true);
      if (_tiles.Get().Type == GOAL)
        goal = PathPoint(_tiles.GetIndex() % _width, _tiles.GetIndex() / _width, true);

      _tiles.IncrementX();
    }
    bool include = true;
    bool shouldClean = false;
    while (start.Y - goal.Y != 0)
    {
      if (start.Y < goal.Y)
        ++start.Y;
      else
        --start.Y;

      if (_tiles[start.X][start.Y].Type == WALL)
        start.Valid = false;
      else
        start.Valid = true;

      if (include)
        path.push_back(start);
      //include = !include;
      shouldClean = true;
    }

    while (start.X - goal.X != 0)
    {
      if (start.X < goal.X)
        ++start.X;
      else
        --start.X;

      if (_tiles[start.X][start.Y].Type == WALL)
        start.Valid = false;
      else
        start.Valid = true;

      if(include)
        path.push_back(start);
      //include = !include;
      shouldClean = true;
    }

    //if(shouldClean)
    //  path.erase(path.end() - 1);

    return Path(path);
  }

  void Manager::clearPathData()
  {
    _tiles.SetIndex(0);

    while (_tiles.GetIndex() < _tiles.Length())
    {
      _tiles.Get().DisplayNum = 0;
      _tiles.Get().IsPath = false;
      _tiles.IncrementX();
    }
  }

  void Manager::applyPathData(const Path &path)
  {
    _tiles.SetIndex(0);

    clearPathData();
    for (size_t i = 0; i < path.ControlPoints.size(); ++i)
    {
      _tiles[path.ControlPoints[i].X][path.ControlPoints[i].Y].DisplayNum = path.ControlPoints[i].PointNum;
      _tiles[path.ControlPoints[i].X][path.ControlPoints[i].Y].IsPath = true;
    }

    _activePath = &path;
  }

  // Runs the primary application
  bool Manager::Run()
  {
    stepOnce(INCLUDE_PARENT);
    applyPathData(_allPathsEver[_allPathsEver.size() - 1]);
    draw(_tiles);
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
    ++_iterations;
    //std::fstream fileObject("log.txt", std::ios::out | std::ios::trunc);
    //for (size_t i = 0; i < _allPathsEver.size(); ++i)
    //  fileObject << _iterations << ", " << _allPathsEver[i].Cost << "\n";

    //if (_iterations > 2000)
    //  return false;
    return true;
  }


    ///////////////////////////////////////////
   // Private Member Funcs, not strictly GA //
  ///////////////////////////////////////////
  // Draw
  void Manager::draw(RConsole::Field2D<Tile> &toDraw)
  {
    toDraw.SetIndex(0);

    while (toDraw.GetIndex() < toDraw.Length())
    {
      const Tile &data = toDraw.Get();
      RConsole::Canvas::Draw(lookupASCII(data), toDraw.GetIndex() % _width, toDraw.GetIndex() / _width, lookupColor(data));
      toDraw.IncrementX();
    }

    float offset = static_cast<float>(_height);
    RConsole::Canvas::DrawString((std::string("Current Path Points: ") + std::to_string(_activePath->ControlPoints.size())).c_str(), 1.0f, ++offset, RConsole::WHITE);
    RConsole::Canvas::DrawString((std::string("Current Path Cost: ") + std::to_string(_activePath->Cost)).c_str(), 1.0f, ++offset, RConsole::WHITE);
    RConsole::Canvas::DrawString((std::string("Current Path Walls: ") + std::to_string(_activePath->WallCount)).c_str(), 1.0f, ++offset, RConsole::WHITE);
    RConsole::Canvas::DrawString((std::string("Current Path Validity: ") + (_activePath->IsValidPath ? "valid" : "invalid")).c_str(), 1.0f, ++offset, RConsole::WHITE);
    RConsole::Canvas::DrawString((std::string("Iterations: ") + std::to_string(_iterations)).c_str(), 1.0f, ++offset, RConsole::WHITE);
  }

  // Characters in the file that need parsing out.
  Tile Manager::parseFileChar(char c)
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
      tile.Type = GOAL;
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
  unsigned char Manager::lookupASCII(Tile t)
  {
    if (t.IsPath)
    {
      return 'A' + t.DisplayNum;
      //return static_cast<unsigned char>(254);
    }

    switch (t.Type)
    {
    case NONE:
      return ' ';
    case WALL:
      return static_cast<unsigned char>(219);
    case GOAL:
      return '@';
    case START:
      return '#';
    default:
      return '?';
    }
  }

  // Allows color of a tile to be looked up.
  RConsole::Color Manager::lookupColor(Tile t)
  {
    if (t.IsPath)
    {
      if (t.Type == WALL)
        return RConsole::RED;

      return RConsole::LIGHTCYAN;
    }

    switch (t.Type)
    {
    case NONE:
      return RConsole::DARKGREY;
    case WALL:
      return RConsole::GREY;
    case GOAL:
      return RConsole::LIGHTMAGENTA;
    case START:
      return RConsole::LIGHTGREEN;
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