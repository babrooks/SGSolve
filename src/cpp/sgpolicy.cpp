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

bool operator< (const SGPolicy & p1, const SGPolicy & p2)
{
  if (p1.action.getState() < p2.action.getState())
    return true;
  if (p1.action.getState() > p2.action.getState())
    return false;

  // States must be equal

  if (p1.action.getAction() < p2.action.getAction())
    return true;
  if (p1.action.getAction() > p2.action.getAction())
    return false;

  // States and actions are equal

  if (p1.bindingPlayer < p2.bindingPlayer)
    return true;
  if (p1.bindingPlayer > p2.bindingPlayer)
    return false;

  // States and actions and regime/binding player are equal
  if (p1.bindingPoint < p2.bindingPoint)
    return true;

  return false;
}
