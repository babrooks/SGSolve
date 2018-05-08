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

#include "sgapprox_v2.hpp"

void SGApprox_V2::end()
{
  // testOFS.close();
  logfs.close();
}

void SGApprox_V2::initialize()
{
  int state, iter, action;

  logfs.open("sg_v2.log",std::ofstream::out);
  // testOFS.open("test.log",std::ofstream::out);

  game.getPayoffBounds(payoffUB,payoffLB);

  actions = vector< list<SGAction> > (numStates);

  const vector< list<int> > & eqActions = game.getEquilibriumActions();
  // Create the intersection arrays.
  for (state = 0; state < numStates; state++)
    {
      if (eqActions.size()==numStates && eqActions[state].size()>0)
	{
	  for (list<int>::const_iterator actionIter = eqActions[state].begin(); 
	       actionIter != eqActions[state].end(); 
	       ++actionIter)
	    actions[state].push_back(SGAction(env,state,*actionIter));
	}
      else
	for (action = 0; action < game.getNumActions_total()[state]; action++)
	  actions[state].push_back(SGAction(env,state,action));
    } // state

  pivot = SGTuple(numStates);
  actionTuple = vector< const SGAction* >(numStates,&nullAction);
  regimeTuple = vector<SG::Regime>(numStates,SG::Binding);
  threatTuple = SGTuple(numStates,payoffLB);

  // Initialize feasible set
  W.clear(); W.reserve(env.getParam(SG::TUPLERESERVESIZE));
  W.push_back(SGHyperplane(SGPoint(1.0,0.0),
			   vector<double>(numStates,payoffUB[0])));
  W.push_back(SGHyperplane(SGPoint(0.0,1.0),
			   vector<double>(numStates,payoffUB[1])));
  W.push_back(SGHyperplane(SGPoint(-1.0,0.0),
			   vector<double>(numStates,-payoffLB[0])));
  W.push_back(SGHyperplane(SGPoint(0.0,-1.0),
			   vector<double>(numStates,-payoffLB[1])));

  if (env.getParam(SG::PRINTTOLOG))
    {
      for (int point=0; point < W.size(); point++)
	logAppend(logfs,0,0,W[point],0,0);
    }

  // Initialize the currentDirection and pivot.
  currentDirection = SGPoint(payoffUB[0]-payoffLB[0],0.0);
  pivot = SGTuple(numStates,SGPoint(payoffLB[0],payoffUB[1]));
  currentLevels = vector<double> (numStates,payoffUB[1]);

  numIterations = 0; 
  numSteps = -1;
  bestRegime = SG::Binding;
  bestAction = actions[0].end();

  // if (env.getParam(SG::STOREITERATIONS) == 2)
  //   soln.push_back(SGIteration(*this,
  // 			       env.getParam(SG::STOREACTIONS)));
  numSteps = 0;

} // initialize

void SGApprox_V2::logAppend(ofstream & logfs, 
			 int step, int iter, const SGHyperplane & hp,
			 int state, int action)
{
  logfs << setw(3) << iter << " " << setw(3) << step << " "
	<< hp << " " << setw(3) << state << " " << setw(3) << action << endl;
}

double SGApprox_V2::generate(bool storeIterations)
{
  cout << endl << "Start of iteration " << numIterations << endl << endl;
  
  // Four steps. First, calculate binding continuation values
  calculateBindingContinuations();

  cout << "Payoff UB: " << payoffUB << ", payoff LB: " << payoffLB << endl;
  cout << "Threats: " << threatTuple << endl << endl;
  
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  cout << "(s,a)=("<< ait->getState() << "," << ait->getAction() << "): "
	       << ait->getPoints()[0] << ","
	       << ait->getPoints()[1] << ", minIC="
	       << ait->getMinICPayoffs() << endl;
	} // ait
    } // state

  cout << endl << "Finding initial pivot..." << endl;
  // Find initial pivot
  findInitialPivot();
  currentDirection = SGPoint (1,0);
  currentNormal = currentDirection.getNormal();
  for (int state = 0; state < numStates; state++)
    currentLevels[state] = pivot[state][1];

  Wp.clear();
  // Wp.push_back(SGHyperplane(currentNormal,currentLevels));

  cout << "Pivot: " << pivot << endl
       << "Actions: (" << actionTuple[0]->getAction() << ", "
       << actionTuple[1]->getAction() << ")" << endl;

  
  
  cout << endl << "Computing W'..." << endl;
  // Find the new direction
  double minDist = numeric_limits<double>::max();
  do
    {

      cout << "Finding the best direction..." << endl;
      findBestDirection();
      assert(bestAction->getState() < game.getNumStates());
      assert(bestAction->getAction() < game.getNumActions_total()[bestAction->getState()]);

      cout << "Calculating new pivot..." << endl;
      
      calculateNewPivot();

      for (int state = 0; state < numStates; state++)
	currentLevels[state] = pivot[state]*currentNormal;
      Wp.push_back(SGHyperplane(currentNormal,currentLevels));

      for (int state = 0; state < numStates; state++)
      	logfs << pivot[state][0] << " " << pivot[state][1] << " ";
      logfs << endl;
      
      cout << "(s,a,r)=";
      for (int state = 0; state < numStates; state++)
	{
	  cout << "(" << state << "," << actionTuple[state]->getAction();
	  if (regimeTuple[state]==SG::Binding)
	    cout << ", binding), ";
	  else
	    cout << ", non-binding), ";
	}
      cout << endl;
      cout << "New hyperplane: " << Wp.back() << endl;
      cout << "New pivot: " << pivot << endl;

      if (Wp.size() > 2)
	{
	  minDist = numeric_limits<double>::max();
	  for (int k = 0; k < Wp.size()-2; k++)
	    minDist = min(minDist,SGHyperplane::distance(Wp.back(),Wp[k]));
	  cout << "Minimum distance: " << minDist << endl << endl;
	}
      
      // if (env.getParam(SG::PRINTTOLOG))
      // 	{
      // 	  logAppend(logfs,numSteps,numIterations,hyperplane,
      // 		    bestAction->getState(),bestAction->getAction());
      // 	}
      
    } while( Wp.size()<4
	     || minDist > 1e-10);
  
  // if (storeIterations)
  //   {
  //     soln.push_back(SGIteration(*this,
  // 				 env.getParam(SG::STOREACTIONS)));
  //     assert(soln.getIterations().back().getBestAction()
  // 	     < game.getNumActions_total()[soln.getIterations().back().getBestState()]);
  //   }
  
  errorLevel = distance();
  W=Wp;
  
  numIterations++;
  
  return errorLevel;
  
} // generate

void SGApprox_V2::findInitialPivot()
{
  // Start with upper bounds on the highest payoff for all states.
  SGPoint normal (0,1);
  vector<double> levels(numStates,payoffUB[1]+1.0); 
  pivot = SGTuple(numStates,SGPoint(payoffLB[0],payoffLB[1]-1.0));

  double pivotError = 1.0;
  // Iterate until convergence
  while (pivotError > env.getParam(SG::UPDATEPIVOTTOL))
    {
      SGTuple newPivot = pivot;
      
      for (int state = 0; state < numStates; state++)
	{
	  // Iterate over all actions to find the one that generates
	  // the highest payoff for player 2.
	  
	  double bestLevel = -numeric_limits<double>::max();
	  
	  for (auto ait = actions[state].begin();
	       ait != actions[state].end();
	       ++ait)
	    {
	      // Take the lower of the binding and non-binding
	      // levels. If the lower of these two is greater than the
	      // best level found so far, switch the pivot.

	      // The binding level
	      SGPoint bindingPayoff = (1-delta)*game.getPayoffs()[state]
		[ait->getAction()]
		+ delta * ait->getHighestPoint();

	      SGPoint nonBindingPayoff = (1-delta)*game.getPayoffs()[state]
		[ait->getAction()]
		+ delta * pivot.expectation(game.getProbabilities()[state][ait->getAction()]);
	      
	      if (min(bindingPayoff[1],nonBindingPayoff[1]) > bestLevel)
		{
		  actionTuple[state] = &(*ait);
		  if (bindingPayoff[1] < nonBindingPayoff[1])
		    {
		      bestLevel = bindingPayoff[1];
		      regimeTuple[state] = SG::Binding;
		      newPivot[state] = bindingPayoff;
		    }
		  else
		    {
		      bestLevel = nonBindingPayoff[1];
		      regimeTuple[state] = SG::NonBinding;
		      newPivot[state] = nonBindingPayoff;
		    }
		} // New best action
	    } // ait
	} // state

      pivotError = SGTuple::distance(newPivot,pivot);

      // Now do Bellman iteration to find new fixed point
      int updatePivotPasses = 0;
      do
	{
	  pivot = newPivot;
	  for (int state = 0; state < numStates; state++)
	    {
	      if (regimeTuple[state] == SG::NonBinding)
		newPivot[state] = (1-delta)*game.getPayoffs()[state]
		  [actionTuple[state]->getAction()]
		  + delta*pivot.expectation(game.getProbabilities()[state]
					    [actionTuple[state]->getAction()]);
	    }
	} while (SGTuple::distance(newPivot,pivot) > env.getParam(SG::UPDATEPIVOTTOL)
		 && (++updatePivotPasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));

      // pivot = newPivot;
      // cout << "Initial pivot: " << newPivot << ", pivotError: " << pivotError <<  endl;
    } // policy iteration
  
} // findInitialPivot

std::string SGApprox_V2::progressString() const
{
  // Add up total number of actions
  int numActsRemaining = 0;
  for (int state = 0; state < numStates; state++)
    numActsRemaining += actions[state].size();

  std::stringstream ss;
  ss << "Error level: " << errorLevel
     << ", iter/step: " << numIterations << "/" << numSteps
     << ", numHyperPlanes: " << Wp.size()
     << ", numActionsRemaining: " << numActsRemaining;

  return ss.str();
}

void SGApprox_V2::findBestDirection()
{
  // Search for the next best direction.
  int state;

  double reverseDirTol = 1e-6;
  
  
  bestAction = actions[0].end(); // use end as the default value for bestAction
  bestRegime = SG::Binding;

  double currentNorm = currentDirection.norm();

  bestDirection = -1.0*currentDirection;
  bestNormal = -1.0*currentNormal;
  // Search over all actions to find the one that generates the
  // shallowest normal that is clockwise relative to the current
  // normal. 
  for (state = 0; state < numStates; state++)
    {
      for (list<SGAction>::const_iterator ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  // Find the best direction for this action, i.e., the
	  // smallest clockwise rotation from the current normal. 

	  // Non-binding payoff
	  const SGPoint & stagePayoff  = game.getPayoffs()[state][ait->getAction()];
	  const vector<double> & prob = game.getProbabilities()[state][ait->getAction()];
	  SGPoint nonBindingPayoff = ( (1-delta)*stagePayoff
				       + delta*pivot.expectation(prob) );
	  SGPoint nonBindingDirection = nonBindingPayoff - pivot[state];
	  SGPoint nonBindingNormal = nonBindingDirection.getNormal();

	  bool nonBindingReverseDirFlag = false;

	  // Flip the direction of the norm if we have bended backwards
	  if (nonBindingDirection * currentNormal > reverseDirTol
	      || (abs(nonBindingDirection*currentNormal) < reverseDirTol
		  && nonBindingDirection * currentNormal.getNormal() > reverseDirTol) )
	    {
	      nonBindingNormal *= -1.0;
	      nonBindingReverseDirFlag = true;
	    }

	  // We can only use the non-binding direction if the
	  // non-binding payoff is "below" the best binding direction.

	  bool nonBindingIsMinimal = false;
	  bool bindingAbovePivotFlag = false;
	  bool dropActionFlag = false;
	  
	  SGPoint bestBindingPayoff;
	  SGPoint bestBindingDirection;
	  SGPoint bestBindingNormal = -1.0*currentNormal;
	  // Binding payoffs
	  for (int p = 0; p < numPlayers; p++)
	    {
	      for (int c = 0; c < ait->getPoints()[p].size(); c++)
		{
		  SGPoint bindingPayoff = ( (1-delta)*stagePayoff
					    + delta*ait->getPoints()[p][c] );
		  SGPoint bindingDirection = bindingPayoff - pivot[state];
		  SGPoint bindingNormal = bindingDirection.getNormal();
		  if (bindingDirection*currentNormal > reverseDirTol
		      || (abs(bindingDirection*currentNormal) < reverseDirTol
			  && bindingDirection * currentNormal.getNormal() > reverseDirTol) )
		    bindingNormal *= -1.0;

		  if (bindingDirection * currentNormal > env.getParam(SG::IMPROVETOL))
		    {
		      // Either pivot is contained in feasible set or
		      // else action is infeasible. If we have
		      // containment, it must be that the non-binding
		      // payoff is below the pivot. 
		      bindingAbovePivotFlag = true;
		    }
		  else if (bindingPayoff * bestBindingNormal > env.getParam(SG::IMPROVETOL))
		    {
		      bestBindingNormal = bindingNormal;
		      bestBindingPayoff = bindingPayoff;
		      bestBindingDirection = bindingDirection;
		    }
		} // continuation value
	    } // player

	  if (bindingAbovePivotFlag)
	    {
	      // Must be that non-binding payoff is (weakly) below the
	      // pivot, or else action is infeasible.
	      if (isBelow(nonBindingDirection,currentNormal)
		  && improves(currentNormal,bestNormal,nonBindingNormal))
		{
		  bestNormal = nonBindingNormal;
		  bestAction = ait;
		  bestDirection = nonBindingDirection;
		  bestRegime = SG::NonBinding;
		}
	    }
	  else
	    {
	      // Binding payoffs are all below the current normal
	      if ( isBelow(nonBindingDirection,bestBindingNormal)
		   && improves(currentNormal,bestNormal,nonBindingNormal))
		{
		  bestNormal = nonBindingNormal;
		  bestAction = ait;
		  bestDirection = nonBindingDirection;
		  bestRegime = SG::NonBinding;
		}
	      else if (improves(currentNormal,bestNormal,bestBindingNormal))
		{
		  bestNormal = bestBindingNormal;
		  bestAction = ait;
		  bestDirection = bestBindingDirection;
		  bestRegime = SG::Binding;
		}
	    }
	  
	  // if (!bindingAbovePivotFlag
	  //     && bestBindingDirection * nonBindingNormal < -reverseDirTol
	  //     && improves(currentNormal,bestNormal,bestBindingNormal))
	  //   {
	  //     bestNormal = bestBindingNormal;
	  //     bestAction = ait;
	  //     bestDirection = bestBindingDirection;
	  //     bestRegime = SG::Binding;
	  //   }
	  // else if ( (nonBindingIsMinimal
	  // 	    || bestBindingDirection * nonBindingNormal > reverseDirTol)
	  // 	    && improves(currentNormal,bestNormal,nonBindingNormal) )
	  //   {
	  //     bestNormal = nonBindingNormal;
	  //     bestAction = ait;
	  //     bestDirection = nonBindingDirection;
	  //     bestRegime = SG::NonBinding;
	  //   }
	} // action
    } // state
  
  if (bestAction == actions[0].end())
    throw(SGException(SG::NO_ADMISSIBLE_DIRECTION));
  
} // findBestDirection

bool SGApprox_V2::isBelow(const SGPoint & dir,
			  const SGPoint & norm) const
{
  return (dir*norm < -env.getParam(SG::IMPROVETOL)
	  || ( abs(dir*norm) < env.getParam(SG::IMPROVETOL)
	       && dir*norm.getNormal() < -env.getParam(SG::IMPROVETOL) ) );
}

bool SGApprox_V2::improves(const SGPoint & current, 
			const SGPoint & best, 
			const SGPoint & newDirection) const
{
  SGPoint newNormal = newDirection.getNormal();
  double currentNorm = current.norm();
  double newNorm = newDirection.norm();
  double level  =  newNormal * current/sqrt(newNorm)/sqrt(currentNorm);

  return ( level > env.getParam(SG::IMPROVETOL)
	   || ( level > -env.getParam(SG::IMPROVETOL)
		&& newDirection*current > 0.0 ) )
    && (newNormal * best/sqrt(newNorm)/sqrt(best.norm()) 
	< env.getParam(SG::IMPROVETOL)); // This broke the kocherlakota example, but
  // made the PD example and others work
  // beautifully (used to be -env.getParam(SG::IMPROVETOL))
} // improves

void SGApprox_V2::calculateNewPivot()
{
  int updatePivotPasses = 0;

  int state, player;

  regimeTuple[bestAction->getState()] = bestRegime;
  actionTuple[bestAction->getState()] = &(*bestAction);

  currentDirection = bestDirection;
  currentNormal = bestNormal/bestNormal.norm();

  // First construct maxMovement array.
  vector<double> movements(numStates,0.0);
  
  double tempMovement;
      
  movements[bestAction->getState()] = 1.0;
  vector<double> changes(movements);

  while (updatePivot(movements,changes,regimeTuple)
	 > env.getParam(SG::UPDATEPIVOTTOL)
	 && (++updatePivotPasses < env.getParam(SG::MAXUPDATEPIVOTPASSES)))
    {}
  if (updatePivotPasses >= env.getParam(SG::MAXUPDATEPIVOTPASSES))
    throw(SGException(SG::TOO_MANY_PIVOT_UPDATES));
  
  double maxDistance = 0;
  for (state=0; state < numStates; state++)
    {
      maxDistance = std::max(maxDistance,movements[state]);
      pivot[state] += movements[state]*currentDirection;
    }

  // Add check here that pivot is self-generated (in the
  // non-binding case)
  for (state=0; state < numStates; state++)
    {
      if (regimeTuple[state]==SG::NonBinding)
	{
	  SGPoint tempPayoff( (1-delta)
			      *game.getPayoffs()[state][actionTuple[state]->getAction()] 
			      +delta*pivot.expectation(game.getProbabilities()[state][actionTuple[state]->getAction()]) );
	  
	  assert( SGPoint::distance(tempPayoff, pivot[state]) < 1e-8 );
	  if ( SGPoint::distance(tempPayoff, pivot[state]) > 1e-5 )
	    cout << "Error. Non-binding pivot does not self generate. Distance: "
		 << SGPoint::distance(tempPayoff, pivot[state]) << endl;
	}
    }
  
  pivot.roundTuple(env.getParam(SG::ROUNDTOL));

} // calculateNewPivot

double SGApprox_V2::updatePivot(vector<double> & movements, 
				vector<double> & changes,
				vector<SG::Regime> & regimeTuple)
{
  // Solve forward the system of equations implied by regimeTuple
  // and actionTuple. If an IC constraint is violated by the forward
  // solution, set that element of the pivot equal to the binding
  // constraint.
  double newError = 0.0;

  vector<double> tempChange(numStates,0.0), tempMovement(numStates,0.0);

  for (int state = 0; state < numStates; state++)
    {
      if (regimeTuple[state]!=SG::NonBinding)
	continue;

      assert(regimeTuple[state]==SG::NonBinding);

      for (int statep = 0; statep < numStates; statep++)
	{
	  tempChange[state] += delta* 
	    game.getProbabilities()[state][actionTuple[state]->getAction()][statep]
	    * changes[statep];
	}
    }

  for (int state=0; state < numStates; state++)
    {
      movements[state] += tempChange[state];
      changes[state] = tempChange[state];

      newError = max(newError,changes[state]);
    }

  return newError;
} // updatePivot

double SGApprox_V2::distance() const
{
  if (numIterations<2)
    return 1.0;

  double newError = 0.0;

  for (auto hp_old = W.begin(); hp_old != W.end() ; hp_old++)
    {
      double distToCurrentIter = numeric_limits<double>::max();
      double tempDist;
      for (auto hp_new = Wp.begin(); hp_new != Wp.end() ; hp_new++)
	{
	  // Find perpendicular to the line between point and point-1
	  // that goes through oldPoint.

	  tempDist = SGHyperplane::distance(*hp_old,*hp_new);

	  if (tempDist <= distToCurrentIter)
	    distToCurrentIter = tempDist;
	} // for new hp

      if (tempDist <= distToCurrentIter)
	distToCurrentIter = tempDist;

      if (distToCurrentIter >= newError)
	newError = distToCurrentIter;
    } // for old hp

  return newError;
}

void SGApprox_V2::calculateBindingContinuations() 
{
  // Calculates the IC intersection points. To be used after updating
  // the threat tuple.
  int state, tupleIndex;
  vector<int> intsctTuples;
  vector<double> xbounds(2), ybounds(2);

  bool anyUpdate = false;

  for (state = 0; state < numStates; state++)
    {
      list<SGAction>::iterator action = actions[state].begin();
      while (action != actions[state].end())
	{
	  for (int player = 0; player < numPlayers; player++)
	    {
	      if (game.getConstrained()[player])
		continue;
	      anyUpdate = true;
	    }
	  
	  action->calculateMinIC(game,
				 vector<bool>(numPlayers,true),
				 threatTuple);
	  action->calculateBindingContinuations(game,W);
	  
	  // Drop the point if no longer IC
	  if ((action->getPoints()[0].size() == 0 && !game.getConstrained()[0])
	      && (action->getPoints()[1].size() == 0 && !game.getConstrained()[1]))
	    {
	      if (actionTuple[state] == &(*action))
		regimeTuple[state] = SG::Binding;
	      actions[state].erase(action++);
	    }
	  else
	    action++;
	} // action
    } // state
} // calculateIntersections
  
