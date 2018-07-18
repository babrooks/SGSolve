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

#ifndef _SGITERATION_V2_HPP
#define _SGITERATION_V2_HPP

#include "sggame.hpp"
#include "sgaction.hpp"
#include "sghyperplane.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/utility.hpp>

// Forward declaration of SGApprox_V2
class SGApprox_V2;

//! Stores data on the behavior of SGApprox_V2::generate()
/*! This class records information on each cut made by the twist
    algorithm. 

    \ingroup src
*/
class SGIteration_V2
{
private:
  int iteration; /*!< The value of SGApprox_V2::numIterations. */
  list<SGTuple> payoffTuples; /*!< The trajectory of the pivot tuple. */
  list<SGHyperplane> hyperplanes; /*!< The hyperplanes that define the
                                     approximation. */
  list<vector<int> > actionTuples; /*!< The indices of the actions
				     that are optimal in each
				     direction. */
  list< vector<SG::Regime> > regimeTuples; /*!< The regimes that are
                                           optimal in each
                                           direction. */
  
  vector< vector<SGBaseAction> > actions; /*!< The actions that can be
                                           supported at the current
                                           iteration */
  
  //! The current threat tuple.
  SGTuple threatTuple;

public:
  //! Default constructor
  SGIteration_V2() {}

  // //! Initializes a new SGIteration_V2 object with data on the current
  // //! iteration
  // /*! By default, the constructor will also copy the data in
  //     SGApprox::actions, so that the user can later recover the test
  //     directions that were available at the given iteration. If the
  //     second argument is false, then these actions will not be
  //     stored. For large games, storing the actions can take a large
  //     amount of memory. */
  // SGIteration_V2(const SGApprox & approx,
  // 	      bool storeActions = true);

  //! Get method for the iteration.
  int getIteration() const { return iteration; } 
  //! Get method for the payoff tuples
  const list<SGTuple> & getPayoffTuples() const { return payoffTuples; }
  //! Get method for the hyperplanes
  const list<SGHyperplane> & getHyperplanes() const { return hyperplanes; }
  //! Get method for the action tuples
  const list<vector<int> > & getActionTuples() const { return actionTuples; }
  //! Get method for the regime tuples
  const list<vector<SG::Regime> > & getRegimeTuples() const { return regimeTuples; }
  //! Get method for the actions available at the current iteration.
  const vector< vector<SGBaseAction> > & getActions() const { return actions; }
  //! Get method for the current threat tuple.
  const SGTuple & getThreatTuple() const { return threatTuple; }
  
  //! Serializes the SGIteration_V2 object using boost.
  friend class boost::serialization::access;
  template<class Archive>

  //! Serialize the iteration using Boost.
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & iteration;
    ar & payoffTuples;
    ar & hyperplanes;
    ar & actionTuples;
    ar & regimeTuples;
    ar & actions;
    ar & threatTuple;
  }
}; // SGIteration_V2

#endif
