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

#include "sggame.hpp"

SGGame::SGGame(const SGAbstractGame & game):
  numPlayers(game.getNumPlayers()),
  delta(game.getDelta()),
  numStates(game.getNumStates()),
  numActions(game.getNumActions()),
  numActions_total(numStates,1),
  payoffs(numStates),
  probabilities(numStates),
  eqActions(numStates),
  unconstrained(numPlayers)
{
  for (int player = 0; player < numPlayers; player++)
    unconstrained[player] = !game.constrained(player);

  for (int state = 0; state < numStates; state++)
    {
      for (int p = 0; p < numPlayers; p++)
	numActions_total[state] *= numActions[state][p];
      
      payoffs[state] = vector<SGPoint>(numActions_total[state],SGPoint(numPlayers,0.0));
      probabilities[state] = vector< vector<double> >(numActions_total[state],
						      vector<double> (numStates,0));
      for (int action = 0; action < numActions_total[state]; action++)
	{
	  payoffs[state][action] = game.payoffs(state,action);
	  for (int statep = 0; statep < numStates; statep++)
	    {
		(probabilities[state][action][statep]
		 = game.probability(state,action,statep));
	      assert (game.probability(state,action,statep)>=0);
	    }

	  eqActions[state].push_back(game.isEquilibriumAction(state,action));
	  
	} // for action 
    } // for state
  if(!transitionProbsSumToOne())
    throw(SGException(SG::PROB_SUM_NOT1));
} // Conversion from SGAbstractGame

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities):
  SGGame(_delta,_numStates,_numActions,_payoffs,_probabilities,
	 vector<vector<bool> >(0),vector<bool>(2,false))
{}

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities,
	       const vector<bool> & _unconstrained):
  SGGame(2,_delta,_numStates,_numActions,_payoffs,_probabilities,
	 vector<vector<bool> >(0),_unconstrained)
{}

SGGame::SGGame(double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities,
	       const vector< vector<bool> > & _eqActions,
	       const vector<bool> & _unconstrained):
  SGGame(2,_delta,_numStates,_numActions,_payoffs,_probabilities,
	 _eqActions,_unconstrained)
{}

SGGame::SGGame(int _numPlayers,
	       double _delta,
	       int _numStates,
	       const vector< vector<int> > & _numActions,
	       const vector< vector< vector<double> > > & _payoffs,
	       const vector< vector< vector<double> > > & _probabilities,
	       const vector< vector<bool> > & _eqActions,
	       const vector<bool> & _unconstrained):
  numPlayers(_numPlayers),
  delta(_delta),
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
    throw(SGException(SG::INCONSISTENT_INPUTS));


  for (int state = 0; 
       state < numStates;
       state++)
    {
      for (int player=0; player<numPlayers; player++)
	numActions_total[state] *= _numActions[state][player];

      if (probabilities[state].size() != numActions_total[state]
	  || _payoffs[state].size() != numActions_total[state])
	throw(SGException(SG::INCONSISTENT_INPUTS));
      
      payoffs.push_back(vector<SGPoint>(numActions_total[state],SGPoint(numPlayers,0.0)));

      for (int action = 0; 
	   action < numActions_total[state];
	   action++)
	{
	  if (probabilities[state][action].size() != numStates
	      || _payoffs[state][action].size() != numPlayers)
	    throw(SGException(SG::INCONSISTENT_INPUTS));

	  payoffs[state][action] 
	    = SGPoint(_payoffs[state][action]);
	}
    } // state

  if(!transitionProbsSumToOne())
    throw(SGException(SG::PROB_SUM_NOT1)); 

  if (eqActions.size() != 0)
    {
      // Deprecated
      // for (int state = 0; state < numStates; state++)
      // 	{
      // 	  for (list<int>::const_iterator action
      // 		 = eqActions[state].begin();
      // 	       action != eqActions[state].end();
      // 	       action++)
      // 	    {
      // 	      if(*action<0 || *action>=numActions_total[state])
      // 		throw(SGException(SG::OUT_OF_BOUNDS));
      // 	    }
      // 	} // for state
    }
  else if (eqActions.size() == 0)
    {
      for (int state = 0; state < numStates; state++)
	{
	  eqActions.push_back(vector<bool>(numActions_total[state],true));
	} // for
    } 

  if(unconstrained.size()!=numPlayers)
    throw(SGException(SG::OUT_OF_BOUNDS));

} // SGGame main constructor

void SGGame::getPayoffBounds(SGPoint & UB, SGPoint & LB) const
{
  UB = SGPoint(numPlayers,numeric_limits<double>::min());
  LB = SGPoint(numPlayers,numeric_limits<double>::max());

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
  vector<bool>::iterator eqActionsvec
    = eqActions[state].begin()+position*ownIncrement;
  
  for (int aj = 0; aj < numActions[state][1-player]; aj++)
    {
      payoffs[state].insert(point,SGPoint(numPlayers,0.0));
      probabilities[state].insert(probvec,newProbabilities);
      if (!eqActions[state].empty())
	eqActions[state].insert(eqActionsvec,true);

      point += otherIncrement;
      probvec += otherIncrement;
      eqActionsvec += otherIncrement;
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
  vector<bool>::iterator eqActionsvec
    = eqActions[state].end()-1
    - (numActions[state][player]-1-position)*ownIncrement;
  
  for (int aj = 0; aj < numActions[state][1-player]; aj++)
    {
      payoffs[state].erase(point--);
      probabilities[state].erase(probvec--);
      if (!eqActions[state].empty())
	eqActions[state].erase(eqActionsvec--);

      point -= otherIncrement-1;
      probvec -= otherIncrement-1;
      eqActionsvec -= otherIncrement-1;
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
  eqActions.insert(eqActions.begin()+position,vector<bool>(0));
  numActions_total.insert(numActions_total.begin()+position,1);
  payoffs.insert(payoffs.begin()+position,
		 vector<SGPoint>(1,SGPoint(numPlayers,0.0)));
  probabilities.insert(probabilities.begin()+position,
		       vector< vector<double> > (1, vector<double>(numStates,0)));
  probabilities[position].back()[position] = 1.0;

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
  eqActions.erase(eqActions.begin()+state);
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


bool SGGame::transitionProbsSumToOne(double tolerance) const
{
  for (int s = 0; s < probabilities.size(); s++)
    {
      for (int a = 0; a < probabilities[s].size(); a++)
	{
	  double probSum = 0.0;
	  for (int sp = 0; sp < probabilities[s][a].size(); sp++)
	    {
	      probSum += probabilities[s][a][sp];
	    }
	  if (abs(probSum-1.0)>tolerance)
	    return false;
	}
    }
  return true;
} // transitionProbsSumToOne
