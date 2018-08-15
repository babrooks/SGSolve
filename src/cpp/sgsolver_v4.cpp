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

#include "sgsolver_v4.hpp"

SGSolver_V4::SGSolver_V4(const SGEnv & _env,
			 const SGGame & _game):
  env(_env),
  game(_game),
  soln(_game),
  numPlayers(_game.getNumPlayers()),
  numStates(_game.getNumStates()),
  delta(_game.getDelta()),
  payoffs(_game.getPayoffs()),
  eqActions(_game.getEquilibriumActions()),
  probabilities(_game.getProbabilities()),
  numActions(_game.getNumActions()),
  numActions_totalByState(_game.getNumActions_total())
{

}

void SGSolver_V4::solve()
{
  // Initialize directions
  int numDirections = 100; // make an even multiple of 4

  assert(100%4==0);

  vector<SGPoint> directions(numDirections);
  vector< vector<double> > levels(numDirections,
				  vector<double>(numStates,0));


  SGPoint payoffLB, payoffUB;
  game.getPayoffBounds(payoffUB,payoffLB);
  cout << "Payoff bounds: " << endl
       << payoffLB << endl << payoffUB << endl;

  SGTuple threatTuple(numStates,payoffLB);

  // Initialize directions
  for (int dir = 0; dir < numDirections; dir++)
    {
      double theta = 2.0*PI
	*static_cast<double>(dir)/static_cast<double>(numDirections);
      directions[dir] = SGPoint(cos(theta),sin(theta));
    } // for dir

  // Initialize actions with a big box as the feasible set
  vector< list<SGAction_V2> > actions(numStates);
  vector<bool> updateICFlags (2,true);
  for (int state = 0; state < numStates; state++)
    {
      for (int action = 0; action < numActions_totalByState[state]; action++)
	{
	  actions[state].push_back(SGAction_V2(env,state,action));
	  actions[state].back().calculateMinIC(game,updateICFlags,threatTuple);
	  
	  for (int dir = 0; dir < 4; dir ++)
	    {
	      double theta = 2.0*PI*static_cast<double>(dir)/4.0;
	      SGPoint currDir = SGPoint(cos(theta),sin(theta));

	      actions[state].back().trim(currDir,
					 max(currDir*payoffUB,currDir*payoffLB));
	    } // for dir

	}
    } // for state

  

  // Main loop
  double errorLevel = 1;
  int numIter = 0;

  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      SGTuple pivot(numStates);
      vector<list<SGAction_V2>::const_iterator> actionTuple(numStates);
      // Pick the initial actions arbitrarily
      for (int state = 0; state < numStates; state++)
	actionTuple[state] = actions[state].begin();
      
      vector<SG::Regime> regimeTuple(numStates,SG::Binding);

      // Reset the trimmed points for the actions
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      ait->updateTrim();
	      // Delete the action if not supportable
	      if (!(ait->supportable()))
		{
		  actions[state].erase(ait++);
		  continue;
		}
	      ait->resetTrimmedPoints();

	      
	    } // for ait
	} // for state
      
      // Reset the error level
      errorLevel = 0;
      
      // Iterate through directions
      for (int dir = 0; dir < directions.size(); dir++)
	{
	  // Compute optimal level in this direction
	  vector<double> newLevels(numStates,0);
	  SGPoint currDir = directions[dir];

	  cout << "Optimizing policy..." << endl;
	  optimizePolicy(pivot,actionTuple,regimeTuple,currDir,actions);
	  for (int state = 0; state < numStates; state++)
	    {
	      // Update the levels and the error level with the
	      // movement in this direction
	      newLevels[state] = pivot[state]*currDir;
	      errorLevel = max(errorLevel,abs(newLevels[state]-levels[dir][state]));
	    } // for state
	  
	  // Trim the actions
	  for (int state = 0; state < numStates; state++)
	    {
	      for (auto ait = actions[state].begin();
		   ait != actions[state].end();
		   ++ait)
		{
		  // Compute the expected level
		  double expLevel = 0;
		  for (int sp = 0; sp < numStates; sp++)
		    expLevel += probabilities[state][ait->getAction()][sp]
		      * newLevels[state];

		  // Trim the action
		  ait->trim(currDir,expLevel);
		} // for ait
	    } // for state

	  levels[dir] = newLevels;
	} // for dir

      for (int state = 0; state < numStates; state++)
	{
	  threatTuple[state][0] = levels[numDirections/2][state];
	  threatTuple[state][1] = levels[numDirections/4][state];
	}

      cout << "Iter: " << numIter
	   << ", errorLvl: " << errorLevel
	   << endl;
      
      numIter++;
    } // while

} // solve


void SGSolver_V4::optimizePolicy(SGTuple & pivot,
				 vector<list<SGAction_V2>::const_iterator> & actionTuple,
				 vector<SG::Regime> & regimeTuple,
				 const SGPoint currDir,
				 const vector<list<SGAction_V2> > & actions)
{
  // Do policy iteration to find the optimal pivot.
  
  double pivotError = 1.0;
  // Iterate until convergence
  while (pivotError > env.getParam(SG::UPDATEPIVOTTOL))
    {
      // Now do Bellman iteration to find new fixed point
      SGTuple newPivot(numStates);
      int updatePivotPasses = 0;
      double bellmanPivotGap = 0;
      do
	{
	  for (int state = 0; state < numStates; state++)
	    {
	      if (regimeTuple[state] == SG::NonBinding)
		newPivot[state] = (1-delta)*payoffs[state]
		  [actionTuple[state]->getAction()]
		  + delta*pivot.expectation(probabilities[state]
					    [actionTuple[state]->getAction()]);
	    }
	  bellmanPivotGap = SGTuple::distance(newPivot,pivot);
	  pivot = newPivot;
	  
	} while (bellmanPivotGap > env.getParam(SG::UPDATEPIVOTTOL)
		 && (++updatePivotPasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));

      for (int state = 0; state < numStates; state++)
	{
	  // Iterate over all actions to find the one that generates
	  // the lowest payoff for player 1.
	  
	  double bestLevel = -numeric_limits<double>::max();
	  
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      // Procedure to find an improvement to the policy
	      // function
	      
	      SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
		[ait->getAction()]
		+ delta * pivot.expectation(probabilities[state][ait->getAction()]);

	      bool bestAPSNotBinding = false;
	      SGPoint bestAPSPayoff;
	      // Make this code valid for an arbitrary direction

	      // Find which payoff is highest in current normal and
	      // break ties in favor of the clockwise 90 degree.
	      int bestBindingPlayer = -1;
	      int bestBindingPoint = -1;
	      double bestBindLvl = -numeric_limits<double>::max();
	      for (int p = 0; p < numPlayers; p++)
		{
		  for (int k = 0; k < ait->getPoints()[p].size(); k++)
		    {
		      double tmpLvl = ait->getPoints()[p][k]*currDir;
		      if (tmpLvl > bestBindLvl)
			{
			  bestBindLvl = tmpLvl;
			  bestBindingPlayer = p;
			  bestBindingPoint = k;
			}
		    } // point
		} // player
	      assert(bestBindingPlayer >= 0);
	      assert(bestBindingPoint >= 0);
	      bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		+ delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];

	      if (ait->getBndryDirs()[bestBindingPlayer][bestBindingPoint]*currDir > 0)
		bestAPSNotBinding = true;

	      if (bestAPSNotBinding
		  || bestAPSPayoff*currDir > nonBindingPayoff*currDir)
		{
		  if (nonBindingPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = nonBindingPayoff*currDir;
	      	      regimeTuple[state] = SG::NonBinding;
	      	      newPivot[state] = nonBindingPayoff;
		    }
		}
	      else
		{
		  if (bestAPSPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = bestAPSPayoff*currDir;
	      	      regimeTuple[state] = SG::Binding;
	      	      newPivot[state] = bestAPSPayoff;
		    }
		}
	    } // ait
	} // state

      pivotError = SGTuple::distance(newPivot,pivot);

    } // policy iteration

} // optimizePolicy
