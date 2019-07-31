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

#ifndef _SGSOLVER_HPP
#define _SGSOLVER_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgapprox.hpp"
#include "sgexception.hpp"
#include "sgsolution_PencilSharpening.hpp"

//! Class for solving stochastic games
/*! This class implements the pencil-sharpening algorithm of Abreu,
  Brooks, and Sannikov (2016). It contains parameters for the
  algorithm, the solve method, as well as the data structure produced
  by solve. It calculates the equilibrium payoff correspondence
  corresponding to an SGGame object.

  \ingroup src
 */
class SGSolver
{
private:
  // Data

  //! SGEnv object to hold parameters
  const SGEnv & env;
  //! Constant reference to the game to be solved.
  const SGGame & game; 
  //! SGSolution object used by SGApprox to store data.
  SGSolution soln;

public:
  //! Default constructor
  SGSolver(); 

  //! Constructor
  /*! Creates a new SGSolver object and initializes it with the given
      game. */
  SGSolver(const SGEnv & _env, 
	   const SGGame & _game);

  //! Destructor
  ~SGSolver() {}

  //! Solve routine
  /*! Initializes a new SGApproximation object and iteratively
      generates it until one of the stopping criteria have been
      met. Stores progress in the data member. */
  void solve();

  //! Returns a constant reference to the SGSolution object storing the
  //! output of the computation.
  const SGSolution& getSolution() const {return soln;}
};


#endif
