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

#ifndef _SGSOLVER_MAXMINMAX_HPP
#define _SGSOLVER_MAXMINMAX_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgaction_maxminmax.hpp"
#include "sgexception.hpp"
#include "sgsolution_maxminmax.hpp"

//! Class for solving stochastic games
/*! This class implements the max-min-max algorithm of Abreu, Brooks,
  and Sannikov (2019) for two players. It contains the parameters for
  the algorithm, the solve method, as well as the data structure
  produced by solve. It calculates the equilibrium payoff
  correspondence corresponding to an SGGame object.

  \ingroup src
 */
class SGSolver_MaxMinMax
{
private:
  // Data

  //! SGEnv object to hold parameters
  const SGEnv & env;
  //! Constant reference to the game to be solved.
  const SGGame & game; 
  //! SGSolution object used by SGApprox to store data.
  SGSolution_MaxMinMax soln;

  // References to objects in the game
  const double delta; /*!< The discount factor, copied from
                         SGApprox_V2::game. */
  const int numPlayers; /*!< The number of players, always 2. */
  const int numStates; /*!< The number of states, copied from
                          SGApprox_V2::game. */

  const vector< vector<bool> > & eqActions; /*!< Constant refernece to
                                            actions that are allowed
                                            to be played in
                                            equilibrium in the game. */
  const vector< vector<SGPoint> > & payoffs; /*!< Constant reference to payoffs in the game. */
  const vector< vector< vector<double> > > & probabilities; /*!< Constant reference to transition probabilities in the game. */
  const vector< vector<int> > numActions; /*!< Number of actions in the game. */
  const vector< int > numActions_totalByState; /*!< Total number of actions in each state. */

  list<SGPoint> directions; /*!< List of directions in which the algorithm bounds payoffs. */
  list< vector<double> > levels; /*!< List of optimal levels attained in the corresponding directions. */
  SGTuple threatTuple; /*!< The current threat payoffs. */
  vector< list<SGAction_MaxMinMax> > actions; /*!< Actions that can still be played. */
  
  const SGPoint dueEast = SGPoint(1.0,0.0); /*!< The direction due east. */
  const SGPoint dueNorth = SGPoint(0.0,1.0); /*!< The direction due north. */

  int numIter; /*!< The number of iterations computed thus far. */
  double errorLevel; /*!< The current error level. */
  
public:
  //! Default constructor
  SGSolver_MaxMinMax(); 

  //! Constructor
  /*! Creates a new SGSolver_MaxMinMax object and initializes it with the given
      game. */
  SGSolver_MaxMinMax(const SGEnv & _env, 
	   const SGGame & _game);

  //! Destructor
  ~SGSolver_MaxMinMax() {}

  //! Solve routine
  /*! Initializes a new SGApproximation object and iteratively
      generates it until one of the stopping criteria have been
      met. Stores progress in the data member. 

      Fixed directions. */
  void solve_fixed();

  //! Solve routine
  /*! Initializes a new SGApproximation object and iteratively
      generates it until one of the stopping criteria have been
      met. Stores progress in the data member. 

      Generates directions endogenously. */
  void solve();

  //! One iteration of the endogenous algorith.
  /*! Return the new error level. */
  double iterate();

  //! Compute approximate Hausdorff distance
double pseudoHausdorff(const list<SGPoint> & newDirections,
					   const list<vector<double> > & newLevels) const;

  //! Initializes the solve routines
  void initialize();

  std::string progressString() const;
  
  //! Optimizes the policy for the given direction
  void optimizePolicy(SGTuple & pivot,
		      vector<double> & penalties,
		      vector<SGActionIter> & actionTuple,
		      vector<SG::Regime> & regimeTuple,
		      const SGPoint currDir,
		      const vector<list<SGAction_MaxMinMax> > & actions) const;

  //! Optimizes the policy for the given direction
  void robustOptimizePolicy(SGTuple & pivot,
			    vector<double> & penalties,
			    vector<SGActionIter> & actionTuple,
			    vector<SG::Regime> & regimeTuple,
			    vector<bool> & bestAPSNotBinding,
			    SGTuple & bestBindingPayoffs,
			    const SGPoint currDir,
			    const vector<list<SGAction_MaxMinMax> > & actions) const;

  //! Find the next clockwise direction at which the optimal tuple
  //! changes
  double sensitivity(const SGTuple & pivot,
		     const vector<double> & penalties,
		     const vector<SGActionIter> & actionTuple,
		     const vector<SG::Regime> & regimeTuple,
		     const SGPoint currDir,
		     const vector<list<SGAction_MaxMinMax> > & actions) const;

  //! Converts a policy function to a payoff function using bellman iteration
  void policyToPayoffs(SGTuple & pivot,
		       const vector<SGActionIter>  & actionTuple,
		       const vector<SG::Regime> & regimeTuple) const;

  //! Converts a policy function to the associated penalties using
  //! bellman iteration, when computing inner approximation
  void policyToPenalties(vector<double> & penalties,
			 const vector<SGActionIter>  & actionTuple,
			 const vector<SG::Regime> & regimeTuple) const;

  //! Lexicographic comparison of points
  /*! Returns true if a is above b in the direction dir, or if they
      are at the same level but a is higher than b in the direction
      dir rotated 90 degrees clockwise. */
  bool lexComp(const SGPoint & a,
	       const double aPenalty,
	       const SGPoint & b,
	       const double bPenalty,
	       const SGPoint & dir ) const;

  //! Computes  the best binding payoff for an action
  bool computeBestBindingPayoff(const SGActionIter ait,
				int & bestBindingPlayer,
				int & bestBindingPoint,
				const SGPoint & dir) const;

  //! Switches regimes from binding to non-binding to minimize levels
  void minimizeRegimes(SGTuple & pivot,
		       vector<double> & penalties,
		       const vector<SGActionIter> & actionTuple,
		       vector<SG::Regime> & regimeTuple,
		       const SGPoint & dir,
		       const SGTuple & bestBindingPayoffs,
		       const vector<bool> & bestAPSNotBinding) const;

  //! Lexicographic comparison of directions
  /*! Returns true if a is above b or a is parallel to b. */
  bool lexAbove(const SGPoint & a, const SGPoint & b ) const;
  
  //! Returns a constant reference to the SGSolution_MaxMinMax object storing the
  //! output of the computation.
  const SGSolution_MaxMinMax& getSolution() const {return soln;}
};


#endif
