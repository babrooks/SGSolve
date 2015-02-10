#ifndef _SGUTILITIES_HPP
#define _SGUTILITIES_HPP

#include "sgcommon.hpp"
/*! \file */

//! Maps a linear index into a multi-index over an array with the given dimension
int indexToVector(int index, 
		  vector<int> & v, 
		  const vector<int> & sizes);

//! Maps a multi-index into a linear index over an array with the given dimension
int vectorToIndex(const vector<int> & v,
		  const vector<int> & sizes);


// //! Calculates the convex hull of a vector of points
// void grahamScan(vector< vector<double> > &); 

// //! Finds the extreme points of the convex set described by a support function
// void findExtremePoints(const vector< vector<double> > &, 
// 		       const vector<double> &, vector< vector<double> > &);

// //! Returns the cosine of a vector.
// double cosine(const vector<double> &);

// //! Returns signed area
// double area(const vector<double> &, 
// 	    const vector<double> &, 
// 	    const vector<double> &); 
// double polarAngle(const vector<double> &);

// //! Returns the dot product of two vectors
// double dotProduct(const vector<double> &, 
// 		  const vector<double> &);

// //! Returns the Euclidean norm of a vector
// double norm(const vector<double> &);

// //! Finds the point of intersection between lines through the given
// //! points with the given slopes
// void findIntersection(vector<double> &,
// 		      const vector<double> &,double,
// 		      const vector<double> &,double);

// //! Generalized modulo arithmetic
// int addMod(int val, int increment, 
// 	   int start, int size);

#endif
