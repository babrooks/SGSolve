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

#ifndef _SGPRODUCTPOLICY_HPP
#define _SGPRODUCTPOLICY_HPP

#include "sgpolicy.hpp"

struct policyComp {
  bool operator() (const SGPolicy & lhs, const SGPolicy & rhs)
  { return lhs < rhs;}
};

typedef set<SGPolicy,policyComp> SGPolicySet;

//! Class for storing product policies
class SGProductPolicy
{
private:
  vector<SGPolicySet> policies;
  SGPoint dir;
  vector<double> levels;

public:
  SGProductPolicy(int _numStates, const SGPoint & _dir):
    policies(_numStates), dir(_dir), levels(_numStates)
  {}

  const SGPolicySet & getPolicies(int state) const
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    return policies[state];
  }

  const SGPoint & getDir() const
  { return dir; }

  const vector<double> & getLevels() const
  { return levels; }

  void insertPolicy(int state, const SGPolicy & policy)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    policies[state].insert(policy);
  }

  void erasePolicy(int state, const SGPolicy & policy)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    policies[state].erase(policy);
  }

  void setLevel(int state, double lvl)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    levels[state] = lvl;
  }

  void clear()
  {
    for (int state = 0; state < policies.size(); state++)
      policies[state].clear();
  }
  
  std::string hash() const
  {
    std::string str;
    for (int s = 0; s < policies.size(); s++)
      {
	for (auto it = policies[s].begin(); it != policies[s].end(); it++)
	  str += it->hash();
      }

    return str;
  }

  int numStates() const {return policies.size();}
}; // SGProductPolicy

//! Class for iterating edges of a product policy
class SGEdgePolicy
{
private:
  const SGProductPolicy & master;
  vector<SGPolicySet::const_iterator> policies;
  SGPolicySet::const_iterator subPolicy;
  int baseState;
  int subState;
  
public:
  SGEdgePolicy(const SGProductPolicy & _master):
    master(_master), policies(_master.numStates())
  {
    for (int s = 0; s < master.numStates(); s++)
      policies[s] = master.getPolicies(s).begin();
    for (int s = 0; s < master.numStates(); s++)
      {
	if (master.getPolicies(s).size()>1)
	  {
	    subPolicy = master.getPolicies(s).begin()++;
	    subState = s;
	    baseState = s;
	    break;
	  }
      }
  }

  bool operator++()
  {
    // Try to increment the substitute policy
    while (++subPolicy == master.getPolicies(subState).end())
      {
	// Ran out of policies in the current state. Try to find
	// another state.
	while (++subState < master.numStates())
	  {
	    if (master.getPolicies(subState).size()>1)
	      {
		subPolicy = master.getPolicies(subState).begin()++;
		return true;
	      } // if
	  } // while
	
      } // while
    
    // If we got to here, ran out of states to increment the
    // subPolicy. Try to increment the base policy instead. First
    // reset the policy in the base state and then increment the base
    // policy.
    policies[baseState] = master.getPolicies(baseState).begin();
    while (++baseState < master.numStates())
      {
	if (master.getPolicies(subState).size() > 1)
	  {
	    policies[baseState] = master.getPolicies(baseState).begin()++;
	    subState = baseState;
	    subPolicy = policies[baseState]++;

	    // Try to increment the substitute policy
	    while (++subPolicy == master.getPolicies(subState).end())
	      {
		// Ran out of policies in the current state. Try to find
		// another state.
		while (++subState < master.numStates())
		  {
		    if (master.getPolicies(subState).size()>1)
		      {
			subPolicy = master.getPolicies(subState).begin()++;
			return true;
		      } // if
		  } // while
	      } // while
	  }
      }

    return false;
  }
  
}; // SGEdgePolicy

#endif
