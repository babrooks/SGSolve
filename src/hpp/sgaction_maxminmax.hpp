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
#ifndef _SGACTION_MAXMINMAX_HPP
#define _SGACTION_MAXMINMAX_HPP

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
class SGAction_MaxMinMax : public SGBaseAction
{
private:
  const SGEnv * env; /*!< Pointer to the parent environment. */


  vector<SGTuple> trimmedPoints; /*!< Stores the "trimmed" points
                                      before updating. */
  vector<SGTuple> trimmedBndryDirs; /*!< Stores the boundary
                                         directions for the trimmed
                                         points. */
  // vector< vector<SGTuple> > bndryDirs_3Player; /*!< Stores boundary
  //                                               directions for three
  //                                               players. */ 

  
public:
  //! Default constructor
  SGAction_MaxMinMax():
    env(new SGEnv()),
    SGBaseAction()
  {}
  
  //! Constructor
  /*! Constructs a null action associated with the given SGEnv. */
  SGAction_MaxMinMax(const SGEnv & _env):
    env(&_env),
    SGBaseAction()
  {}

  //! Constructor
  /*! Grandfather in two player code. */
  SGAction_MaxMinMax(const SGEnv & _env,
		     int _state,
		     int _action):
    SGAction_MaxMinMax(_env,2,_state,_action)
  {}

  //! Constructor
  /*! Constructs an action for the given state and action index in the
      given environment. */
  SGAction_MaxMinMax(const SGEnv & _env,
		     int _numPlayers,
		     int _state,
		     int _action):
    env(&_env),
    SGBaseAction(_numPlayers,_state,_action),
    trimmedBndryDirs(_numPlayers,SGTuple(2,SGPoint(2,0.0)))
  {
    trimmedPoints.resize(_numPlayers);
    // cout << numPlayers << endl;
  }

  //! Resets the trimmed points for two players
  /*!< Sets the trimmed points to rays that point from the minimum
     incentive compatible continuation value along the positive
     coordinate axes. */
  void resetTrimmedPoints();
  
  //! Resets the trimmed points for three players
  /*!< Sets the trimmed points to a large box in the coordinate plane
     between the minimum incentive compatible payoff and the payoff
     upper bound. */
  void resetTrimmedPoints(const SGPoint & payoffUB);
  
  //! Sets points equal to the trimmed points
  void updateTrim() 
  { 
    points = trimmedPoints; 
    for (int player = 0; player < numPlayers; player++)
      {
	if (points[player].size() == 0)
	  tuples[player] = vector<int>(0);
      }
    bndryDirs = trimmedBndryDirs;
  }
    
  //! Intersects the segment with a half space
  void intersectHalfSpace(const SGPoint & normal,
			  const double level,
			  int player,
			  SGTuple & segment,
			  SGTuple & segmentDirs);

  //! Intersects the IC polygon with a half space
  void intersectPolygonHalfSpace(const SGPoint & normal,
				 const double level,
				 int player,
				 SGTuple & extPnts,
				 SGTuple & extPntDirs);
  
  //! Trims the trimmedPoints using intersectRaySegment.
  void trim(const SGPoint & normal,
	    double level);


  //! Calculates the minimum incentive compatible continuation payoff
  void calculateMinIC(const SGGame & game,
  		      const vector<bool> & update,
  		      const SGTuple & threatTuple);

  //! Calculates the minimum incentive compatible continuation payoff
  void calculateMinIC(const SGGame & game,
  		      const SGTuple & threatTuple)
  {
    calculateMinIC(game,vector<bool>(numPlayers,true),threatTuple);
  }

  // //! Computes non-redudant boundary directions
  // /*! Takes as input a list of bounding hyperplanes and computes which
  //     ones are extreme around a given point. */
  // void computeBndryDirs_3Player(const list<SGHyperplane> & hyperplanes);

  //! Calculates the IC constraint.
  /*! Calculates the minimum incentive compatible expected
      continuation value for the given action, relative to the given
      threat tuple and for the given SGGame. */
  static double calculateMinIC(int action,int state, int player,
  			       const SGGame & game,
  			       const SGTuple & threatTuple);

  //! Get method for trimmed points
  const vector<SGTuple> & getTrimmedPoints() const { return trimmedPoints; }

  //! Get method for trimmed points
  const vector<SGTuple> & getTrimmedBndryDirs() const { return trimmedBndryDirs; }

  //! Get boundary direction for 3 players
  /*!< Returns the cross product of the boundary directions at the
     given point, i.e., the direction along an edge of the boundary
     that points into the feasible and IC region. Only works for three
     players. */
  const SGPoint getBndryDir(const int player,const int point) const;
  
  // Returns whether the action can be supported
  bool supportable(const SGPoint & feasiblePoint ) const
  {
    bool tf = false;
    for (int player = 0; player < points.size(); player++)
      if (points[player].size()>0)
	return true;

    if (feasiblePoint >= minIC-1e-6)
      return true;

    return false;
  }

  void testThreePlayerIntersection();

}; // SGAction_MaxMinMax

bool operator==(const SGAction_MaxMinMax & lhs,
		const SGAction_MaxMinMax & rhs);

bool operator<(const SGAction_MaxMinMax & lhs,
	       const SGAction_MaxMinMax & rhs);

template class std::list<SGAction_MaxMinMax>;

#endif
