#include "sgsimulator.hpp"

void SGSimulator::initialize()
{
  // Build the transition table
  const SGGame & game = soln.game;
  const int numStates = game.getNumStates();
  const double delta = game.getDelta();

  transitionTableSS.str(""); // Reset the string
  
  // First find the iteration that starts the last revolution.
  startOfLastRev = soln.iterations.end();
  startOfLastRev--;
  while (startOfLastRev->revolution == soln.iterations.back().revolution)
    startOfLastRev--;
  startOfLastRev++;

  int itersInLastRev = soln.iterations.back().iteration
    - startOfLastRev->iteration+1;

  // Initialize the transition table
  transitionTable
    = vector< vector< list<transitionPair> > > (itersInLastRev,
						vector< list<transitionPair> > (numStates));
    
  // For each state/tuple, find the clockwise next tuple and the
  // probability of going to that tuple. 
  list<SGIteration>::const_iterator currentIter = startOfLastRev;

  int tupleCounter = 0;
  // for (int tupleCounter = 0; 
  // 	 tupleCounter < soln.iterations.size() - startOfLastRev->iteration;
  // 	 tupleCounter++ ) 
  while (currentIter != soln.iterations.end())
    {
      for (int state = 0; state < numStates; state++)
	{
	  if (currentIter->regimeTuple[state]==SG::NonBinding)
	    {
	      transitionTable[tupleCounter][state]
		.push_back(transitionPair(currentIter,1.0));

	      transitionTableSS << "Tuple " << currentIter->iteration
				<< ", action " << currentIter->actionTuple[state]
				<< ", non-binding" << endl;
	    }
	  else
	    {
	      int action = currentIter->actionTuple[state];

	      list<SGIteration>::const_iterator iter = startOfLastRev;
	      SGPoint continuationValue
		= (currentIter->pivot[state]
		   - (1-delta)*soln.game.getPayoffs()[state][action])/delta;
	      SGPoint expPivot
		= iter->pivot.expectation(game.getProbabilities()[state][action]);

	      transitionTableSS << "Tuple " << currentIter->iteration
				<< ", action " << currentIter->actionTuple[state];

	      if (currentIter->regimeTuple[state] != SG::Binding01)
		{
		
		  transitionTableSS << ", binding 0 or 1";
		  
		  // Find the iteration that is the closest to
		  // continuationValue
		  list<SGIteration>::const_iterator nextIter = startOfLastRev;
		  nextIter++;

		  SGPoint nextExpPivot;

		  double minDistance = 1.0;
		  double newDist;
		  SGPoint weights;

		  while (iter != soln.iterations.end())
		    {

		      SGPoint nextExpPivot
			= nextIter->pivot.expectation(game.getProbabilities()
						      [state][action]);


		      // Check interior closest point
		      SGPoint dir = nextExpPivot - expPivot;
		      if (dir.norm() > 1e-7)
			{

			  double det = SGPoint::invertSystem(weights,
							     continuationValue-expPivot,
							     dir,dir.getNormal());
			  // assert(det > 1e-12);
		      
			  SGPoint closestPoint = expPivot+weights[0]*dir;
			
			  if (weights[0] >= -weightTol && weights[0]<=1+weightTol)
			    {
			      newDist = (continuationValue-closestPoint)
				*(continuationValue-closestPoint);

			      if (newDist < minDistance)
				{
				  minDistance = newDist;

				  transitionTable[tupleCounter][state].clear();

				  transitionTable[tupleCounter][state]
				    .push_back(transitionPair(iter,weights[0]));
				  transitionTable[tupleCounter][state]
				    .push_back(transitionPair(nextIter,1-weights[0]));
				} // if new distance is smaller
			    
			    } // if weights[0] is between 0 and 1
			  else if (weights[0] < -weightTol)
			    {
			      newDist = (continuationValue-expPivot)
				*(continuationValue-expPivot);

			      if (newDist < minDistance)
				{
				  transitionTable[tupleCounter][state].clear();

				  transitionTable[tupleCounter][state]
				    .push_back(transitionPair(iter,1.0));
				  minDistance = newDist;
				}
			    } // weights[0] < 0
			}

		      expPivot = nextExpPivot;

		      iter++;
		      nextIter++;
		      if (nextIter == soln.iterations.end())
			nextIter = startOfLastRev;
		    } // while

		  // assert(transitionTable[tupleCounter][state].first->iteration
		  //        >= startOfLastRev->iteration);

		} // binding case
	      else if (currentIter->regimeTuple[state]==SG::Binding01)
		{
		  // Just project the continuation value away from
		  // currentIter->pivot and find the segment on
		  // the other side.
		    
		  SGPoint expStartOfLastRev = iter->pivot
		    .expectation(game.getProbabilities()[state][action]);
		  SGPoint direction = continuationValue - expStartOfLastRev,
		    normal = direction.getNormal();
		  double level = continuationValue * normal;

		  ++iter;
		  do
		    {
		      expPivot = iter->pivot
			.expectation(game.getProbabilities()[state][action]);
			
		      double newLevel = expPivot * normal;
		      if (newLevel < level
			  && (expPivot-expStartOfLastRev)
			  *(expPivot-expStartOfLastRev) > 1e-5)
			{
			  list<SGIteration>::const_iterator nextIter = (iter--);
			  SGPoint oldExpPivot = iter->pivot
			    .expectation(game.getProbabilities()[state][action]);
			  double oldLevel = oldExpPivot*normal;
			  double weightOnNew
			    = (level-oldLevel)/(newLevel-oldLevel);
			  SGPoint average = weightOnNew*expPivot
			    + (1-weightOnNew)*oldExpPivot;

			  oldLevel = expStartOfLastRev * direction;
			  newLevel = average * direction;
			  level = continuationValue * direction;
			  double weightOnAvg
			    = (level-oldLevel)/(newLevel-oldLevel);

			  if (weightOnNew < -weightTol | weightOnNew > 1+weightTol
			      | weightOnAvg < -weightTol && weightOnAvg > 1+weightTol)
			    throw(SGException(SGException::SIMERROR));

			  transitionTable[tupleCounter][state]
			    .push_back(transitionPair(startOfLastRev,
						      1-weightOnAvg));
			  transitionTable[tupleCounter][state]
			    .push_back(transitionPair(iter,
						      weightOnAvg*(1-weightOnNew)));
			  transitionTable[tupleCounter][state]
			    .push_back(transitionPair(nextIter,
						      weightOnAvg*weightOnNew));
			  
			  break;
			}

		      ++iter;
		    } while (iter != soln.iterations.end() );// for iter
		    
		    
		}
	    }

	} // state

      currentIter++;
      tupleCounter++;
    } // iter

} // initialize

void SGSimulator::simulate(int _numIter, 
			   int initialState, 
			   int initialTuple)
{
  numIter = _numIter;
  const int numStates = soln.game.getNumStates();
  const vector<int> & numActions_total = soln.game.getNumActions_total();
  const SGGame & game = soln.game;

  // Random number generator
  unsigned seed (std::chrono::system_clock::now().time_since_epoch().count());
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  // Reinitialize distribution containers
  stateDistr = vector<int>(numStates,0);
  tupleDistr = vector<int>(soln.iterations.back().iteration
			   - startOfLastRev->iteration+1,0);
  actionDistr = vector< vector<int> > (numStates);
  for (int state = 0; state < numStates; state++)
    actionDistr[state] = vector<int>(numActions_total[state],0);

  int currentState = initialState;
  list<SGIteration>::const_iterator currentTuple = soln.iterations.end();

  assert(initialTuple >= startOfLastRev->iteration);
  assert(initialTuple <= soln.iterations.back().iteration);
    
  currentTuple--;
  while (currentTuple->iteration != initialTuple)
    currentTuple--;

  assert(currentTuple->iteration >= startOfLastRev->iteration);
      
  int currentAction = currentTuple->actionTuple[currentState];

  if (logFlag)
    ss.str(""); // clear the stringstream

  // Main simulation loop
  for (int iter = 0; iter < numIter; iter++)
    {
      if (logFlag & iter<200)
	ss << "Period: " << iter
	   << ", state: " << currentState
	   << ", action: " << currentAction
	   << ", tuple: " << currentTuple->iteration
	   << endl;
	  

      // Increment state/action counters
      stateDistr[currentState]++;
      actionDistr[currentState][currentTuple->actionTuple[currentState]]++;
      tupleDistr[currentTuple->iteration-startOfLastRev->iteration]++;
	
      // Find new tuple/state/action
      double probSum = 0;
      double tupleDraw = distribution(generator);
      list<transitionPair>::const_iterator pairIter
	= transitionTable[currentTuple->iteration
			  - startOfLastRev->iteration]
	[currentState].begin();
      list<SGIteration>::const_iterator newTuple;
      while (pairIter
	     != transitionTable[currentTuple->iteration
				- startOfLastRev->iteration][currentState].end())
	{
	  probSum += pairIter->second;
	  newTuple = pairIter->first;
	  if (tupleDraw <= probSum)
	    break;
	  ++pairIter;
	}

      // If we have exceeded number of iterations, wrap around to
      // start of last revolution.
      if (newTuple == soln.iterations.end())
	newTuple = startOfLastRev;

      // Find the new state
      probSum = 0;
      double stateDraw = distribution(generator);
      int newState=0;
      while (newState < numStates-1)
	{
	  probSum += soln.game.getProbabilities()[currentState]
	    [currentAction][newState];
	  if (stateDraw < probSum)
	    break;
	  newState++;
	}

      // Update state variables
      currentTuple = newTuple;
      currentState = newState;
      currentAction = currentTuple->actionTuple[currentState];
    }
} // simulate
