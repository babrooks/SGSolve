// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.

//! Same as guitester.cpp but uses the jyc algorithm
//! @example
#include "sg.hpp"
#include "sgjycsolver.hpp"
#include <ctime>

int main ()
{
  try
    {
      SGGame game;
 
      SGGame::load(game,"./games/rsg_ne=5_c2e=25_d=0.85_p=0.sgm");

      cout << "Building solver" << endl;
      SGJYCSolver solver(game,100);

      std::clock_t start = std::clock();

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
      cout << duration << " seconds elapsed" << endl;
    }
  catch (SGException e)
    {
      cout << "Caught the following exception: " << endl
	   << e.what() << endl;
    }
  return 0;
}
