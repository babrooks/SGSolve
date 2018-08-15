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
  int numDirections = 100;

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
	  
	  // cout << "Starting trim operation..." << endl;

	  for (int dir = 0; dir < 4; dir ++)
	    {
	      double theta = 2.0*PI*static_cast<double>(dir)/4.0;
	      SGPoint currentDir = SGPoint(cos(theta),sin(theta));

	      actions[state].back().trim(currentDir,
					 max(currentDir*payoffUB,currentDir*payoffLB));
	    } // for dir

	  // cout << "Trimmed points: " << endl
	  //      << actions[state].back().getTrimmedPoints()[0] << endl 
	  //      << actions[state].back().getTrimmedPoints()[1] << endl;
	  // cout << "Finished trimming. Now do update!" << endl;
	  // actions[state].back().updateTrim();
	  // cout << "Points: " << endl
	  //      << actions[state].back().getPoints()[0] << endl 
	  //      << actions[state].back().getPoints()[1] << endl;
	}
    } // for state

  

  // Main loop
  double errorLevel = 1;
  int numIter = 0;
  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      // Reset the trimmed points for the actions
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      ait->updateTrim();
	      // Delete the action if not supportable
	      if (~ait->supportable())
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
	  SGPoint currentDir = directions[dir];
	  

	  // Calculate new threat tuples
	  
	  
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
		  ait->trim(currentDir,expLevel);
		}

	      // Update the errorlevel with the movement in this direction
	      errorLevel = max(errorLevel,abs(newLevels[state]-levels[dir][state]));
	    }

	  levels[dir] = newLevels;
	} // for dir

      numIter++;
    } // while

} // solve


// void SGSolver_V4::optimizePolicy()
// {
//   // Do policy iteration to find a first pivot. 
  
//   // Start with upper bounds on the highest payoff for all states.
//   SGPoint normal (0,1);
//   vector<double> levels(numStates,payoffUB[1]+1.0); 
//   pivot = SGTuple(numStates,SGPoint(payoffLB[0],payoffLB[1]-1.0));

//   double pivotError = 1.0;
//   // Iterate until convergence
//   while (pivotError > env.getParam(SG::UPDATEPIVOTTOL))
//     {
//       SGTuple newPivot = pivot;
      
//       for (int state = 0; state < numStates; state++)
// 	{
// 	  // Iterate over all actions to find the one that generates
// 	  // the lowest payoff for player 1.
	  
// 	  double bestLevel = -numeric_limits<double>::max();
	  
// 	  for (auto ait = actions[state].begin();
// 	       ait != actions[state].end();
// 	       ++ait)
// 	    {
// 	      // Procedure to find an improvement to the policy
// 	      // function
	      
// 	      SGPoint nonBindingPayoff = (1-delta)*game.getPayoffs()[state]
// 		[ait->getAction()]
// 		+ delta * pivot.expectation(game.getProbabilities()[state][ait->getAction()]);

// 	      bool bestAPSNotBinding = false;
// 	      SGPoint bestAPSPayoff;
// 	      // Make this code valid for an arbitrary direction

// 	      // Find which payoff is highest in current normal and
// 	      // break ties in favor of the clockwise 90 degree.
// 	      bestBindingPlayer = 0;
// 	      bestBindingPoint = 0;
// 	      for (int p = 0; p < numPlayers; p++)
// 		{
// 		  for (int k = 0; k < ait->getPoints()[p].size(); k++)
// 		    {
// 		      double tmpLvl = ait->getPoints()[p][k]*currDir;
// 		      if (tmpLvl > bestBindLvl)
// 			{
// 			  bestBindingPlayer = p;
// 			  bestBindingPayoff = k;
// 			}
// 		    } // point
// 		} // player
// 	      if (ait-getBndryDirs()[p][k]*currDir > 0)
// 		bestAPSNotBinding = true;

// 	      if (bestAPSNotBinding
// 		  || bestAPSPayoff*currDir > nonBindingPayoff*currDir)
// 		{
// 		  if (nonBindingPayoff*currDir > bestLevel)
// 		    {
// 	      	      bestLevel = nonBindingPayoff*currDir;
// 	      	      regimeTuple[state] = SG::NonBinding;
// 	      	      newPivot[state] = nonBindingPayoff;
// 		    }
// 		}
// 	      else
// 		{
// 		  if (bestAPSPayoff*currDir > bestLevel)
// 		    {
// 	      	      bestLevel = bestAPSPayoff*currDir;
// 	      	      regimeTuple[state] = SG::Binding;
// 	      	      newPivot[state] = bestAPSPayoff;
// 		    }
// 		}
// 	    } // ait
// 	} // state

//       pivotError = SGTuple::distance(newPivot,pivot);

//       // Now do Bellman iteration to find new fixed point
//       int updatePivotPasses = 0;
//       do
// 	{
// 	  pivot = newPivot;
// 	  for (int state = 0; state < numStates; state++)
// 	    {
// 	      if (regimeTuple[state] == SG::NonBinding)
// 		newPivot[state] = (1-delta)*game.getPayoffs()[state]
// 		  [actionTuple[state]->getAction()]
// 		  + delta*pivot.expectation(game.getProbabilities()[state]
// 					    [actionTuple[state]->getAction()]);
// 	    }
// 	} while (SGTuple::distance(newPivot,pivot) > env.getParam(SG::UPDATEPIVOTTOL)
// 		 && (++updatePivotPasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));

//     } // policy iteration
  
// } // optimizePolicy
