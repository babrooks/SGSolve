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

#ifndef _SGPREDATION_HPP
#define _SGPREDATION_HPP

#include "sg.hpp"

//! A version of the predation game of Wiseman (2017) "When does
//! predation dominate collusion"?
class SGPredationGame : public SGAbstractGame
{
public:
  enum EndowmentMode { Consumption, Endowment};
private:
  int numFirmStates;
  int numQuantities;
  double alpha = 0.5; // Probability of financial health
			  // decreasing if running a loss
  double beta = 0.0; // Prob of financial health changing if
			// running a profit
  vector<double> qGrid;
  double A = 1;
  double B = 1;
  double c = 1.0;
  double maxQ = 1;
  
public:
  SGPredationGame(double delta,
		  int _numFirmStates,
		  int _numQuantities):
    numFirmStates(_numFirmStates),
    numQuantities(_numQuantities),
    qGrid(_numQuantities,0),
    SGAbstractGame(delta,
		   _numFirmStates*_numFirmStates,
		   vector< vector<int> > (_numFirmStates*_numFirmStates,
					  vector<int>(2,_numQuantities)))
  {
    // Set up the quantity grid and the numbers of actions
    for (int k = 0; k < numQuantities; k++)
      {
	qGrid[k] = maxQ * static_cast<double>(k)/(static_cast<double>(numQuantities)-1);
      } // for k
    // Players must produce zero when they exit
    for (int k = 0; k<numFirmStates; k++)
      {
	numActions[k][1]=1;
	numActions[k*numFirmStates][0]=1;
      }
  } // constructor

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    vector<double> q(2,0.0);
    vector<int> states(2,0);
    for (int player = 0; player < 2; player++)
      {
	states[player] = state % numFirmStates;
	state -= states[player]; state /= numFirmStates;

	// Can only produce if still active
	if (states[player]>0)
	  q[player]=qGrid[actions[player]];
      }
    double p = max(A-B*(q[0]+q[1]),0.0);

    return SGPoint(p*q[0]-c*q[0]*q[0],
		   p*q[1]-c*q[1]*q[1]);
  } // payoffs

  virtual double probability(int state,
			     const vector<int> & actions,
			     int statep) const
  {
    const SGPoint pi = SGPredationGame::payoffs(state,actions);
    vector<int> states(2,0), statesp(2,0);
    for (int player = 0; player < 2; player++)
      {
	states[player] = state % numFirmStates;
	state -= states[player]; state /= numFirmStates;
	statesp[player] = statep % numFirmStates;
	statep -= statesp[player]; statep /= numFirmStates;
      }

    // Only assign positive prob to adjacent transitions
    if (abs(statesp[0]-states[0])>1
	|| abs(statesp[1]-states[1])>1)
      return 0.0;
    
    double prob = 1.0;
    for (int player = 0; player < numPlayers; player++)
      {
	if (states[player] == 0) // In the absorbing state
	  {
	    if (statesp[player]!=0)
	      return 0.0; // zero is an absorbing state
	  }
	else if ( pi[player]<0) // Neg profit
	  {
	    if (statesp[player] == states[player]-1)
	      prob *= alpha; // Neg profit and state decreased by 1
	    else if (statesp[player] == states[player])
	      prob *= 1.0-alpha; // Neg profit and state stayed
	    // the same
	    else
	      return 0.0; // Neg profit but state does not stay
	    // the same or decrease by 1
	  }
	else if (pi[player] >= 0)// Non-neg profit
	  {
	    if (states[player] == statesp[player])
	      {
		double remainProb = 1.0-2*beta;
		if (states[player]==1)
		  remainProb += beta;
		if (states[player] == numFirmStates-1)
		  remainProb += beta;
		prob *= remainProb;
	      }
	    else if (statesp[player] == states[player]+1)
	      prob *= beta;
	    else if (statesp[player] == states[player]-1
		     && statesp[player] > 0)
	      prob *= beta;
	    else
	      return 0.0;
	  }

      }
    return prob;
  } // probability
  
};

#endif
