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
  SGException(SG::EXCEPTION_TYPE _type = SG::DEFAULT):type(_type) {}

  //! Returns the message corresponding to the ExceptionType
  virtual const char* what() const throw();

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
