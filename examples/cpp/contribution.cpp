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

//! One state prisoner's dilemma
//! @example

#include "sg.hpp"
#include "sgsolver_maxminmax_3player.hpp"
#include "sgcontribution.hpp"

int main ()
{
  double delta = 2.0/3.0;
  // delta=0.9;
  // delta=0.5;
  
  int action, state, player;
  int numPlayers = 3;
  int numStates = 2;


  ContributionGame cg(numPlayers,delta,numStates);
  
  try
    {
      cout << "Constructing game object" << endl;
      SGGame game(cg);

      SGEnv env;

      env.setParam(SG::DIRECTIONTOL,1e-12); 
      env.setParam(SG::NORMTOL,1e-12); 
      env.setParam(SG::LEVELTOL,1e-12); 
      env.setParam(SG::IMPROVETOL,1e-13); 
      env.setParam(SG::MAXITERATIONS,100); 

      env.setParam(SG::ICTOL,1e-6);
      
      // cout << "Building solver" << endl;
      // SGSolver_MaxMinMax_3Player solver(env,game);

      // cout << "Starting solve routine" << endl;
      // solver.solve_fixed();
      // cout << "Saving data... ";
      // SGSolution_MaxMinMax soln = solver.getSolution();
      // SGSolution_MaxMinMax::save(soln,"./solutions/contribution_fixed.sln2");
      // cout << "Done with fixed!" << endl;

      cout << "Building solver2" << endl;
      SGSolver_MaxMinMax_3Player solver2(env,game);
      try
	{
	  solver2.solve_endogenous();
	}
      catch(SGException e)
	{
	  cout << "Caught the following exception:" << endl
	       << e.what() << endl;
	}
      SGSolution_MaxMinMax soln2 = solver2.getSolution();
      
      cout << "Saving data... ";
      SGSolution_MaxMinMax::save(soln2,"./solutions/contribution_endog.sln2");
      cout << "Done with endogenous!" << endl;
    }
  catch (SGException e)
    {
      cout << "Caught the following exception:" << endl
	   << e.what() << endl;
    }

  return 0;
}
