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

#include "sg.hpp"

class RiskSharingGame : public SGAbstractGame
{
public:
  enum EndowmentMode { Consumption, Endowment};
private:
  int numEndowments;
  int c2e;
  double persistence;
  vector<double> E;
  int midPoint;
  double eIncr;
  double cIncr;
  vector< vector<double> > stateProbSum;
  vector<int> numActions_total;
  EndowmentMode endowmentMode;

  double consumption(int e, int t) const
  {
    assert(E[e]+t*cIncr>=0);
    assert(E[e]+t*cIncr<=1);
    return E[e]+t*cIncr;
  }

  double cdf(double x) const
  {
    if (persistence>0)
      return 1-exp(-persistence*max(0.0,x));
    else
      return x;
  }

  double probHelper(int e, int t, int ep) const
  {
    double mode;
    switch (endowmentMode)
      {
      case Consumption:
	mode = consumption(e,t);
	break;
      case Endowment:
	mode = E[e];
	break;
      }
    
    return ( cdf(E[ep]-mode+eIncr) - cdf(E[ep]-mode-eIncr)
	     + cdf(mode-E[ep]+eIncr) - cdf(mode-E[ep]-eIncr) );
  } // probHelper
  
public:
  RiskSharingGame(double delta,
		  int _numEndowments,
		  int _c2e,
		  double _persistence,
		  EndowmentMode _mode):
    numEndowments(_numEndowments),
    c2e(_c2e),
    persistence(_persistence),
    E(numEndowments,0),
    midPoint((_numEndowments-1)/2),
    stateProbSum(_numEndowments),
    numActions_total(_numEndowments,1),
    SGAbstractGame(delta,
		   _numEndowments,
		   vector< vector<int> > (_numEndowments,vector<int>(2,1))),
    endowmentMode(_mode)
  {
    // // A couple of checks on the number of endowments
    // assert( (numEndowments%2) == 1 );
    // assert( numEndowments>2 );

    // Set up the endowment grid and the numbers of actions
    for (int e = 0; e < numEndowments; e++)
      {
	E[e] = (1.0*e)/(numEndowments-1); // player 0's endowment

	numActions[e][0] = e*c2e+1;
	numActions[e][1] = (numEndowments-e-1)*c2e+1;
	numActions_total[e] = numActions[e][0]*numActions[e][1];
      } // for e

    eIncr = (E[1]-E[0])/2.0;
    cIncr = 1.0/( (numEndowments-1)*c2e + 1 );

    // Sum the pseudo probabilities for each state and action
    for (int e = 0; e < numEndowments; e++)
      {
	stateProbSum[e] = vector<double> (numActions_total[e],0);
	for (int a = 0; a < numActions_total[e]; a++)
	  {
	    int t = (a/numActions[e][0])-(a%numActions[e][0]);
	    for (int ep = 0; ep < numStates; ep++)
	      stateProbSum[e][a] += probHelper(e,t,ep);
	  } // for t
      } // for e
  } // constructor

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    int t = actions[1]-actions[0]; // net transfer from 0 to 1
    double c = consumption(state,t);

    return SGPoint(sqrt(c),sqrt(1-c));
  } // payoffs

  virtual double probability(int e, const vector<int> & actions, int ep) const
  {
    int t = actions[1]-actions[0];
    int a = actions[0]+actions[1]*numActions[e][0];
    return probHelper(e,t,ep)/stateProbSum[e][a];
  } // probability

  virtual bool isEquilibriumAction(int state, const vector<int> & actions) const
  {
    // Return true iff one of the players' actions is zero.
    if ( (actions[0] == 0) || (actions[1] == 0) )
      return true;
    return false;
  } // isEquilibriumAction
  
};
