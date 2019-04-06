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

#ifndef _SGRISKSHARING_3PLAYER_HPP
#define _SGRISKSHARING_3PLAYER_HPP

#include "sg.hpp"

class RiskSharingGame_3Player : public SGAbstractGame
{
private:
  int numEndowments;
  int c2e;
  vector<vector<int> > E;
  double eIncr;
  double cIncr;
  vector<int> numActions_total;
  vector< vector<int> > transferList;;

public:
  RiskSharingGame_3Player(double delta,
		  int _numEndowments,
		  int _c2e):
    numEndowments(_numEndowments),
    c2e(_c2e),
    E(numEndowments),
    numActions_total(_numEndowments,1),
    SGAbstractGame(3,delta,
		   _numEndowments,
		   vector< vector<int> > (_numEndowments,vector<int>(3,1)))
  {
    // // A couple of checks on the number of endowments
    // assert( (numEndowments%2) == 1 );
    // assert( numEndowments>2 );
    
    // Set up the endowment grid and the numbers of actions
    E.clear();
    numStates=0;
    for (int e0 = 0; e0 < numEndowments; e0++)
      {
	for (int e1 = 0; e1 < numEndowments-e0; e1++)
	  {
	    vector<int>newEndowments(3,0);
	    newEndowments[0] = e0; // player 0's endowment	    
	    newEndowments[1] = e1; // player 0's endowment
	    newEndowments[2] = numEndowments-1 -e0-e1;
	    E.push_back(newEndowments);

	    numStates++;
	  } // for e1
      } // for e0


    eIncr = 1.0/(numEndowments-1.0);
    cIncr = 1.0/( (numEndowments-1)*c2e );

    transferList.clear();

    vector<int> transferStartPositions(numEndowments+1,0);
    // position in the transfer list where transfers for the given
    // endowment level begin. 
    
    for (int e = 0; e < numEndowments; e++)
      {
	transferStartPositions[e] = transferList.size();

	for (int t0 = 0; t0 < e*c2e+1; t0++)
	  {
	    for (int t1 = 0; t1 < e*c2e+1-t0; t1++)
	      {
		vector<int> newTransfers (2,0);
		newTransfers[0] = t0;
		newTransfers[1] = t1;
		
		transferList.push_back(newTransfers);
	      }
	  }
      }
    transferStartPositions.back() = transferList.size();
    
    numActions = vector< vector<int> > (numStates,vector<int>(numPlayers,0));
    numActions_total = vector< int > (numStates,1);
    
    for (int s = 0; s < numStates; s++)
      {
	for (int p = 0; p < numPlayers; p++)
	  {
	    // Action is a vector of how much to transfer to (i+1)%2 and (i+2)%2.
	    numActions[s][p] = transferStartPositions[E[s][p]+1]; 
	    numActions_total[s] *= numActions[s][p];
	  }
      } // for e0 

  } // constructor

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    // Make the player's action a choice of how much to give away. We
    // will compute how to distribute this to maximize welfare.
    vector<int> netTransfers(numPlayers,0);
    for (int p = 0; p < numPlayers; p++)
      {
	netTransfers[p] -= transferList[actions[p]][0];
	netTransfers[p] -= transferList[actions[p]][1];
	netTransfers[(p+1)%numPlayers] += transferList[actions[p]][0];
	netTransfers[(p+2)%numPlayers] += transferList[actions[p]][1];
      }

    SGPoint payoffs(numPlayers,0.0);
    for (int p = 0; p < numPlayers; p++)
      payoffs[p] = sqrt(E[state][p]*eIncr + netTransfers[p]*cIncr);
    
    return payoffs;
  } // payoffs

  virtual double probability(int e, const vector<int> & actions, int ep) const
  {
    return 1.0/numStates; // uniform distribution
  } // probability

  virtual bool isEquilibriumAction(int state, const vector<int> & actions) const
  {
    // Return true if exactly one player receives transfers (from one
    // or two players) or exactly two players receive transfers from
    // one player.
    int numReceivingXfers = 0;
    int numSendingXfers = 0;
    
    vector<bool> receives(numPlayers,false);
    
    for (int p = 0; p < numPlayers; p++)
      {
	numSendingXfers += actions[p]>0;
	if (transferList[actions[p]][0]>0)
	  receives[(p+1)%numPlayers] = true;
	if (transferList[actions[p]][1]>0)
	  receives[(p+2)%numPlayers] = true;
      }

    for (int p = 0; p < numPlayers; p++)
      numReceivingXfers += receives[p];
      
    return (numReceivingXfers <= 1
	    || (numReceivingXfers == 2 && numSendingXfers == 1));
  } // isEquilibriumAction
  
};

#endif
