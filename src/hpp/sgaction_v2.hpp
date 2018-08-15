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
#ifndef _SGACTION_V2_HPP
#define _SGACTION_V2_HPP

#include "sgcommon.hpp"
#include "sgtuple.hpp"
#include "sgenv.hpp"
#include "sggame.hpp"
#include "sgbaseaction.hpp"
#include "sghyperplane.hpp"

//! Enhanced version of SGBaseAction
/*! Same functionality as SGBaseAction, but includes additional
    methods for computing payoffs and a reference to a parent SGEnv
    object to control parameters for the computation.

  \ingroup src
*/
class SGAction_V2 : public SGBaseAction
{
private:
  const SGEnv & env; /*!< Constant reference to the parent
                        environment. */


  vector<SGTuple> bndryDirs; /*!< Stores the slope of the
                                         frontier at the extreme
                                         payoffs. */
  
  vector< SGTuple > trimmedPoints; /*!< Stores the "trimmed" points
                                      before updating. */

public:
  //! Constructor
  /*! Constructs a null action associated with the given SGEnv. */
  SGAction_V2(const SGEnv & _env):
    env(_env),
    SGBaseAction()
  {}

  //! Constructor
  /*! Constructs an action for the given state and action index in the
      given environment. */
  SGAction_V2(const SGEnv & _env, int _state, int _action):
    env(_env), SGBaseAction(_state,_action),
    bndryDirs(2,SGTuple(2))
  {
    trimmedPoints.resize(2);
  }

  void resetTrimmedPoints()
  {
    SGPoint point = minIC;
    trimmedPoints[0].clear();
    trimmedPoints[0].push_back(point);
    point[1] = numeric_limits<double>::max();
    trimmedPoints[0].push_back(point);
    point[1] =  minIC[1];
    trimmedPoints[1].push_back(point);
    point[0] = numeric_limits<double>::max();
    trimmedPoints[1].push_back(point);
  } // resetTrimmedPoints

  void resetPoints()
  {
    resetTrimmedPoints();
    points = trimmedPoints;
  } // resetPoints

  //! Sets points equal to the trimmed points
  void updateTrim() 
  { 
    points = trimmedPoints; 
    for (int player = 0; player < 2; player++)
      {
	if (points[player].size() == 0)
	  tuples[player] = vector<int>(0);
      }
  }
    
  //! Intersects the segment with the ray emanating from the pivot
  void intersectHalfSpace(const SGPoint & normal,
			  const double level,
			  int player,
			  SGTuple & segment);


  
  //! Trims the trimmedPoints using intersectRaySegment.
  void trim(const SGPoint & normal,
	    double level);


  //! Calculates the minimum incentive compatible continuation payoff
  void calculateMinIC(const SGGame & game,
  		      const vector<bool> & update,
  		      const SGTuple & threatTuple);


  //! Calculates the IC constraint.
  /*! Calculates the minimum incentive compatible expected
      continuation value for the given action, relative to the given
      threat tuple and for the given SGGame. */
  static double calculateMinIC(int action,int state, int player,
  			       const SGGame & game,
  			       const SGTuple & threatTuple);

  //! Get method for trimmed points
  const vector<SGTuple> & getTrimmedPoints() const { return trimmedPoints; }

  // Returns whether the action can be supported
  bool supportable() const
  {
    bool tf = false;
    for (int player = 0; player < points.size(); player++)
      if (points[player].size()>0)
	tf = true;

    return tf;
  }
  
  ////////////////////////////// old methods
  
  // //! Static method to carry out trimming operations
  // /*! Intersects the action with the ray emanating from pivot in the
  //     given direction. */
  // void intersectRaySegment(const SGPoint & normal,
  // 			   double level,
  // 			   int player);

  // //! Trims binding continuation segments
  // /*! Intersects the binding continuation segments in SGAction_V2::points
  //     with the half space that is below pivot in
  //     direction.getNormal(). */
  // void intersectRay(const SGPoint & normal, 
  // 		    double level);

  // //! Get method for bndryDirs
  // const vector<SGTuple> & getBndryDirs() const { return bndryDirs; }

  // //! Trims binding continuation segments
  // /*! Intersects the binding continuation segments in SGAction_V2::points
  //     with the half space that is below pivot in
  //     direction.getNormal(). */
  // void intersectRay(const SGPoint & pivot, 
  // 		    const SGPoint & direction);
  // //! Static method to carry out trimming operations
  // /*! Intersects the action with the ray emanating from pivot in the
  //     given direction. */
  // void intersectRaySegment(const SGPoint & pivot,
  // 			   const SGPoint & direction,
  // 			   int player);

  // //! Intersects the segment with the ray emanating from the pivot
  // void intersectRaySegment(const SGPoint & pivot,
  // 			   const SGPoint & direction,
  // 			   SGTuple & segment);

  // //! Trims the trimmedPoints using intersectRaySegment.
  // void trim(const SGPoint & pivot,
  // 	    const SGPoint & direction);

  // //! Trims binding continuation segments
  // /*! Intersects the binding continuation segments in SGAction_V2::points
  //     with the half space that is below pivot in
  //     direction.getNormal(). */
  // void intersectRay(const SGPoint & normal, 
  // 		    double level);
  // //! Static method to carry out trimming operations
  // /*! Intersects the action with the ray emanating from pivot in the
  //     given direction. */
  // void intersectRaySegment(const SGPoint & normal,
  // 			   double level,
  // 			   int player);

  // //! Intersects the segment with the ray emanating from the pivot
  // void intersectRaySegment(const SGPoint & normal,
  // 			   const double level,
  // 			   int player,
  // 			   SGTuple & segment);


  
  // //! Trims the trimmedPoints using intersectRaySegment.
  // void trim(const SGPoint & normal,
  // 	    double level);

  // //! Calculates binding continuation values from hyperplane constraints
  // void calculateBindingContinuations(const SGGame & game,
  // 				     const vector<SGHyperplane> & W);
  
  // void calculateBindingContinuations(const vector<bool> & updatedThreatTuple,
  // 				     const SGGame & game,
  // 				     const vector<SGTuple> & extremeTuples,
  // 				     const SGTuple & threatTuple,
  // 				     const SGTuple & pivot,
  // 				     const SGPoint & currentDirection,
  // 				     int oldWest);

  

}; // SGAction_V2


#endif
