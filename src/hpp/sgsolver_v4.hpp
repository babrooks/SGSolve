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

#ifndef _SGSOLVER_V4_HPP
#define _SGSOLVER_V4_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgaction_v2.hpp"
#include "sgexception.hpp"
#include "sgsolution.hpp"

//! Class for solving stochastic games
/*! This class contains parameters for the algorithm, the solve
  method, as well as the data structure produced by solve. It
  calculates the equilibrium payoff correspondence corresponding to an
  SGGame object.

  Version that implements ABS.

  \ingroup src
 */
class SGSolver_V4
{
private:
  // Data

  //! SGEnv object to hold parameters
  const SGEnv & env;
  //! Constant reference to the game to be solved.
  const SGGame & game; 
  //! SGSolution object used by SGApprox to store data.
  SGSolution soln;

  // References to objects in the game
  const double delta; /*!< The discount factor, copied from
                         SGApprox_V2::game. */
  const int numPlayers; /*!< The number of players, always 2. */
  const int numStates; /*!< The number of states, copied from
                          SGApprox_V2::game. */

  const vector< list<int> > & eqActions;
  const vector< vector<SGPoint> > & payoffs;
  const vector< vector< vector<double> > > & probabilities;
  const vector< vector<int> > numActions;
  const vector< int > numActions_totalByState;
  
public:
  //! Default constructor
  SGSolver_V4(); 

  //! Constructor
  /*! Creates a new SGSolver_V4 object and initializes it with the given
      game. */
  SGSolver_V4(const SGEnv & _env, 
	   const SGGame & _game);

  //! Destructor
  ~SGSolver_V4() {}

  //! Solve routine
  /*! Initializes a new SGApproximation object and iteratively
      generates it until one of the stopping criteria have been
      met. Stores progress in the data member. */
  void solve();

  //! Optimizes the policy for the given direction
  void optimizePolicy();

  //! Returns a constant reference to the SGSolution object storing the
  //! output of the computation.
  const SGSolution& getSolution() const {return soln;}
};


#endif
