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

#ifndef _SGITERATION_HPP
#define _SGITERATION_HPP

#include "sggame.hpp"
#include "sgaction_pencilsharpening.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/utility.hpp>

// Forward declaration of SGApprox
class SGApprox;

//! Stores data on the behavior of SGApprox::generate()
/*! This class records information on each cut made by the twist
    algorithm. 

    Part of the pencil sharpening algorithm.

    \ingroup src
*/
class SGIteration
{
private:
  int iteration; /*!< The value of SGApprox::numIterations. */
  int revolution; /*!< The value of SGApprox::numRevolutions. */
  int numExtremeTuples; /*!< The size of SGApprox::extremeTuples. */
  SGTuple pivot; /*!< The current value of SGApprox::pivot. */
  SGPoint direction; /*!< The shallowest admissible direction at the
                        current revolution. */
  vector< vector<SGBaseAction> > actions; /*!< The actions that can be
                                           supported at the current
                                           iteration */

  //! The state that generated the best direction.
  int bestState; 
  //! The action that generated the best direction.
  int bestAction; 
  //! True if the best direction was non-binding.
  SG::Regime regime; 
  //! The current action tuple.
  vector<int> actionTuple; 
  //! The states in which IC constraints are not binding. 
  vector<SG::Regime> regimeTuple; 
  //! The current threat tuple.
  SGTuple threatTuple;

public:
  //! Default constructor
  SGIteration() {}
  //! Initializes a new SGIteration object with data on the current
  //! iteration
  /*! By default, the constructor will also copy the data in
      SGApprox::actions, so that the user can later recover the test
      directions that were available at the given iteration. If the
      second argument is false, then these actions will not be
      stored. For large games, storing the actions can take a large
      amount of memory. */
  SGIteration(const SGApprox & approx,
	      bool storeActions = true);

  //! Get method for the iteration.
  int getIteration() const { return iteration; } 
  //! Get method for the revolution.
  int getRevolution() const { return revolution; }
  //! Get method for the number of extreme tuples.
  int getNumExtremeTuples() const { return numExtremeTuples; }
  //! Get method for the current pivot.
  const SGTuple & getPivot() const { return pivot; }
  //! Get method for the current direction
  const SGPoint & getDirection() const { return direction; }
  //! Get method for the actions available at the current iteration.
  const vector< vector<SGBaseAction> > & getActions() const { return actions; }
  //! Get method for the best state.
  int getBestState() const { return bestState; }
  //! Get method for the best action.
  int getBestAction() const { return bestAction; }
  //! Get method for the regime corresponding to the best direction.
  SG::Regime getRegime() const { return regime; }
  //! Get method for the action tuple.
  const vector<int> & getActionTuple() const { return actionTuple; }
  //! Get method for the regime tuple.
  const vector<SG::Regime> & getRegimeTuple() const { return regimeTuple; }
  //! Get method for the current threat tuple.
  const SGTuple & getThreatTuple() const { return threatTuple; }
  
  //! Serializes the SGIteration object using boost.
  friend class boost::serialization::access;
  template<class Archive>

  //! Serialize the iteration using Boost.
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & iteration;
    ar & revolution;
    ar & numExtremeTuples;
    ar & pivot;
    ar & direction;
    ar & actions;
    ar & bestState;
    ar & bestAction;
    ar & regime;
    ar & actionTuple;
    ar & regimeTuple;
    ar & threatTuple;
  }
}; // SGIteration

#endif
