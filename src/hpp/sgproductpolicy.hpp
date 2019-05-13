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

//! Function object for comparing two policies
struct policyComp {
  bool operator() (const SGPolicy & lhs, const SGPolicy & rhs)
  { return lhs < rhs;}
};

typedef set<SGPolicy,policyComp> SGPolicySet;

//! Class for storing product policies
/*!< This class is part of the routine for the exact computation of
   the max-min-max operator in SGSolver_MaxMinMax_3Player. */
class SGProductPolicy
{
private:
  vector<SGPolicySet> policies; /*!< The vector of sets of policies
                                   for each state. */
  SGPoint dir; /*!< The direction in which these policies are optimal. */
  vector<double> levels; /*!< The associated optimal levels. */

public:
  //! Constructor
  SGProductPolicy(int _numStates, const SGPoint & _dir):
    policies(_numStates), dir(_dir), levels(_numStates)
  {}

  //! Get method for policies in a given state
  const SGPolicySet & getPolicies(int state) const
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    return policies[state];
  }

  //! Get method for the direction
  const SGPoint & getDir() const
  { return dir; }

  //! Get method for levels
  const vector<double> & getLevels() const
  { return levels; }

  //! Inserts a new policy in the given state
  void insertPolicy(int state, const SGPolicy & policy)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    policies[state].insert(policy);
  }

  //! Removes a policy from the given state
  void erasePolicy(int state, const SGPolicy & policy)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    policies[state].erase(policy);
  }

  //! Sets the level in a given state
  void setLevel(int state, double lvl)
  {
    if (state >= policies.size() || state<0)
      throw(SGException(SG::OUT_OF_BOUNDS));

    levels[state] = lvl;
  }

  //! Clears the policies in all states.
  void clear()
  {
    for (int state = 0; state < policies.size(); state++)
      policies[state].clear();
  }

  //! Returns whether the policy set is empty in any state
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

  //! Returns the total number of policies, minus the number of states
  int dimension() const
  {
    int dim = 0;
    for (int state = 0; state < policies.size(); state++)
      dim += policies[state].size();
    
    return dim-policies.size()+1;
  }

  //! Returns a unique identifier for the product policy
  std::string hash() const;

  //! Returns the number of states.
  int numStates() const {return policies.size();}
}; // SGProductPolicy

#endif
