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

#include "sgpolicy.hpp"


std::string SGPolicy::hash() const
{
  std::string str;

  str += "s";
  str += std::to_string(action->getState());
  str += "a";
  str += std::to_string(action->getAction());
  if (regime==SG::NonBinding)
    str += "NB";
  else
    {
      str += "Bp";
      str += std::to_string(bindingPlayer);
      str += "k";
      str += std::to_string(bindingPoint);
    }
    
  return str;
}

bool operator< (const SGPolicy & p1, const SGPolicy & p2)
{
  if (p1.action->getState() < p2.action->getState())
    return true;
  if (p1.action->getState() > p2.action->getState())
    return false;

  // States must be equal

  if (p1.action->getAction() < p2.action->getAction())
    return true;
  if (p1.action->getAction() > p2.action->getAction())
    return false;

  // States and actions are equal

  if (p1.regime == SG::NonBinding && p2.regime != SG::NonBinding)
    return true;

  if (p1.bindingPlayer < p2.bindingPlayer)
    return true;
  if (p1.bindingPlayer > p2.bindingPlayer)
    return false;

  // States, actions, regime, and binding player are equal
  if (p1.bindingPoint < p2.bindingPoint)
    return true;

  return false;
}

ostream& operator<<(ostream& out, const SGPolicy& rhs)
{
  out.setf(std::ios::fixed,std::ios::floatfield);
  out << "s" << rhs.getAction()->getState()
      << "a" << rhs.getAction()->getAction()
      << "r";
  if (rhs.getRegime()==SG::NonBinding)
    out << "N";
  else
    {
      out << "Bp" << rhs.getBindingPlayer()
	  << "k" << rhs.getBindingPoint();
    }

  return out;
}

