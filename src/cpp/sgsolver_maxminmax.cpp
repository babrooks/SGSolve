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

#include "sgsolver_maxminmax.hpp"

SGSolver_MaxMinMax::SGSolver_MaxMinMax(const SGEnv & _env,
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

void SGSolver_MaxMinMax::solve()
{
  initialize();
  
  // Initialize directions
  int numDirections = 200;

  // make an even multiple of 4
  numDirections += (-numDirections)%4;

  // Initialize directions
  list<vector< double> > :: const_iterator dueSouthLvl, dueWestLvl;
  for (int dir = 0; dir < numDirections; dir++)
    {
      double theta = 2.0*PI
	*static_cast<double>(dir)/static_cast<double>(numDirections);
      directions.push_back(SGPoint(cos(theta),sin(theta)));
      levels.push_back(vector<double>(numStates,0));
    } // for dir

  {
    int dirCnt = 0;
    for (auto lit = levels.cbegin();
	 lit != levels.cend();
	 ++lit)
      {
	if (dirCnt == numDirections/2)
	  dueWestLvl = lit;
	if (dirCnt == 3*numDirections/4)
	  dueSouthLvl = lit;
	dirCnt++;
      }
  }

  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  SGIteration_MaxMinMax iter;
  
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

	      // WARNING: NO GUARANTEE THAT THIS POINT IS FEASIBLE
	      if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							       [ait->getAction()]))))
		{
		  actions[state].erase(ait++);
		  continue;
		}
	      
	    } // for ait
	} // for state

      if (env.getParam(SG::STOREITERATIONS))
	iter = SGIteration_MaxMinMax(actions,threatTuple);
      
      // Reset the error level
      errorLevel = 0;

      {
	list<vector< double> >::iterator lvl;
	int dirCnt;
	list<SGPoint>::const_iterator dir;

	// Iterate through directions
	for (dir = directions.cbegin(),
	       lvl = levels.begin(),
	       dirCnt = 0;
	     dir != directions.cend(),
	       lvl != levels.end();
	     dir++, lvl++, dirCnt ++ )
	  {
	    // Compute optimal level in this direction
	    vector<double> newLevels(numStates,0);
	    SGPoint currDir = *dir;

	    optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
			   actions,feasibleTuple);

	    for (int state = 0; state < numStates; state++)
	      {
		// Update the levels and the error level with the
		// movement in this direction
		newLevels[state] = pivot[state]*currDir;
		errorLevel = max(errorLevel,abs(newLevels[state]-(*lvl)[state]));
		(*lvl)[state] = newLevels[state];
	      } // for state


	    if (env.getParam(SG::STOREITERATIONS))
	      iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
				    SGHyperplane(currDir,newLevels),actions));
	  
	  } // for dir lvl dirCntr
      }

      // Update the the threat tuple
      for (int state = 0; state < numStates; state++)
	{
	  threatTuple[state][0] = -(*dueWestLvl)[state];
	  threatTuple[state][1] = -(*dueSouthLvl)[state];
	}

      if (env.getParam(SG::STOREITERATIONS)==2
	  || (env.getParam(SG::STOREITERATIONS)==1
	      && ( errorLevel < env.getParam(SG::ERRORTOL)
		   || numIter+1 >= env.getParam(SG::MAXITERATIONS) ) ) )
	soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

      findFeasibleTuple(feasibleTuple,actions);
      
      // Recalculate minimum IC continuation payoffs
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      ait->calculateMinIC(game,threatTuple);
	      // ait->resetTrimmedPoints();

	      {
		list<SGPoint>::const_iterator dir;
		list< vector< double> >::const_iterator lvl;
		for (dir = directions.cbegin(),
		       lvl = levels.cbegin();
		     dir != directions.cend(),
		       lvl != levels.cend();
		     dir++, lvl++)
		  {
		    // Compute the expected level
		    double expLevel = 0;
		    for (int sp = 0; sp < numStates; sp++)
		      expLevel += probabilities[state][ait->getAction()][sp]
			* (*lvl)[sp];
		  
		    // Trim the action
		    ait->trim(*dir,expLevel);
		  } // for dir
	      }
	    } // for ait
	  
	} // for state

      cout << "Iter: " << numIter
	   << ", errorLvl: " << scientific << errorLevel
	   << ", remaining actions: ( ";
      for (int state = 0; state < numStates; state++)
	cout << actions[state].size() << " ";
      cout << ")" << endl;
      
      numIter++;
    } // while

  cout << "Converged!" << endl;

} // solve

void SGSolver_MaxMinMax::solve_endogenous()
{
  initialize();
  
  // Main loop

  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      iterate_endogenous();

      cout << progressString() << endl;
    } // while --- main loop

  cout << "Converged!" << endl;
  
} // solve_endogenous

std::string SGSolver_MaxMinMax::progressString() const
{
  std::stringstream ss;
  // Print summary of iteration
  ss << "Iter: " << numIter
     << ", errorLvl: " << scientific << errorLevel
     << ", remaining actions: ( ";
  for (int state = 0; state < numStates; state++)
    ss << actions[state].size() << " ";
  ss << ")"
     << ", numDirections = " << directions.size();

  return ss.str();
}


double SGSolver_MaxMinMax::iterate_endogenous()
{
  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  SGIteration_MaxMinMax iter;
  
  // Clear the directions and levels
  list<SGPoint> newDirections;
  list< vector<double> > newLevels;
      
  SGTuple newThreatTuple(threatTuple);

  vector<SGActionIter> actionTuple(numStates);
  // Pick the initial actions arbitrarily
  for (int state = 0; state < numStates; state++)
    actionTuple[state] = actions[state].begin();
      
  vector<SG::Regime> regimeTuple(numStates,SG::Binding);

  if (env.getParam(SG::STOREITERATIONS))
    iter = SGIteration_MaxMinMax (actions,threatTuple);
      
  // Iterate through directions
  SGPoint currDir = SGPoint(1.0,0.0); // Start pointing due east
  bool passEast = false;
  while (!passEast)
    {
      // Compute optimal level in this direction
      optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
		     actions,feasibleTuple);

      // Do sensitivity analysis to find the next direction
      SGPoint normDir = currDir.getNormal();
      double bestLevel = sensitivity(pivot,actionTuple,regimeTuple,
				     currDir,actions);

      SGPoint newDir = 1.0/(bestLevel+1.0)*currDir
	+ bestLevel/(bestLevel+1.0)*normDir;
      newDir /= newDir.norm();
	  
      newLevels.push_back(vector<double>(numStates,0));
      newDirections.push_back(newDir);
      for (int state = 0; state < numStates; state++)
	{
	  newLevels.back()[state] = pivot[state]*newDir;
	} // for state
      if (env.getParam(SG::STOREITERATIONS))
	iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
			      SGHyperplane(newDir,newLevels.back()),actions));

      // Move the direction slightly to break ties
      newDir.rotateCCW(PI*1e-4);

      // If new direction passes due west or due south, update the
      // corresponding threat tuple using the current pivot
      if (currDir*dueNorth > 0 && newDir*dueNorth <= 0) // Passing due west
	{
	  for (int state = 0; state < numStates; state++)
	    newThreatTuple[state][0] = pivot[state][0];
	}
      else if (currDir*dueEast < 0 && newDir*dueEast >= 0) // Passing due south
	{
	  for (int state = 0; state < numStates; state++)
	    newThreatTuple[state][1] = pivot[state][1];
	}
      else if (currDir*dueNorth < 0 && newDir*dueNorth >= 0)
	passEast = true;
	  
      currDir = newDir;
    } // while !passEast

  // Recompute the error level
  errorLevel = 0;
  {
    // Rather heavy handed, but do this for now.

    auto dir0 = directions.cbegin(), dir1 = newDirections.cbegin();
    auto lvl0 = levels.cbegin(), lvl1 = newLevels.cbegin();
    while (dir1 != newDirections.cend()
	   && lvl1 != newLevels.cend())
      {
	dir0 = directions.cbegin();
	lvl0 = levels.cbegin();
	double minDist = numeric_limits<double>::max();
	while (dir0 != directions.cend()
	       && lvl0 != levels.cend() )
	  {
	    double tmp = 0;
	    for (int state = 0; state < numStates; state++)
	      tmp = max(tmp,abs((*lvl0)[state]-(*lvl1)[state]));
		  
	    minDist = min(minDist,SGPoint::distance(*dir0,*dir1)+tmp);
		  
	    dir0++;
	    lvl0++;
	  }
	errorLevel = max(errorLevel,minDist);
	      
	dir1++;
	lvl1++;
      }
  }
  
  if (env.getParam(SG::STOREITERATIONS)==2
      || (env.getParam(SG::STOREITERATIONS)==1
	  && ( errorLevel < env.getParam(SG::ERRORTOL)
	       || numIter+1 >= env.getParam(SG::MAXITERATIONS) ) ) )
    soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

  findFeasibleTuple(feasibleTuple,actions);
      
  // Update the the threat tuple, directions, levels
  threatTuple = newThreatTuple;
  directions = newDirections;
  levels = newLevels;
      
  // Recalculate minimum IC continuation payoffs
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  ait->calculateMinIC(game,threatTuple);
	  //ait->resetTrimmedPoints();

	  // Trim the actions
	  auto dir = directions.cbegin();
	  auto lvl = levels.cbegin();
	  while (dir != directions.cend()
		 && lvl != levels.cend())
	    {
	      // Trim the action
	      double expLevel = 0;
	      for (int sp = 0; sp < numStates; sp++)
		expLevel += probabilities[state][ait->getAction()][sp]
		  * (*lvl)[sp];

	      ait->trim(*dir,expLevel);

	      dir++;
	      lvl++;
	    } // for dir, lvl
	  ait->updateTrim();

	  // Delete the action if not supportable
	  if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							   [ait->getAction()]))))
	    {
	      actions[state].erase(ait++);
	      continue;
	    }
	} // for ait
	  
    } // for state

  numIter++;
  return errorLevel;
  
} // iterate_endogenous

void SGSolver_MaxMinMax::initialize()
{
  errorLevel = 1;
  numIter = 0;
  
  SGPoint payoffLB, payoffUB;
  game.getPayoffBounds(payoffUB,payoffLB);

  threatTuple = SGTuple(numStates,payoffLB);

  // Clear the solution
  soln.clear();
  directions.clear();
  levels.clear();
  
  // Initialize actions with a big box as the feasible set
  actions.clear();
  actions = vector< list<SGAction_MaxMinMax> > (numStates);
  
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = eqActions[state].cbegin();
	   ait != eqActions[state].cend();
	   ait++)
	{
	  actions[state].push_back(SGAction_MaxMinMax(env,state,*ait));
	  actions[state].back().calculateMinIC(game,threatTuple);
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
} // initialize_endogenous

void SGSolver_MaxMinMax::optimizePolicy(SGTuple & pivot,
				 vector<SGActionIter> & actionTuple,
				 vector<SG::Regime> & regimeTuple,
				 const SGPoint currDir,
				 const vector<list<SGAction_MaxMinMax> > & actions,
				 const SGTuple & feasibleTuple) const
{
  // Do policy iteration to find the optimal pivot.
  
  double pivotError = 1.0;
  int numPolicyIters = 0;
  // Iterate until convergence
  SGTuple newPivot(numStates);

  vector<SGActionIter> newActionTuple(actionTuple);
  vector<SG::Regime> newRegimeTuple(regimeTuple);

  vector<bool> bestAPSNotBinding(numStates,false);
  SGTuple bestBindingPayoffs(numStates);

  
  // policy iteration
  do
    {
      pivotError = 0;

      // Look in each state for improvements
      for (int state = 0; state < numStates; state++)
	{
	  double bestLevel = -numeric_limits<double>::max();

	  if (numPolicyIters > 0)
	    bestLevel = pivot[state]*currDir;
	  
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      // Procedure to find an improvement to the policy
	      // function
	      
	      SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
		[ait->getAction()]
		+ delta * pivot.expectation(probabilities[state][ait->getAction()]);

	      bool APSNotBinding = false;
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

	      if (bestBindingPlayer < 0 // didn't find a binding payoff
		  || (ait->getBndryDirs()[bestBindingPlayer][bestBindingPoint]
		      *currDir > 1e-6) // Can improve on the best
				       // binding payoff by moving in
				       // along the frontier
		  )
		{
		  APSNotBinding = true;
		}
	      else // Found a binding payoff
		bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		  + delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];

	      if ( APSNotBinding // NB bestAPSPayoff has only been
				    // set if bestAPSNotBinding ==
				    // false
		   || (bestAPSPayoff*currDir >= nonBindingPayoff*currDir) ) 
		{
		  // ok to use non-binding payoff
		  if (nonBindingPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = nonBindingPayoff*currDir;

		      bestAPSNotBinding[state] = APSNotBinding;
		      if (!APSNotBinding)
			bestBindingPayoffs[state] = bestAPSPayoff;
		      
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::NonBinding;
	      	      newPivot[state] = nonBindingPayoff;
		    }
		}
	      else if (bestAPSPayoff*currDir < nonBindingPayoff*currDir)
		{
		  if (bestAPSPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = bestAPSPayoff*currDir;
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::Binding;
	      	      newPivot[state] = bestAPSPayoff;
		    }
		}
	    } // ait

	  pivotError = max(pivotError,abs(bestLevel-pivot[state]*currDir));
	} // state

      pivot = newPivot;
      actionTuple = newActionTuple;
      regimeTuple = newRegimeTuple;

      // Inner loop to fix regimes
      bool anyViolation = false;
      do
	{
	  // Do Bellman iteration to find new fixed point
	  policyToPayoffs(pivot,actionTuple,regimeTuple);

	  anyViolation = false;

	  // Check if any reversals between best binding and non-binding

	  // First determine the maximum gap
	  vector<double> gaps(numStates,0);
	  double maxGap = 0;
	  for (int state = 0; state < numStates; state++)
	    {
	      if (!bestAPSNotBinding[state]
		  && regimeTuple[state] == SG::NonBinding) 
		{
		  gaps[state] = pivot[state]*currDir - bestBindingPayoffs[state]*currDir;
		  if (gaps[state] > maxGap)
		    {
		      anyViolation = true;
		      maxGap = gaps[state];
		    }
		}
	    }
	  
	  for (int state = 0; state < numStates; state++)
	    {
	      if (!bestAPSNotBinding[state]
		  && regimeTuple[state] == SG::NonBinding
		  && gaps[state] >= delta*maxGap) 
		{
		  pivot[state] = bestBindingPayoffs[state];
		  regimeTuple[state] = SG::Binding;
		}
	    }
	} while (anyViolation);


      if (numPolicyIters == env.getParam(SG::MAXPOLICYITERATIONS)/2)
	cout << "Cycling detected at direction: " << currDir << endl;
      if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS)/2)
	{
	  cout << "Policy iter: " << numPolicyIters
	       << ", action tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << actionTuple[state]->getAction() << " ";
	  cout << "), regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << regimeTuple[state] << " ";
	  cout << "), new regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << newRegimeTuple[state] << " ";
	  cout << "), pivot error: " << scientific << pivotError;
	  cout << endl;
	}


    } while (pivotError > env.getParam(SG::POLICYITERTOL)
	     && ++numPolicyIters < env.getParam(SG::MAXPOLICYITERATIONS));

  if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS))
    cout << "WARNING: Maximum policy iterations reached." << endl;

} // optimizePolicy

double SGSolver_MaxMinMax::sensitivity(const SGTuple & pivot,
				const vector<SGActionIter> & actionTuple,
				const vector<SG::Regime> & regimeTuple,
				const SGPoint currDir,
				const vector<list<SGAction_MaxMinMax> > & actions) const
{
  SGPoint normDir = currDir.getNormal();
  
  double nonBindingIndiffLvl = -1;
  double bindingIndiffLvl = -1;
  double bestLevel = numeric_limits<double>::max()-1.0;
  
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
	  double numer = (pivot[state]-nonBindingPayoff)*currDir;
	  if (SGPoint::distance(pivot[state],nonBindingPayoff) > 1e-6
	      && abs(denom) > 1e-10)
	    {
	      nonBindingIndiffLvl = numer/denom;

	      if (nonBindingIndiffLvl < bestLevel
		  && nonBindingIndiffLvl > -1e-6)
		{
		  SGPoint indiffDir = currDir + normDir * nonBindingIndiffLvl;

		  // See if a binding payoff is higher in the
		  // indifference direction
		  double bestBindLvl = -numeric_limits<double>::max();
		  int bestBindingPlayer = -1;
		  int bestBindingPoint = 0;
		  for (int p = 0; p < numPlayers; p++)
		    {
		      for (int k = 0; k < ait->getPoints()[p].size(); k++)
			{
			  double tmpLvl = ait->getPoints()[p][k]*indiffDir;
			  if (tmpLvl > bestBindLvl
			      || (tmpLvl > bestBindLvl-1e-8
				  && ait->getPoints()[p][k]*normDir >= 0))
			    {
			      bestBindLvl = tmpLvl;
			      bestBindingPlayer = p;
			      bestBindingPoint = k;
			    }
			} // point
		    } // player

		  bool bestAPSNotBinding = false;
		  if (bestBindingPlayer < 0 // didn't find a binding payoff
		      || (ait->getBndryDirs()[bestBindingPlayer][bestBindingPoint]
			  *indiffDir > 1e-6) // Can improve on the
		      // best binding payoff by
		      // moving in along the
		      // frontier
		      )
		    {
		      bestAPSNotBinding = true;
		    }


		  if ( bestAPSNotBinding // NB bestAPSPayoff has only been
		       // set if bestAPSNotBinding ==
		       // true
		       || bestBindLvl > nonBindingPayoff*indiffDir - 1e-10) 
		    {
		      // If we get to here, non-binding regime is
		      // available in the indifferent direction, and
		      // this direction is smaller than the best level
		      // found so far.

		      if ( (ait != actionTuple[state] && denom> 1e-6)
			   || (ait == actionTuple[state]
			       && denom < -1e-6
			       && regimeTuple[state] == SG::Binding) )
			bestLevel = nonBindingIndiffLvl;
		    }
		} // Non-binding indifference level is smaller than best level
	    } // Positive level of indifference


	      // Now check the binding directions
	  for (int p = 0; p < numPlayers; p++)
	    {
	      for (int k = 0; k < ait->getPoints()[p].size(); k++)
		{
		  SGPoint bindingPayoff = (1-delta)*payoffs[state]
		    [ait->getAction()]
		    + delta * ait->getPoints()[p][k];
		  double denom = normDir*(bindingPayoff-pivot[state]);
		  double numer = (pivot[state]-bindingPayoff)*currDir;
		  if (SGPoint::distance(pivot[state],bindingPayoff)>1e-6
		      && abs(denom) > 1e-10)
		    {
		      bindingIndiffLvl = numer/denom;

		      if (bindingIndiffLvl < bestLevel
			  && bindingIndiffLvl > -1e-6)
			{
			  SGPoint indiffDir = currDir + normDir * bindingIndiffLvl;

			  if (nonBindingPayoff*indiffDir
			      >= bindingPayoff*indiffDir-1e-6)
			    {
			      if ( (ait != actionTuple[state]
				    && denom > 1e-6 )
				   || (ait == actionTuple[state]
				       && (regimeTuple[state]==SG::NonBinding
					   && denom < -1e-6 )
				       || (regimeTuple[state]==SG::Binding
					   && denom > 1e-6) ) )
				bestLevel = bindingIndiffLvl;
			    } // Binding payoff is available
			} // Smaller than the current bestLvl
		    } // Denominator is positive
		} // point
	    } // player
	} // ait

    } // state

  return std::max(bestLevel,0.0);

} // sensitivity


void SGSolver_MaxMinMax::findFeasibleTuple(SGTuple & feasibleTuple,
				    const vector<list<SGAction_MaxMinMax> > & actions) const
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
		{
		  throw(SGException(SG::NOFEASIBLETUPLE));
		}
	    }
	} // for state
    } // if anyNonBinding

} // findFeasibleTuple

void SGSolver_MaxMinMax::policyToPayoffs(SGTuple & pivot,
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

  if (updatePivotPasses == env.getParam(SG::MAXUPDATEPIVOTPASSES) )
    cout << "WARNING: Maximum pivot update passes reached." << endl;
  
} // policyToPayoffs
