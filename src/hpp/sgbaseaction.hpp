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
    isNull(true)
  {}

  //! Constructor
  /*! Constructs an action for the given state and action index in the
      given environment. */
  SGBaseAction(int _state, int _action):
    state(_state),
    action(_action),
    minIC(-numeric_limits<double>::max()),
    isNull(false),
    corner(false)
  {
    points.resize(2);
    tuples.resize(2); 
  }

  //! Destructor
  ~SGBaseAction() {}

  //! Returns true if the action is null.
  bool getIsNull() const {return isNull;}

  //! Returns the action
  int getAction() const { return action; }
  //! Returns the tuples array
  const vector< vector<int> > & getTuples() const { return tuples; }
  //! Returns the state
  int getState() const { return state; }
  //! Returns the minimum IC continuation values
  const SGPoint & getMinICPayoffs() const {return minIC;}
  //! Returns the array of binding continuation values.
  const vector<SGTuple> & getBindingContinuations() const { return points; }
  //! Serializes the action using the boost::serialization library
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & state;
    ar & action;
    ar & minIC;
    ar & points;
    ar & tuples;
    ar & isNull;
    ar & corner;
  }

  friend class boost::serialization::access;
}; // SGBaseAction


#endif
