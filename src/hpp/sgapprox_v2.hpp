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

#ifndef _SGAPPROX_V2_HPP
#define _SGAPPROX_V2_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgexception.hpp"
#include "sgsolution.hpp"
#include "sgnamespace.hpp"

//! Approximation of the equilibrium payoff correspondence.
/*! This class contains an approximation of the equilibrium payoff
  correspondence. At its core, it contains a sequence of extreme
  tuples that have been generated thus far, a pivot, and a
  direction. The main method, SGApprox_V2::generate(), finds a
  new direction that will not intersect the equilibrium payoff
  correspondence and updates the pivot in that direction. By
  successively calling SGApprox_V2::generate(), the
  approximation will be refined and asymptotically it will converge
  to the equilibrium payoff correspondence. 
  
  \ingroup src
*/
class SGApprox_V2
{
private:
  const SGEnv & env; /*!< Constant reference to the parent
                        environment. */
  const SGGame & game; /*!< Constant reference to the game being
                          solved. */
  SGSolution & soln; /*!< Reference to the SGSolution object in which output
                    is being stored. */

  const double delta; /*!< The discount factor, copied from
                         SGApprox_V2::game. */
  const int numPlayers; /*!< The number of players, always 2. */
  const int numStates; /*!< The number of states, copied from
                          SGApprox_V2::game. */

  std::ofstream logfs; /*!< File stream for log file. */

  int numSteps; /*!< Number of steps within iteration. */
  int numIterations; /*!< Elapsed number of iterations. */
  double errorLevel; /*!< Current error level */
  
  vector< list<SGAction> > actions; /*!< actions[state] is a list of
                                       actions that can still be
                                       supported according to the
                                       current approximation. */
  vector<SGHyperplane> W; /*!< Feasible set. */
  vector<SGHyperplane> Wp; /*!< New feasible set. */

  SGPoint payoffUB; /*!< Upper bound on payoffs. */
  SGPoint payoffLB; /*!< Lower bound on payoffs. */
  
  SGTuple threatTuple; /*!< Current threat tuple. */

  SGTuple pivot; /*!< Current pivot. */
  SGPoint currDir; /*!< The current direction. */
  SGPoint currNormal; /*!< The current direction. */
  vector<double> currLevels; /*!< The levels attained by the pivot
                                   in the current normal direction. */
  vector< const SGAction* > actionTuple; /*!< actionTuple[state] is a
                                            pointer to the SGAction
                                            object that generates
                                            pivot[state]. */
  vector<SG::Regime> regimeTuple; /*!< regimeTuple[state] gives the
				    manner in which pivot[state] was
				    generated. */
  
  list<SGAction>::const_iterator bestAction; /*!< Pointer to the
					       action profile that
					       generates the
					       shallowest
					       direction. */
  SGPoint bestDir; /*!< The shallowest direction at the current
                            iteration. */
  SGPoint bestNormal; /*!< The normal to the shallwest direction. */
  SG::Regime bestRegime; /*!< Indicates which incentive
                                      constraints were binding for the
                                      best direction. */

  SGAction nullAction;

  std::ofstream testOFS; 
  

  // Methods

  //! Calculates the minimum IC continuation values
  /*! This method calculates for each SGAction object in
      SGApprox_V2::actions the minimum incentive compatible
      continuation value, relative to the current threat tuple. */
  void updateMinPayoffs();

  //! Calculates binding continuation values
  /*! For each SGAction objection in SGApprox_V2::actions, this
      method computes the extreme binding continuation values relative
      to the current threat tuple and the trajectory of the pivot on
      the previous revolution. */
  void calculateBindingContinuations();

  //! Finds an initial pivot, which maximizes player 2's payoff.
  void findInitialPivot();
  
  //! Calculates the best direction
  /*! Iteraties over the SGAction objects in SGApprox_V2::actions
      to find the shallowest admissible direction, and stores it in
      bestDirection. */
  void findBestDir();

  //! Calculates the new pivot
  /*! After the best direction has been found, this method updates the
      pivot in the new current direction. First, it calculates the
      maximum movements in the current direction that would not
      violate incentive compatibility, and then iterates
      SGApprox_V2::updatePivot until the pivot converges. */
  void calculateNewPivot();

  //! Updates the pivot
  /*! Bellman operator that advances the pivots corresponding to
      non-binding states in the current direction. If an IC constraint
      would be violated in state s, the movement in state s is set to
      maxMovement[s] and the state is put into the binding
      regime. Returns the distance the pivot moves. */
  double updatePivot(vector<double> & movements, 
		     vector<double> & changes,
		     vector<SG::Regime> & regimeTuple);

  //! Updates flags before the next iteration
  /*! This method checks whether or not the threat tuple has increased
      and sets the flags for recalculating binding continuation
      values. Also checks whether or not the cardinal direction has
      changed, and updates SGApprox_V2::facingEastNorth and
      SGApprox_V2::passNorth. */
  void updateFlags();

  //! Calculates the distance between revolutions
  /*! Returns the distance between successive iterations. Only runs
      when SGApprox_V2::passNorth is true. Currently only runs
      when the number of SGTuple objects on successive revolutions is
      the same, and then takes the maximum over all distances between
      tuples that are in corresponding positions in the
      revolutions. */
  double distance() const;

  double distHelper(const SGPoint & p, 
		    const SGPoint & qA, 
		    const SGPoint & qB) const;

  //! Checks whether or not newDir is shallower than best, relative to current
  /*! Returns true if the cosine between newDir and best is
      greater than SGEnv::improveTol, or if best and newDir
      are colinear, whether or not best has a larger norm. Non-static
      so it can read the parameter values in
      SGApprox_V2::env. */
  bool improves(const SGPoint & current, 
		const SGPoint & best, 
		const SGPoint & newDir) const;

  bool isBelow (const SGPoint & dir,
		const SGPoint & norm) const;

  //! Outputs progress to the log file every iteration
  void logAppend(ofstream & logfs,
		 int iter, int rev, const SGHyperplane & hp,
		 int state, int action);

public:
  //! Constructor for SGApprox_V2 class
  SGApprox_V2(const SGEnv & _env,
	      const SGGame & _game,
	      SGSolution & _soln):
    env(_env), game(_game), soln(_soln),
    delta(game.getDelta()), numPlayers(game.getNumPlayers()),
    numStates(game.getNumStates()), errorLevel(1), 
    nullAction(env)
  { }
  
  //! Prepares the approximation for generation
  /*! Opens the log file, constructs the actions array, initializes
      extremeTuples to a large "box" correspondence that contains the
      equilibrium payoff correspondence. Also initializes the pivot
      and the first direction. Sets the flags so that
      SGApprox_V2::generate() will calculate new binding
      continuation values on the first pass. */
  void initialize();

  //! Returns the number of iterations thus far
  int getNumIterations() const {return numIterations; }
  //! Returns the number of revolutions of the pivot thus far
  int getNumSteps() const {return numSteps; }
  //! Returns the number of hyperplanes in W
  int getNumHyperplanes() const {return W.size(); }
  //! Returns the regime in which the best test direction was
  //! generated
  SG::Regime getBestRegime() const {return bestRegime; }
  //! Returns a constant iterator for the SGAction in which the best
  //! test direction was generated
  list<SGAction>::const_iterator  getBestAction() const { return bestAction; }
  //! Returns the best test direction
  const SGPoint & getBestDirection() const { return bestDir; }
  //! Returns the current action tuple
  const vector<const SGAction*> & getActionTuple() const { return actionTuple; }
  //! Returns the current regime tuple
  const vector<SG::Regime> & getRegimeTuple() const { return regimeTuple; }
  //! Returns the current direction
  const SGPoint & getDirection() const { return currDir; }
  //! Returns the current pivot
  const SGTuple & getPivot() const {return pivot; }
  //! Returns the current threat tuple
  const SGTuple & getThreatTuple() const {return threatTuple; }
  //! Returns the array of SGAction objects that can currently be
  //! supported
  const vector< list<SGAction> > & getActions() const { return actions; }
  //! Returns the array of extreme tuples
  const vector<SGHyperplane> & getW() const {return W; }

  //! Returns a string indicating the algorithms progress
  std::string progressString() const;

  //! Refines the approximation
  /*! Main public routine for the SGApprox_V2 class. Updates minimum
   IC continuation values and binding continuation values, finds the
   best new direction, advances the pivot, and resets the flags. Also
   returns the distance between revolutions when a revolution is
   completed. Otherwise, returns 1. */
  double generate(bool storeIteration = true);

  //! Destructor
  /*! Only purpose right now is to close the log file. */
  void end();

};


#endif
