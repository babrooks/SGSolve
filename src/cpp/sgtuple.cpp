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
#include "sgtuple.hpp"


/* SGTuple */

SGPoint SGTuple::expectation(const vector<double> & prob) const
{
  if(prob.size() != points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  SGPoint point(points.front().size(),0.0);
  for (int state=0; state<points.size(); state++)
    point += (prob[state] * points[state]);
  return point;
}

double SGTuple::expectation(const vector<double> & prob,
			    int player) const
{
  if(prob.size() != points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  double e = 0.0;
  for (int state=0; state<points.size(); state++)
    e += (prob[state] * points[state].x[player]);
  return e;
}

SGPoint SGTuple::average() const
{
  if (points.size()==0)
    throw(SGException(SG::EMPTY_TUPLE));

  SGPoint p;
  for (int state=0; state < points.size(); state++)
    p += points[state]/points.size();
  return p;
}

double SGTuple::average(int player) const
{
  if(points.size() == 0)
    throw(SGException(SG::EMPTY_TUPLE));

  double e = 0.0;
  for (int state=0; state<points.size(); state++)
    e += points[state].x[player]/points.size();
  return e;
}

SGPoint& SGTuple::operator[](int state)
{
  // state = state % points.size(); // Wrap around
  if(state < 0 || 
     state >= points.size())
    throw(SGException(SG::OUT_OF_BOUNDS));
  if (points.size() == 0)
    throw(SGException(SG::EMPTY_TUPLE));

  return points[state];
}

const SGPoint& SGTuple::operator[](int state) const
{
  if (points.size() == 0)
    throw(SGException(SG::EMPTY_TUPLE));
  if(state < 0 || 
     state >= points.size())
    throw(SGException(SG::OUT_OF_BOUNDS));
  // state = state % points.size(); // Wrap around

  return points[state];
}

SGTuple& SGTuple::operator=(const SGTuple & rhs)
{
  if (this != &rhs)
    {
      this->points = rhs.points;
    }
  return *this;
}

SGTuple& SGTuple::operator+=(const SGTuple & rhs)
{
  if(this->points.size() != rhs.points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] += rhs.points[i];
  return *this;
}

SGTuple& SGTuple::operator+=(const SGPoint & rhs)
{
  if (this->points.size()==0)
    throw(SGException(SG::EMPTY_TUPLE));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] += rhs;
  return *this;
}

SGTuple& SGTuple::operator-=(const SGTuple & rhs)
{
  if(this->points.size() != rhs.points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] -= rhs.points[i];
  return *this;
}

SGTuple& SGTuple::operator-=(const SGPoint & rhs)
{
  for (int i=0; i < this->points.size(); i++)
    this->points[i] -= rhs;
  return *this;
}

SGTuple& SGTuple::operator*=(double d)
{
  for (int i=0; i < this->points.size(); i++)
    this->points[i] *= d;
  return *this;
}

SGTuple& SGTuple::operator/=(double d)
{
  if(d<=0)
    throw(SGException(SG::DIVIDE_BY_ZERO));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] /= d;
  return *this;
}

const SGTuple SGTuple::operator+(const SGTuple & rhs) const
{ return (SGTuple(*this) += rhs); }

const SGTuple SGTuple::operator+(const SGPoint & rhs) const
{ return (SGTuple(*this) += rhs); }

const SGTuple SGTuple::operator-(const SGTuple & rhs) const
{ return (SGTuple(*this) -= rhs); }

const SGTuple SGTuple::operator-(const SGPoint & rhs) const
{ return (SGTuple(*this) -= rhs); }

SGTuple operator*(double d,const SGTuple & tuple)
{ return (SGTuple(tuple) *= d); }

SGTuple operator*(const SGTuple & tuple,double d)
{ return (SGTuple(tuple) *= d); }

SGTuple operator/(const SGTuple & tuple,double d)
{ return (SGTuple(tuple) /= d); }

ostream& operator<<(ostream& out, const SGTuple& rhs)
{
  out << "{ ";
  if (rhs.points.size() > 0)
    {
      for (int state=0; state<rhs.points.size()-1; state++)
	out << rhs.points[state] << ", ";
      out << rhs.points.back();
    }
  out << "}";
  return out;
}

double SGTuple::distance(const SGTuple& t0,
			 const SGTuple& t1)
{
  if(t0.points.size() != t1.points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  double d = 0.0;
  
  for (int state=0; state<t0.points.size(); state++)
    d = std::max(d,SGPoint::distance(t0[state],t1[state]));
  return d;
}

bool SGTuple::strictlyLessThan(const SGTuple & tuple,int coordinate) const
{
  int state;
  bool lessthan = true;
  bool strict = false;

  if (this->points.size()!=tuple.points.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  for (state=0; state < points.size(); state++)
    {
      if (~strict && points[state][coordinate] 
	  < tuple.points[state][coordinate])
	strict = true;
      if (points[state][coordinate] > tuple.points[state][coordinate])
	lessthan = false;
    }
  return lessthan && strict;
}

void SGTuple::push_back(const SGPoint & point)
{
  points.push_back(point); 
}

void SGTuple::clear()
{
  points.clear(); 
}

void SGTuple::erase(int start, int end)
{
  points.erase(points.begin()+start,points.begin()+end);
}

void SGTuple::emplace(int location,const SGPoint & point)
{
  points.emplace(points.begin()+location,point);
}


double SGTuple::max(int coordinate) const
{
  double val = -2^15;
  for (int state = 0; state < points.size(); state++)
    val =  std::max(val,points[state][coordinate]);
  
  return val;
}

double SGTuple::min(int coordinate) const
{
  double val = 2^15;
  for (int state = 0; state < points.size(); state++)
    val = std::min(val,points[state][coordinate]);
  
  return val;
}

SGPoint SGTuple::max() const
{
  return SGPoint(this->max(0),this->max(1));
}

SGPoint SGTuple::min() const
{
  return SGPoint(this->min(0),this->min(1));
}

double SGTuple::max(int coordinate, int &index) const
{
  double val = -2^15;
  for (int state = 0; state < points.size(); state++)
    {
      if (val < points[state][coordinate])
	{
	  val = points[state][coordinate];
	  index = state;
	}
    }
  
  return val;
}

double SGTuple::min(int coordinate, int &index) const
{
  double val = 2^15;
  for (int state = 0; state < points.size(); state++)
    {
      if (val > points[state][coordinate])
	{
	  val = points[state][coordinate];
	  index = state;
	}
    }
  return val;
}


void SGTuple::maxmin(int coordinate, 
		     double & max, int & maxIndex, 
		     double & min,int & minIndex) const
{
  max = -numeric_limits<double>::max();
  min = numeric_limits<double>::max();
  for (int state = 0; state < points.size(); state++)
    {
      if (max < points[state][coordinate])
	{
	  max = points[state][coordinate];
	  maxIndex = state;
	}
      if (min > points[state][coordinate])
	{
	  min = points[state][coordinate];
	  minIndex = state;
	}
    }
}

void SGTuple::roundTuple(double tol)
{
  for (int state = 0; state<points.size(); state++)
    points[state].roundPoint(tol);
}

