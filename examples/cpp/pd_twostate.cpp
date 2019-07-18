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

int main ()
{
  double delta = 2.0/3.0;
  
  int action, state, player;
  int numPlayers = 2;
  int numStates = 2;

  vector<bool> unconstrained(2,false);

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,2));
  vector<int> numActions_total(numStates,4);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(4,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 0;
  payoffs[0][1][0] = -1;
  payoffs[0][2][0] = 2;
  payoffs[0][3][0] = 1;
  payoffs[0][0][1] = 0;
  payoffs[0][1][1] = 2;
  payoffs[0][2][1] = -1;
  payoffs[0][3][1] = 1;

  payoffs[1] = payoffs[0];
  payoffs[1][0][0] += 2;
  payoffs[1][1][0] += 2;
  payoffs[1][2][0] += 2;
  payoffs[1][3][0] += 2;
  payoffs[1][0][1] += 2;
  payoffs[1][1][1] += 2;
  payoffs[1][2][1] += 2;
  payoffs[1][3][1] += 2;
       
  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(4,vector<double>(numStates,0.5)));

  probabilities[0][0][0] = 1.0/3.0; // Less likely to stay in same state if play same actions
  probabilities[0][0][1] = 2.0/3.0;
  probabilities[0][3][0] = 1.0/3.0; 
  probabilities[0][3][1] = 2.0/3.0;
  probabilities[1][0][0] = 2.0/3.0; 
  probabilities[1][0][1] = 1.0/3.0;
  probabilities[1][3][0] = 2.0/3.0; 
  probabilities[1][3][1] = 1.0/3.0;

  try
    {
      cout << "Constructing game object" << endl;
      SGGame game(delta,
		  numStates,
		  numActions,
		  payoffs,
		  probabilities,
		  unconstrained);

      SGEnv env;

      env.setParam(SG::DIRECTIONTOL,1e-12); 
      env.setParam(SG::NORMTOL,1e-12); 
      env.setParam(SG::LEVELTOL,1e-12); 
      env.setParam(SG::IMPROVETOL,1e-13); 

      cout << "Building solver" << endl;
      SGSolver_MaxMinMax solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve();

      SGSolution_MaxMinMax soln = solver.getSolution();
      
      cout << "Saving data... ";
      SGSolution_MaxMinMax::save(soln,"./solutions/pd_twostate.sln2");
      cout << "Done!" << endl;

      SGGame::save(game,"./games/pd_twostate.sgm");

      ofstream ofs ("pd_twostate.log");
      
      for (auto iterit = soln.getIterations().cbegin();
	   iterit != soln.getIterations().cend();
	   iterit++)
	{
	  for (int state = 0; state < numStates; state++)
	    {
	      for (int player = 0; player < numPlayers; player++)
		{
		  for (auto stepit = iterit->getSteps().cbegin();
		       stepit != iterit->getSteps().cend();
		       stepit++)
		    {
		      ofs << stepit->getPivot()[state][player] << " ";
		    }
		  ofs << endl;
		} // for player
	    } // for state
	} // for iter
      ofs.close();

    }
  catch (SGException e)
    {
      cout << "Caught the following exception:" << endl
	   << e.what() << endl;
    }

  return 0;
}
