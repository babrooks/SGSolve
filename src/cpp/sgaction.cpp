// BAB 9-25-2012
#include "sgaction.hpp"

/* SGAction */
SGAction::SGAction(const SGEnv & _env, int _state, int _action):
  env(_env),
  state(_state),
  action(_action),
  minIC(-numeric_limits<double>::max()),
  isNull(false)
{
  points.resize(2);
  tuples.resize(2); 
}

void SGAction::intersectRay(const SGPoint& pivot, 
			    const SGPoint& direction)
{
  for (int player=0; player < points.size(); player++)
    intersectRaySegment(pivot,direction,player);
}

void SGAction::intersectRaySegment(const SGPoint& pivot, 
				   const SGPoint& direction,
				   int player)
{

  SGPoint normal = direction.getNormal();
  double level = pivot * normal;

  // First north south.
  if (points[player].size() == 2)
    {
      // Two points of intersection for the ns IC constraint.
	      
      // Determine which of the two points is on the far side
      // of the clockwise ray.
      double l0 = normal * points[player][0];
      double l1 = normal * points[player][1];

      if (l0 > level + env.ICTol
	  && l1 > level + env.ICTol)
	{
	  // Both lie above the ray.
	  points[player].clear();
	  tuples[player].clear();
	}
      else if (l0 < level
	       && l1 < level)
	{
	  // Leave points alone.
	}
      else if (abs(l0 - l1)>env.intersectTol)
	{
	  // Can take intersection.
	  double weightOn1 = (level - l0)/(l1 - l0);

	  if (weightOn1 > 1)
	    points[player][0] = points[player][1];
	  else if (weightOn1 < 0)
	    points[player][1] = points[player][0];
	  else
	    {
	      SGPoint intersection 
		= weightOn1 * points[player][1] +
		(1.0 - weightOn1) * points[player][0];
	      int replace1 = (l0 < l1);
	  
	      points[player][replace1] = intersection;
	    }
	}
    }
  else
    {
      // No points of intersection. Do nothing.
      points[player] = SGTuple();
      tuples[player] = vector<int>(0);
    }
}

void SGAction::calculateMinIC(const SGGame & game,
			      const vector<bool> & update,
			      const SGTuple & threatTuple)
{
  for (int player = 0;
       player < game.getNumPlayers();
       player++)
    {
      if (!update[player])
	continue;

      minIC[player] = SGAction::calculateMinIC(action,state,player,
					       game,threatTuple);
    }

} // calculateMinIC

double SGAction::calculateMinIC(int action,int state,int player,
				const SGGame & game,
				const SGTuple & threatTuple)
{
  vector<int> playersActions, playersDeviations;
  int deviation, deviationIndex;
  double currentGains;

  double minIC = -numeric_limits<double>::max();

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

      if (currentGains > minIC)
	minIC = currentGains;
    } // deviation

  return minIC;
}
