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

// Classes for storing data for stochastic games.
// BAB 9-25-2012
#ifndef _SGPOINT_HPP
#define _SGPOINT_HPP

#include "sgcommon.hpp"

//! A vector in \f$\mathbb{R}^n\f$
/*! A simple two-dimensional vector that supports arithmetic operations. 

  \ingroup src
 */
class SGPoint
{
protected:
  vector<double> x; /*!< Two dimensional vector of doubles. */

public:
  //! Default constructor that sets vector equal to zero.
  SGPoint():  x(2,0.0) {}
  //! Sets both elements of the vector equal to x.
  SGPoint(double _x): x(2,_x) {}
  //! Creates an n dimensional zero vector
  SGPoint(int n): x(n,0) {}
  //! Creates an n dimensional zero vector
  SGPoint(int n, double _x): x(n,_x) {}
  //! Creates an SGPoint from the two-vector _x. 
  SGPoint(vector<double> _x)
  {
    x = _x;
  }
  //! Creates an SGPoint with elements x and y.
  SGPoint(double _x0, double _x1): x(2,0.0)
  { x[0] = _x0; x[1] = _x1;  }

  //! Destructor
  ~SGPoint(){}

  //! Returns the number of coordinates
  int size () const {return x.size();}
  //! Returns the counter-clockwise normal vector.
  SGPoint getNormal() const;
  //! Returns the Euclidean norm.
  double norm() const {return std::sqrt((*this)*(*this));}
  //! Returns the angle in radians relative to base.
  double angle(const SGPoint& base) const;
  //! Calculates distance between p0 and p1 in the sup-norm.
  static double distance(const SGPoint& p0,
			 const SGPoint& p1);
  //! Cross product for 3-vectors
  static SGPoint cross(const SGPoint& p0,
		       const SGPoint& p1);
  //! Rotates the point clockwise by pi radians
  bool rotateCCW(double pi);
  //! Rotates the point clockwise by pi radians
  bool rotateCW(double pi) {return rotateCCW(-pi);}
  //! Normalizes so that the norm is one
  bool normalize();
  
  //! Rounds off significant digits smaller than tol.
  void roundPoint(double tol);

  //! Takes the pointwise minimum with another SGPoint
  void max(const SGPoint & p);
  //! Takes the pointwise minimum with another SGPoint
  void min(const SGPoint & p);

  // Operators
  //! Access operator.
  double& operator[](int player);
  //! Constant access operator.
  const double& operator[](int player) const;
  //! Assignment operator
  SGPoint& operator=(const SGPoint & rhs);
  //! Sets both coordinates equal to d.
  SGPoint& operator=(double d);
  //! Augmented addition
  SGPoint& operator+=(const SGPoint & rhs);
  //! Augmented subtraction
  SGPoint& operator-=(const SGPoint & rhs);
  //! Augmented subtraction
  SGPoint& operator-=(double d);
  //! Augmented scalar multiplication
  SGPoint& operator*=(double d);
  //! Augmented scalar division
  SGPoint& operator/=(double d);
  //! Vector addition
  const SGPoint operator+(const SGPoint & rhs) const;
  //! Vector subtraction
  const SGPoint operator-(const SGPoint & rhs) const;
  //! Vector subtraction
  const SGPoint operator-(double d) const;
  //! Left-scalar multiplication
  friend SGPoint operator*(double d,const SGPoint & point);
  //! Right-scalar multiplication
  friend SGPoint operator*(const SGPoint & point,double d);
  //! Right scalar division.
  friend SGPoint operator/(const SGPoint & point,double d);
  //! Dot product.
  double operator*(const SGPoint & rhs) const; // dot product
  //! Equality
  bool operator==(const SGPoint & rhs) const;
  //! Not equls
  bool operator!=(const SGPoint & rhs) const;
  //! Greater than or equal
  bool operator>=(const SGPoint & rhs) const;
  //! Strictly greater
  bool operator>(const SGPoint & rhs) const;
  //! Less than or equal
  bool operator<=(const SGPoint & rhs) const;
  //! Strictly less
  bool operator<(const SGPoint & rhs) const;
  //! Greater than or equal to a scalar
  bool operator>=(double rhs) const;
  //! Strictly greater than a scalar
  bool operator>(double rhs) const;
  //! Less than or equal to a scalar
  bool operator<=(double rhs) const;
  //! Strictly less than a scalar
  bool operator<(double rhs) const;
  //! Output SGPoint to ostream
  friend ostream& operator<<(ostream& out, const SGPoint& rhs);

  //! Inverts a linear system
  /*! Solves the system of equations
      \f$\left[\begin{array}{c}{a_0}{a_1}\end{array}\right]x=b\f$. */
  static double invertSystem(SGPoint& x, const SGPoint&b,
			     const SGPoint& a0, const SGPoint& a1);
  //! Calculates intersection of a segment with a ray.
  /*! Calculates the intersection of the ray defined by
      \f$pivot+t\cdot{}direction\f$ with the line segment
      \f$\alpha{}t_0+(1-\alpha)t_1\f$ for \f$\alpha\in[0,1]\f$. */
  static double intersectRay(SGPoint& intersection,
			     SGPoint& weights,
			     const SGPoint& pivot,
			     const SGPoint& direction,
			     const SGPoint& t0,
			     const SGPoint& t1);
  //! Calculates determinant.
  /*! Calculates the signed area of the parallelogram starting at
      \f$p_0\f$ and going in the directions \f$p_1\f$ and
      \f$p_2\f$. */
  static double signedArea(const SGPoint& p0,
			   const SGPoint& p1,
			   const SGPoint& p2);

  //! Serialize an SGPoint
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & x;
  }

  friend class boost::serialization::access;
  friend class SGTuple;
}; // SGPoint

#endif
