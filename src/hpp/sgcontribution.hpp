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

#ifndef _SGCONTRIBUTION_HPP
#define _SGCONTRIBUTION_HPP

#include "sg.hpp"

class ContributionGame : public SGAbstractGame
{
public:
  ContributionGame(int _numPlayers,
		   double _delta,
		   int _numStates):
    SGAbstractGame(_numPlayers,
		   _delta,
		   _numStates,
		   vector< vector<int> >(_numStates,vector<int>(_numPlayers,2)))
  {}

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    SGPoint point(numPlayers,0.0);
    
    double actionSum = 0.0;
    for (int p=0; p < numPlayers; p++)
      actionSum += actions[p];

    // Payoff is 2*sum a_j - 3* a_i + 5*state.
    for (int p=0; p < numPlayers; p++)
      point[p] = 2.0*actionSum - 3.0*actions[p]
	+ 20.0*static_cast<double>(state)/(static_cast<double>(numStates)-1.0);
    
    return point;
  } // payoffs

  virtual double probability(int state, const vector<int> & actions, int statep) const
  {
    return 1.0/static_cast<double>(numStates);
  } // probability
  
  
}; // PD_TwoState_3Player


#endif
