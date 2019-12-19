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

      SGPoint dir(3,0.0);
      trimmedBndryDirs[p].clear();
      dir[(p+1)%numPlayers] = -1;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = 0;
      dir[(p+2)%numPlayers] = -1;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = 1;
      dir[(p+2)%numPlayers] = 0;
      trimmedBndryDirs[p].push_back(dir);
      dir[(p+1)%numPlayers] = 0;
      dir[(p+2)%numPlayers] = 1;
      trimmedBndryDirs[p].push_back(dir);
    }
} // resetTrimmedPoints


bool SGAction_MaxMinMax::trim(const SGPoint& normal,
			      double level)
{
  bool tf = false;
  if (numPlayers==2)
    {
      assert(trimmedBndryDirs.size()== 2);
      for (int player=0; player < points.size(); player++)
	{
	  assert(trimmedPoints[player].size() == trimmedBndryDirs[player].size());
	  if (intersectHalfSpace(normal,level,player,
					trimmedPoints[player],
				 trimmedBndryDirs[player]))
	    tf = true;
	}
    }
  else if (numPlayers==3)
    {
      for (int player=0; player < points.size(); player++)
	{
	  if (trimmedPoints[player].size()==0)
	    continue;

	  if (intersectPolygonHalfSpace(normal,
					level,
					player,
					trimmedPoints[player],
					trimmedBndryDirs[player]))
	    tf = true;
	}
    }
  return tf;
} // trim

  //! Sets points equal to the trimmed points
void SGAction_MaxMinMax::updateTrim() 
{
  points = trimmedPoints;
  for (int player = 0; player < numPlayers; player++)
    {
      if (points[player].size() == 0)
	tuples[player] = vector<int>(0);
    }
  bndryDirs = trimmedBndryDirs;
}

double SGAction_MaxMinMax::distToTrimmed() const
{
  double dist = 0;
  for (int player = 0; player < 2; player++)
    {
      if (points[player].size() != trimmedPoints[player].size())
	return 1.0;

      for (int k = 0; k < points[player].size(); k++)
	{
	  for (int p = 0; p < numPlayers; p++)
	    dist = max(dist,abs(points[player][k][p]
				-trimmedPoints[player][k][p]));
	}
    }
  return dist;
} // distToTrimmed
    
const SGPoint SGAction_MaxMinMax::getBndryDir(const int player,
					      const int point) const
{
  assert(numPlayers == 3);
  assert(point < bndryDirs[player].size());
  
  return SGPoint::cross(bndryDirs[player][point],
			bndryDirs[player][(point+1+bndryDirs[player].size())%bndryDirs[player].size()]);

} // getBndryDir


bool SGAction_MaxMinMax::intersectHalfSpace(const SGPoint& normal,
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
      else if (l0 < level+env->getParam(SG::ICTOL)
	       && l1 < level+env->getParam(SG::ICTOL))
	{
	  // Leave points alone.
	  return false;
	}
      else if (abs(l0 - l1)>env->getParam(SG::INTERSECTTOL))
	{
	  // Can take intersection.
	  //double weightOn1 = max(0.0,min((level - l0)/(l1 - l0),1.0));

	  double weightOn1 = (level-l0)/(l1-l0);
	  // In first two cases, must be very close to being coplanar
	  // with the normal. Just make both points the same. 
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
		segmentDirs[replace] = (-1.0)*normal.getNormal();
	    }
	}
    }
  else
    {
      // No points of intersection. Do nothing.
      segment = SGTuple();
      return false;
    }

  return true;
} // intersectHalfSpace

bool SGAction_MaxMinMax::intersectPolygonHalfSpace(const SGPoint & normal,
						   const double level,
						   int player,
						   SGTuple & extPnts,
						   SGTuple & extPntDirs)
{
  assert(numPlayers == 3);

  if (extPnts.size() == 0)
    return false;
  
  // Iterate to find a point inside the half space.
  int k0 = 0; 
  int k1, k2;
  double l0;
  for (k0 = 0; k0 < extPnts.size(); k0++)
    {
      l0 = extPnts[k0] * normal;
      k1 = (k0+1+extPnts.size())%extPnts.size();

      // Conditions for being considered "inside" are that either (a)
      // the point itself is below the hyperplane or (b) the point is
      // on the hyperplane but the boundary direction points below or
      // (c) the boundary direction points above and if we substitute
      // in the normal for this point, the new boundary direction
      // would point above the old normal.
      if (l0 < level-env->getParam(SG::ICTOL)
	  || (abs(l0-level) <= env->getParam(SG::ICTOL)
	      && (SGPoint::cross(extPntDirs[k0],extPntDirs[k1])*normal < env->getParam(SG::ICTOL) ) ) )
  	break;
    }

  if (k0 == extPnts.size())
    {
      // Whole set is ouside the half space.
      extPnts.clear();
      extPntDirs.clear();
      return true;
    }

  // Now keep iterating to find a point outside the half space;
  double l1;
  for (k1 = (k0+1)%extPnts.size();
       k1 != k0;
       k1 = (k1+1)%extPnts.size())
    {
      l1 = extPnts[k1] * normal;
      k2 = (k1+1+extPnts.size())%extPnts.size();

      // Conditions for being considered "outside" are that either (a)
      // the point itself is above the hyperplane or (b) the point is
      // on the hyperplane but the boundary direction points outside
      // and if we substitute in the normal for the previous bounding
      // direction, the new boundary direction would point above the
      // old normal.
      if (l1 > level+env->getParam(SG::ICTOL)
	  || (abs(l1-level) <= env->getParam(SG::ICTOL)
	      && (SGPoint::cross(extPntDirs[k1],extPntDirs[k2])*normal >= env->getParam(SG::ICTOL) ) ) )
  	break;
    }

  if (k1 == k0)
    {
      // Whole set is inside the half space. Do nothing.
      return false;
    }

  // Remaining case is that there is at least one point (k0) that is
  // inside and one point (k1) outside the half space. Move k0 so that
  // it is the first point inside after k1.
  for (k0 = (k1+1)%extPnts.size();
       k0 != k1;
       k0 = (k0+1)%extPnts.size())
    {
      l0 = extPnts[k0] * normal;
      k2 = (k0+1+extPnts.size())%extPnts.size();
      if (l0 < level-env->getParam(SG::ICTOL)
	  || (abs(l0-level) <= env->getParam(SG::ICTOL)
	      && (SGPoint::cross(extPntDirs[k0],extPntDirs[k2])*normal < env->getParam(SG::ICTOL) ) ) )
  	break;
    }

  // add two new points for the intersections.
  bool tf = true;
  
  int k = (k1-1+extPnts.size())%extPnts.size(); // The inside point just before k1
  double l = extPnts[k] * normal;
  double weight1 = 0.0, weight0 = 0.0;
  if (abs(l1-l)>1e-12)
    {
      weight1 = (level-l)/(l1-l);

    }
  else
    {
      weight1=0.0;
    }
  SGPoint intersection1 = weight1*extPnts[k1]+(1.0-weight1)*extPnts[k];

  k = (k0-1+extPnts.size())%extPnts.size(); // The outside point just before k0
  l = extPnts[k] * normal;
  if (abs(l0-l)>1e-12)
    {
      weight0 = max(0.0,min((level-l)/(l0-l),1.0));
      
    }
  else
    {
      weight0=0.0;
    }
  SGPoint intersection0 = weight0*extPnts[k0]+(1.0-weight0)*extPnts[k];

  assert(!intersection1.anyNaN());
  assert(!intersection0.anyNaN());

  // Replace k1 (which is outside) with the intersection between k1 and k1-1
  extPnts[k1] = intersection1;

  // if (weight==0.0)
  //      extPntDirs[k1] = normal;

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


  return tf;
} // intersectPolygonHalfSpace


void SGAction_MaxMinMax::calculateMinIC(const SGGame & game,
					const vector<bool> & update,
					const SGTuple & threatTuple)
{
  if(numPlayers != game.getNumPlayers())
	  throw(SGException(SG::WRONG_NUMBER_OF_PLAYERS));
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

  assert(state<game.getPayoffs().size());
  assert(action<game.getPayoffs()[state].size());
  
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

void SGAction_MaxMinMax::mergeDuplicatePoints(const double tol)
{
  for (int p = 0; p < points.size(); p++)
    {
      for (int k = 0; k < points[p].size(); k++)
	{
	  for (int kp = k+1; kp < points[p].size(); kp++)
	    {
	      if (SGPoint::distance(points[p][k],points[p][kp])<tol)
		{
		  points[p].erase(kp,kp+1);
		  bndryDirs[p].erase(kp,kp+1);
		}
	    } // kp
	} // k
    }
}

bool SGAction_MaxMinMax::supportable() const
{
  for (int player = 0; player < points.size(); player++)
    if (points[player].size()>0)
      return true;
  return false;
}
