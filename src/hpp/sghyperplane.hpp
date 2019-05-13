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

#ifndef _SGHYPERPLANE_HPP
#define _SGHYPERPLANE_HPP

#include "sgcommon.hpp"
#include "sgpoint.hpp"
#include "sgexception.hpp"

//! Represents a hyperplane in \f$\mathbb{R}^N\f$
/*! Consists of a normal direction and a vector of levels, one for
    each state.

    Used to represent half spaces in SGSolver_MaxMinMax and
    SGSolver_MaxMinMax_3Player.

  \ingroup src
*/
class SGHyperplane
{
private:
  SGPoint normal; /*!< Normal normal to the hyperplanes. */
  vector<double> levels; /*!< Level attained in each state. */

public:
  //! Initializes tuple to _numStates zero-vectors.
  SGHyperplane(int numStates): levels(numStates,0)
  {}

  //! Initializes tuple to _numPoints copies of point.
  SGHyperplane(const SGPoint & _normal,
	       const vector<double> & _levels):
    normal(_normal),
    levels(_levels)
  {}

  //! Default constructor for empty tuple.
  SGHyperplane(): normal(0,0), levels(1,0) {}
  ~SGHyperplane() {}

  //! Mathematical expectation
  /*! Returns the weighted sum of the levels using the weights in
      prob. */
  double expectation(const vector<double> & prob) const;

  //! Calculates the distance between t0 and t1 in the sup norm.
  static double distance(const SGHyperplane& t0,
			 const SGHyperplane& t1);
  
  //! Adds a new level to the back of the hyperplane.
  void push_back(double level);
  //! Sets the levels equal to an empty vector.
  void clear();

  //! Returns the number of levels in the tuple.
  int size() const { return levels.size(); }

  //! Set the normal
  void setNormal(const SGPoint & newNorm)
  { normal = newNorm; }
  //! Get the normal
  const SGPoint & getNormal() const
  { return normal; }
  //! Get the levels
  const vector<double> & getLevels() const
  { return levels; }
  
  // Operators
  //! Random access the elements of the tuple.
  double& operator[](int state);
  //! Constant random access to elements of the tuple.
  const double& operator[](int state) const;
  //! Assignment operator
  SGHyperplane& operator=(const SGHyperplane & rhs);
  //! Augmented addition of tuples
  SGHyperplane& operator+=(const SGHyperplane & rhs);
  //! Augmented subtraction of tuples
  SGHyperplane& operator-=(const SGHyperplane & rhs);
  //! Augmented addition of tuple with a point
  /*! Adds rhs to each level. */
  SGHyperplane& operator+=(double rhs);
  //! Scalar multiplication of the hyperplane.
  SGHyperplane& operator*=(double d);
  //! Left scalar multiplication of a tuple.
  friend SGHyperplane operator*(double d,const SGHyperplane & hp);
  //! Right scalar multiplication of a tuple.
  friend SGHyperplane operator*(const SGHyperplane & hp,double d);
  //! Output formatted tuple to file stream.
  friend ostream& operator<<(ostream& out, const SGHyperplane& rhs);

  //! Round off significant digits smaller than tol.
  void round(double tol);

  //! Serialize using boost
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & normal & levels;
  }

  friend class boost::serialization::access;
}; // SGHyperplane

#endif
