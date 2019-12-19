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


void SGSolver_MaxMinMax::solve_fixed()
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
  vector<double> penalties (numStates,env.getParam(SG::SUBGENFACTOR));

  SGIteration_MaxMinMax iter;
  
  while (errorLevel > env.getParam(SG::ERRORTOL))
    {
      if(numIter >= env.getParam(SG::MAXITERATIONS))
  	throw(SGException(SG::MAX_ITERATIONS_REACHED));
      else{
        vector<SGActionIter> actionTuple(numStates);
        // Pick the initial actions arbitrarily
        for (int state = 0; state < numStates; state++)
	actionTuple[state] = actions[state].begin();
      
        vector<SG::Regime> regimeTuple(numStates,SG::NonBinding);
	vector<bool> bestAPSNotBinding(numStates);
	SGTuple bestBindingPayoffs(numStates);
	updateBestBinding(actionTuple,regimeTuple,directions.front(),
			  bestBindingPayoffs,bestAPSNotBinding);
	minimizeRegimes(pivot,penalties,actionTuple,regimeTuple,directions.front(),
			bestBindingPayoffs,bestAPSNotBinding);

        // Reset the trimmed points for the actions
        for (int state = 0; state < numStates; state++)
	  {
	    auto ait = actions[state].begin();
	    while (ait != actions[state].end())
	      {
	        ait->updateTrim();

	        if (!(ait->supportable() ) )
		  {
		    actions[state].erase(ait++);
		    continue;
		  }
	        ait++;
	      
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

	      // Make sure payoffs are min-max in new direction
	      updateBestBinding(actionTuple,regimeTuple,currDir,
				bestBindingPayoffs,bestAPSNotBinding);
	      minimizeRegimes(pivot,penalties,actionTuple,regimeTuple,currDir,
			      bestBindingPayoffs,bestAPSNotBinding);

	      robustOptimizePolicy(pivot,penalties,
				   actionTuple,regimeTuple,
				   bestAPSNotBinding,
				   bestBindingPayoffs,
				   currDir,
				   actions);

	      for (int state = 0; state < numStates; state++)
	        {
		  // Update the levels and the error level with the
		  // movement in this direction
		  newLevels[state] = (pivot[state]*currDir-penalties[state]);
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

        // Recalculate minimum IC continuation payoffs
        for (int state = 0; state < numStates; state++)
	  {
	    for (auto ait = actions[state].begin();
	         ait != actions[state].end();
	         ++ait)
	      {
	        ait->calculateMinIC(game,threatTuple);
	        ait->resetTrimmedPoints();
	      
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
      }
    } // while

  cout << "Converged" << endl;

} // solve_fixed

void SGSolver_MaxMinMax::solve()
{
  initialize();
  
  if(!game.transitionProbsSumToOne(env.getParam(SG::TRANSITION_PROB_TOL)))
    throw(SGException(SG::PROB_SUM_NOT1));
  // Main loop

  while (errorLevel > env.getParam(SG::ERRORTOL))
  {
    if(numIter >= env.getParam(SG::MAXITERATIONS))
      throw(SGException(SG::MAX_ITERATIONS_REACHED));
    else
      {
        iterate();

        cout << progressString() << endl;
      } 
   }  // while --- main loop
   cout << "Converged!" << endl;
 
} // solve

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


double SGSolver_MaxMinMax::iterate()
{
  SGTuple pivot = threatTuple;
  vector<double> penalties(numStates,0.0);
  
  SGIteration_MaxMinMax iter;
  
  // Clear the directions and levels
  list<SGPoint> newDirections;
  list< vector<double> > newLevels;
      
  SGTuple newThreatTuple(threatTuple);

  vector<SGActionIter> actionTuple(numStates);
  // Pick the initial actions arbitrarily and compute an initial pivot
  for (int state = 0; state < numStates; state++)
    actionTuple[state] = actions[state].begin();

  if (env.getParam(SG::STOREITERATIONS))
    iter = SGIteration_MaxMinMax (actions,threatTuple);
  
  // Iterate through directions
  SGPoint currDir = SGPoint(0.0,1.0); // Start pointing due north
  bool passNorth = false;

  // Construct initial pivot

  // Start with all regimes non-binding (needed so that
  // updateBestBinding will set the binding payoffs)
  vector<SG::Regime> regimeTuple(numStates,SG::NonBinding);
  vector<bool> bestAPSNotBinding(numStates,false);
  SGTuple bestBindingPayoffs(numStates);

  // Update the best binding payoffs
  updateBestBinding(actionTuple,regimeTuple,currDir,
		    bestBindingPayoffs,bestAPSNotBinding);
  // minimize regimes
  minimizeRegimes(pivot,penalties,actionTuple,regimeTuple,currDir,
		  bestBindingPayoffs,bestAPSNotBinding);

  while (!passNorth)
    {
      // Compute optimal level in this direction
      robustOptimizePolicy(pivot,penalties,
			   actionTuple,
			   regimeTuple,
			   bestAPSNotBinding,
			   bestBindingPayoffs,
			   currDir,
			   actions);

      // Do sensitivity analysis to find the next direction
      SGPoint normDir = -1.0*currDir.getNormal(); // rotate direction clockwise 90 degrees
      double bestLevel = sensitivity(pivot,penalties,actionTuple,regimeTuple,
				     currDir,actions);

      SGPoint newDir = 1.0/(bestLevel+1.0)*currDir
	+ bestLevel/(bestLevel+1.0)*normDir;
      newDir /= newDir.norm();
	  
      newLevels.push_back(vector<double>(numStates,0));
      newDirections.push_back(newDir);
      for (int state = 0; state < numStates; state++)
	{
	  newLevels.back()[state] = (pivot[state]*newDir-penalties[state]);
	} // for state
      if (env.getParam(SG::STOREITERATIONS))
	iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
			      SGHyperplane(newDir,newLevels.back()),actions));
      
      // Move the direction slightly to break ties
      // newDir.rotateCW(PI*1e-4);

      // If new direction passes due west or due south, update the
      // corresponding threat tuple using the current pivot
      if (currDir*dueNorth < 0 && newDir*dueNorth >= 0) // Passing due west
	{
	  for (int state = 0; state < numStates; state++)
	    newThreatTuple[state][0] = max(pivot[state][0]+penalties[state],threatTuple[state][0]);
	}
      else if (currDir*dueEast > 0 && newDir*dueEast <= 0) // Passing due south
	{
	  for (int state = 0; state < numStates; state++)
	    newThreatTuple[state][1] = max(pivot[state][1]+penalties[state],threatTuple[state][1]);
	}
      else if (currDir*dueEast < 0 && newDir*dueEast >= 0)
	passNorth = true;
	  
      currDir = newDir;
    } // while

  // Recompute the error level
  errorLevel = pseudoHausdorff(newDirections,newLevels);
  
  if (env.getParam(SG::STOREITERATIONS)==2
      || (env.getParam(SG::STOREITERATIONS)==1
	  && ( errorLevel < env.getParam(SG::ERRORTOL)
	       || numIter+1 >= env.getParam(SG::MAXITERATIONS) ) ) )
    soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

  // Update the the threat tuple, directions, levels
  threatTuple = newThreatTuple;
  directions = newDirections;
  levels = newLevels;

  // TODO: Add another parameter to control convergence criterion
  // // Recalculate minimum IC continuation payoffs
  // errorLevel = 0.0;
  
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  ait->calculateMinIC(game,threatTuple);
	  ait->resetTrimmedPoints();

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

	  // errorLevel = max(errorLevel,ait->distToTrimmed());
	  
	  ait->updateTrim();

	} // for ait
	  
    } // for state

  for (int state = 0; state < numStates; state++)
    {
      auto ait = actions[state].begin();
      while (ait != actions[state].end())
	{
	  // Delete the action if not supportable
	  if (!(ait->supportable()))
	    {
	      actions[state].erase(ait++);
	      continue;
	    }
	  ++ait;
	} // while ait
	  
    } // for state

  numIter++;
  return errorLevel;
  
} // iterate

double SGSolver_MaxMinMax::pseudoHausdorff(const list<SGPoint> & newDirections,
					   const list<vector<double> > & newLevels) const
{
  // Recompute the error level
  double newErrorLevel = 0;
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
      newErrorLevel = max(newErrorLevel,minDist);
	      
      dir1++;
      lvl1++;
    }
  return newErrorLevel;
}

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
      if (eqActions[state].size()>0)
	{
	  for (int a=0; a<numActions_totalByState[state]; a++)
	    {
	      if (eqActions[state][a])
		actions[state].push_back(SGAction_MaxMinMax(env,state,a));
	    }
	}
      else
	{
	  for (int a=0; a<numActions_totalByState[state]; a++)
	    actions[state].push_back(SGAction_MaxMinMax(env,state,a));
	}
      
      for (auto ait = actions[state].begin(); ait != actions[state].end(); ait++)
	{
	  ait->calculateMinIC(game,threatTuple);
	  ait->resetTrimmedPoints();
	  
	  for (int dir = 0; dir < 4; dir ++)
	    {
	      double theta = 2.0*PI*static_cast<double>(dir)/4.0;
	      SGPoint currDir = SGPoint(cos(theta),sin(theta));

	      double level = max(currDir*payoffLB,currDir*payoffUB);
	      ait->trim(currDir,level);
	    } // for dir

	  ait->updateTrim();
	}
    } // for state

  for (int state = 0; state < numStates; state++)
    {
      auto ait = actions[state].begin();
      while (ait != actions[state].end())
	{
	  // Delete the action if not supportable
	  if (!(ait->supportable()))
	    {
	      actions[state].erase(ait++);
	      continue;
	    }
	  ++ait;
	} // while ait
	  
    } // for state

} // initialize

void SGSolver_MaxMinMax::robustOptimizePolicy(SGTuple & pivot,
					      vector<double> & penalties,
					      vector<SGActionIter> & actionTuple,
					      vector<SG::Regime> & regimeTuple,
					      vector<bool> & bestAPSNotBinding,
					      SGTuple & bestBindingPayoffs,
					      const SGPoint currDir,
					      const vector<list<SGAction_MaxMinMax> > & actions) const
{
  // Do policy iteration to find the optimal pivot.
  bool actionsChanged;
  int numPolicyIters = 0;
  // Iterate until convergence
  SGTuple newPivot(pivot);
  vector<double> newPenalties(penalties);

  vector<SGActionIter> newActionTuple(actionTuple);
  vector<SG::Regime> newRegimeTuple(regimeTuple);
  
  bool doInner = env.getParam(SG::SUBGENFACTOR)>0;
  double bindingPenalty = 0;
  if (doInner)
    bindingPenalty = env.getParam(SG::SUBGENFACTOR);

        
  // policy iteration
  do
    {
      // Iterate as long as actions are changing in some state.
      actionsChanged = false;
      
      // Look in each state for improvements
      for (int state = 0; state < numStates; state++)
	{
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      // Procedure to find an improvement to the policy
	      // function
	      
	      SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
		[ait->getAction()]
		+ delta * pivot.expectation(probabilities[state][ait->getAction()]);
	      double nonBindingPenalty = 0.0;
	      if (doInner)
		{
		  nonBindingPenalty = env.getParam(SG::SUBGENFACTOR);
		  for (int sp = 0; sp < numStates; sp++)
		    nonBindingPenalty += delta*probabilities[state][ait->getAction()][sp]*penalties[sp];
		}
	      
	      // Find which payoff is highest in current normal and
	      // break ties in favor of the clockwise 90 degree.
	      int bestBindingPlayer, bestBindingPoint;
	      bool APSNotBinding=computeBestBindingPayoff(ait,bestBindingPlayer,
							  bestBindingPoint,currDir);
	      SGPoint bestAPSPayoff;
	      if (!APSNotBinding) 
		bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		  + delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];
	      
	      if ( APSNotBinding // NB bestAPSPayoff has only been
		   // set if APSNotBinding ==
		   // false
		   || ( !lexComp(nonBindingPayoff,nonBindingPenalty,
				 bestAPSPayoff,bindingPenalty,
				 currDir ) ) ) 
		{
		  // ok to use non-binding payoff
		  if (lexComp(nonBindingPayoff,nonBindingPenalty,
			      newPivot[state],newPenalties[state],
			      currDir) )
		    {
		      bestAPSNotBinding[state] = APSNotBinding;
		      if (!APSNotBinding)
			bestBindingPayoffs[state] = bestAPSPayoff;
		      
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::NonBinding;
	      	      newPivot[state] = nonBindingPayoff;
	      	      newPenalties[state] = nonBindingPenalty;

		      actionsChanged = true;
		    }
		}
	      else
		{
		  if (lexComp(bestAPSPayoff,bindingPenalty,
			      newPivot[state],newPenalties[state],
			      currDir) )
		    {
		      bestAPSNotBinding[state] = APSNotBinding;
		      bestBindingPayoffs[state] = bestAPSPayoff;
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::Binding;
	      	      newPivot[state] = bestAPSPayoff;
		      newPenalties[state] = bindingPenalty;

		      actionsChanged = true;
		    }
		}
	    } // ait
	} // state

      if (numPolicyIters == env.getParam(SG::MAXPOLICYITERATIONS)/2)
	cout << "Cycling detected at direction: " << currDir << endl;
      if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS)/2)
	{
	  cout << "Policy iter: " << numPolicyIters
	       << ", action tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << actionTuple[state]->getAction() << " ";
	  cout << "), new action tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << newActionTuple[state]->getAction() << " ";
	  cout << "), regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << regimeTuple[state] << " ";
	  cout << "), new regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << newRegimeTuple[state] << " ";
	  cout << "), actionsChanged: " << scientific << actionsChanged;
	  cout << endl;
	  cout << "\tpivot: "  << pivot
	       << ", newPivot: " << newPivot;
	  cout << endl;
	}
      
      pivot = newPivot;
      actionTuple = newActionTuple;
      regimeTuple = newRegimeTuple;

      updateBestBinding(actionTuple,regimeTuple,currDir,
			bestBindingPayoffs,bestAPSNotBinding);
      
      // minimize regimes
      minimizeRegimes(pivot,penalties,actionTuple,regimeTuple,currDir,
		      bestBindingPayoffs,bestAPSNotBinding);

      if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS)/2)
	{
	  cout << "\tpivot: " << pivot;
	  cout << ", regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << regimeTuple[state] << " ";
	  cout << endl;
	}
      
    } while (actionsChanged
	     && ++numPolicyIters < env.getParam(SG::MAXPOLICYITERATIONS));

  if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS))
    cout << "WARNING: Maximum policy iterations reached." << endl;

} // robustOptimizePolicy

void SGSolver_MaxMinMax::updateBestBinding(const vector<SGActionIter> & actionTuple,
					   const vector<SG::Regime> & regimeTuple,
					   const SGPoint & dir,
					   SGTuple & bestBindingPayoffs,
					   vector<bool> & bestAPSNotBinding) const
{
  // Make sure current pivot is minimal for new direction.
  for (int state = 0; state < numStates; state++)
    {
      if (regimeTuple[state]==SG::Binding)
	continue;
      int bestBindingPlayer, bestBindingPoint;
      bestAPSNotBinding[state] = computeBestBindingPayoff(actionTuple[state],
							  bestBindingPlayer,
							  bestBindingPoint,dir);
      bestBindingPayoffs[state] = (1-delta)*payoffs[state][actionTuple[state]->getAction()]
	+ delta * actionTuple[state]->getPoints()[bestBindingPlayer][bestBindingPoint];
    }
} // updateBestBinding

bool SGSolver_MaxMinMax::lexComp(const SGPoint & a,
				 const double aPenalty,
				 const SGPoint & b,
				 const double bPenalty,
				 const SGPoint & dir) const
{
  double diff=a*dir-aPenalty-b*dir+bPenalty;
  if (diff > env.getParam(SG::LEXIMPROVETOL) )
    return true;
  else if ( diff> -1.0*env.getParam(SG::LEXSUBOPTOL) )
    {
      const SGPoint dir2(dir[1],-dir[0]);
      if (a*dir2-aPenalty-b*dir2+bPenalty > env.getParam(SG::LEXIMPROVETOL) )
	return true;
    }
  return false;
} // lexComp

bool SGSolver_MaxMinMax::lexAbove(const SGPoint & a, const SGPoint & b) const
{
  if ( a*b>env.getParam(SG::LEXIMPROVETOL) )
    return true;
  else if ( a*b>-1.0*env.getParam(SG::LEXSUBOPTOL) )
    {
      const SGPoint c(b[1],-b[0]);
      if (a*c > env.getParam(SG::LEXIMPROVETOL) )
	return true;
    }
  return false;
} // lexAbove

bool SGSolver_MaxMinMax::computeBestBindingPayoff(const SGActionIter ait,
						  int & bestBindingPlayer,
						  int & bestBindingPoint,
						  const SGPoint & dir) const
{
  bestBindingPlayer = -1;
  for (int p = 0; p < numPlayers; p++)
    {
      for (int k = 0; k < ait->getPoints()[p].size(); k++)
	{
	  if (bestBindingPlayer<0
	      || lexComp(ait->getPoints()[p][k],0,
			 ait->getPoints()[bestBindingPlayer][bestBindingPoint],0,
			 dir) )
	    {
	      bestBindingPlayer = p;
	      bestBindingPoint = k;
	    }
	} // point
    } // player

  if (lexAbove(ait->getBndryDirs()[bestBindingPlayer][bestBindingPoint],
	       dir) ) // Can improve on the best
    // binding payoff by moving in
    // along the frontier
    {
      return true;
    }
  return false;
} // computeBestBindingPayoff

void SGSolver_MaxMinMax::minimizeRegimes(SGTuple & pivot,
					 vector<double> & penalties,
					 const vector<SGActionIter> & actionTuple,
					 vector<SG::Regime> & regimeTuple,
					 const SGPoint & dir,
					 const SGTuple & bestBindingPayoffs,
					 const vector<bool> & bestAPSNotBinding) const
{
  // Minimize regimes, only changes from binding to non-binding
  bool regimesChanged;
  // Keep track of switch to non-binding, which is irreversible.
  vector<bool> switchToNonBinding(numStates,false);
  
  do
    {
      policyToPayoffs(pivot,actionTuple,regimeTuple);
      policyToPenalties(penalties,actionTuple,regimeTuple);

      regimesChanged = false;
      for (int state = 0; state < numStates; state++)
	{
	  if (bestAPSNotBinding[state])
	    {
	      if (regimeTuple[state] == SG::Binding)
		{
		  regimeTuple[state] = SG::NonBinding;
		  regimesChanged = true;
		}
	      continue;
	    }
	  
	  SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
	    [actionTuple[state]->getAction()]
	    + delta * pivot.expectation(probabilities[state][actionTuple[state]->getAction()]);
	  double nonBindingPenalty = env.getParam(SG::SUBGENFACTOR);
	  for (int sp = 0; sp < numStates; sp++)
	    nonBindingPenalty += delta*probabilities[state][actionTuple[state]->getAction()][sp]*penalties[sp];
	  
	  if (regimeTuple[state] == SG::Binding
	      && lexComp(pivot[state],penalties[state],
			 nonBindingPayoff,nonBindingPenalty,
			 dir))
	    {
	      switchToNonBinding[state] = true; 
	      regimeTuple[state] = SG::NonBinding;
	      regimesChanged=true;
	    }
	  else if (!switchToNonBinding[state]
		   && regimeTuple[state] == SG::NonBinding
		   && lexComp(pivot[state],penalties[state],
			      bestBindingPayoffs[state],env.getParam(SG::SUBGENFACTOR),
			      dir))
	    {
	      regimesChanged = true;
	      regimeTuple[state] = SG::Binding;
	      pivot[state] = bestBindingPayoffs[state];
	    }
	}

    } while (regimesChanged);

} // minimizeRegimes

double SGSolver_MaxMinMax::sensitivity(const SGTuple & pivot,
				       const vector<double> & penalties,
				       const vector<SGActionIter> & actionTuple,
				       const vector<SG::Regime> & regimeTuple,
				       const SGPoint currDir,
				       const vector<list<SGAction_MaxMinMax> > & actions) const
{
  SGPoint normDir = -1.0*currDir.getNormal(); // Rotate the direction clockwise by pi/2 radians
  
  
  double nonBindingIndiffLvl = -1;
  double bindingIndiffLvl = -1;
  double bestLevel = numeric_limits<double>::max()-1.0;
  
  bool doInner = env.getParam(SG::SUBGENFACTOR)>0;
  double bindingPenalty = 0;
  if (doInner)
    bindingPenalty = env.getParam(SG::SUBGENFACTOR);

  int bestBindingPlayer,bestBindingPoint;
  
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
	  double nonBindingPenalty = 0.0;
	  if (doInner)
	    {
	      nonBindingPenalty = env.getParam(SG::SUBGENFACTOR);
	      for (int sp = 0; sp < numStates; sp++)
		nonBindingPenalty += delta*probabilities[state][ait->getAction()][sp]*penalties[sp];
	    }

	  // Calculate the lvl at which indifferent to the pivot
	  // pivot[state]*(currDir+tmp*normDir)-penalties[state]<=nonBindingPayoff*(currDir+tmp*normDir)-nonBindingPenalty;
	  // (pivot[state]-nonBindingPayoff)*currDir-(penalties[state]-nonBindingPenalty))<=-tmp*normDir*(pivot[state]-nonBindingPayoff)
	  double denom = normDir*(nonBindingPayoff-pivot[state]);
	  double numer = (pivot[state]-nonBindingPayoff)*currDir
	    -(penalties[state]-nonBindingPenalty);
	  if (SGPoint::distance(pivot[state],nonBindingPayoff) > 1e-10
	      && abs(denom) > 1e-10)
	    {
	      nonBindingIndiffLvl = numer/denom;

	      if (nonBindingIndiffLvl < bestLevel
		  && nonBindingIndiffLvl > 1e-12)
		{
		  SGPoint indiffDir = currDir + normDir * nonBindingIndiffLvl;

		  // See if a binding payoff is higher in the
		  // indifference direction
		  double bestBindLvl = -numeric_limits<double>::max();
		  bool bestAPSNotBinding=computeBestBindingPayoff(ait,bestBindingPlayer,
								  bestBindingPoint,indiffDir);
		  SGPoint bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		    + delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];


		  if ( bestAPSNotBinding // NB bestAPSPayoff has only been
		       // set if bestAPSNotBinding ==
		       // true
		       || lexComp(bestAPSPayoff,bindingPenalty,
				  nonBindingPayoff,nonBindingPenalty,
				  indiffDir) ) 
		    {
		      // If we get to here, non-binding regime is
		      // available in the indifferent direction, and
		      // this direction is smaller than the best level
		      // found so far.

		      if ( (ait != actionTuple[state] && denom> 1e-10)
			   || (ait == actionTuple[state]
			       && denom < -1e-10
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
		  double numer = (pivot[state]-bindingPayoff)*currDir
		    - (penalties[state]-bindingPenalty);
		  if (SGPoint::distance(pivot[state],bindingPayoff)>1e-6
		      && abs(denom) > 1e-10)
		    {
		      bindingIndiffLvl = numer/denom;

		      if (bindingIndiffLvl < bestLevel
			  && bindingIndiffLvl > 1e-12)
			{
			  SGPoint indiffDir = currDir + normDir * bindingIndiffLvl;

			  if (nonBindingPayoff*indiffDir-nonBindingPenalty
			      >= bindingPayoff*indiffDir-bindingPenalty-1e-6)
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


void SGSolver_MaxMinMax::policyToPayoffs(SGTuple & pivot,
					 const vector<SGActionIter>  & actionTuple,
					 const vector<SG::Regime> & regimeTuple)
  const
{
  // Do Bellman iteration to find new fixed point
  int updatePasses = 0;
  double bellmanGap = 0;
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
      bellmanGap = SGTuple::distance(newPivot,pivot);
      pivot = newPivot;
    } while (bellmanGap > env.getParam(SG::UPDATEPIVOTTOL)
	     && (++updatePasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));

  if (updatePasses == env.getParam(SG::MAXUPDATEPIVOTPASSES) )
    cout << "WARNING: Maximum pivot update passes reached." << endl;
  
} // policyToPayoffs

void SGSolver_MaxMinMax::policyToPenalties(vector<double> & penalties,
					   const vector<SGActionIter>  & actionTuple,
					   const vector<SG::Regime> & regimeTuple)
  const
{
  // Do Bellman iteration to find new fixed point
  assert(penalties.size()==numStates);
  
  int updatePasses = 0;
  double bellmanGap = 0;

  do
    {
      bellmanGap = 0.0;

      vector<double> newPenalties(numStates,env.getParam(SG::SUBGENFACTOR));
      for (int state = 0; state < numStates; state++)
	{
	  if (regimeTuple[state] == SG::NonBinding)
	    {
	      for (int sp = 0; sp < numStates; sp++)
		newPenalties[state] += delta*probabilities[state]
		  [actionTuple[state]->getAction()][sp]*penalties[sp];

	    }
	  bellmanGap = max(bellmanGap,abs(newPenalties[state]-penalties[state]));
	}
      penalties = newPenalties;
    } while (bellmanGap > env.getParam(SG::UPDATEPIVOTTOL)
	     && (++updatePasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));

  if (updatePasses == env.getParam(SG::MAXUPDATEPIVOTPASSES) )
    cout << "WARNING: Maximum pivot update passes reached." << endl;
  
} // policyToPenalties
