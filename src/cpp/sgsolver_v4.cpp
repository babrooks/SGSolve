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
  // Open a log file
  ofstream logofs("sgsolver_v4_test.log");
  logofs << scientific;
  
  // Initialize directions
  int numDirections = 200;

  // make an even multiple of 4
  numDirections += (4-numDirections%4);

  vector<SGPoint> directions(numDirections);
  vector< vector<double> > levels(numDirections,
				  vector<double>(numStates,0));
  vector<SGTuple> pivots(numDirections);

  SGPoint payoffLB, payoffUB;
  game.getPayoffBounds(payoffUB,payoffLB);

  SGTuple threatTuple(numStates,payoffLB);

  // Clear the solution
  soln.clear();
  
  // Initialize directions
  for (int dir = 0; dir < numDirections; dir++)
    {
      double theta = 2.0*PI
	*static_cast<double>(dir)/static_cast<double>(numDirections);
      directions[dir] = SGPoint(cos(theta),sin(theta));
    } // for dir

  // Print directions to the log
  // Initialize directions
  for (int dir = 0; dir < numDirections; dir++)
    logofs << directions[dir][0] << " ";
  logofs << endl;
  for (int dir = 0; dir < numDirections; dir++)
    logofs << directions[dir][1] << " ";
  logofs << endl;
  
  // Initialize actions with a big box as the feasible set
  vector< list<SGAction_V2> > actions(numStates);
  vector<bool> updateICFlags (2,true);
  cout << "Initial threatTuple: " << threatTuple << endl;
  
  for (int state = 0; state < numStates; state++)
    {
      for (int action = 0; action < numActions_totalByState[state]; action++)
	{
	  actions[state].push_back(SGAction_V2(env,state,action));
	  actions[state].back().calculateMinIC(game,updateICFlags,threatTuple);
	  actions[state].back().resetTrimmedPoints();
	  
	  for (int dir = 0; dir < 4; dir ++)
	    {
	      double theta = 2.0*PI*static_cast<double>(dir)/4.0;
	      SGPoint currDir = SGPoint(cos(theta),sin(theta));

	      // double level = max(currDir[0]*payoffLB[0],currDir[0]*payoffUB[0])
	      // 	+max(currDir[1]*payoffLB[1],currDir[1]*payoffUB[1]);

	      double level = max(currDir*payoffLB,currDir*payoffUB);
	      actions[state].back().trim(currDir,level);
	    } // for dir

	  actions[state].back().updateTrim();
	  
	  // cout << "(state,action)=(" << state << "," << action << "), "
	  //      << "numPoints = (" << actions[state].back().getPoints()[0].size()
	  //      << "," << actions[state].back().getPoints()[1].size() << ")"
	  //      << ", minIC: " << actions[state].back().getMinICPayoffs() << endl;
	}
    } // for state

  // Main loop
  double errorLevel = 1;
  int numIter = 0;

  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      vector<SGActionIter> actionTuple(numStates);
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
	      // cout << "(state,action)=(" << state << "," << ait->getAction() << "), "
	      // 	   << "numPoints = (" << ait->getPoints()[0].size()
	      // 	   << "," << ait->getPoints()[1].size() << ")"
	      // 	   << ", minIC: " << ait->getMinICPayoffs()
	      // 	   << ", trimmed points[0]: " << ait->getTrimmedPoints()[0]
	      // 	   // << ", points[0]: " << ait->getPoints()[0]
	      // 	   << endl;

	      // WARNING: NO GUARANTEE THAT THIS POINT IS FEASIBLE
	      if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							       [ait->getAction()]))))
		{
		  actions[state].erase(ait++);
		  continue;
		}
	      
	    } // for ait
	} // for state
      
      SGIteration_V2 iter (numIter,actions,threatTuple);
      
      // Reset the error level
      errorLevel = 0;
      
      // Iterate through directions
      for (int dir = 0; dir < directions.size(); dir++)
	{
	  // Compute optimal level in this direction
	  vector<double> newLevels(numStates,0);
	  SGPoint currDir = directions[dir];

	  optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
			 actions,feasibleTuple);

	  for (int state = 0; state < numStates; state++)
	    {
	      // Update the levels and the error level with the
	      // movement in this direction
	      newLevels[state] = pivot[state]*currDir;
	      pivots[dir] = pivot;
	      errorLevel = max(errorLevel,abs(newLevels[state]-levels[dir][state]));
	    } // for state

	  iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
				SGHyperplane(currDir,newLevels),actions));
	  
	  levels[dir] = newLevels;
	} // for dir


      cout << "Iter: " << numIter
	   << ", errorLvl: " << scientific << errorLevel
	   << ", remaining actions: ( ";
      for (int state = 0; state < numStates; state++)
	cout << actions[state].size() << " ";
      cout << ")" << endl;
      
      soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

      findFeasibleTuple(feasibleTuple,actions);
      
      // Update the the threat tuple
      for (int state = 0; state < numStates; state++)
	{
	  threatTuple[state][0] = -levels[numDirections/2][state];
	  threatTuple[state][1] = -levels[3*numDirections/4][state];
	}
      // cout << "New threat tuple: " << threatTuple << endl;

      // Recalculate minimum IC continuation payoffs
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      ait->calculateMinIC(game,updateICFlags,threatTuple);
	      
	      for (int dir = 0; dir < directions.size(); dir++)
		{
		  // Compute the expected level
		  double expLevel = 0;
		  for (int sp = 0; sp < numStates; sp++)
		    expLevel += probabilities[state][ait->getAction()][sp]
		      * levels[dir][sp];
		  
		  // Trim the action
		  ait->trim(directions[dir],expLevel);
		} // for dir
	      // Trim the actions
	    } // for ait
	  
	} // for state

      // Print new levels to the log file
      for (int state = 0; state < numStates; state++)
	{
	  for (int dir = 0; dir < numDirections; dir++)
	    logofs << levels[dir][state] << " ";
	  logofs << endl;
	  for (int dir = 0; dir < numDirections; dir++)
	    logofs << pivots[dir][state][0] << " ";
	  logofs << endl;
	  for (int dir = 0; dir < numDirections; dir++)
	    logofs << pivots[dir][state][1] << " ";
	  logofs << endl;
	}

      numIter++;
    } // while

  cout << "Converged!" << endl;

  logofs.close();
  
} // solve

void SGSolver_V4::solve_endogenous()
{
  list<SGPoint> directions;
  list< vector<double> > levels;

  SGPoint payoffLB, payoffUB;
  game.getPayoffBounds(payoffUB,payoffLB);

  SGTuple threatTuple(numStates,payoffLB),
    newThreatTuple; // newThreatTuple holds the calculation of the
		    // next threat point

  // Clear the solution
  soln.clear();
  
  // Initialize actions with a big box as the feasible set
  vector< list<SGAction_V2> > actions(numStates);
  vector<bool> updateICFlags (2,true);
  
  for (int state = 0; state < numStates; state++)
    {
      for (int action = 0; action < numActions_totalByState[state]; action++)
	{
	  actions[state].push_back(SGAction_V2(env,state,action));
	  actions[state].back().calculateMinIC(game,updateICFlags,threatTuple);
	  actions[state].back().resetTrimmedPoints();
	  
	  for (int dir = 0; dir < 4; dir ++)
	    {
	      double theta = 2.0*PI*static_cast<double>(dir)/4.0;
	      SGPoint currDir = SGPoint(cos(theta),sin(theta));

	      double level = max(currDir*payoffLB,currDir*payoffUB);
	      actions[state].back().trim(currDir,level);
	    } // for dir

	  actions[state].back().updateTrim();
	  
	}
    } // for state

  // Main loop
  double errorLevel = 1;
  int numIter = 0;

  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      // Clear the directions and levels
      directions.clear();
      levels.clear();
      
      vector<SGActionIter> actionTuple(numStates);
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
	      // WARNING: NO GUARANTEE THAT THIS POINT IS FEASIBLE
	      if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							       [ait->getAction()]))))
		{
		  actions[state].erase(ait++);
		  continue;
		}
	      
	    } // for ait
	} // for state
      
      SGIteration_V2 iter (numIter,actions,threatTuple);
      
      // Reset the error level
      errorLevel = 0;
      
      // Iterate through directions
      SGPoint currDir = SGPoint(1,0); // Start pointing due east
      bool passEast = false;
      while (!passEast)
	{
	  // Compute optimal level in this direction
	  vector<double> newLevels(numStates,0);

	  optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
			 actions,feasibleTuple);

	  // Update the levels and the error level with the movement
	  // in this direction
	  for (int state = 0; state < numStates; state++)
	    {
	      newLevels[state] = pivot[state]*currDir;
	      errorLevel = max(errorLevel,abs(newLevels[state]-levels.back()[state]));
	    } // for state

	  iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
				SGHyperplane(currDir,newLevels),actions));
	  directions.push_back(currDir);
	  levels.push_back(newLevels);

	  // Do sensitivity analysis to find the next direction

	  // If new direction passes due west or due south, update the
	  // corresponding threat tuple using the current pivot

	} // while !passEast

      soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

      // Print summary of iteration
      cout << "Iter: " << numIter
	   << ", errorLvl: " << scientific << errorLevel
	   << ", remaining actions: ( ";
      for (int state = 0; state < numStates; state++)
	cout << actions[state].size() << " ";
      cout << ")" << endl;
      
      findFeasibleTuple(feasibleTuple,actions);
      
      // Update the the threat tuple 
      threatTuple = newThreatTuple;
      
      // Recalculate minimum IC continuation payoffs
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      ait->calculateMinIC(game,updateICFlags,threatTuple);

	      for (auto step = soln.getIterations().back().getSteps().cbegin();
		   step != soln.getIterations().back().getSteps().cend();
		   step++)
		{
		  // Trim the action
		  ait->trim(step->getHyperplane().getNormal(),
			    step->getHyperplane().expectation(probabilities[state][ait->getAction()]));
		} // for step
	      // Trim the actions
	    } // for ait
	  
	} // for state

      numIter++;
    } // while

  cout << "Converged!" << endl;
  
} // solve_endogenous



void SGSolver_V4::optimizePolicy(SGTuple & pivot,
				 vector<SGActionIter> & actionTuple,
				 vector<SG::Regime> & regimeTuple,
				 const SGPoint currDir,
				 const vector<list<SGAction_V2> > & actions,
				 const SGTuple & feasibleTuple) const
{
  // Do policy iteration to find the optimal pivot.
  
  double pivotError = 1.0;
  int numPolicyIters = 0;
  // Iterate until convergence
  SGTuple newPivot(numStates);
  
  while (pivotError > env.getParam(SG::UPDATEPIVOTTOL)
	 && numPolicyIters < 1e2)
    {
      
      pivotError = 0;

      // Now do Bellman iteration to find new fixed point
      policyToPayoffs(pivot,actionTuple,regimeTuple);

      // Look in each state for improvements
      for (int state = 0; state < numStates; state++)
	{
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

	      // WARNING: ACTION MAY STILL BE SUPPORTABLE IF
	      // bestBindingPlayer<0, BUT STILL CANNOT USE NON-BINDING
	      // PAYOFF, BECAUSE NOT FEASIBLE
	      
	      if (!ait->supportable(feasibleTuple
				    .expectation(probabilities[state]
						 [ait->getAction()])) )
		{
		  // TODO delete action and move on
		}
	      
	      if (bestBindingPlayer < 0 // didn't find a binding payoff
		  || (ait->getBndryDirs()[bestBindingPlayer][bestBindingPoint]
		      *currDir > 1e-6) // Can improve on the best
				       // binding payoff by moving in
				       // along the frontier
		  )
		{
		  bestAPSNotBinding = true;
		}
	      else // Found a binding payoff
		bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		  + delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];


	      if ( bestAPSNotBinding // NB bestAPSPayoff has only been
				    // set if bestAPSNotBinding ==
				    // true
		    || bestAPSPayoff*currDir > nonBindingPayoff*currDir) 
		{
		  // ok to use non-binding payoff
		  if (nonBindingPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = nonBindingPayoff*currDir;
		      actionTuple[state] = ait;
	      	      regimeTuple[state] = SG::NonBinding;
	      	      newPivot[state] = nonBindingPayoff;
		    }
		}
	      else if (bestAPSPayoff*currDir < nonBindingPayoff*currDir)
		{
		  if (bestAPSPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = bestAPSPayoff*currDir;
		      actionTuple[state] = ait;
	      	      regimeTuple[state] = SG::Binding;
	      	      newPivot[state] = bestAPSPayoff;
		    }
		}
	    } // ait

	  pivotError = max(pivotError,abs(bestLevel-pivot[state]*currDir));
	} // state

      pivot = newPivot;
      numPolicyIters++;
    } // policy iteration

} // optimizePolicy

void SGSolver_V4::sensitivity(SGPoint & nextDir,
			      const SGTuple & pivot,
			      const vector<SGActionIter> & actionTuple,
			      const vector<SG::Regime> & regimeTuple,
			      const SGPoint currDir,
			      const vector<list<SGAction_V2> > & actions) const
{
  SGPoint normDir = currDir.getNormal();
  
  double nonBindingIndiffLvl = -1;
  double bindingIndiffLvl = -1;
  double bestLevel = numeric_limits<double>::max();
  
  // Look in each state for improvements
  for (int state = 0; state < numStates; state++)
    {

      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  // Find the smallest weight on normDir such that this action
	  // improves in that direction. For each of the binding 
	      
	  SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
	    [ait->getAction()]
	    + delta * pivot.expectation(probabilities[state][ait->getAction()]);

	  // Calculate the lvl at which indifferent to the pivot
	  // pivot[state]*(currDir+tmp*normDir)<=nonBindingPayoff*(currDir+tmp*normDir);
	  // (pivot[state]-nonBindingPayoff)*currDir<=-tmp*normDir*(pivot[state]-nonBindingPayoff)
	  double denom = normDir*(nonBindingPayoff-pivot[state]);
	  if (denom > -1e-10)
	    {
	      nonBindingIndiffLvl = (pivot[state]-nonBindingPayoff)*currDir/denom;
	      SGPoint indiffDir = currDir + normDir * nonBindingIndiffLvl;

	      // See if a binding payoff is higher in the
	      // indifference direction
	      double bestBindLvl = -numeric_limits<double>::max();
	      for (int p = 0; p < numPlayers; p++)
		{
		  for (int k = 0; k < ait->getPoints()[p].size(); k++)
		    {
		      double tmpLvl = ait->getPoints()[p][k]*indiffDir;
		      if (tmpLvl > bestBindLvl)
			{
			  bestBindLvl = tmpLvl;
			  bestBindingPlayer = p;
			  bestBindingPoint = k;
			}
		    } // point
		} // player
	      
	    }
	  else // No positive weight exists s.t. pivot and non-binding payoff tie
	    nonBindingIndiffLvl = -1;

	} // ait

    } // state

} // sensitivity


void SGSolver_V4::findFeasibleTuple(SGTuple & feasibleTuple,
				    const vector<list<SGAction_V2> > & actions) const
{
  // Update the APS-feasible tuple

  // These are just in case we cannot find binding APS payofs. NB not
  // the same as the actionTuple and regimeTuple that determine the
  // pivot.
  vector<SGActionIter> actionTuple(numStates);
  vector< SG::Regime > regimeTuple(numStates,SG::Binding);

  bool anyNonBinding = false;
  for (int state = 0; state < numStates; state++)
    {
      // Search for an action with feasible binding continuations
      // and just pick one. If we don't find any action with
      // feasible binding continuations, it either means (i) the
      // game has no pure strategy SPNE, or (ii) any feasible
      // payoff tuple (within the set of remaining actions) is an
      // APS payoff.
      bool foundFeasiblePlayer = false;
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ait++)
	{
	  int feasiblePlayer = -1;
	  for (int player = 0; player < numPlayers; player++)
	    {
	      if (ait->getPoints()[player].size()>0)
		{
		  feasiblePlayer = player;
		  break;
		}
	    }
	  if (feasiblePlayer >= 0)
	    {
	      feasibleTuple[state] = (1-delta)*payoffs[state][ait->getAction()]
		+delta*ait->getPoints()[feasiblePlayer][0];
	      foundFeasiblePlayer = true;
	      break;
	    }
	} // for ait
      if (!foundFeasiblePlayer)
	{
	  regimeTuple[state] = SG::NonBinding;
	  actionTuple[state] = actions[state].begin();
	  anyNonBinding = true;
	}
    } // for state

  bool notAllIC = anyNonBinding;
  while (notAllIC)
    {
      // Have to do Bellman iteration to find the new pivot.
      policyToPayoffs(feasibleTuple,actionTuple,regimeTuple);
      notAllIC = false;
      
      // Check if resulting tuple is IC.
      for (int state = 0; state < numStates; state++)
	{
	  if (!(feasibleTuple.expectation(probabilities[state]
					  [actionTuple[state]->getAction()])
		>= actionTuple[state]->getMinICPayoffs()))
	    {
	      notAllIC = true;
	      // Try advancing the action and recomputing
	      if ((++actionTuple[state])==actions[state].end())
		throw(SGException(SG::NOFEASIBLETUPLE));
	    }
	} // for state
    } // if anyNonBinding

} // findFeasibleTuple

void SGSolver_V4::policyToPayoffs(SGTuple & pivot,
				  const vector<SGActionIter>  & actionTuple,
				  const vector<SG::Regime> & regimeTuple) const
{
  // Do Bellman iteration to find new fixed point
  int updatePivotPasses = 0;
  double bellmanPivotGap = 0;
  SGTuple newPivot(pivot);

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

} // policyToPayoffs
