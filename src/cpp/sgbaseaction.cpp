#include "sgbaseaction.hpp"

bool SGBaseAction::isCorner(const int p, const int k) const
{
  return SGPoint::distance(points[p][k],minIC)<1e-6;
}
