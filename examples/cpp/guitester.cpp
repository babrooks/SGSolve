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
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

//! Utility for loading an external game and then solving it
//! @example

#include "sg.hpp"
#include "sgsimulator.hpp"

int main ()
{
  SGGame game;

  try
    {
      SGGame::load(game,"./games/rsg_ne=5_c2e=25_d=0.85_p=0.sgm");

      SGEnv env;
      env.setParam(SG::PRINTTOCOUT,true);
      env.setParam(SG::IMPROVETOL,1e-13);
  
      cout << "Building solver" << endl;
      SGSolver solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      cout << "Saving data... ";
      SGSolution::save(solver.getSolution(),"sgtest.sln");
      cout << "Done!" << endl;
      
      SGSolution soln;
      SGGame::load(game,"./solutions/rsg_ne=5_c2e=25_d=0.85_p=0.sln");

      SGSimulator sim(soln);
      
      sim.initialize();

      sim.simulate(100,100,0,soln.getIterations().back().getIteration());
      
    }
  catch (SGException e)
    {
      cout << "I caught an exception." << endl;
      cout << e.what() << endl;
    }


  return 0;
}
