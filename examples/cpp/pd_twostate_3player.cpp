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

class ContributionGame : public SGAbstractGame
{
public:
  ContributionGame(int _numPlayers,
		   double _delta,
		   int _numStates):
    SGAbstractGame(_numPlayers,
		   _delta,
		   _numStates,
		   vector< vector<int> >(_numStates,vector<int>(_numPlayers,2)))
  {}

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    SGPoint point(numPlayers,0);

    double actionSum = 0.0;
    for (int p=0; p < numPlayers; p++)
      actionSum += actions[p];

    // Payoff is sum a_j - 2 a_i + state.
    for (int p=0; p < numPlayers; p++)
      point[p] = 1.0*actionSum - 2.0*actions[p]
	+ static_cast<double>(state)/(static_cast<double>(numStates)-1.0);
    
    return point;
  } // payoffs

  virtual double probability(int state, const vector<int> & actions, int statep) const
  {
    double prob = 0;

    double actionSum = 0.0;
    for (int p=0; p < numPlayers; p++)
      actionSum += actions[p];

    if (numStates==1)
      prob=1.0;
    else if (actionSum == numPlayers) // everyone contributing makes
				      // the highest state more likely.
      {
	if (statep == numStates-1)
	  prob = 2.0/3.0;
	else
	  prob = 1.0/3.0/(static_Cast<double>(numStates-1));
      }
    else
      prob = 1.0/static_cast<double>(numStates);
    
    return prob;
  } // probability
  
  
}; // PD_TwoState_3Player

int main ()
{
  double delta = 2.0/3.0;
  
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

      cout << "Building solver" << endl;
      SGSolver_V4 solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve_endogenous();

      SGSolution_V2 soln = solver.getSolution();
      
      cout << "Saving data... ";
      SGSolution_V2::save(soln,"sgtest.sln2");
      cout << "Done!" << endl;

      SGGame::save(game,"pd.sgm");

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
