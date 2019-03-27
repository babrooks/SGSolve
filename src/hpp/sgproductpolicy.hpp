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
  
  bool isempty() const
  {
    bool tf = false;
    for (int state = 0; state < policies.size(); state++)
      {
	if (policies[state].size()==0)
	  tf = true;
      }
    return tf;
  }
  
  int dimension() const
  {
    int dim = 0;
    for (int state = 0; state < policies.size(); state++)
      dim += policies[state].size();
    
    return dim-policies.size()+1;
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

#endif
