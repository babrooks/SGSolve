// BAB 9-25-2012
#include "sgtuple.hpp"


/* SGTuple */

SGPoint SGTuple::expectation(const vector<double> & prob) const
{
  if(prob.size() != points.size())
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

  SGPoint point(0);
  for (int state=0; state<points.size(); state++)
    point += (prob[state] * points[state]);
  return point;
}

double SGTuple::expectation(const vector<double> & prob,
			    int player) const
{
  if(prob.size() != points.size())
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

  double e = 0.0;
  for (int state=0; state<points.size(); state++)
    e += (prob[state] * points[state].xy[player]);
  return e;
}

SGPoint SGTuple::average() const
{
  if (points.size()==0)
    throw(SGException(SGException::EMPTY_TUPLE));

  SGPoint p;
  for (int state=0; state < points.size(); state++)
    p += points[state]/points.size();
  return p;
}

double SGTuple::average(int player) const
{
  if(points.size() == 0)
    throw(SGException(SGException::EMPTY_TUPLE));

  double e = 0.0;
  for (int state=0; state<points.size(); state++)
    e += points[state].xy[player]/points.size();
  return e;
}

SGPoint& SGTuple::operator[](int state)
{
  if(state < 0 || 
     state >= points.size())
    throw(SGException(SGException::OUT_OF_BOUNDS));

  return points[state];
}

const SGPoint& SGTuple::operator[](int state) const
{
  if(state < 0 || 
     state >= points.size())
    throw(SGException(SGException::OUT_OF_BOUNDS));

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
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] += rhs.points[i];
  return *this;
}

SGTuple& SGTuple::operator+=(const SGPoint & rhs)
{
  if (this->points.size()==0)
    throw(SGException(SGException::EMPTY_TUPLE));

  for (int i=0; i < this->points.size(); i++)
    this->points[i] += rhs;
  return *this;
}

SGTuple& SGTuple::operator-=(const SGTuple & rhs)
{
  if(this->points.size() != rhs.points.size())
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

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
    throw(SGException(SGException::DIVIDE_BY_ZERO));

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
  for (int state=0; state<rhs.points.size()-1; state++)
    out << rhs.points[state] << ", ";
  out << rhs.points.back() << "}";
  return out;
}

double SGTuple::distance(const SGTuple& t0,
			 const SGTuple& t1)
{
  if(t0.points.size() != t1.points.size())
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

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
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));

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

