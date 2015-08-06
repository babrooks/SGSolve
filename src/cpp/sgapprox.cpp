#include "sgapprox.hpp"

void SGApprox::end()
{
  logfs.close();
}

void SGApprox::initialize()
{
  int state, iter, action;

  oldWest = 0; oldOldWest = 0; westPoint = 0; newWest = 0;

  logfs.open("sg.log",std::ofstream::out);

  numIterations = 0; numRevolutions = 0;

  SGPoint payoffUB, payoffLB;
  game.getPayoffBounds(payoffUB,payoffLB);

  actions = vector< list<SGAction> > (numStates);
  // Create the intersection arrays.
  for (state = 0; state < numStates; state++)
    {
      if (game.eqActions.size()==numStates 
	  && game.eqActions[state].size()>0)
	{
	  for (list<int>::const_iterator actionIter = game.eqActions[state].begin(); 
	       actionIter != game.eqActions[state].end(); 
	       ++actionIter)
	    actions[state].push_back(SGAction(env,state,*actionIter));
	}
      else
	for (action = 0; action < game.numActions_total[state]; action++)
	  actions[state].push_back(SGAction(env,state,action));
    } // state

  pivot = SGTuple(numStates);
  SGAction nullAction(env);
  actionTuple = vector< const SGAction* >(numStates,&nullAction);
  regimeTuple = vector<SG::Regime>(numStates,SG::Binding);
  threatTuple = SGTuple(numStates,payoffLB);

  // Initialize extremeTuples
  extremeTuples.clear(); extremeTuples.reserve(env.tupleReserveSize);
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffUB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffUB[0],payoffUB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffUB[0],payoffLB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffLB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffUB[1]))); 

  if (env.printToLog)
    {
      for (int point=0; point < extremeTuples.size(); point++)
	logAppend(logfs,0,0,extremeTuples[point],0,0);
    }

  facingEastNorth = vector<bool>(2,true);
  westPoint = 0;
  newWest = -1; 

  // Set flags to build the initial IC array.
  updatedThreatTuple = vector<bool>(2,true);

  // Initialize the currentDirection and pivot.
  currentDirection = SGPoint(payoffUB[0]-payoffLB[0],0.0);
  pivot = extremeTuples[0];
  
  if (env.storeIterations)
    soln.push_back(SGIteration(-1,0,
			       extremeTuples.size(),
			       -1,SG::Binding,
			       pivot,currentDirection,
			       actionTuple,regimeTuple,
			       threatTuple));
  
} // initialize

void SGApprox::logAppend(ofstream & logfs, 
			 int iter, int rev, const SGTuple & tuple,
			 int state, int action)
{
  logfs << setw(3) << rev << " " << setw(3) << iter << " "
	<< tuple << " " << setw(3) << state << " " << setw(3) << action << endl;
}

double SGApprox::generate()
{
  // Four steps. First, update the minimum IC continuation values
  updateMinPayoffs();

  // Next, calculate binding continuation values
  calculateBindingContinuations();

  // Trim the binding continuation values;
  // trimBindingContinuations();

  // Find the new direction
  findBestDirection();
  
  // Update the pivot.
  calculateNewPivot();
  
  // Final steps before next iteration. Set flags for updating binding
  // continuation values.
  updateFlags();
  
  if (env.storeIterations)
    soln.push_back(SGIteration(numIterations,numRevolutions,
			       extremeTuples.size(),
			       bestAction->getState(),bestRegime,
			       pivot,bestDirection,
			       actionTuple,regimeTuple,
			       threatTuple));

  if (passNorth)
    {
      // for (int state = 0; state < numStates; state++)
      // 	{
      // 	  for (list<SGAction>::iterator action = actions[state].begin();
      // 	       action != actions[state].end();
      // 	       ++action)
      // 	    action->updateTrim();
      // 	}

      errorLevel = distance(westPoint, newWest, oldWest, westPoint);

      if (env.printToCout)
	cout << progressString() << endl; 
      
      numRevolutions++;
      numIterations++;
    }
  else
    {
      numIterations++;
    }
  return errorLevel;

} // generate

std::string SGApprox::progressString() const
{
  // Add up total number of actions
  int numActsRemaining = 0;
  for (int state = 0; state < numStates; state++)
    numActsRemaining += actions[state].size();

  std::stringstream ss;
  ss << "Error level: " << errorLevel
     << ", rev/iter: " << numRevolutions << "/" << numIterations
     << ", numExtremePoints: " << newWest-westPoint
     << ", numActionsRemaining: " << numActsRemaining;

  return ss.str();
}

void SGApprox::findBestDirection()
{
  // Search for the next best direction.
  int state;

  bestAction = NULL;
  bestRegime = SG::Binding;

  bestDirection = -1.0*currentDirection;
  
  SGPoint currentNormal = currentDirection.getNormal();
  double currentNorm = currentDirection.norm();

  for (state = 0; state < numStates; state++)
    {
      for (list<SGAction>::const_iterator action = actions[state].begin();
	   action != actions[state].end();
	   ++action)
	{
	  SG::Regime bestBindingRegime = SG::Binding;

	  SGPoint expPivot = pivot.expectation(game.probabilities[state][action->action]);
	  SGPoint stagePayoff = game.payoffs[state][action->action];
	  SGPoint nonBindingPayoff = (1-delta) * stagePayoff + delta * expPivot;
	  SGPoint nonBindingDirection = nonBindingPayoff - pivot[state];
	  double nonBindingNorm = nonBindingDirection.norm();
	  
	  // if ( !improves(currentDirection,bestDirection,nonBindingDirection)
	  //      && nonBindingNorm > env.normTol )
	  //   continue;

	  if (expPivot >= action->minIC
	      && nonBindingNorm > env.normTol)
	    {
	      if (env.backBendingWarning
		  && currentNormal*nonBindingDirection
		  / sqrt(currentNorm*nonBindingNorm)
		  >= env.backBendingTol)
	      	cout << "Warning: Detected back-bending direction" << endl;

	      if (improves(currentDirection,bestDirection,nonBindingDirection))
		{
		  bestDirection = nonBindingDirection;
		  bestAction = &(*action);
		  bestRegime = SG::NonBinding;

		  continue;
		}
	    }
	  
	  bool available(false), foundAbove(false), foundBelow (false);
	  SGPoint aboveDirection, belowDirection;

	  for (int player = 0; player < numPlayers; player++)
	    {
	      if ( action->points[player].size() == 0
		   || expPivot[player] >= action->minIC[player])
		continue;
	      
	      SGTuple bindingPayoffs = delta*action->points[player] 
		+ (1-delta)*stagePayoff;
	      SGTuple bindingDirections = bindingPayoffs - pivot[state];

	      SGPoint nonBindingNormal = nonBindingDirection.getNormal();
	      for (int point = 0; point < bindingDirections.size(); point++)
		{
		  double bindingNorm = bindingDirections[point].norm();
		  double bindingLevel = (bindingDirections[point]*nonBindingNormal)
		    / std::sqrt(nonBindingNorm * bindingNorm);
		      
		  if (env.backBendingWarning
		      && currentNormal*bindingDirections[point]
		      / std::sqrt(currentNorm
				  *bindingDirections[point].norm())
		      >= env.backBendingTol)
		    cout << "Warning: Detected back-bending direction" << endl;

		  if (nonBindingNorm > env.normTol
		      && bindingNorm > env.normTol)
		    {
		      if (bindingLevel < env.levelTol
			  && (!foundBelow 
			      || improves(nonBindingDirection,belowDirection,
					  bindingDirections[point]) ) )
			{
			  belowDirection = bindingDirections[point];
			  if (point==1 && action->corner)
			    bestBindingRegime = SG::Binding01;
			  else if (player==0)
			    bestBindingRegime = SG::Binding0;
			  else if (player==1)
			    bestBindingRegime = SG::Binding1;
			  foundBelow = true;
			}
		      if (bindingLevel > -env.levelTol
			  && (!foundAbove
			      || improves(aboveDirection,nonBindingDirection,
					  bindingDirections[point]) ) )
			{
			  aboveDirection = bindingDirections[point];
			  foundAbove = true;
			}
		      else if ((point==0 && player == 0) ||
			       (point == 1 && player == 1 && !action->corner))
			{
			  // Also determine slope of feasible set clockwise
			  // relative to the binding payoff.
			  int nextPoint = action->tuples[player][point];
			  while (nextPoint < extremeTuples.size())
			    {
			      SGPoint newExpContVal = extremeTuples[nextPoint]
				.expectation(game.probabilities[state][action->action]);
			      SGPoint nextDirection = (1-delta)*stagePayoff
				+ delta*newExpContVal
				- pivot[state];
			      double nextBindingLevel
				= (nextDirection*nonBindingNormal)
				/ std::sqrt(nonBindingNorm * nextDirection.norm());

			      if (nextBindingLevel > bindingLevel
				  && newExpContVal >= action->minIC)
				{
				  bindingLevel = nextBindingLevel;
				  bindingDirections[point] = nextDirection;

				  if (bindingLevel > -env.levelTol
				      && (!foundAbove
					  || improves(aboveDirection,
						      nonBindingDirection,
						      bindingDirections[point]) ) )
				    {
				      aboveDirection = bindingDirections[point];
				      foundAbove = true;
				      break;
				    }
				  
				  nextPoint++;
				}
			      else // Already reached maximum bindingLevel.
				break;
			    }
			}

		    }

		  if (foundAbove && foundBelow
		      && (aboveDirection * belowDirection.getNormal() 
			  >= env.levelTol
			  || (belowDirection*aboveDirection >= env.levelTol
			      && belowDirection * nonBindingDirection 
			      >= env.levelTol) ) )
		    {
		      available = true;
		      
		      if (&(*action) == &(*actionTuple[state]))
			{
			  SGPoint oldContVal = (pivot[state]-(1-delta)*stagePayoff)
			    /delta;
			  bool pointOut = false;
			  for (int playerp = 0; playerp < numPlayers; playerp++)
			    {
			      if (oldContVal[playerp] <= action->minIC[player]
				  && nonBindingDirection[playerp]<0)
				pointOut=true;
			    }
			  if (pointOut)
			    continue;
			}

		      if (nonBindingNorm > env.normTol
			  && improves(currentDirection,bestDirection,
				      nonBindingDirection))
			{
			  if (env.backBendingWarning
			      && currentNormal*nonBindingDirection
			      /std::sqrt(currentNorm*nonBindingNorm)
			      >= env.backBendingTol)
			    cout << "Warning: Detected back-bending direction" << endl;

			  bestDirection = nonBindingDirection;
			  bestAction = &(*action);
			  bestRegime = SG::NonBinding;
			}
		    } // Non binding direction is feasible
		
		  if (available)
		    break;
		} // point
	    
	      if (available)
		break;

	    } // player

	  if ( !(available || !foundBelow)
	       && improves(currentDirection,bestDirection,belowDirection) )
	    {
	      bestDirection = belowDirection;
	      bestAction = &(*action);
	      bestRegime = bestBindingRegime;
	    }

	} // action
    } // state

  if (bestAction == NULL)
    throw(SGException(SGException::NO_ADMISSIBLE_DIRECTION));

} // findBestDirection

bool SGApprox::improves(const SGPoint & current, 
			const SGPoint & best, 
			const SGPoint & newDirection) const
{
  SGPoint newNormal = newDirection.getNormal();
  double currentNorm = current.norm();
  double newNorm = newDirection.norm();
  double level  =  newNormal * current/sqrt(newNorm)/sqrt(currentNorm);

  return ( level > env.improveTol
	   || ( level > -env.improveTol
		&& newDirection*current > 0.0 ) )
    && (newNormal * best/sqrt(newNorm)/sqrt(best.norm()) 
	< env.improveTol); // This broke the kocherlakota example, but
			   // made the PD example and others work
			   // beautifully (used to be -env.improveTol)
} // improves

void SGApprox::calculateNewPivot()
{
  int updatePivotPasses = 0;

  int state, player;

  regimeTuple[bestAction->state] = bestRegime;
  actionTuple[bestAction->state] = bestAction;
  bool flatDetected = false;
  if (env.mergeTuples)
    flatDetected = bestDirection.getNormal()*currentDirection
      < env.flatTol*sqrt(bestDirection.norm() * currentDirection.norm());
  currentDirection = bestDirection;

  // First construct maxMovement array.
  vector<double> maxMovement(numStates,0);
  vector<SG::Regime> maxMovementConstraints(numStates,SG::Binding0);
  vector<double> movements(numStates,0.0);
  
  double tempMovement;

  maxMovement = vector<double>(numStates,numeric_limits<double>::max());
  
      
  for (player = 0; player < numPlayers; player++)
    {
      if (currentDirection[player]<0)
	{
	  for (int state = 0; state < numStates; state++)
	    {
	      // Calculate how far we can move before we hit this
	      // player's IC constraint.
	      if (regimeTuple[state] != SG::NonBinding)
		continue;

	      tempMovement = (delta*actionTuple[state]->minIC[player]
			      -pivot[state][player]
			      +(1-delta)*game.payoffs[state][actionTuple[state]->action][player])
		/ currentDirection[player];
	      
	      if (tempMovement < maxMovement[state]
		  && tempMovement > 0)
		{
		  maxMovement[state] = tempMovement;
		  if (player==1)
		    maxMovementConstraints[state] = SG::Binding1;
		}
		
	    } // state
	}
    } // player

  movements[bestAction->state] = 1.0;
  vector<double> changes(movements);

  while (updatePivot(movements,changes,regimeTuple,
		     maxMovement,maxMovementConstraints) > env.updatePivotTol
	 && (++updatePivotPasses < env.maxUpdatePivotPasses))
    {}
  if (updatePivotPasses >= env.maxUpdatePivotPasses)
    throw(SGException(SGException::TOO_MANY_PIVOT_UPDATES));
  
  double maxDistance = 0;
  for (state=0; state < numStates; state++)
    {
      maxDistance = std::max(maxDistance,movements[state]);
      pivot[state] += movements[state]*currentDirection;
    }
  
  pivot.roundTuple(env.roundTol);
  if (env.mergeTuples && (flatDetected
			  || maxDistance < env.movementTol) 
      && numIterations>0)
    {
      // cout << "Flat detected!" << endl;
      extremeTuples.back() = pivot;
    }
  else
    extremeTuples.push_back(pivot);

  if (env.printToLog)
    {
      logAppend(logfs,numIterations,numRevolutions,pivot,
		bestAction->state,bestAction->action);
    }
} // calculateNewPivot

double SGApprox::updatePivot(vector<double> & movements, 
			     vector<double> & changes,
			     vector<SG::Regime> & regimeTuple,
			     const vector<double> & maxMovement,
			     const vector<SG::Regime> & maxMovementConstraints)
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

      for (int statep = 0; statep < numStates; statep++)
	{
	  tempChange[state] += delta* 
	    game.probabilities[state][actionTuple[state]->action][statep]
	    * changes[statep];
	}
    }

  for (int state=0; state < numStates; state++)
    {
      tempMovement[state] = movements[state]+tempChange[state];
      if (tempMovement[state] <= maxMovement[state])
	{
	  movements[state] = tempMovement[state];
	  changes[state] = tempChange[state];
	}
      else
	{
	  changes[state] = maxMovement[state]-movements[state];
	  movements[state] = maxMovement[state];
	  regimeTuple[state] = maxMovementConstraints[state];
	}

      newError = max(newError,changes[state]);
    }

  return newError;
} // updatePivot

void SGApprox::updateFlags()
{
  int state;

  updatedThreatTuple = vector<bool>(2,false);
  passNorth = false;

  for (int player=0; player < numPlayers; player++)
    {
      if (!facingEastNorth[player] 
	  && currentDirection[player] > env.directionTol)
	{ 
	  // Passed due north
	  facingEastNorth[player] = true;
	  if (player==0)
	    {
	      passNorth = true;
	      
	      oldOldWest = oldWest;
	      oldWest = westPoint;
	      westPoint = newWest;
	      newWest = extremeTuples.size() - 1;
	    }

	  // Update player 1's threat tuple
	  for (state = 0; state < numStates; state++)
	    {
	      if (pivot[state][player] > (threatTuple[state][player]
					  + env.pastThreatTol) )
		{
		  threatTuple[state][player] = extremeTuples[extremeTuples.size()-2][state][player];
		  updatedThreatTuple[player] = true;
		}
	    } // state
	}
      else if (facingEastNorth[player]
	       && currentDirection[player] < -env.directionTol) // Passed due south
	facingEastNorth[player] = false;

    } // player

  // Calculate error
  

} // updateBindingFlags

double SGApprox::distance(int newStart, int newEnd,
			  int oldStart, int oldEnd) const
{

  double newError = 1.0;

  // Calculate maximum distance from extreme tuples on most recent
  // revolution to the trajectory of two revolutions ago.

  if (numRevolutions > 0)
    {
      newError = 0.0;

      int oldPoint = oldEnd;
      for (int point = newEnd; point >= newStart; point--)
	{
	  SGPoint p = extremeTuples[point].average();

	  double distToPrevRev = numeric_limits<double>::max();
	  double tempDist;
	  // Find minimum distance between point and convex
	  // combinations of oldPoint and oldPoint-1.
	  while(true)
	    {
	      tempDist = SGTuple::distance(extremeTuples[point],
					   extremeTuples[oldPoint]);
	      
	      SGPoint p0 = extremeTuples[oldPoint].average();
	      SGPoint p1 = extremeTuples[oldPoint-1].average();
	      
	      SGPoint edge = p1 - p0, edgeNormal = edge.getNormal();
	      
	      double l = edgeNormal * p,
		l0 = p0 * edgeNormal,
		l1 = p1 * edgeNormal;

	      if ( abs(l1-l0) > env.flatTol )
		{
		  double weightOn1 = (l-l0)/(l1-l0);
		  if (weightOn1 <= 1 && weightOn1 >= 0)
		    tempDist
		      = std::min(tempDist,
				 SGTuple::distance(extremeTuples[point],
						   extremeTuples[oldPoint]
						   *(1.0-weightOn1)
						   +extremeTuples[oldPoint-1]
						   *weightOn1));
		}
	      distToPrevRev = std::min(distToPrevRev,tempDist);
	      
	      if ( SGTuple::distance(extremeTuples[point],
				     extremeTuples[oldPoint-1])
		   > distToPrevRev + 1e-12
		   || oldPoint < oldStart )
		break;
	      else
		oldPoint--;
	    } // for oldPoint

	  newError = std::max(newError,distToPrevRev);
	} // for point

    } // if

  return newError;
}

void SGApprox::updateMinPayoffs()
{
  list<SGAction>::iterator action;
  vector<bool> update(2,true);

  for (int player = 0; player < numPlayers; player++)
    {
      if (!updatedThreatTuple[player] 
	  || game.unconstrained[player])
	update[player] = false;
    }

  for (int state = 0; state < numStates; state++)
    {
      for (action = actions[state].begin();
  	   action != actions[state].end();
  	   action ++)
	action->calculateMinIC(game,update,threatTuple);

    } // state
} // updateMinPayoffs

void SGApprox::calculateBindingContinuations() 
{
  // Calculates the IC intersection points. To be used after updating
  // the threat tuple.
  int state, tupleIndex;
  SGPoint intersection, point, nextPoint;
  vector<int> intsctTuples;
  vector<double> xbounds(2), ybounds(2);

  bool anyUpdate = false;

  for (state = 0; state < numStates; state++)
    {
      list<SGAction>::iterator action = actions[state].begin();
      while (action != actions[state].end())
	{
	  reverse_iterator< vector<SGTuple>::const_iterator > tuple, nextTuple;
	      
	  vector<SGTuple> newPoints(2);
	  vector< vector<int> > newTuples(2);
	  for (int player = 0; player < numPlayers; player++)
	    {
	      if (!updatedThreatTuple[player]
		  || game.unconstrained[player])
		continue;
	      anyUpdate = true;

	      action->tuples[player].clear(); 
	      action->points[player].clear(); 
	      
	      nextPoint = extremeTuples.back()
		.expectation(game.probabilities[action->state]
			     [action->action]);

	      for (tuple = extremeTuples.rbegin(),
		     nextTuple = tuple+1,
		     tupleIndex = extremeTuples.size()-1; 
		   tupleIndex > oldWest;
		   // nextTuple != extremeTuples.rend(); 
		   ++tuple,++nextTuple, --tupleIndex)
		{
		  point = nextPoint;
		  nextPoint = nextTuple->expectation(game.probabilities
						     [action->state]
						     [action->action]);

		  double gap = point[player] - nextPoint[player];
		  if ( abs(gap) < env.flatTol
		       && abs(point[player] - action->minIC[player]) < env.flatTol )
		    {
		      // A flat.
		      newTuples[player].push_back(tupleIndex);
		      newTuples[player].push_back(tupleIndex - 1);
		      newPoints[player].push_back(point);
		      newPoints[player].push_back(nextPoint);
		    }
		  else if ( (point[player] <= action->minIC[player]
			     && action->minIC[player] < nextPoint[player])
			    || (point[player] >= action->minIC[player]
				&& action->minIC[player] > nextPoint[player]) )
		    {
		      // Points flank the minimum IC payoff
		      double alpha = (action->minIC[player]
				      - nextPoint[player] ) / gap;
		      newTuples[player].push_back(tupleIndex);
		      newPoints[player].push_back((1-alpha)*nextPoint
						  + alpha*point);
		    }

		  // Break when the payoff for this player is below
		  // but within env.pastThreatTol/2.0 of the threat
		  // tuple
		  if ( tuple->strictlyLessThan(threatTuple,player) 
		       && !threatTuple
		       .strictlyLessThan(*tuple+SGPoint(env.pastThreatTol/2.0),
					 player) )
		    break;
		} // for point
	    } // player
	
	  for (int player = 0; player < numPlayers; player++)
	    {	      
	      if (updatedThreatTuple[player] 
		  && !game.unconstrained[player])
		{
		  // Remove points that are not IC
		  int maxIndex, minIndex;
		  double maxOtherPayoff, minOtherPayoff;
		  newPoints[player].maxmin(1-player,maxOtherPayoff,maxIndex,
					   minOtherPayoff,minIndex);

		  if (maxOtherPayoff >= action->minIC[1-player])
		    {
		      action->points[player].push_back(newPoints[player]
						       [maxIndex]);
		      action->tuples[player].push_back(newTuples[player]
						       [maxIndex]);
		      if (minOtherPayoff < action->minIC[1-player])
			{
			  action->points[player]
			    .push_back(action->minIC);
			  action->tuples[player].push_back(-1);
			  action->corner = true;
			}
		      else
			{
			  action->points[player].push_back(newPoints[player]
							   [minIndex]);
			  action->tuples[player].push_back(newTuples[player]
							   [minIndex]);
			}

		      SGPoint expPivot 
			= pivot.expectation(game.probabilities[state][action->getAction()]);
		      action->intersectRaySegment(expPivot,currentDirection,
						  player);
		      
		    }
		  // Otherwise, not IC.
		}
	      else if (updatedThreatTuple[1-player]
		       && action->points[player].size()>0)
		{
		  if (action->points[player][0][1-player] 
		      >= action->minIC[1-player])
		    {
		      if (action->points[player][1][1-player]
			  < action->minIC[1-player])
			{
			  action->points[player][1] = action->minIC;
			  action->tuples[player][1] = -1;
			}
		    }
		  else
		    {
		      action->points[player].clear();
		      action->tuples[player].clear();
		    }
		}
	    } // for player

	  for(int player= 0; player < numPlayers; player++)
	    assert(action->points[player].size()==0
		   || (action->points[player].size()==2
		       && (action->points[player][0][1-player]
			   >= action->points[player][1][1-player]
			   -env.pastThreatTol)));

	  // Drop the point if no longer IC
	  if ((action->points[0].size() == 0 && !game.unconstrained[0])
	      && (action->points[1].size() == 0 && !game.unconstrained[1]))
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

void SGApprox::trimBindingContinuations()
{
  // Updates the actions array after the cut from pivot towards
  // direction. 
  int state;
  list<SGAction>::iterator action;
  SGPoint expPivot;

  for (state = 0; state < numStates; state++)
    {
      action = actions[state].begin();
      while(action != actions[state].end())
	{
	  // Check if the line starting from pivot towards direction
	  // cuts any of the intersection lines.
	  expPivot 
	    = pivot.expectation(game.probabilities[state][action->getAction()]);

	  action->trim(expPivot,currentDirection);

	  // Make sure that the points are correct in size and in the
	  // correct order.
	  bool drop = true;
	  for (int player = 0; player < numPlayers; player++)
	    {
	      assert(action->trimmedPoints[player].size()==0
		     || (action->trimmedPoints[player].size()==2
			 && (action->trimmedPoints[player][0][1-player]
			     >= action->trimmedPoints[player][1][1-player]
			     -env.pastThreatTol)));
	      if (action->trimmedPoints[player].size()>0)
		drop = false;
	    }
	  // Drop the point if no longer IC
	  // if (drop)
	  //   actions[state].erase(action++);
	  // else
	  //   action++;
	  action++;
	} // action
    } // state
} // trimBindingContinuations
  
