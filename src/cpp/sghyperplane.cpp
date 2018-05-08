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
#include "sghyperplane.hpp"

/* SGHyperplane */

double SGHyperplane::expectation(const vector<double> & prob) const
{
  if(prob.size() != levels.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  double exp = 0.0;
  for (int state=0; state<levels.size(); state++)
    exp += (prob[state] * levels[state]);
  return exp;
}

double& SGHyperplane::operator[](int state)
{
  if(state < 0 || 
     state >= levels.size())
    throw(SGException(SG::OUT_OF_BOUNDS));

  return levels[state];
}

const double& SGHyperplane::operator[](int state) const
{
  if(state < 0 || 
     state >= levels.size())
    throw(SGException(SG::OUT_OF_BOUNDS));

  return levels[state];
}

SGHyperplane& SGHyperplane::operator=(const SGHyperplane & rhs)
{
  if (this != &rhs)
    {
      this->levels = rhs.levels;
      this->normal = rhs.normal;
    }
  return *this;
}

SGHyperplane& SGHyperplane::operator+=(const SGHyperplane & rhs)
{
  if(this->levels.size() != rhs.levels.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  for (int i=0; i < this->levels.size(); i++)
    this->levels[i] += rhs.levels[i];
  return *this;
}

SGHyperplane& SGHyperplane::operator-=(const SGHyperplane & rhs)
{
  if(this->levels.size() != rhs.levels.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  for (int i=0; i < this->levels.size(); i++)
    this->levels[i] -= rhs.levels[i];
  return *this;
}

SGHyperplane& SGHyperplane::operator*=(double d)
{
  for (int i=0; i < this->levels.size(); i++)
    this->levels[i] *= d;
  return *this;
}

SGHyperplane operator*(double d,const SGHyperplane & tuple)
{ return (SGHyperplane(tuple) *= d); }

SGHyperplane operator*(const SGHyperplane & tuple,double d)
{ return (SGHyperplane(tuple) *= d); }

double SGHyperplane::distance(const SGHyperplane& t0,
			      const SGHyperplane& t1)
{
  // Want to compute a normalized distance
  if(t0.levels.size() != t1.levels.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));

  // double ratio = ( (t1.normal[0]+t1.normal[1] )
  // 		   / (t0.normal[0]+t0.normal[1]) );
  double ratio = 1;
  
  double d = SGPoint::distance(t0.getNormal()*ratio,t1.getNormal());
  
  for (int state=0; state<t0.levels.size(); state++)
    d = std::max(d,abs(t0[state]*ratio-t1[state]));
  return d;
}

void SGHyperplane::push_back(double level)
{
  levels.push_back(level); 
}

void SGHyperplane::clear()
{
  levels.clear(); 
}

void SGHyperplane::round(double tol)
{
  for (int state = 0; state<levels.size(); state++)
    levels[state] = std::round(levels[state]/tol)*tol;
  normal.roundPoint(tol);
}

ostream& operator<<(ostream& out, const SGHyperplane& rhs)
{
  out << "{ " << rhs.getNormal() << ", ";
  for (int state=0; state<rhs.levels.size()-1; state++)
    out << rhs.levels[state] << ", ";
  out << rhs.levels.back() << "}";
  return out;
}

