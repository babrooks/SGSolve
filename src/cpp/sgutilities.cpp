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

#include "sgexception.hpp"
#include "sgutilities.hpp"

int indexToVector(int index, 
		  vector<int> & v, 
		  const vector<int> & sizes)
{
  if (index < 0)
    throw(SGException(SG::OUT_OF_BOUNDS));

  if(sizes.size()==0)
    throw(SGException(SG::EMPTY_TUPLE));

  v.resize(sizes.size(),0);

  for (int coordinate = 0;
       coordinate < sizes.size();
       coordinate++)
    {
      v[coordinate] = index % sizes[coordinate];
      index -= v[coordinate]; index /= sizes[coordinate];
    }
  return index;
}

int vectorToIndex(const vector<int> & v, 
		  const vector<int> & sizes)
{
  int index = 0;

  if (v.size() != sizes.size())
    throw(SGException(SG::TUPLE_SIZE_MISMATCH));
  for (int coordinate = sizes.size()-1;
       coordinate >= 0;
       coordinate -- )
    {
      index *= sizes[coordinate];
      index += v[coordinate];
    }
  return index;
}
