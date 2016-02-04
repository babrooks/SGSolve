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

// BAB 9-25-2012
#include "sgpoint.hpp"
#include "sgexception.hpp"

/* SGPoint */
SGPoint SGPoint::getNormal() const
{
  return SGPoint(-xy[1],xy[0]);
}

double SGPoint::angle(const SGPoint& base) const
{
  const double colinearTol = 1e-13;

  if ( abs( (*this)*base.getNormal() ) <  colinearTol )
    {
      if ( this->xy[0] * base.xy[0] > 0
  	   || this->xy[1] * base.xy[1] > 0) // same direction
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

double SGPoint::distance(const SGPoint & p0,
			 const SGPoint& p1)
{ return std::max(abs(p0.xy[0]-p1.xy[0]),abs(p0.xy[1]-p1.xy[1])); }

void SGPoint::max(const SGPoint & p)
{
  xy[0] = std::max(xy[0],p[0]);
  xy[1] = std::max(xy[1],p[1]);
}

void SGPoint::min(const SGPoint & p)
{
  xy[0] = std::min(xy[0],p[0]);
  xy[1] = std::min(xy[1],p[1]);
}

double& SGPoint::operator[](int player)
{
  if(player < 0 || player >= 2)
    throw(SGException(SG::OUT_OF_BOUNDS));

  return xy[player];
}

const double& SGPoint::operator[](int player) const
{
  if(player < 0 || player >= 2)
    throw(SGException(SG::OUT_OF_BOUNDS));

  return xy[player];
}

SGPoint& SGPoint::operator=(const SGPoint & rhs)
{
  if (this != &rhs)
    {
      this->xy.resize(2,0.0);
      this->xy = rhs.xy;
    }
  return *this;
}

SGPoint& SGPoint::operator=(double d)
{
  this->xy[0] = d; this->xy[1] = d;
  return *this;
}

SGPoint& SGPoint::operator+=(const SGPoint & rhs)
{
  for (int i=0; i<2; i++)
    this->xy[i] += rhs.xy[i];
  return *this;
}

SGPoint& SGPoint::operator-=(const SGPoint & rhs)
{
  for (int i=0; i<2; i++)
    this->xy[i] -= rhs.xy[i];
  return *this;
}

SGPoint& SGPoint::operator*=(double d)
{
  for (int i=0; i<2; i++)
    this->xy[i] *= d;
  return *this;
}

SGPoint& SGPoint::operator/=(double d)
{
  if(d==0.0)
    throw(SGException(SG::DIVIDE_BY_ZERO));

  for (int i=0; i<2; i++)
    this->xy[i] /= d;
  return *this;
}

const SGPoint SGPoint::operator+(const SGPoint & rhs) const
{ return (SGPoint(*this) += rhs); }

const SGPoint SGPoint::operator-(const SGPoint & rhs) const
{ return (SGPoint(*this) -= rhs); }

SGPoint operator*(double d,const SGPoint & point)
{ return (SGPoint(point) *= d); }

SGPoint operator*(const SGPoint & point,double d)
{ return (SGPoint(point) *= d); }

SGPoint operator/(const SGPoint & point,double d)
{ return (SGPoint(point) /= d); } 

double SGPoint::operator*(const SGPoint & rhs) const
{ return this->xy[0]*rhs.xy[0] + this->xy[1]*rhs.xy[1]; }

bool SGPoint::operator==(const SGPoint & rhs) const
{
  for (int coordinate = 0; coordinate < this->xy.size(); coordinate++)
    {
      if (abs(this->xy[coordinate]-rhs.xy[coordinate]) > 0.0)
	return false;
    }
  return true;
}

bool SGPoint::operator!=(const SGPoint & rhs) const
{ return !((*this)==rhs); }

bool SGPoint::operator>=(const SGPoint & rhs) const
{ return (this->xy[0]>=rhs.xy[0] && this->xy[1]>=rhs.xy[1]); }
bool SGPoint::operator>(const SGPoint & rhs) const
{ return !((*this)<=rhs); }
bool SGPoint::operator<=(const SGPoint & rhs) const
{ return (this->xy[0]<=rhs.xy[0] && this->xy[1]<=rhs.xy[1]); }
bool SGPoint::operator<(const SGPoint & rhs) const
{ return !((*this)>=rhs); }
bool SGPoint::operator>=(double rhs) const
{ return (this->xy[0]>=rhs && this->xy[1]>=rhs); }
bool SGPoint::operator>(double rhs) const
{ return !((*this)<=rhs); }
bool SGPoint::operator<=(double rhs) const
{ return (this->xy[0]<=rhs && this->xy[1]<=rhs); }
bool SGPoint::operator<(double rhs) const
{ return !((*this)>=rhs); }

ostream& operator<<(ostream& out, const SGPoint& rhs)
{
  // out << rhs.xy[0] << " " << rhs.xy[1];
  out.setf(std::ios::fixed,std::ios::floatfield);
  out.precision(3);
  out << "(" << std::setw(8) << rhs.xy[0] 
      << ", " << std::setw(8) << rhs.xy[1] << ")";
  out.width(2);
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
  return ((p1.xy[0] - p0.xy[0])*(p2.xy[1]-p0.xy[1])
	  -(p1.xy[1] - p0.xy[1])*(p2.xy[0]-p0.xy[0]));
}

void SGPoint::roundPoint(double tol)
{
  if (tol>0)
    {
      xy[0] = round(xy[0]/tol)*tol;
      xy[1] = round(xy[1]/tol)*tol;
    }
}

