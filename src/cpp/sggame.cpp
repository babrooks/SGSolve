#include "sggame.hpp"

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities):
  SGGame(_delta,_numStates,_numActions,_payoffs,_probabilities,
	 vector<list<int> >(0),vector<bool>(2,false))
{}

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities,
	       const vector<bool> & _unconstrained):
  SGGame(_delta,_numStates,_numActions,_payoffs,_probabilities,
	 vector<list<int> >(0),_unconstrained)
{}

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities,
	       const vector< list<int> > & _eqActions,
	       const vector<bool> & _unconstrained):
  delta(_delta),
  numPlayers(2),
  numStates(_numStates), 
  numActions(_numActions),
  probabilities(_probabilities),
  numActions_total(numStates,1),
  eqActions(_eqActions),
  unconstrained(_unconstrained)
{
  double probSum;
  double currentPayoff;

  // Convert payoffs input to an array of SGPoints.
  payoffs.reserve(numStates);

  // Check inputs.
  if (probabilities.size() != numStates
      || _payoffs.size() != numStates
      || numActions_total.size() != numStates
      || !(eqActions.size() == numStates
	   || eqActions.size() == 0))
    throw(SGException(SGException::INCONSISTENT_INPUTS));


  for (int state = 0; 
       state < numStates;
       state++)
    {
      bool sumNotOne = false;

      for (int player=0; player<numPlayers; player++)
	numActions_total[state] *= _numActions[state][player];

      if (probabilities[state].size() != numActions_total[state]
	  || _payoffs[state].size() != numActions_total[state])
	throw(SGException(SGException::INCONSISTENT_INPUTS));
      
      payoffs.push_back(vector<SGPoint>(numActions_total[state]));

      for (int action = 0; 
	   action < numActions_total[state];
	   action++)
	{
	  if (probabilities[state][action].size() != numStates
	      || _payoffs[state][action].size() != numPlayers)
	    throw(SGException(SGException::INCONSISTENT_INPUTS));
	  
	  payoffs[state][action] 
	    = SGPoint(_payoffs[state][action]);

	  // Check that probabilities add up to 1.
	  probSum = 0.0;
	  for (int statep = 0; 
	       statep < numStates;
	       statep ++)
	    probSum 
	      += probabilities[state][action][statep];
	  
	  if (abs(probSum-1.0)>1e-9)
	    sumNotOne = true;
	}

      if (sumNotOne)
	{
	  stringstream ss;
	  ss << "WARNING: Transition probabilities in state " << state
	     << " do not sum to 1." << endl;
	  cout << ss.str();
	}
    } // state

  if (eqActions.size() != 0)
    {
      for (int state = 0; state < numStates; state++)
	{
	  for (list<int>::const_iterator action
		 = eqActions[state].begin();
	       action != eqActions[state].end();
	       action++)
	    {
	      if(*action<0 || *action>=numActions_total[state])
		throw(SGException(SGException::OUT_OF_BOUNDS));
	    }
	} // for state
    }

  if(unconstrained.size()!=2)
    throw(SGException(SGException::OUT_OF_BOUNDS));

} // SGGame

void SGGame::getPayoffBounds(SGPoint & UB, SGPoint & LB) const
{
  UB = SGPoint(numeric_limits<double>::min());
  LB = SGPoint(numeric_limits<double>::max());

  for (int state = 0; 
       state < numStates;
       state++)
    {
      for (int action = 0; 
	   action < numActions_total[state];
	   action++)
	{
	  // Find bounds.
	  for (int player=0; player<numPlayers; player++)
	    {
	      UB.max(payoffs[state][action]);
	      LB.min(payoffs[state][action]);
	    }
	}

    } // state

} // getPayoffUB

bool SGGame::setDiscountFactor(double newDelta)
{
  if (newDelta>0 && newDelta<1)
    {
      delta = newDelta;
      return true;
    }
  return false;
} // setDiscountFactor

bool SGGame::setPayoff(int state, int action,
		       int player, double payoff)
{
  if (player >= 0 && player < numPlayers
      && state >= 0 && state < numStates
      && action >= 0 && action < numActions_total[state])
    {
      payoffs[state][action][player] = payoff;
      return true;
    }
  return false;
} // setPayoff

bool SGGame::setProbability(int state, int action,
			    int newState, double prob)
{
  if (state >= 0 && state < numStates
      && newState >= 0 && newState < numStates
      && action >= 0 && action < numActions_total[state]
      && prob >= 0)
    {
      probabilities[state][action][newState] = prob;
      return true;
    }
  return false;
}

bool SGGame::addAction(int state, int player, int position)
{
  if (player < 0 || player >= numPlayers)
    return false;
  if (state >= numStates || state < 0)
    return false;
  if (position < 0 || position > numActions[state][player])
    return false;
  
  payoffs[state].reserve(payoffs[state].size()+numActions[state][1-player]);
  probabilities[state].reserve(probabilities[state].size()
			       +numActions[state][1-player]);

  vector<double> newProbabilities(numStates,0);
  newProbabilities[state] = 1;

  int ownIncrement = 1;
  int otherIncrement = numActions[state][player]+1;
  if (player==1)
    {
      ownIncrement = numActions[state][1-player];
      otherIncrement = 1;
    }
  
  // Start the iterator at the position of the last action
  vector<SGPoint>::iterator point
    = payoffs[state].begin()+position*ownIncrement;
  vector< vector<double> >::iterator probvec
    = probabilities[state].begin()+position*ownIncrement;
  
  for (int aj = 0; aj < numActions[state][1-player]; aj++)
    {
      payoffs[state].insert(point,SGPoint(0,0));
      probabilities[state].insert(probvec,newProbabilities);

      point += otherIncrement;
      probvec += otherIncrement;
    }

  numActions[state][player] ++;
  numActions_total[state] = numActions[state][0] * numActions[state][1];

  return true;
} // addAction

bool SGGame::removeAction(int state, int player, int position)
{
  if (player < 0 || player >= numPlayers)
    return false;
  if (state >= numStates || state < 0)
    return false;
  if (position < 0 || position >= numActions[state][player])
    return false;
  if (numActions[state][player] == 1)
    return false;
  
  int ownIncrement = 1;
  int otherIncrement = numActions[state][player];
  if (player==1)
    {
      ownIncrement = numActions[state][1-player];
      otherIncrement = 1;
    }
  
  // Start the iterator at the position of the last action
  vector<SGPoint>::iterator point
    = payoffs[state].end()-1
    - (numActions[state][player]-1-position)*ownIncrement;
  vector< vector<double> >::iterator probvec
    = probabilities[state].end()-1
    - (numActions[state][player]-1-position)*ownIncrement;
  
  for (int aj = 0; aj < numActions[state][1-player]; aj++)
    {
      payoffs[state].erase(point--);
      probabilities[state].erase(probvec--);

      point -= otherIncrement-1;
      probvec -= otherIncrement-1;
    }
  
  numActions[state][player] --;
  numActions_total[state] = numActions[state][0] * numActions[state][1];

  return true;
} // removeAction

bool SGGame::addState(int position)
{
  if (position < 0 || position > numStates)
    return false;

  numStates++;
  numActions.insert(numActions.begin()+position,
		    vector<int>(2,1));
  numActions_total.insert(numActions_total.begin()+position,1);
  payoffs.insert(payoffs.begin()+position,
		 vector<SGPoint>(1,SGPoint(0,0)));
  probabilities.insert(probabilities.begin()+position,
		       vector< vector<double> > (1, vector<double>(numStates,0)));
  probabilities[position].back().back() = 1.0;

  for (int state = 0; state < numStates; state++)
    {
      if (state == position)
	continue;
      
      for (int action = 0; action < numActions_total[state]; action++)
	probabilities[state][action]
	  .insert(probabilities[state][action].begin()+position,0.0);
    }

  return true;
} // addState

bool SGGame::removeState(int state)
{
  if (numStates==1)
    return false;
  if (state < 0 || state >= numStates)
    return false;
  
  numStates--;

  numActions.erase(numActions.begin()+state);
  numActions_total.erase(numActions_total.begin()+state);
  payoffs.erase(payoffs.begin()+state);
  probabilities.erase(probabilities.begin()+state);
  
  for (int state = 0; state < numStates; state++)
    {
      for (int action = 0; action < numActions_total[state]; action++)
	probabilities[state][action].erase(probabilities[state][action].begin()
					    +state);
    }
  return true;
} // removeState

bool SGGame::setConstrained(const vector<bool> & _unconstrained)
{
  if (_unconstrained.size()!=2)
    return false;

  unconstrained = _unconstrained;
  return true;
} // setConstrained
