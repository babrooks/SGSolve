// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2019 Benjamin A. Brooks
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
#include "sgsolver_jyc.hpp"
#include "sgcontribution.hpp"
#include <ctime>

int main ()
{
  double delta = 2.0/3.0;
  // delta=0.8;
  // delta=1.0/3.0;
  // delta=0.25;
    
  int action, state, player;
  int numPlayers = 3;
  int numStates = 2;
  
  ContributionGame cg(numPlayers,delta,numStates);
  
  try
    {
      cout << "Constructing game object" << endl;
      SGGame game(cg);

      SGEnv env;

      // env.setParam(SG::DIRECTIONTOL,1e-12); 
      // env.setParam(SG::NORMTOL,1e-12); 
      // env.setParam(SG::LEVELTOL,1e-12); 
      // env.setParam(SG::IMPROVETOL,1e-13); 
      //env.setParam(SG::ERRORTOL,1e-10); 
      env.setParam(SG::ICTOL,1e-10); // Seems to work well on the
				     // delta=2/3 and delta=1/3
				     // contribution games
      env.setParam(SG::MAXITERATIONS,100);
      env.setParam(SG::STOREITERATIONS,2);

      
      clock_t start;
      double duration;

      start = clock();
      SGSolver_MaxMinMax_3Player solver(env,game);
      cout << "Starting solve routine" << endl;
      try
      	{
	  solver.solve_fixed();
	}
      catch(SGException e)
      	{
      	  cout << "Caught the following exception:" << endl
      	       << e.what() << endl;
      	}
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << "Fixed dir solver time elapsed: "<< duration << " seconds" << endl;
      SGSolution_MaxMinMax soln = solver.getSolution();
      SGSolution_MaxMinMax::save(soln,"./solutions/contribution_fixed.sln2");
      
      SGGame::save(game,"./games/contribution.sgm");

      start = clock();
      SGSolver_MaxMinMax_3Player solver2(env,game);
      cout << "Starting solve routine" << endl;
      try
      	{
      	  solver2.solve_fixed(200,false,false);
      	}
      catch(SGException e)
      	{
      	  cout << "Caught the following exception:" << endl
      	       << e.what() << endl;
      	}
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << "Fixed dir solver time elapsed: "<< duration << " seconds" << endl;
      
      start=clock();
      SGSolver_JYC solver_jyc(game,200);
      solver_jyc.solve();
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << "JYC solver time elapsed: "<< duration << " seconds" << endl;
      
    }
  catch (SGException e)
    {
      cout << "Caught the following exception:" << endl
	   << e.what() << endl;
    }

  return 0;
}
