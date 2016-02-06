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
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

#ifndef _SGENV_HPP
#define _SGENV_HPP

#include "sgexception.hpp"

//! Manages parameters for algorithm behavior
/*!  This class contains parameters for the algorithm.

  TODO: Add more checks on the correctness of passed parameter values.

  \ingroup src
 */
class SGEnv
{
private:
  // Parameters

  //! Double parameters
  vector<double> doubleParams;

  //! Bool parameters
  vector<bool> boolParams;

  //! Int parameters
  vector<int> intParams;
  
  //! Output stream. Not currently used.
  ofstream os; 
  
public:

  //! Constructor
  /*! Creates a new SGEnv object and initializes with default
      parameter values. */
  SGEnv();

  ~SGEnv() {}

  //! Method for setting double parameters.
  void setParam(SG::DBL_PARAM param, double value);

  //! Method for setting boolean parameters.
  void setParam(SG::BOOL_PARAM param, bool value);

  //! Method for setting integer parameters.
  void setParam(SG::INT_PARAM param, int value);

  //! Method for getting double parameters.
  double getParam(SG::DBL_PARAM param) const;

  //! Method for getting boolean parameters.
  bool getParam(SG::BOOL_PARAM param) const;

  //! Method for getting integer parameters.
  int getParam(SG::INT_PARAM param) const;

  //! Method for redirecting output
  void setOStream(ostream & newOS)
  {
    os.basic_ios<char>::rdbuf(newOS.rdbuf());
  }

  //! Return reference to the output stream.
  ofstream & getOS() { return os; }
  
  //! Method for restoring default values for all parameters.
  void restoreDefaults();

};

#endif
