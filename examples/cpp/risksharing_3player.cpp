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
#include "sgrisksharing_3player.hpp"
#include <ctime>

int main ()
{
  double delta = 2.0/3.0;
  // delta=0.8;
  // delta=1.0/3.0;
  // delta=0.25;
  delta=0.6;
  
  int action, state, player;

  int numEndowments = 3;
  int c2e = 3;

  int numDirections = 300;

  bool dropRedundant = false;
  bool addEndogenous = false;
  
  RiskSharingGame_3Player rg(delta,numEndowments,c2e);

  stringstream ss;

  ss << "./solutions/risksharing_3player_endog_e=" << numEndowments
     << "_d=" << setprecision(3) << delta
     << "_c2e=" << c2e
     << "_nd=" << numDirections;
  if (dropRedundant)
    ss << "_dropredund";
  if (addEndogenous)
    ss << "_addendog";
  ss << ".sln2";
  
  try
    {
      cout << "Constructing game object" << endl;
      SGGame game(rg);

      SGEnv env;

       env.setParam(SG::DIRECTIONTOL,1e-12); 
      env.setParam(SG::NORMTOL,1e-12); 
      env.setParam(SG::LEVELTOL,1e-12); 
      env.setParam(SG::IMPROVETOL,1e-13); 
      //env.setParam(SG::ERRORTOL,1e-10); 
      env.setParam(SG::ICTOL,1e-13); // Seems to work well on the
				     // delta=2/3 and delta=1/3
				     // contribution games
      env.setParam(SG::MAXITERATIONS,60);
      env.setParam(SG::STOREITERATIONS,2);
      
      clock_t start;
      double duration;

      start = clock();
      SGSolver_MaxMinMax_3Player solver(env,game);
      cout << "Starting solve routine" << endl;
      try
      	{
	  solver.solve_fixed(numDirections,dropRedundant,addEndogenous);
	}
      catch(SGException e)
      	{
      	  cout << "Caught the following exception:" << endl
      	       << e.what() << endl;
      	}
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << "Fixed dir solver time elapsed: "<< duration << " seconds" << endl;
      SGSolution_MaxMinMax soln = solver.getSolution();
      SGSolution_MaxMinMax::save(soln,ss.str().c_str());

    }
  catch (SGException e)
    {
      cout << "Caught the following exception:" << endl
	   << e.what() << endl;
    }

  return 0;
}
