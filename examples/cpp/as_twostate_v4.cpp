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

//! Two state version of the example from Abreu and Sannikov.
//! @example
#include "sg.hpp"
#include "sgsimulator.hpp"
#include "sgsolver_v4.hpp"

int main ()
{
  double delta = 0.7;
  delta = 0.45;
  
  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numStates = 2;

  vector<bool> unconstrained(2,false);

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,3));
  vector<int> numActions_total(numStates,9);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(9,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 16;
  payoffs[0][1][0] = 21;
  payoffs[0][2][0] = 9;
  payoffs[0][3][0] = 3;
  payoffs[0][4][0] = 10;
  payoffs[0][5][0] = 5;
  payoffs[0][6][0] = 1;
  payoffs[0][7][0] = -1;
  payoffs[0][8][0] = -5;
  payoffs[0][0][1] = 9;
  payoffs[0][1][1] = 1;
  payoffs[0][2][1] = 0;
  payoffs[0][3][1] = 13;
  payoffs[0][4][1] = 4;
  payoffs[0][5][1] = -4;
  payoffs[0][6][1] = 3;
  payoffs[0][7][1] = 0;
  payoffs[0][8][1] = -10;


  double offset = 5;
  offset = 0;
  for (action=0; action<9; action++)
    {
      payoffs[1][action][0] = payoffs[0][action][0]-offset;
      payoffs[0][action][0] += offset;
      payoffs[1][action][1] = payoffs[0][action][1]+offset;
      payoffs[0][action][1] -= offset;
    } // for action

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(9,vector<double>(numStates,0.0)));
  double persistence = 0.3;
  for (state=0; state<numStates; state++)
    {
      for (action=0; action<9; action++)
	{
	  probabilities[state][action][state] = persistence;
	  probabilities[state][action][1-state] = 1-persistence;
	} // for action
    } // for state

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
      env.setParam(SG::STOREITERATIONS,1);
      env.setParam(SG::MAXITERATIONS,10);
      env.setParam(SG::LEVELTOL,1e-12);
  
      cout << "Building solver" << endl;

      SGSolver_V4 solver(env,game);

      solver.solve();
      
    }
  catch (SGException e)
    {
      cout << "Caught the following exception:" << endl
	   << e.what() << endl;
    }
  

  return 0;
}