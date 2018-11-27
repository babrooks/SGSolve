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

// BAB 9-25-2012
#include "sgaction_maxminmax.hpp"

/* SGAction_MaxMinMax */

void SGAction_MaxMinMax::trim(const SGPoint& normal,
			      double level)
{
  assert(trimmedBndryDirs.size()== 2);
  for (int player=0; player < points.size(); player++)
    {
      assert(trimmedPoints[player].size() == trimmedBndryDirs[player].size());
      intersectHalfSpace(normal,level,player,
			 trimmedPoints[player],
			 trimmedBndryDirs[player]);
    }
}

const SGPoint SGAction_MaxMinMax::getBndryDir(const int player,
					      const int point) const
{
  assert(numPlayers == 3);
  assert(point < bndryDirs[player].size());
  
  return SGPoint::cross(bndryDirs[player][point],
			bndryDirs[player][(point-1)%bndryDirs.size()]);

} // getBndryDir


void SGAction_MaxMinMax::intersectHalfSpace(const SGPoint& normal,
					    const double level,
					    const int player,
					    SGTuple & segment,
					    SGTuple & segmentDirs)
{
  if (segment.size() == 2)
    {
      // Two points of intersection for this IC constraint.
	      
      // Determine which of the two points is on the far side
      // of the clockwise ray.
      double l0 = normal * segment[0];
      double l1 = normal * segment[1];
      
      if (l0 > level + env.getParam(SG::ICTOL)
	  && l1 > level + env.getParam(SG::ICTOL))
	{
	  // Both lie above the ray.
	  // cout << "Warning: No binding IC Payoffs for (s,a)=(" << state << "," << action << ")" << endl;
	  segment.clear();
	  segmentDirs.clear();
	}
      else if (l0 < level
	       && l1 < level)
	{
	  // Leave points alone.
	}
      else if (abs(l0 - l1)>env.getParam(SG::INTERSECTTOL))
	{
	  // Can take intersection.
	  double weightOn1 = (level - l0)/(l1 - l0);

	  // In first two cases, must be very close to being coplanar
	  // with the normal. Just make both points the same. (Is this
	  // necessary? Seems like it might screw things up.)
	  if (weightOn1 > 1)
	    segment[0] = segment[1];
	  else if (weightOn1 < 0)
	    segment[1] = segment[0];
	  else
	    {
	      // Intersection is interior.
	      SGPoint intersection 
		= weightOn1 * segment[1] +
		(1.0 - weightOn1) * segment[0];
	      int replace = (l0 < l1);

	      // Flip the sign of the normal to point along the
	      // frontier, depending on which of the 4 binding points
	      // we are at.
	      segment[replace] = intersection;
	      if (player == replace)
		segmentDirs[replace] = normal.getNormal();
	      else
		segmentDirs[replace] = (-1)*normal.getNormal();
	    }
	}
    }
  else
    {
      // No points of intersection. Do nothing.
      segment = SGTuple();
    }
} // intersectHalfSpace

void SGAction_MaxMinMax::intersectPolygonHalfSpace(const SGPoint & normal,
						   const double level,
						   int player,
						   SGTuple & extPnts,
						   SGTuple & extPntDirs)
{
  assert(numPlayers == 3);

  // Iterate to find a point inside the half space.
  int k0 = -1; 
  double l0;
  do
    {
      k0++;
      l0 = extPnts[k0] * normal;
    } while (l0 > level && k0 < extPnts.size());

  if (k0 == extPnts.size())
    {
      // Whole set is ouside the half space.
      extPnts.clear();
      extPntDirs.clear();
      return;
    }

  // Now keep iterating to find a point outside the half space;
  int k1 = k0;
  double l1;
  do
    {
      k1 = (k1+1)%extPnts.size();
      l1 = extPnts[k1] * normal;
    } while (l1 <= level && k1 != k0);

  if (k1 == k0)
    {
      // Whole set is inside the half space. Do nothing.
      return;
    }

  // Remaining case is that there is at least one point (k0) that is
  // inside and one point (k1) outside. Move k0 so that it is the
  // first point inside after k1.
  k0 = k1;
  do
    {
      k0 = (k0+1)%extPnts.size();
      l0 = extPnts[k0] * normal;
    } while (l0 > level);

  // add two new points for the intersections.
  int k = (k1-1)%extPnts.size();
  double l = extPnts[k] * normal;
  double weight = (level-l)/(l1-l);
  SGPoint intersection1 = weight*extPnts[k1]+(1-weight)*extPnts[k];

  k = (k0+1)%extPnts.size();
  l = extPnts[k] * normal;
  weight = (level-l)/(l0-l);
  SGPoint intersection0 = weight*extPnts[k0]+(1-weight)*extPnts[k];

  extPnts[k1] = intersection1;
  extPntDirs[k1] = normal;
  if (k==k1)
    {
      extPnts.emplace(k,intersection0);
      extPntDirs.emplace(k,normal);
    }
  else
    {
      extPnts[k] = intersection0;
      extPntDirs[k] = normal;

      // Delete points strictly after k1 and strictly before k (which are
      // all outside)
      if (k1<k)
	{
	  extPnts.erase(k1+1,k);
	  extPntDirs.erase(k1+1,k);
	}
      else // k0>k1
	{
	  extPnts.erase(k1+1,extPnts.size());
	  extPnts.erase(0,k);
	}
    }
} // intersectPolygonHalfSpace


void SGAction_MaxMinMax::calculateMinIC(const SGGame & game,
					const vector<bool> & update,
					const SGTuple & threatTuple)
{
  assert(numPlayers == game.getNumPlayers());
  for (int player = 0;
       player < game.getNumPlayers();
       player++)
    {
      if (!update[player])
	continue;

      minIC[player] = SGAction_MaxMinMax::calculateMinIC(action,state,player,
							 game,threatTuple);
    }
  resetTrimmedPoints();

} // calculateMinIC

double SGAction_MaxMinMax::calculateMinIC(int action,int state,int player,
					  const SGGame & game,
					  const SGTuple & threatTuple)
{
  vector<int> playersActions, playersDeviations;
  int deviation, deviationIndex;
  double currentGains;

  double tmpMinIC = -numeric_limits<double>::max();

  indexToVector(action,playersActions,
		game.getNumActions()[state]);
  playersDeviations = playersActions;
  
  for (deviation = 0;
       deviation < game.getNumActions()[state][player];
       deviation++)
    {
      playersDeviations[player] = deviation;

      deviationIndex = vectorToIndex(playersDeviations,
				     game.getNumActions()[state]);
      currentGains 
	= (1-game.getDelta())/game.getDelta() 
	* (game.getPayoffs()[state][deviationIndex][player] 
	   - game.getPayoffs()[state][action][player])
	+ threatTuple.expectation(game.getProbabilities()[state][deviationIndex], 
				  player);

      if (currentGains > tmpMinIC)
	tmpMinIC = currentGains;
    } // deviation

  return tmpMinIC;
}  // calculateMinIC

