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

#include "sgedgepolicy.hpp"

//! Class for iterating edges of a product policy
SGEdgePolicy::SGEdgePolicy(const SGProductPolicy & _master):
  master(_master), policies(_master.numStates()),
  subState(0), baseState(0)
{
  int s;
  for (s = 0; s < master.numStates(); s++)
    policies[s] = master.getPolicies(s).cbegin();
    
  for (s = 0; s < master.numStates(); s++)
    {
      if (master.getPolicies(s).size()>1)
	{
	  subPolicy = master.getPolicies(s).begin();
	  subPolicy++;
	  subState = s;
	  baseState = s;
	  return;
	}
    }

  // If we got here, could not find a state with more than one policy.
  throw(SGException(SG::NOEDGES));
} // constructor

bool SGEdgePolicy::incrementBasePolicy()
{
  int state = master.numStates();
  while (state-- > 0)
    {
      // Try to increment the policy in this state. If unable to, keep
      // going down. 
      if (++policies[state] != master.getPolicies(state).cend())
	{
	  // We were able to increment the policy. Now set the
	  // policies in higher states back to the beginning.
	  while (++state < master.numStates())
	    {
	      policies[state] = master.getPolicies(state).cbegin();
	    }

	  // Finally, reset the substitution.
	  subState = 0;
	  subPolicy = policies[subState];
	  return incrementSubPolicy();

	  // Couldn't reset the sub policy
	  return false;
	} // if
    } // while
  // Couldn't increment the base policy.
  return false;
} // incrementBasePolicy

bool SGEdgePolicy::incrementSubPolicy()
{
  if (++subPolicy != master.getPolicies(subState).cend())
    return true;

  // Need to increment the substate.
  while (++subState < master.numStates())
    {
      subPolicy = policies[subState];
      if (++subPolicy != master.getPolicies(subState).cend())
	return true;
    }

  // Couldn't increment the sub policy.
  return false;
}

bool SGEdgePolicy::operator++()
{
  return (incrementSubPolicy() || incrementBasePolicy() );
} // operator++

