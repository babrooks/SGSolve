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

#ifndef _SGITERATION_MAXMINMAX_HPP
#define _SGITERATION_MAXMINMAX_HPP

#include "sggame.hpp"
#include "sgstep.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/utility.hpp>

//! Stores data on the behavior of SGSolver_MaxMinMax
/*! This class records the status of the max-min-max algorithm after
    each implementation of the max-min-max operator.

    \ingroup src
*/
class SGIteration_MaxMinMax
{
private:
  vector< vector<SGBaseAction> > actions; /*!< The actions that can be
                                           supported at the current
                                           iteration */
  
  SGTuple threatTuple; /*!< The current threat tuple. */

  list<SGStep> steps; /*!< The steps in the iteration */
  
public:
  //! Default constructor
  SGIteration_MaxMinMax() {}

  //! Initializes a new SGIteration_MaxMinMax object with data on the current
  //! iteration
  /*! The constructor will copy the data in
      SGSolver_MaxMinMax::actions, so that the user can later recover
      the test directions that were available at each step. For large
      games, storing the actions can take a large amount of memory. */
  SGIteration_MaxMinMax (const vector<list<SGAction_MaxMinMax> > & _actions,
		  const SGTuple & _threatTuple):
    threatTuple{_threatTuple}
  {
    // Copy the actions and recast them as SGBaseActions
    actions.resize(_actions.size());
    for (int state = 0; state < _actions.size(); state++)
      {
	actions[state].reserve(_actions[state].size());
	for (auto ait = _actions[state].begin();
	     ait != _actions[state].end();
	     ait++)
	  actions[state].push_back(static_cast<SGBaseAction>(*ait));
      }
  }

  //! Add an SGStep
  void push_back(const SGStep & step)
  {
    steps.push_back(step);
  }
  //! Get method for the steps
  const list<SGStep> & getSteps() const { return steps; }
  //! Get method for the actions available at the current iteration.
  const vector< vector<SGBaseAction> > & getActions() const { return actions; }
  //! Get method for the current threat tuple.
  const SGTuple & getThreatTuple() const { return threatTuple; }
  
  //! Serializes the SGIteration_MaxMinMax object using boost.
  friend class boost::serialization::access;
  template<class Archive>

  //! Serialize the iteration using Boost.
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & steps;
    ar & actions;
    ar & threatTuple;
  }
}; // SGIteration_MaxMinMax

#endif
