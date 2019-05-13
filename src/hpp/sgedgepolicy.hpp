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
/*!< This code is used by SGSolver_MaxMinMax_3Player for exact
   computation of the max-min-max operator when there are three
   players. Given a "master" product policy, which represents a face
   of \f$\tilde{B}({\bf W})\f$, this class will iterate through all edges
   of face. These edges correspond to (i) a selection of a policy in
   the product policy and (ii) a substitution. */
class SGEdgePolicy
{
private:
  const SGProductPolicy & master; /*!< The product policy describing
                                     the face. */
  vector<SGPolicySet::const_iterator> policies; /*!< The vector of
                                                   policies that makes
                                                   up the current
                                                   edge. */
  SGPolicySet::const_iterator subPolicy; /*!< The substituted policy. */
  int baseState; /*!< The state in which we are currently iterating
                    the policies, as we work our way through the
                    product policy. */
  int subState; /*!< State in which the policy is substituted. */
  
public:
  //! Constructor
  SGEdgePolicy(const SGProductPolicy & _master);

  //! Returns the current policy, as a vector of iterators
  const vector<SGPolicySet::const_iterator> & getPolicies() const
  { return policies; }

  //! Returns the current substitution.
  const SGPolicySet::const_iterator & getSubPolicy() const
  { return subPolicy; }

  //! Increments the base policy.
  bool incrementBasePolicy();
  //! Increments the substitution
  bool incrementSubPolicy();
  //! Returns the base state
  const int getBaseState() const
  { return baseState; }
  //! Returns the substitution state
  const int getSubState() const
  { return subState; }
  //! Generates a unique identifier for the edge.
  /*!< This identifier is used in tracking which edges have been visited. */
  std::string hash() const;
  //! Increment the edge by either advancing the substitution or the base policy.
  bool operator++();
  
}; // SGEdgePolicy

#endif
