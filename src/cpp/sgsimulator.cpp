#include "sgsimulator.hpp"

void SGSimulator::initialize()
{
  // Build the transition table
  const SGGame & game = soln.getGame();
  const int numStates = game.getNumStates();
  const double delta = game.getDelta();

  transitionTableSS.str(""); // Reset the string
  
  // First find the iteration that starts the last revolution.
  startOfLastRev = soln.getIterations().end();
  startOfLastRev--;
  while (startOfLastRev->getRevolution() == soln.getIterations().back().getRevolution()
	 && startOfLastRev != soln.getIterations().begin())
    startOfLastRev--;
  if (startOfLastRev != soln.getIterations().begin())
    startOfLastRev++;

  int itersInLastRev = soln.getIterations().back().getIteration()
    - startOfLastRev->getIteration()+1;

  // Initialize the transition table
  transitionTable
    = vector< vector< list<transitionPair> > > (itersInLastRev,
						vector< list<transitionPair> > (numStates));
    
  // For each state/tuple, find the clockwise next tuple and the
  // probability of going to that tuple. 
  list<SGIteration>::const_iterator currentIter = startOfLastRev;

  int tupleCounter = 0;
  // for (int tupleCounter = 0; 
  // 	 tupleCounter < soln.getIterations().size() - startOfLastRev->getIteration();
  // 	 tupleCounter++ ) 
  while (currentIter != soln.getIterations().end())
    {
      for (int state = 0; state < numStates; state++)
	{
	  transitionTableSS << "Tuple " << currentIter->getIteration()
			    << ", state " << state
			    << ", action " << currentIter->getActionTuple()[state];

	  if (currentIter->getRegimeTuple()[state]==SG::NonBinding)
	    {
	      transitionTable[tupleCounter][state]
		.push_back(transitionPair(currentIter,1.0));

	      transitionTableSS << ", non-binding";
	    }
	  else
	    {
	      int action = currentIter->getActionTuple()[state];

	      list<SGIteration>::const_iterator iter = startOfLastRev;
	      SGPoint continuationValue
		= (currentIter->getPivot()[state]
		   - (1-delta)*soln.getGame().getPayoffs()[state][action])/delta;
	      SGPoint expPivot
		= iter->getPivot().expectation(game.getProbabilities()[state][action]);

	      if (currentIter->getRegimeTuple()[state] != SG::Binding01)
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

		  while (iter != soln.getIterations().end())
		    {

		      SGPoint nextExpPivot
			= nextIter->getPivot().expectation(game.getProbabilities()
						      [state][action]);
		      SGPoint dir = nextExpPivot - expPivot;

		      double contLevel = dir*continuationValue;
		      double expLevel = dir*expPivot;
		      double nextLevel = dir*nextExpPivot;

		      if (contLevel >= expLevel)
			{
			  double weightOnNext = 0;

			  if (nextLevel >= contLevel
			      && contLevel >= expLevel)
			    weightOnNext = (contLevel-expLevel)/(nextLevel-expLevel);
			  else if (contLevel >= nextLevel)
			    weightOnNext = 1;

			  SGPoint closestPoint = weightOnNext * nextExpPivot
			    +(1-weightOnNext)*expPivot;

			  double tempDistance = SGPoint::distance(closestPoint,
								  continuationValue);

			  if (tempDistance < minDistance)
			    {
			      minDistance = tempDistance;
			      transitionTable[tupleCounter][state].clear();
			      transitionTable[tupleCounter][state]
				.push_back(transitionPair(iter,1-weightOnNext));
			      transitionTable[tupleCounter][state]
				.push_back(transitionPair(nextIter,weightOnNext));
			    }
			}

		      expPivot = nextExpPivot;

		      iter++;
		      nextIter++;
		      if (nextIter == soln.getIterations().end())
			nextIter = startOfLastRev;
		    } // while

		  // assert(transitionTable[tupleCounter][state].first->getIteration()
		  //        >= startOfLastRev->getIteration());

		} // binding case
	      else if (currentIter->getRegimeTuple()[state]==SG::Binding01)
		{
		  // Just project the continuation value away from
		  // currentIter->getPivot() and find the segment on
		  // the other side.

		  transitionTableSS << ", binding 0 and 1";
		  
		  SGPoint expStartOfLastRev = iter->getPivot()
		    .expectation(game.getProbabilities()[state][action]);
		  SGPoint direction = continuationValue - expStartOfLastRev,
		    normal = direction.getNormal();
		  double level = continuationValue * normal;

		  ++iter;
		  do
		    {
		      expPivot = iter->getPivot()
			.expectation(game.getProbabilities()[state][action]);
			
		      double newLevel = expPivot * normal;
		      if (newLevel < level
			  && (expPivot-expStartOfLastRev)
			  *(expPivot-expStartOfLastRev) > 1e-5)
			{
			  list<SGIteration>::const_iterator nextIter = (iter--);
			  SGPoint oldExpPivot = iter->getPivot()
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
			    throw(SGException(SG::SIMERROR));

			  transitionTable[tupleCounter][state].clear();
			  
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
		      
		    } while (iter != soln.getIterations().end() );// for iter
		    
		    
		} // currentIter
	    } // binding01

	  transitionTableSS << ", (iter,weight): ";
	  
	  int continuationTupleCounter = 0;
	  for (list<transitionPair>::const_iterator continuationIter
		 = transitionTable[tupleCounter][state].begin();
	       continuationIter != transitionTable[tupleCounter][state].end();
	       ++continuationIter)
	    {
	      transitionTableSS << continuationTupleCounter
				<< ": ("
				<< continuationIter->first->getIteration()
				<< ", "
				<< setprecision(2)
				<< continuationIter->second
				<< " ), ";
	      continuationTupleCounter++;
	    } // for continuationIter
	  transitionTableSS << endl;

	} // state

      currentIter++;
      tupleCounter++;
    } // currentIter

} // initialize

void SGSimulator::simulate(int _numSim,
			   int _numIter, 
			   int initialState, 
			   int initialTuple)
{
  numSim = _numSim;
  numIter = _numIter;
  const int numStates = soln.getGame().getNumStates();
  const vector<int> & numActions_total = soln.getGame().getNumActions_total();
  const SGGame & game = soln.getGame();

  // Random number generator
  unsigned seed (std::chrono::system_clock::now().time_since_epoch().count());
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  // Reinitialize distribution containers
  stateDistr = vector<int>(numStates,0);
  tupleDistr = vector<int>(soln.getIterations().back().getIteration()
			   - startOfLastRev->getIteration()+1,0);
  actionDistr = vector< vector<int> > (numStates);
  for (int state = 0; state < numStates; state++)
    actionDistr[state] = vector<int>(numActions_total[state],0);


  if (logFlag)
    ss.str(""); // clear the stringstream

  assert(initialTuple >= startOfLastRev->getIteration());
  assert(initialTuple <= soln.getIterations().back().getIteration());
    
  list<SGIteration>::const_iterator initialTupleIt = soln.getIterations().end();

  initialTupleIt--;
  while (initialTupleIt->getIteration() != initialTuple)
    initialTupleIt--;

  assert(initialTupleIt->getIteration() >= startOfLastRev->getIteration());
      
  // Main simulation loop
  for (int sim = 0; sim < numSim; sim++)
    {
      int currentState = initialState;
      list<SGIteration>::const_iterator currentTuple = initialTupleIt;
      int currentAction = currentTuple->getActionTuple()[currentState];

      for (int iter = 0; iter < numIter; iter++)
	{
	  if (logFlag && sim == 0 && iter<200)
	    ss << "Simulation: " << sim
	       << ", Period: " << iter
	       << ", state: " << currentState
	       << ", action: " << currentAction
	       << ", tuple: " << currentTuple->getIteration()
	       << endl;
	  

	  // Increment state/action counters
	  stateDistr[currentState]++;
	  actionDistr[currentState][currentTuple->getActionTuple()[currentState]]++;
	  tupleDistr[currentTuple->getIteration()-startOfLastRev->getIteration()]++;
	
	  // Find new tuple/state/action
	  double probSum = 0;
	  double tupleDraw = distribution(generator);
	  list<transitionPair>::const_iterator pairIter
	    = transitionTable[currentTuple->getIteration()
			      - startOfLastRev->getIteration()]
	    [currentState].begin();
	  list<SGIteration>::const_iterator newTuple;
	  while (pairIter
		 != transitionTable[currentTuple->getIteration()
				    - startOfLastRev->getIteration()][currentState].end())
	    {
	      probSum += pairIter->second;
	      newTuple = pairIter->first;
	      if (tupleDraw <= probSum)
		break;
	      ++pairIter;
	    }

	  // If we have exceeded number of iterations, wrap around to
	  // start of last revolution.
	  if (newTuple == soln.getIterations().end())
	    newTuple = startOfLastRev;

	  // Find the new state
	  probSum = 0;
	  double stateDraw = distribution(generator);
	  int newState=0;
	  while (newState < numStates-1)
	    {
	      probSum += soln.getGame().getProbabilities()[currentState]
		[currentAction][newState];
	      if (stateDraw < probSum)
		break;
	      newState++;
	    }

	  // Update state variables
	  currentTuple = newTuple;
	  currentState = newState;
	  currentAction = currentTuple->getActionTuple()[currentState];
	} // iter
    } // sim
} // simulate
