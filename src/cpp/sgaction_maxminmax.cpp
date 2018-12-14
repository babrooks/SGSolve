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


void SGAction_MaxMinMax::resetTrimmedPoints()
{
  assert(numPlayers==2);
    
  SGPoint point = minIC;
  trimmedPoints[0].clear();
  trimmedPoints[0].push_back(point);
  point[1] = numeric_limits<double>::max();
  trimmedPoints[0].push_back(point);

  point[1] =  minIC[1];
  trimmedPoints[1].clear();
  trimmedPoints[1].push_back(point);
  point[0] = numeric_limits<double>::max();
  trimmedPoints[1].push_back(point);

  trimmedBndryDirs = vector<SGTuple> (2,SGTuple(2,SGPoint(2,0.0)));
} // resetTrimmedPoints


void SGAction_MaxMinMax::resetTrimmedPoints(const SGPoint & payoffUB)
{
  assert(numPlayers==3);
    
  trimmedBndryDirs = vector<SGTuple> (3,SGTuple(4,SGPoint(3,0.0)));
  for (int p = 0; p < numPlayers; p++)
    {
      SGPoint point = minIC;
      trimmedPoints[p].clear();
      trimmedPoints[p].push_back(point);
      point[(p+1)%numPlayers] = payoffUB[(p+1)%numPlayers];
      trimmedPoints[p].push_back(point);
      point[(p+2)%numPlayers] = payoffUB[(p+2)%numPlayers];
      trimmedPoints[p].push_back(point);
      point[(p+1)%numPlayers] = minIC[(p+1)%numPlayers];
      trimmedPoints[p].push_back(point);

      // SGPoint dir(3,0.0);
      // trimmedBndryDirs[p].clear();
      // dir[(p+1)%numPlayers] = -1;
      // trimmedBndryDirs[p].push_back(dir);
      // dir[(p+1)%numPlayers] = 0;
      // dir[(p+2)%numPlayers] = 1;
      // trimmedBndryDirs[p].push_back(dir);
      // dir[(p+1)%numPlayers] = 1;
      // dir[(p+2)%numPlayers] = 0;
      // trimmedBndryDirs[p].push_back(dir);
      // dir[(p+1)%numPlayers] = 0;
      // dir[(p+2)%numPlayers] = -1;
      // trimmedBndryDirs[p].push_back(dir);

      SGPoint dir(3,0.0);
      trimmedBndryDirs[p].clear();
      dir[(p+2)%numPlayers] = -1;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = 1;
      dir[(p+2)%numPlayers] = 0;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = 0;
      dir[(p+2)%numPlayers] = 1;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = -1;
      dir[(p+2)%numPlayers] = 0;
      trimmedBndryDirs[p].push_back(dir);
    }
} // resetTrimmedPoints


void SGAction_MaxMinMax::trim(const SGPoint& normal,
			      double level)
{
  if (numPlayers==2)
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
  else if (numPlayers==3)
    {
      for (int player=0; player < points.size(); player++)
	{
	  if (trimmedPoints[player].size()==0)
	    continue;
	  
	  intersectPolygonHalfSpace(normal,
				    level,
				    player,
				    trimmedPoints[player],
				    trimmedBndryDirs[player]);
	}
    }
} // trim

const SGPoint SGAction_MaxMinMax::getBndryDir(const int player,
					      const int point) const
{
  assert(numPlayers == 3);
  assert(point < bndryDirs[player].size());
  
  return SGPoint::cross(bndryDirs[player][(point+1)%bndryDirs[player].size()],
			bndryDirs[player][point]);

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
      
      if (l0 > level + env->getParam(SG::ICTOL)
	  && l1 > level + env->getParam(SG::ICTOL))
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
      else if (abs(l0 - l1)>env->getParam(SG::INTERSECTTOL))
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
  int k0 = 0; 
  double l0;
  for (k0 = 0; k0 < extPnts.size(); k0++)
    {
      l0 = extPnts[k0] * normal;
      if (l0 < level+env->getParam(SG::ICTOL))
	break;
    } 

  if (k0 == extPnts.size())
    {
      // Whole set is ouside the half space.
      extPnts.clear();
      extPntDirs.clear();

      // cout << "Alert: No binding continuation values found. Player: " << player
      // 	   << ", state: " << state
      // 	   << ", action: " << action
      // 	   << endl;
      return;
    }

  // Now keep iterating to find a point outside the half space;
  int k1;
  double l1;
  for (k1 = (k0+1)%extPnts.size();
       k1 != k0;
       k1 = (k1+1)%extPnts.size())
    {
      l1 = extPnts[k1] * normal;
      if (l1 >= level+env->getParam(SG::ICTOL))
	break;
    }

  if (k1 == k0)
    {
      // Whole set is inside the half space. Do nothing.
      return;
    }

  // Remaining case is that there is at least one point (k0) that is
  // inside and one point (k1) outside the half space. Move k0 so that
  // it is the first point inside after k1.
  for (k0 = (k1+1)%extPnts.size();
       k0 != k1;
       k0 = (k0+1)%extPnts.size())
    {
      l0 = extPnts[k0] * normal;
      if (l0 < level+env->getParam(SG::ICTOL))
	break;
    } 

  // add two new points for the intersections.
  int k = (k1-1+extPnts.size())%extPnts.size(); // The inside point just before k1
  double l = extPnts[k] * normal;
  double weight = (level-l)/(l1-l);
  SGPoint intersection1 = weight*extPnts[k1]+(1-weight)*extPnts[k];

  k = (k0-1+extPnts.size())%extPnts.size(); // The outside point just before k0
  l = extPnts[k] * normal;
  weight = (level-l)/(l0-l);
  SGPoint intersection0 = weight*extPnts[k0]+(1-weight)*extPnts[k];

  // Replace k1 (which is outside) with the intersection between k1 and k1-1
  extPnts[k1] = intersection1;
  // extPntDirs[k1] = normal;

  // Now have to insert intersection0 and delete points that are
  // outside the half space.
  if (k==k1)
    {
      // There was only one point outside, which was k1. Then we have
      // to insert a new point.
      extPnts.emplace(k0,intersection0);
      extPntDirs.emplace(k0,normal);
    }
  else
    {
      // There is more than one point outside. Replace point at k with
      // intersection0.
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
	  extPntDirs.erase(k1+1,extPntDirs.size());
	  extPntDirs.erase(0,k);
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
  // resetTrimmedPoints();

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

bool operator==(const SGAction_MaxMinMax & lhs,
		const SGAction_MaxMinMax & rhs)
{
  return lhs.getState() == rhs.getState() && lhs.getAction() == rhs.getAction();
}

bool operator<(const SGAction_MaxMinMax & lhs,
	       const SGAction_MaxMinMax & rhs)
{
  return lhs.getState() < rhs.getState() ||
    (lhs.getState() == rhs.getState() && lhs.getAction() < rhs.getAction());
}
