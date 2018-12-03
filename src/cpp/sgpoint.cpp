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

// BAB 9-25-2012
#include "sgpoint.hpp"
#include "sgexception.hpp"

/* SGPoint */
SGPoint SGPoint::getNormal() const
{
  assert(x.size()==2);
  return SGPoint(-x[1],x[0]);
}

double SGPoint::angle(const SGPoint& base) const
{
  assert(x.size()==2);
  const double colinearTol = 1e-13;

  if ( abs( (*this)*base.getNormal() ) <  colinearTol )
    {
      if ( this->x[0] * base.x[0] > 0
  	   || this->x[1] * base.x[1] > 0) // same direction
  	return 0.0;
      return PI; // Opposite direction.
    }

  double c = (*this)*base 
    / sqrt( ((*this)*(*this)) * (base*base)) ;

  if (c<-1)
    return PI;
  else if (c>1)
    return 0.0;

  double radians = acos(c);
  if ((*this)*base.getNormal() > 0)
    radians = 2.0*PI - radians;

  assert(!std::isnan(radians));

  return radians;
}

bool SGPoint::rotateCCW(double pi)
{
  assert(x.size()==2);
  double tmp = cos(pi)*x[0]-sin(pi)*x[1];
  x[1] = sin(pi)*x[0]+cos(pi)*x[1];
  x[0] = tmp;
  return true;
}

bool SGPoint::normalize()
{
  double sum = 0;
  for ( int k = 0; k < x.size(); k++)
    sum += x[k]*x[k];
  double tmp = sqrt(sum);
  if (tmp < 1e-15)
    return false;
  for ( int k = 0; k < x.size(); k++)
    x[k] /= tmp;

  return true;
}

double SGPoint::distance(const SGPoint & p0,
			 const SGPoint& p1)
{
  assert(p0.size() == p1.size());
  double m = 0;
  for (int k = 0; k < p0.size(); k++)
    m = std::max(m,abs(p0.x[k]-p1.x[k]));
  return m;
}

SGPoint SGPoint::cross(const SGPoint& p0,
		       const SGPoint& p1)
{
  assert(p0.size() == p1.size());
  assert(p0.size() == 3);

  SGPoint crossProd(3,0);
  crossProd[0] = p0[1]*p1[2]-p0[2]*p1[1];
  crossProd[1] = p0[2]*p1[0]-p0[0]*p1[2];
  crossProd[2] = p0[0]*p1[1]-p0[1]*p1[0];
  return crossProd;
}

void SGPoint::max(const SGPoint & p)
{
  assert(x.size() == p.size());
  for (int k = 0; k < x.size(); k++)
    x[k] = std::max(x[k],p[k]);
}

void SGPoint::min(const SGPoint & p)
{
  assert(x.size() == p.size());
  for (int k = 0; k < x.size(); k++)
    x[k] = std::min(x[k],p[k]);
}

double& SGPoint::operator[](int player)
{
  // player = player % x.size(); // Wrap around
  if(player < 0 || player >= x.size())
    throw(SGException(SG::OUT_OF_BOUNDS));

  return x[player];
}

const double& SGPoint::operator[](int player) const
{
  // player = player % x.size(); // Wrap around
  if(player < 0 || player >= x.size())
    throw(SGException(SG::OUT_OF_BOUNDS));

  return x[player];
}

SGPoint& SGPoint::operator=(const SGPoint & rhs)
{
  // assert(this->x.size() == rhs.x.size());
  if (this != &rhs)
    {
      this->x = rhs.x;
    }
  return *this;
}

SGPoint& SGPoint::operator=(double d)
{
  for (int k = 0; k < x.size(); k++)
    this->x[k] = d;
  return *this;
}

SGPoint& SGPoint::operator+=(const SGPoint & rhs)
{
  assert(this->x.size() == rhs.x.size());
  for (int k=0; k<x.size(); k++)
    this->x[k] += rhs.x[k];
  return *this;
}

SGPoint& SGPoint::operator-=(const SGPoint & rhs)
{
  assert(this->x.size() == rhs.x.size());
  for (int k=0; k<x.size(); k++)
    this->x[k] -= rhs.x[k];
  return *this;
}

SGPoint& SGPoint::operator-=(double d)
{
  for (int k=0; k<x.size(); k++)
    this->x[k] -= d;
  return *this;
}

SGPoint& SGPoint::operator*=(double d)
{
  for (int k=0; k<x.size(); k++)
    this->x[k] *= d;
  return *this;
}

SGPoint& SGPoint::operator/=(double d)
{
  if(d==0.0)
    throw(SGException(SG::DIVIDE_BY_ZERO));

  for (int k=0; k<x.size(); k++)
    this->x[k] /= d;
  return *this;
}

const SGPoint SGPoint::operator+(const SGPoint & rhs) const
{ return (SGPoint(*this) += rhs); }

const SGPoint SGPoint::operator-(const SGPoint & rhs) const
{ return (SGPoint(*this) -= rhs); }

const SGPoint SGPoint::operator-(double d) const
{
  return SGPoint(*this) -= d;
}

SGPoint operator*(double d,const SGPoint & point)
{ return (SGPoint(point) *= d); }

SGPoint operator*(const SGPoint & point,double d)
{ return (SGPoint(point) *= d); }

SGPoint operator/(const SGPoint & point,double d)
{ return (SGPoint(point) /= d); } 

double SGPoint::operator*(const SGPoint & rhs) const
{
  double sum = 0;
  for (int k = 0; k < x.size(); k++)
    sum += this->x[k]*rhs.x[k];
  return sum;
}

bool SGPoint::operator==(const SGPoint & rhs) const
{
  for (int k = 0; k < this->x.size(); k++)
    {
      if (abs(this->x[k]-rhs.x[k]) > 0.0)
	return false;
    }
  return true;
}

bool SGPoint::operator!=(const SGPoint & rhs) const
{ return !((*this)==rhs); }

bool SGPoint::operator>=(const SGPoint & rhs) const
{
  assert(x.size() == rhs.size());
  bool tf = false;
  for (int k = 0; k < x.size(); k++)
    tf = tf && (this->x[k]>=rhs.x[k]);
  return tf; 
}
bool SGPoint::operator>(const SGPoint & rhs) const
{ return !((*this)<=rhs); }
bool SGPoint::operator<=(const SGPoint & rhs) const
{
  assert(x.size() == rhs.size());
  bool tf = false;
  for (int k = 0; k < x.size(); k++)
    tf = tf && (this->x[k]<=rhs.x[k]);
  return tf; 
}
bool SGPoint::operator<(const SGPoint & rhs) const
{ return !((*this)>=rhs); }
bool SGPoint::operator>=(double rhs) const
{
  bool tf = false;
  for (int k = 0; k < x.size(); k++)
    tf = tf && (this->x[k]>=rhs);
  return tf; 
}
bool SGPoint::operator>(double rhs) const
{ return !((*this)<=rhs); }
bool SGPoint::operator<=(double rhs) const
{
  bool tf = false;
  for (int k = 0; k < x.size(); k++)
    tf = tf && (this->x[k]<=rhs);
  return tf; 
}
bool SGPoint::operator<(double rhs) const
{ return !((*this)>=rhs); }

ostream& operator<<(ostream& out, const SGPoint& rhs)
{
  if (rhs.x.size() > 0)
    {
      out.setf(std::ios::fixed,std::ios::floatfield);
      out.precision(3);
      out << "(";
      for (int k = 0; k < rhs.size()-1; k++)
	out << std::setw(8) << rhs.x[k] << ", ";
      out << std::setw(8) << rhs.x.back() << ")";
      out.width(2);
    }
  else
    {
      out << "()";
    }
  return out;
}

double SGPoint::invertSystem(SGPoint& x, const SGPoint&b,
			     const SGPoint& a0, const SGPoint& a1)
{
  // Solves a0*x[0] + a1*x[1] = b
  double determinant = a0[0]*a1[1] - a0[1]*a1[0];
  if (abs(determinant)>1e-13)
    {
      x[0] = (b[0]*a1[1] - b[1]*a1[0]) / determinant;
      x[1] = (a0[0]*b[1] - a0[1]*b[0]) / determinant;
    }
  else
    {
      x[0] = 0; x[1] = 0;
    }
  return determinant;
} // invertSystem

double SGPoint::signedArea(const SGPoint& p0,
			   const SGPoint& p1,
			   const SGPoint& p2)
{
  return ((p1.x[0] - p0.x[0])*(p2.x[1]-p0.x[1])
	  -(p1.x[1] - p0.x[1])*(p2.x[0]-p0.x[0]));
}

void SGPoint::roundPoint(double tol)
{
  if (tol>0)
    {
      for (int k = 0; k < x.size(); k++)
	x[k] = round(x[k]/tol)*tol;
    }
}

