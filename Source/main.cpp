/*!***************************************************************************
@file    main.cpp
@author  Matthew Cech
@par     cech.m@digipen.edu
@date    12/13/2017

@brief 
GA experiments, and more!
*****************************************************************************/
#include <iostream>               // std::cout
#include <RUtils/RTimekeeper.hpp>  // Rutils::RException
#include <cstdlib>
#include <ctime>

// Input and drawing utilities.
#include "console-input.h"
#include "console-utils.hpp"

// Currently defined classes
#include "Tile.hpp"
#include "Manager.hpp"



// Application entry point
int main(int argc, char** argv)
{
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  // Verify number of args
  if (argc != 2)
  {
    std::cout << "Incorrect number of arguments! Format is: <appname> <file input>\n\n";
    return -1;
  }

  // Variables
  GA::Manager manager(argv[1]);

  // Setup
  RConsole::Canvas::SetCursorVisible(true);

  // Primary loop
  while (manager.Run()) 
  {
    RConsole::Canvas::Update();
  }
  return 0;

  /*
  // Timing start
  RUtils::Timekeeper t;
  t.StartFrame();   
  
  // <Some code that does things>
  for (int i{ 0 }; i < 10; ++i)
    std::cout << "I'm printed line " << i + 1 << "!" << '\n';

  // Timing end
  t.EndFrame(); 
  std::cout << "Printing all that took: " << t.GetLastTimeMS() << "ms\n";

  // Normal termination
  return 0;
  */
}
