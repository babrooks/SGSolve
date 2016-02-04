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

#include "sgcomparator.hpp"

// bool SGComparator::operator() (const vector<double> & p0, 
// 				const vector<double> & p1)
// {
//   double absoluteZero=1E-10;

//   assert(p0.size()==2);
//   assert(p1.size()==2);

//   int playerCounter;
//   vector<double> diff0(p0), diff1(p1);
//   for (playerCounter=0; playerCounter<2; playerCounter++)
//     {
//       diff0[playerCounter] -= origin[playerCounter];
//       diff1[playerCounter] -= origin[playerCounter];
//     }

//   return ( (cosine(diff0) > cosine(diff1)+absoluteZero)
// 	   || (abs(cosine(diff0)-cosine(diff1))<absoluteZero
// 	       && norm(diff0)+absoluteZero<norm(diff1) ) );
// }

// void SGComparator::setOrigin(const vector<double> & input)
// {
//   assert(input.size()==2);

//   origin=input;
// }

