#ifndef _SGUTILITIES_HPP
#define _SGUTILITIES_HPP

#include "sgcommon.hpp"
/*! \file
  \ingroup src
 */

//! Maps a linear index into a multi-index over an array with the given dimension
int indexToVector(int index, 
		  vector<int> & v, 
		  const vector<int> & sizes);

//! Maps a multi-index into a linear index over an array with the given dimension
int vectorToIndex(const vector<int> & v,
		  const vector<int> & sizes);


#endif
