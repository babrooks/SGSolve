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

class SGPolicy
{
private:
  SGActionIter action;
  SG::Regime regime;
  int bindingPlayer;
  int bindingPoint;

public:
  SGPolicy(const SGActionIter & _action,
	   const SG::Regime _regime,
	   const int _bindingPlayer = -1,
	   const int _bindingPoint = -1):
    action(_action),
    regime(_regime),
    bindingPlayer(_bindingPlayer),
    bindingPoint(_bindingPoint)
  {}

  const SGActionIter & getAction() const
  { return action; }
  const SG::Regime & getRegime() const
  { return regime; }
  const int getBindingPlayer() const
  { return bindingPlayer; }
  const int getBindingPoint() const
  { return bindingPoint; }

  std::string hash() const;

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
