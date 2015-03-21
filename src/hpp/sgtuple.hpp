#ifndef _SGTUPLE_HPP
#define _SGTUPLE_HPP

#include "sgcommon.hpp"
#include "sgpoint.hpp"
#include "sgexception.hpp"

//! Tuple of SGPoint objects
/*! Essentially a vector of SGPoint objects that supports arithmetic
  operations. 

  \ingroup src
*/
class SGTuple
{
private:
  vector<SGPoint> points; /*!< Vector of SGTuple objects. */

public:
  //! Initializes tuple to _numStates zero-vectors.
  SGTuple(int numStates)
  { points = vector<SGPoint> (numStates); }

  //! Initializes tuple to _numPoints copies of point.
  SGTuple(int _numStates,
	  const SGPoint & point)
  { 
    points = vector<SGPoint> (_numStates,point); 
  }

  //! Default constructor for empty tuple.
  SGTuple() {}
  ~SGTuple() {}

  //! Mathematical expectation
  /*! Returns the weighted sum of the points in the tuple using the
      weights in prob. */
  SGPoint expectation(const vector<double> & prob) const;

  //! Mathematical expectation for one player.
  /*! Returns the weighted sum of the player coordinate of the SGPoint
      objects in the tuple using the weights in prob. */
  double expectation(const vector<double> & prob, int player) const;

  //! Returns the average of the points in the tuple.
  SGPoint average() const;
  //! Returns the average coordinate for a player in the tuple.
  double average(int player) const;

  //! Calculates the distance between t0 and t1 in the sup norm.
  static double distance(const SGTuple& t0,
			 const SGTuple& t1);
  //! Strictly less than
  /*! Returns true if the calling object is strictly less than tuple
      in the player coordinate. */
  bool strictlyLessThan(const SGTuple & tuple, int coordinate) const;

  //! Round off significant digits smaller than tol.
  void roundTuple(double tol);

  //! Returns the maximum in a given coordinate
  double max(int coordinate) const;
  //! Returns the minimum in a given coordinate
  double min(int coordinate) const;

  //! Max and puts argmax in index
  double max(int coordinate, int & index) const;
  //! Min and puts argmin in index
  double min(int coordinate, int & index) const;

  //! Returns a point equal to the pointwise maximum
  SGPoint max() const;
  //! Returns a point equal to the pointwise minimum
  SGPoint min() const;

  //! Max and min
  /*! Sets both the max, argmax, min, argmin in a given coordinate. */
  void maxmin(int coordinate, 
	      double & max, int & maxIndex, 
	      double & min,int & minIndex) const;

  //! Adds a new point to the back of the tuple.
  void push_back(const SGPoint & point);
  //! Sets the tuple equal to an empty tuple.
  void clear();

  //! Returns the number of points in the tuple.
  int size() const { return points.size(); }

  // Operators
  //! Random access the elements of the tuple.
  SGPoint& operator[](int state);
  //! Constant random access to elements of the tuple.
  const SGPoint& operator[](int state) const;
  //! Assignment operator
  SGTuple& operator=(const SGTuple & rhs);
  //! Augmented addition of tuples
  SGTuple& operator+=(const SGTuple & rhs);
  //! Augmented subtraction of tuples
  SGTuple& operator-=(const SGTuple & rhs);
  //! Augmented addition of tuple with a point
  /*! Adds rhs to each element of the tuple. */
  SGTuple& operator+=(const SGPoint & rhs);
  //! Augmented subtraction of tuple with a point
  /*! Subtracts rhs from each element of the tuple. */
  SGTuple& operator-=(const SGPoint & rhs);
  //! Scalar multiplication of the tuple.
  SGTuple& operator*=(double d);
  //! Scalar division of the tuple.
  SGTuple& operator/=(double d);
  //! Adds corresponding elements of lhs and rhs.
  const SGTuple operator+(const SGTuple & rhs) const;
  //! Adds rhs to each element of lhs.
  const SGTuple operator+(const SGPoint & rhs) const;
  //! Subtracts corresponding elements of rhs from lhs.
  const SGTuple operator-(const SGTuple & rhs) const;
  //! Subtracts rhs from each element of lhs.
  const SGTuple operator-(const SGPoint & rhs) const;
  //! Left scalar multiplication of a tuple.
  friend SGTuple operator*(double d,const SGTuple & point);
  //! Right scalar multiplication of a tuple.
  friend SGTuple operator*(const SGTuple & point,double d);
  //! Right scalar division of a tuple.
  friend SGTuple operator/(const SGTuple & point,double d);
  //! Output formatted tuple to file stream.
  friend ostream& operator<<(ostream& out, const SGTuple& rhs);

  //! Serialize using boost
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & points;
  }

  friend class boost::serialization::access;
}; // SGTuple

#endif
