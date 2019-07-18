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

//! Example from Abreu-Sannikov. Game with only one state.

#include "sg.hpp"

int main ()
{
  double delta = 0.3;
  
  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numStates = 1;

  vector< vector< int > > numActions(1,vector<int>(numPlayers,3));
  vector<int> numActions_total(1,9);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(1,vector< vector<double> >(9,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 18;
  payoffs[0][1][0] = 23;
  payoffs[0][2][0] = 11;
  payoffs[0][3][0] = 5;
  payoffs[0][4][0] = 12;
  payoffs[0][5][0] = 7;
  payoffs[0][6][0] = 2;
  payoffs[0][7][0] = 1;
  payoffs[0][8][0] = -3;
  payoffs[0][0][1] = 11;
  payoffs[0][1][1] = 3;
  payoffs[0][2][1] = 2;
  payoffs[0][3][1] = 15;
  payoffs[0][4][1] = 6;
  payoffs[0][5][1] = -2;
  payoffs[0][6][1] = 5;
  payoffs[0][7][1] = 2;
  payoffs[0][8][1] = -13;
  
  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(1,vector< vector<double> >(9,vector<double>(1,1.0)));

  // Equilibrium Actions
  list<int> actionsPerState;
  for (action = 0; action<9; action++)
    actionsPerState.push_back(action);

  try
    {  cout << "Constructing game object" << endl;
      SGGame game(delta,
		  numStates,
		  numActions,
		  payoffs,
		  probabilities);

      cout << "Building solver" << endl;
      SGEnv env;
      SGSolver solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      cout << "Saving data... ";
      SGSolution::save(solver.getSolution(),"./solutions/abreusanikov.sln");
      cout << "Done!" << endl;

      SGGame::save(game,"./games/abreusanikov.sgm");
    }
  catch (SGException e)
    {
      cout << "Caught the following exception: " << endl
	   << e.what() << endl;
    }

  return 0;
}

