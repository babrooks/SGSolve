#ifndef _SGSOLVER_HPP
#define _SGSOLVER_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgapprox.hpp"
#include "sgexception.hpp"
#include "sgsolution.hpp"

//! Class for solving stochastic games
/*!  This class contains parameters for the algorithm, the solve
  method, as well as the data structure produced by solve. It
  calculates the equilibrium payoff correspondence corresponding to an
  SGGame object.

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

/** \example as_twostate.cpp 
    \example kocherlakota.cpp
    \example abreusannikov.cpp
 */

#endif
