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

// Classes for storing data for stochastic games.
#ifndef _SGBASEACTION_HPP
#define _SGBASEACTION_HPP

#include "sgcommon.hpp"
#include "sgtuple.hpp"
#include "sgnamespace.hpp"

//! Describes an action in the game
/*! Stores IC region information for a single action. Stores the
  action, minimum incentive compatible payoffs, points of intersection
  between the IC region and the expected feasible set, and indices of
  the tuples that generate the points of intersection.

  \ingroup src
*/
class SGBaseAction
{
protected:
  int numPlayers;
  int state; /*!< The state in which this action profile can be played. */
  int action; /*!< The index of the action profile. */
  SGPoint minIC; /*!< The minimum continuation value to support
                    incentive compatibility, relative to the current
                    threat tuple. In particular, this is the maximum
                    over all deviations of the expected threat point
                    under the deviation, plus (1-delta)/delta times
                    the static gains from deviating. */
  vector< SGTuple > points; /*!< Extreme points of the set of expected
                               feasible continuation values at which
                               some player's incentive constraint
                               binds. points[i] is an SGTuple
                               consisting of either 0 or 2 SGPoint
                               objects, which are the extreme binding
                               payoffs on player i's incentive
                               constraint. The convention is that the
                               first element of the tuple is the
                               northern or easternmost of the two
                               binding payoffs. */ 

  vector< vector< int > > tuples; /*!< The vector tuples[i][j] points
                                     to the element of
                                     SGApproximation::extremeTuples
                                     whose expectation is just
                                     clockwise relative to
                                     points[i][j]. */

  bool isNull; /*!< Flag to indicate that this is the place holder
                  "null" action. */

  bool corner; /*!< Flag that indicates that the feasible set has a
                  corner. */
  
  
public:
  //! Constructor
  /*! Constructs a null action associated with the given SGEnv. */
  SGBaseAction():
    numPlayers(2),
    isNull(true),
    action(-1),
    state(-1)
  {}

  //! Constructor
  /*! Grandfather in two-player code. */
  SGBaseAction(int _state, int _action):
    SGBaseAction(2,_state,_action)
  { }
  
  //! Constructor
  /*! Constructs an action for the given state and action index in the
      given environment. */
  SGBaseAction(int _numPlayers,
	       int _state,
	       int _action):
    numPlayers(_numPlayers),
    state(_state),
    action(_action),
    minIC(_numPlayers,-numeric_limits<double>::max()),
    isNull(false),
    corner(false)
  {
    points.resize(numPlayers);
    tuples.resize(numPlayers); 
  }

  //! Destructor
  ~SGBaseAction() {}
  
  //! Returns true if the action is null.
  bool getIsNull() const {return isNull;}
  //! Returns the action
  int getAction() const { return action; }
  //! Returns the points array
  const vector<SGTuple> & getPoints() const { return points; }
  //! Returns the tuples array
  const vector< vector<int> > & getTuples() const { return tuples; }
  //! Returns the state
  int getState() const { return state; }
  //! Returns whether or not the action has a corner
  bool hasCorner() const { return corner; }
  //! Returns the minimum IC continuation values
  const SGPoint & getMinICPayoffs() const {return minIC;}
  //! Returns the array of binding continuation values.
  const vector<SGTuple> & getBindingContinuations() const { return points; }

  //! Sets the minimum IC continuation values
  void setMinICPayoffs(const SGPoint & newMinIC)
  {
    minIC = newMinIC;
  } // setMinIC
  //! Sets the tuples array
  void setTuples(const vector< vector<int> > & newTuples)
  {
    assert(newTuples.size()<=numPlayers);
    tuples = newTuples;
  } // setTuples
  //! Sets the points array  
  void setPoints(const vector<SGTuple> & newPoints)
  {
    assert(newPoints.size()<=numPlayers);
    points = newPoints;
  } // setPoints
  void setPointsAndTuples(const vector<SGTuple> & newPoints,
			  const vector< vector<int> > & newTuples)
  {
    assert(newPoints.size() <= numPlayers);
    assert(newPoints.size() == newTuples.size());
    points = newPoints;
    tuples = newTuples;
  } // setPointsAndTuples
  void setCorner(bool tf)
  {
    corner=tf;
  }

  //! Serializes the action using the boost::serialization library
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & numPlayers;
    ar & state;
    ar & action;
    ar & minIC;
    ar & points;
    ar & tuples;
    ar & isNull;
    ar & corner;
  } // serialize

  friend class boost::serialization::access;
}; // SGBaseAction


#endif
