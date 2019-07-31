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

#ifndef _SGSIMULATOR_HPP
#define _SGSIMULATOR_HPP

#include "sgsolution_pencilsharpener.hpp"
#include <chrono>
#include <random>
#include <utility>


//! Class for forward simulating equilibria
/*! This class simulates an equilibrium based on a given SGSolution
  and initial state and an initial iteration from the final
  revolution. This iteration describes a basic equilibrium that
  maximizes in the given iteration's current direction.

  In order to simulate, you first have to call the
  SGSimulator::initialize method in order to construct the
  "transition table". This table determines, for each iteration in
  the final revolution and for each state, which action generates
  the flow payoffs and which continuation equilibrium should be
  used. If incentive constraints are not binding, the continuation
  equilibrium is described by the same iteration. Otherwise, some
  constraint binds, and the transition table decomposes the
  continuation equilibrium as a lottery over basic equilibria
  corresponding to tuples on the last revolution. If only one
  player's incentive constraint binds, then this decomposition is
  generically unique. If, however, more than one constraint binds or
  there is some non-genericity in payoffs, then the algorithm
  selects one decomposition arbitrarily.

  Having constructed the transition table, the equilibrium can be
  forward simulated using SGSimulator::simulate. The arguments to the
  simulate method are the number of periods for which to simulate, the
  initial state, and the initial tuple. When forward simulating, the
  class keeps track of the distributions of tuples (in the final
  revolution), states, and actions in each state. These can be
  retrieved using their various get
  methods. SGSimulator::getLongRunPayoffs will compute average payoffs
  for the players over the course of the simulation. The class will
  save a text version of the transition table in transitionTableSS,
  and it will save a text version of the first 200 periods of the
  simulation in SS. 

  TODO: Refine the procedure for constructing the transition table
  when both incentive constraints bind.

  This code is part of the pencil-sharpening algorithm and has not
  been updated to the max-min-max algorithm.

  \ingroup src
*/
class SGSimulator
{
  typedef pair< list<SGIteration>::const_iterator, double > transitionPair;

private:
  //! The associated SGSolution object.
  const SGSolution & soln;

  //! Points to the beginning of the last revolution.
  list<SGIteration>::const_iterator startOfLastRev;

  //! The transition table
  /*! This table consists of, for each state and tuple, a list
      transitionTable[tuple][state] of transitionPair data
      points. Each pair consists of an iterator that points to some
      continuation iteration, and a double which represents the
      probability of transitioning to the corresponding continuation
      equilibrium. */
  vector< vector< list<transitionPair> > > transitionTable;

  //! The distribution of actions in each state
  /*! actionDistr[state][action] is the frequency of that action in
      the corresponding state over the course of the simulation. */
  vector< vector<int> > actionDistr;
  //! The frequency distribution of tuples over the course of the
  //! simulation.
  vector<int> tupleDistr;
  //! The frequency distribution of states over the course of the
  //! simulation.
  vector<int> stateDistr;

  //! The number of simulations to run
  int numSim;
  //! The number of periods for each simulation.
  int numIter;

  //! True if saving log information to the stringstreams.
  bool logFlag;

  //! Tolerance for computing the transition table.
  double weightTol;

  //! Contains a text description of the first 200 periods of the
  //! simulation.
  std::stringstream ss;

  //! Contains a text description of the transition table.
  std::stringstream transitionTableSS;
  
public:
  //! Constructor
  SGSimulator(const SGSolution & _soln): 
    soln(_soln), logFlag(false), weightTol(1e-4)
  {}

  //! Returns the action frequency distributions.
  const vector< vector<int> > getActionDistr() const { return actionDistr; }
  //! Returns the state frequency distribution.
  const vector<int> getStateDistr() const { return stateDistr; } 
  //! Returns the tuple frequency distribution.
  const vector<int> getTupleDistr() const { return tupleDistr; }
  //! Returns the start of the last revolution.
  int getStartOfLastRev() const { return startOfLastRev->getIteration(); }
  //! Returns the number of iterations for the current simulation.
  int getNumIter() const { return numIter; } 

  //! Mutator method for the log flag.
  void setLogFlag(bool newFlag) { logFlag = newFlag; };

  //! Returns the stringstream describing the first 200 periods.
  const std::stringstream & getStringStream() const {return ss;}
  //! Returns the stringstream describing the transition table.
  const std::stringstream & getTransitionTableStringStream() const
  {return transitionTableSS;}

  //! Initializes the transition table.
  void initialize ();

  //! Forward simulates the equilibrium.
  void simulate(int _numSim, int _numIter, int initialState, int initialTuple);

  //! Returns the long run action distribution
  SGPoint getLongRunPayoffs()
  {
    SGPoint payoffs(0.0,0.0);
    for (int state = 0; 
	 state < soln.getGame().getNumStates(); 
	 state++)
      {
	for (int action = 0;
	     action < soln.getGame().getNumActions_total()[state];
	     action++)
	  {
	    payoffs += (1.0*actionDistr[state][action])/(1.0*numIter*numSim)
	      * soln.getGame().getPayoffs()[state][action];
	  }
      } // state

    return payoffs;
  }

}; // SGSimulator

#endif
