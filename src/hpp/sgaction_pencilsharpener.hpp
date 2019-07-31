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

// Classes for storing data for stochastic games.
#ifndef _SGACTION_HPP
#define _SGACTION_HPP

#include "sgcommon.hpp"
#include "sgtuple.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgbaseaction.hpp"

//! Enhanced version of SGBaseAction
/*! Same functionality as SGBaseAction, but includes additional
    methods for computing payoffs and a reference to a parent SGEnv
    object to control parameters for the computation.

    This class is used by the pencil sharpening algorithm and has been
    deprecated.

  \ingroup src
*/
class SGAction : public SGBaseAction
{
private:
  const SGEnv & env; /*!< Constant reference to the parent
                        environment. */


  vector< SGTuple > trimmedPoints; /*!< Stores the "trimmed" points
                                      before updating. */ 

public:
  //! Constructor
  /*! Constructs a null action associated with the given SGEnv. */
  SGAction(const SGEnv & _env):
    env(_env),
    SGBaseAction()
  {}

  //! Constructor
  /*! Constructs an action for the given state and action index in the
      given environment. */
  SGAction(const SGEnv & _env, int _state, int _action):
    env(_env), SGBaseAction(_state,_action)
  {
    trimmedPoints.resize(2);
  }

  //! Get method for trimmed points
  const vector<SGTuple> & getTrimmedPoints() const { return trimmedPoints; }

  //! Trims binding continuation segments
  /*! Intersects the binding continuation segments in SGAction::points
      with the half space that is below pivot in
      direction.getNormal(). */
  void intersectRay(const SGPoint & pivot, 
  		    const SGPoint & direction);
  //! Static method to carry out trimming operations
  /*! Intersects the action with the ray emanating from pivot in the
      given direction. */
  void intersectRaySegment(const SGPoint & pivot,
			   const SGPoint & direction,
			   int player);

  //! Intersects the segment with the ray emanating from the pivot
  void intersectRaySegment(const SGPoint & pivot,
			   const SGPoint & direction,
			   SGTuple & segment);

  //! Trims the trimmedPoints using intersectRaySegment.
  void trim(const SGPoint & pivot,
	    const SGPoint & direction);

  //! Calculates the minimum incentive compatible continuation payoff
  void calculateMinIC(const SGGame & game,
		      const vector<bool> & update,
		      const SGTuple & threatTuple);

  //! Sets points equal to the trimmed points
  void updateTrim();

  //! Calculates sup norm distance between comparable points and trimmed points
  double distToTrimmed() const;

  //! Calculates binding continuation values.
  /*! This code is deprecated. Was part of the pencil sharpening algorithm. */
  void calculateBindingContinuations(const vector<bool> & updatedThreatTuple,
				     const SGGame & game,
				     const vector<SGTuple> & extremeTuples,
				     const SGTuple & threatTuple,
				     const SGTuple & pivot,
				     const SGPoint & currentDirection,
				     int oldWest);

  //! Calculates the IC constraint.
  /*! Calculates the minimum incentive compatible expected
      continuation value for the given action, relative to the given
      threat tuple and for the given SGGame. */
  static double calculateMinIC(int action,int state, int player,
			       const SGGame & game,
			       const SGTuple & threatTuple);

}; // SGAction


#endif
