// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
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

#ifndef _SGEXCEPTION_HPP
#define _SGEXCEPTION_HPP

#include "sgnamespace.hpp"
#include "sgcommon.hpp"

//! SGSolve specific exceptions
/*! This class is derived from std::exception and handles special
  messages that are informative about abnormal behavior in SGSolver
  and associated classes. 

  \ingroup src
*/
class SGException : public std::exception
{
public:
  //! Constructor for new SGException.
  SGException(SG::EXCEPTION_TYPE _type):type(_type) {}

  //! Returns the message corresponding to the ExceptionType
  virtual const char* what() const throw()
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

      case SG::SIMERROR:
	return "An error occurred within SGSimuator.";
      default:
	return "No message specified.";
      }
  }

  //! Returns the type of the exception.
  SG::EXCEPTION_TYPE getType()
  {
    return type;
  }
 
private:
  //! Flag that indicates the type of error encountered.
  SG::EXCEPTION_TYPE type; 
};

#endif
