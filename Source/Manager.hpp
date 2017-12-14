#pragma once
#include "console-utils.hpp"
#include "Tile.hpp"
#include <string>
#include <vector>

#define NONE_COST 0
#define WALL_COST 15

namespace GA
{
  class Path
  {
  public:
    Path(const Path &rhs)
      : ControlPoints(rhs.ControlPoints)
      , IsValidPath(rhs.IsValidPath)
      , Cost(rhs.Cost)
      , WallCount(rhs.WallCount)
    {  }

    Path(const std::vector<PathPoint> path) 
      : ControlPoints(path)
      , IsValidPath(true)
      , Cost(0) 
      , WallCount(0)
    { ReCalculate(); }

    void ReCalculate()
    {
      IsValidPath = true;
      Cost = 0;
      WallCount = 0;

      PathPoint prev = ControlPoints[0];
      for (size_t i = 0; i < ControlPoints.size(); ++i)
      {
        ControlPoints[i].PointNum = i;
        PathPoint cur = ControlPoints[i];
        if (ControlPoints[i].Valid == false)
        {
          Cost += WALL_COST;
          WallCount += 1;
          IsValidPath = false;
        }
        else
        {
          float xDif = static_cast<float>(cur.X) - prev.X;
          float yDif = static_cast<float>(cur.Y) - prev.Y;
          Cost += NONE_COST + std::sqrt((xDif * xDif) + (yDif * yDif)) * 2;
        }

        prev = ControlPoints[i];
      }
    }

    std::vector<PathPoint> ControlPoints;
    bool IsValidPath = false;
    float Cost = static_cast<float>(INT_MAX);
    size_t WallCount = 0;
  };

  class Manager
  {
  public:
    Manager(size_t width, size_t height);
    Manager(std::string file);
    bool Run();
    void PlaceRock(size_t x, size_t y, size_t side_size);

  private:
    // GA-related
    void init();
    void stepOnce(bool includeParent);
    Path mutate(const Path &path, float percentage = 0.1f);
    Path getSquarePath();
    void clearPathData();
    void applyPathData(const Path &points);

    // Private Functions
    void Manager::draw(RConsole::Field2D<Tile> &toDraw);
    unsigned char lookupASCII(Tile t);
    Tile parseFileChar(char c);
    RConsole::Color lookupColor(Tile t);
    Tile *getLocSafe(size_t x, size_t y);

    // Private Variables
    RConsole::Field2D<Tile> _tiles;
    const Path *_activePath;
    std::vector<Path> _allPathsEver;
    size_t _width;
    size_t _height;
    size_t _iterations = 0;
  };
}
