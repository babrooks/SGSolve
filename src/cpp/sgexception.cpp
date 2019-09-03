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

#include "sgexception.hpp"

const char* SGException::what() const throw()
{
  switch (type)
    {
    case SG::TOO_MANY_PIVOT_UPDATES:
      return "Too many calls to SGSolver::updatePivot() in SGSolver::solve()";
    case SG::NO_ADMISSIBLE_DIRECTION:
      return "bestAction==NULL. Could not find an admissible direction.";
    case SG::FAILED_OPEN:
      return "Unable to open file.";
    case SG::UNKNOWN_PARAM:
      return "Could not recognize parameter name.";
    case SG::TUPLE_SIZE_MISMATCH:
      return "You tried to perform an arithmetic operation on tuples of different sizes";
    case SG::OUT_OF_BOUNDS:
      return "You passed an argument that did not satisfy bound restrictions.";
    case SG::DIVIDE_BY_ZERO:
      return "You cannot divide by zero.";
    case SG::INCONSISTENT_INPUTS:
      return "Inputs are incompatible or inconsistent with one another, e.g., arrays that should be the same size are not.";
    case SG::PROB_SUM_NOT1:
      return "Transition probabilities do not sum to one.";
    case SG::SIMERROR:
      return "An error occurred within SGSimuator.";
    case SG::NOFEASIBLETUPLE:
      return "Could not find a feasible tuple for APS. Equilibrium payoff correspondence is possibly empty";
    case SG::NOEDGES:
      return "Could not find an edge in the product policy.";
    case SG::NOACTIONS:
      return "Could not find an action in some state.";
    case SG::NOOPTIMALPOLICIES:
      return "Could not find an optimal policy in some state.";
    case SG::WRONG_NUMBER_OF_PLAYERS:
      return "Game has wrong number of players for viewer.";
    case SG::MAX_ITERATIONS_REACHED:
      return "The maximum number of iterations has been reached.";
    case SG::NO_ITERATIONS: 
      return "Cannot load a solution with no iterations.";
    default:
      return "No message specified.";
    }
}
