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

#include "sgbaseaction.hpp"

//! Class for storing policies

class SGPolicy
{
private:
  const SGBaseAction & action;
  SG::Regime regime;
  int bindingPlayer;
  int bindingPoint;

public:
  SGPolicy(const SGBaseAction & _action,
	   const SG::Regime _regime,
	   const int _bindingPlayer = -1,
	   const int _bindingPoint = -1):
    action(_action),
    regime(_regime),
    bindingPlayer(_bindingPlayer),
    bindingPoint(_bindingPoint)
  {}

  std::string hash() const
  {
    std::string str;

    str += "s";
    str += std::to_string(action.getState());
    str += "a";
    str += std::to_string(action.getAction());
    if (regime==SG::NonBinding)
      str += "NB";
    else
      {
	str += "Bp";
	str += std::to_string(bindingPlayer);
	str += "pnt";
	str += std::to_string(bindingPoint);
      }
    
    return str;
  }

  friend bool operator< (const SGPolicy &, const SGPolicy &);
}; // SGPolicy


#endif
