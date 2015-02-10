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

