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

#ifndef _SGPOLICY_HPP
#define _SGPOLICY_HPP

#include "sgaction_maxminmax.hpp"

//! Class for storing policies
typedef list<SGAction_MaxMinMax>::const_iterator SGActionIter;

//! A policy for the max-min-max algorithm
/*!< This class represents a policy in a single state. Part of the
   exact computation routine in SGSolver_MaxMinMax_3Player. */ 
class SGPolicy
{
private:
  SGActionIter action; /*!< The action. */
  SG::Regime regime; /*!< The regime. */
  int bindingPlayer; /*!< The player with the binding incentive
                        constraint if the regime is binding. */
  int bindingPoint; /*!< The index of the binding continuation value
                       when the regime is binding. */

public:
  //! Constructor
  SGPolicy(const SGActionIter & _action,
	   const SG::Regime _regime,
	   const int _bindingPlayer = -1,
	   const int _bindingPoint = -1):
    action(_action),
    regime(_regime),
    bindingPlayer(_bindingPlayer),
    bindingPoint(_bindingPoint)
  {}

  //! Action get method
  const SGActionIter & getAction() const
  { return action; }
  //! Regime get method
  const SG::Regime & getRegime() const
  { return regime; }
  //! Binding player get method
  const int getBindingPlayer() const
  { return bindingPlayer; }
  //! Binding point get method
  const int getBindingPoint() const
  { return bindingPoint; }

  //! Returns a unique identifying string for the policy
  std::string hash() const;

  //! Tests equality of two policies
  bool isEqual(const SGActionIter & _action,
	       const SG::Regime & _regime,
	       const int _bindingPlayer=-1,
	       const int _bindingPoint=-1) const
  {
    return (_action != action
	    || _regime != regime
	    || (regime == SG::Binding && 
		(bindingPlayer != _bindingPlayer
		 || bindingPoint != _bindingPoint)));
  }

  friend bool operator< (const SGPolicy &, const SGPolicy &);
  friend ostream& operator<<(ostream& out, const SGPolicy &);

}; // SGPolicy


#endif
