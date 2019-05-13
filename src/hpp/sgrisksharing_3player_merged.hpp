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

#ifndef _SGRISKSHARING_3PLAYER_MERGED_HPP
#define _SGRISKSHARING_3PLAYER_MERGED_HPP

#include "sg.hpp"

//! Two player version of the Kocherlakota (1996) risk sharing game
/*!< This version of the game corresponds to the game in
   RiskSharingGame_3Player, but where players 2 and 3 behave
   cooperatively. */
class RiskSharingGame_3Player_Merged : public SGAbstractGame
{
private:
  int numEndowments;
  int c2e;
  vector<double> E;
  double eIncr;
  double cIncr;
  vector<int> numActions_total;
  double probDenom; 

  double consumption(int e, int t) const
  {
    assert(E[e]+t*cIncr>=-1e-6);
    assert(E[e]+t*cIncr<=1+1e-6);
    double cons = E[e]+t*cIncr;
    if (cons < 0)
      return 0;
    if (cons > 1)
      return 1;
    return cons;
  }

public:
  RiskSharingGame_3Player_Merged(double delta,
		  int _numEndowments,
		  int _c2e):
    numEndowments(_numEndowments),
    c2e(_c2e),
    E(numEndowments,0),
    numActions_total(_numEndowments,1),
    SGAbstractGame(delta,
		   _numEndowments,
		   vector< vector<int> > (_numEndowments,vector<int>(2,1)))
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
    cIncr = 1.0/( (numEndowments-1)*c2e );

    probDenom = 0.0;
    for (int e = 0; e < numEndowments; e++)
      probDenom += numEndowments-e;
  } // constructor

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    int t = actions[1]-actions[0]; // net transfer from 0 to 1
    double c = consumption(state,t);

    return SGPoint(sqrt(c),2.0*sqrt((1-c)/2.0));
  } // payoffs

  virtual double probability(int e, const vector<int> & actions, int ep) const
  {
    int t = actions[1]-actions[0];
    int a = actions[0]+actions[1]*numActions[e][0];
    
    return (1.0*numEndowments-ep)/probDenom;
  } // probability

  virtual bool isEquilibriumAction(int state, const vector<int> & actions) const
  {
    // Return true iff one of the players' actions is zero.
    if ( (actions[0] == 0) || (actions[1] == 0) )
      return true;
    return false;
  } // isEquilibriumAction
  
};

#endif
