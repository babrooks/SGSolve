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

#ifndef _SGEDGEPOLICY_HPP
#define _SGEDGEPOLICY_HPP

#include "sgproductpolicy.hpp"

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
  SGEdgePolicy(const SGProductPolicy & _master);
  
  const vector<SGPolicySet::const_iterator> & getPolicies() const
  { return policies; }

  const SGPolicySet::const_iterator & getSubPolicy() const
  { return subPolicy; }

  bool incrementBasePolicy();
  bool incrementSubPolicy();
  
  const int getBaseState() const
  { return baseState; }

  const int getSubState() const
  { return subState; }

  bool operator++();
  
}; // SGEdgePolicy

#endif
