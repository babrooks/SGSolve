#include "sgapprox.hpp"

void SGApprox::end()
{
  logfs.close();
}

void SGApprox::initialize()
{
  int state, iter, action;

  sufficiencyFlag = true;
  
  oldWest = 0; westPoint = 0; newWest = 0;

  logfs.open("sg.log",std::ofstream::out);

  SGPoint payoffUB, payoffLB;
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
  SGAction nullAction(env);
  actionTuple = vector< const SGAction* >(numStates,&nullAction);
  regimeTuple = vector<SG::Regime>(numStates,SG::Binding);
  threatTuple = SGTuple(numStates,payoffLB);

  // Initialize extremeTuples
  extremeTuples.clear(); extremeTuples.reserve(env.getParam(SG::TUPLERESERVESIZE));
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffUB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffUB[0],payoffUB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffUB[0],payoffLB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffLB[1]))); 
  extremeTuples.push_back(SGTuple(numStates,SGPoint(payoffLB[0],payoffUB[1]))); 

  if (env.getParam(SG::PRINTTOLOG))
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

  numIterations = -1; 
  numRevolutions = 0;
  bestRegime = SG::Binding;
  bestAction = actions[0].end();

  if (env.getParam(SG::STOREITERATIONS) == 2)
    soln.push_back(SGIteration(*this,
			       env.getParam(SG::STOREACTIONS)));
  numIterations = 0;
  
} // initialize

void SGApprox::logAppend(ofstream & logfs, 
			 int iter, int rev, const SGTuple & tuple,
			 int state, int action)
{
  logfs << setw(3) << rev << " " << setw(3) << iter << " "
	<< tuple << " " << setw(3) << state << " " << setw(3) << action << endl;
}

double SGApprox::generate(bool storeIterations)
{
  // Four steps. First, update the minimum IC continuation values
  updateMinPayoffs();

  // Next, calculate binding continuation values
  calculateBindingContinuations();

  // Trim the binding continuation values;
  // trimBindingContinuations();

  // Find the new direction
  findBestDirection();
  
  assert(bestAction->getState() < game.getNumStates());
  assert(bestAction->getAction() < game.getNumActions_total()[bestAction->getState()]);
  
  if (storeIterations)
    {
      soln.push_back(SGIteration(*this,
				 env.getParam(SG::STOREACTIONS)));
      assert(soln.getIterations().back().getBestAction()
	     < game.getNumActions_total()[soln.getIterations().back().getBestState()]);
    }

  // Update the pivot.
  calculateNewPivot();
  
  // Final steps before next iteration. Set flags for updating binding
  // continuation values.
  updateFlags();
  
  if (passNorth)
    {
      errorLevel = distance(westPoint, newWest, oldWest, westPoint);

      if (env.getParam(SG::PRINTTOCOUT))
	cout << progressString() << endl; 
      
      numRevolutions++;
    }

  numIterations++;

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

  if (!sufficiencyFlag)
    ss << "\nWARNING: Sufficient conditions for containment not met.";
  
  return ss.str();
}

void SGApprox::findBestDirection()
{
  // Search for the next best direction.
  int state;

  bestAction = actions[0].end(); // use end as the default value for bestAction
  bestRegime = SG::Binding;

  bestDirection = -1.0*currentDirection;
  
  SGPoint currentNormal = currentDirection.getNormal();
  double currentNorm = currentDirection.norm();

  int belowBindingPlayer, belowBindingPoint,
    bestBindingPoint = 0, bestBindingPlayer = 0;

  sufficiencyFlag = true;
  
  for (state = 0; state < numStates; state++)
    {
      for (list<SGAction>::const_iterator action = actions[state].begin();
	   action != actions[state].end();
	   ++action)
	{
	  SG::Regime bestBindingRegime = SG::Binding;

	  SGPoint expPivot = pivot.expectation(game.getProbabilities()[state][action->getAction()]);
	  SGPoint stagePayoff = game.getPayoffs()[state][action->getAction()];
	  SGPoint nonBindingPayoff = (1-delta) * stagePayoff + delta * expPivot;
	  SGPoint nonBindingDirection = nonBindingPayoff - pivot[state];
	  double nonBindingNorm = nonBindingDirection.norm();
	  
	  // if ( !improves(currentDirection,bestDirection,nonBindingDirection)
	  //      && nonBindingNorm > env.getParam(SG::NORMTOL) )
	  //   continue;

	  if (expPivot >= action->getMinICPayoffs()
	      && nonBindingNorm > env.getParam(SG::NORMTOL))
	    {
	      if (env.getParam(SG::BACKBENDINGWARNING)
		  && currentNormal*nonBindingDirection
		  / sqrt(currentNorm*nonBindingNorm)
		  >= env.getParam(SG::BACKBENDINGTOL))
	      	cout << "Warning: Detected back-bending direction" << endl;

	      if (improves(currentDirection,bestDirection,nonBindingDirection))
		{
		  bestDirection = nonBindingDirection;
		  bestAction = action;
		  bestRegime = SG::NonBinding;

		  continue;
		}
	    }
	  
	  bool available(false), foundAbove(false), foundBelow (false);
	  SGPoint aboveDirection, belowDirection;

	  for (int player = 0; player < numPlayers; player++)
	    {
	      if ( action->getPoints()[player].size() == 0
		   || expPivot[player] >= action->getMinICPayoffs()[player])
		continue;
	      
	      SGTuple bindingPayoffs = delta*action->getPoints()[player] 
		+ (1-delta)*stagePayoff;
	      SGTuple bindingDirections = bindingPayoffs - pivot[state];

	      SGPoint nonBindingNormal = nonBindingDirection.getNormal();
	      for (int point = 0; point < bindingDirections.size(); point++)
		{
		  double bindingNorm = bindingDirections[point].norm();
		  double bindingLevel = (bindingDirections[point]*nonBindingNormal)
		    / std::sqrt(nonBindingNorm * bindingNorm);
		      
		  if (env.getParam(SG::BACKBENDINGWARNING)
		      && currentNormal*bindingDirections[point]
		      / std::sqrt(currentNorm
				  *bindingDirections[point].norm())
		      >= env.getParam(SG::BACKBENDINGTOL))
		    cout << "Warning: Detected back-bending direction" << endl;

		  if (nonBindingNorm > env.getParam(SG::NORMTOL)
		      && bindingNorm > env.getParam(SG::NORMTOL))
		    {
		      if (bindingLevel < env.getParam(SG::LEVELTOL)
			  && (!foundBelow 
			      || improves(nonBindingDirection,belowDirection,
					  bindingDirections[point]) ) )
			{
			  belowBindingPlayer = player;
			  belowBindingPoint = point;
			  belowDirection = bindingDirections[point];
			  if (point==1 && action->hasCorner())
			    bestBindingRegime = SG::Binding01;
			  else if (player==0)
			    bestBindingRegime = SG::Binding0;
			  else if (player==1)
			    bestBindingRegime = SG::Binding1;
			  foundBelow = true;
			}
		      if (bindingLevel > -env.getParam(SG::LEVELTOL)
			  && (!foundAbove
			      || improves(aboveDirection,nonBindingDirection,
					  bindingDirections[point]) ) )
			{
			  aboveDirection = bindingDirections[point];
			  foundAbove = true;
			}
		      else if ((point==0 && player == 0) ||
			       (point == 1 && player == 1 && !action->hasCorner()))
			{
			  // Also determine slope of feasible set clockwise
			  // relative to the binding payoff.
			  int nextPoint = action->getTuples()[player][point];
			  while (nextPoint < extremeTuples.size())
			    {
			      SGPoint newExpContVal = extremeTuples[nextPoint]
				.expectation(game.getProbabilities()[state][action->getAction()]);
			      SGPoint nextDirection = (1-delta)*stagePayoff
				+ delta*newExpContVal
				- pivot[state];
			      double nextBindingLevel
				= (nextDirection*nonBindingNormal)
				/ std::sqrt(nonBindingNorm * nextDirection.norm());

			      if (nextBindingLevel > bindingLevel
				  && newExpContVal >= action->getMinICPayoffs())
				{
				  bindingLevel = nextBindingLevel;
				  bindingDirections[point] = nextDirection;

				  if (bindingLevel > -env.getParam(SG::LEVELTOL)
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
			  >= env.getParam(SG::LEVELTOL)
			  || (belowDirection*aboveDirection >= env.getParam(SG::LEVELTOL)
			      && belowDirection * nonBindingDirection 
			      >= env.getParam(SG::LEVELTOL)) ) )
		    {
		      available = true;
		      
		      if (&(*action) == &(*actionTuple[state]))
			{
			  SGPoint oldContVal = (pivot[state]-(1-delta)*stagePayoff)
			    /delta;
			  bool pointOut = false;
			  for (int playerp = 0; playerp < numPlayers; playerp++)
			    {
			      if (oldContVal[playerp] <= action->getMinICPayoffs()[player]
				  && nonBindingDirection[playerp]<0)
				pointOut=true;
			    }
			  if (pointOut)
			    continue;
			}

		      if (nonBindingNorm > env.getParam(SG::NORMTOL)
			  && improves(currentDirection,bestDirection,
				      nonBindingDirection))
			{
			  if (env.getParam(SG::BACKBENDINGWARNING)
			      && currentNormal*nonBindingDirection
			      /std::sqrt(currentNorm*nonBindingNorm)
			      >= env.getParam(SG::BACKBENDINGTOL))
			    cout << "Warning: Detected back-bending direction" << endl;

			  bestDirection = nonBindingDirection;
			  bestAction = action;
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
	      bestBindingPoint = belowBindingPoint;
	      bestBindingPlayer = belowBindingPlayer;
	      bestAction = action;
	      bestRegime = bestBindingRegime;
	    }

	} // action
    } // state

  if (bestAction == actions[0].end())
    throw(SGException(SG::NO_ADMISSIBLE_DIRECTION));

  // Check sufficient condition for containment if best direction
  // is binding
  if (env.getParam(SG::CHECKSUFFICIENT)
      && bestRegime == SG::Binding)
    {
      // Compute the direction along the frontier from the best binding payoff
      int tupleIndex = bestAction->getTuples()[bestBindingPlayer][bestBindingPoint];
      if (tupleIndex > -1)
	{
	  SGPoint nextFrontierDirection = extremeTuples[tupleIndex].average() -
	    extremeTuples[tupleIndex-1].average();

	  if ( improves(currentDirection,nextFrontierDirection,bestDirection) )
	    sufficiencyFlag = false;
	}

      
    } // sufficient condition
  
} // findBestDirection

bool SGApprox::improves(const SGPoint & current, 
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

void SGApprox::calculateNewPivot()
{
  int updatePivotPasses = 0;

  int state, player;

  regimeTuple[bestAction->getState()] = bestRegime;
  actionTuple[bestAction->getState()] = &(*bestAction);
  bool flatDetected = false;
  if (env.getParam(SG::MERGETUPLES))
    flatDetected = bestDirection.getNormal()*currentDirection
      < env.getParam(SG::FLATTOL)*sqrt(bestDirection.norm() * currentDirection.norm());
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

	      tempMovement = (delta*actionTuple[state]->getMinICPayoffs()[player]
			      -pivot[state][player]
			      +(1-delta)*game.getPayoffs()[state][actionTuple[state]->getAction()][player])
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

  movements[bestAction->getState()] = min(1.0,maxMovement[bestAction->getState()]);
  vector<double> changes(movements);

  while (updatePivot(movements,changes,regimeTuple,
		     maxMovement,maxMovementConstraints) > env.getParam(SG::UPDATEPIVOTTOL)
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
  if (env.getParam(SG::MERGETUPLES) && (flatDetected
					|| maxDistance < env.getParam(SG::MOVEMENTTOL)) 
      && numIterations>0)
    {
      // cout << "Flat detected!" << endl;
      extremeTuples.back() = pivot;
    }
  else
    extremeTuples.push_back(pivot);

  if (env.getParam(SG::PRINTTOLOG))
    {
      logAppend(logfs,numIterations,numRevolutions,pivot,
		bestAction->getState(),bestAction->getAction());
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
	  && currentDirection[player] > env.getParam(SG::DIRECTIONTOL))
	{ 
	  // Passed due north
	  facingEastNorth[player] = true;
	  if (player==0)
	    {
	      passNorth = true;
	      
	      oldWest = westPoint;
	      westPoint = newWest;
	      newWest = extremeTuples.size() - 1;
	    }

	  // Update player 1's threat tuple
	  for (state = 0; state < numStates; state++)
	    {
	      if (pivot[state][player] > (threatTuple[state][player]
					  + env.getParam(SG::PASTTHREATTOL)) )
		{
		  threatTuple[state][player] = extremeTuples[extremeTuples.size()-2][state][player];
		  updatedThreatTuple[player] = true;
		}
	    } // state
	}
      else if (facingEastNorth[player]
	       && currentDirection[player] < -env.getParam(SG::DIRECTIONTOL)) // Passed due south
	facingEastNorth[player] = false;

    } // player

  // Calculate error
  

} // updateBindingFlags

double SGApprox::distance(int newStart, int newEnd,
			  int oldStart, int oldEnd) const
{
  if (numRevolutions<2)
    return 1.0;

  double newError = 0.0;

  for (int oldPoint = oldEnd; oldPoint >= oldStart; oldPoint--)
    {
      SGPoint p = extremeTuples[oldPoint].average();

      double distToCurrentRev = numeric_limits<double>::max();
      double tempDist;
      for (int point = newEnd; point > newStart; point--)
	{
	  // Find perpendicular to the line between point and point-1
	  // that goes through oldPoint.

	  tempDist = distHelper(p,
				extremeTuples[point].average(),
				extremeTuples[point-1].average());

	  if (tempDist <= distToCurrentRev)
	    distToCurrentRev = tempDist;
	} // for point

      tempDist = distHelper(p,
			    extremeTuples[newStart].average(),
			    extremeTuples[newEnd].average());

      if (tempDist <= distToCurrentRev)
	distToCurrentRev = tempDist;

      if (distToCurrentRev >= newError)
	newError = distToCurrentRev;
    } // for oldPoint

  return newError;
}

double SGApprox::distHelper(const SGPoint & p, 
			    const SGPoint & qA, 
			    const SGPoint & qB) const
{
  SGPoint qA_qB = qA-qB;

  double lA = qA*qA_qB;
  double lB = qB*qA_qB;
  double l = p*qA_qB;

  double tempDist;

  SGPoint p_qA = p-qA;

  if (lA>=l && l>=lB)
    {
      // Perpendicular goes through the segment between qA and
      // qB.

      double alpha = (l-lB)/(lA-lB);
      SGPoint closestPoint = alpha*qA+(1-alpha)*qB;

      return SGPoint::distance(closestPoint,p);
    }
  else if (p_qA*qA >= p_qA*qB)
    {
      return SGPoint::distance(qA,p);
    }
  return numeric_limits<double>::max();
}

void SGApprox::updateMinPayoffs()
{
  list<SGAction>::iterator action;
  vector<bool> update(2,true);

  for (int player = 0; player < numPlayers; player++)
    {
      if (!updatedThreatTuple[player] 
	  || game.getConstrained()[player])
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
	      if (!updatedThreatTuple[player]
		  || game.getConstrained()[player])
		continue;
	      anyUpdate = true;
	    }
	
	  action->calculateBindingContinuations(updatedThreatTuple,
						game,extremeTuples,
						threatTuple,
						pivot,currentDirection,
						oldWest);
	  
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
	    = pivot.expectation(game.getProbabilities()[state][action->getAction()]);

	  action->trim(expPivot,currentDirection);

	  // Make sure that the points are correct in size and in the
	  // correct order.
	  bool drop = true;
	  for (int player = 0; player < numPlayers; player++)
	    {
	      assert(action->getTrimmedPoints()[player].size()==0
		     || (action->getTrimmedPoints()[player].size()==2
			 && (action->getTrimmedPoints()[player][0][1-player]
			     >= action->getTrimmedPoints()[player][1][1-player]
			     -env.getParam(SG::PASTTHREATTOL))));
	      if (action->getTrimmedPoints()[player].size()>0)
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
  
