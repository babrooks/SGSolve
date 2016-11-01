// Ben's test code for Qhull

#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include <iostream>
#include <random>

using namespace std;
// using orgQhull::RboxPoints;
// using orgQhull::Qhull;
using namespace orgQhull;

int main ()
{
  QHULL_LIB_CHECK;

  default_random_engine generator;
  uniform_real_distribution<coordT> distribution(0.0,1.0);
  
  RboxPoints rbox;

  int dim = 3;
  int numPoints = 1e4;
  
  coordT coords[dim];
  
  Qhull qhull;
  for (int k = 0; k < numPoints; k++)
    {
      for (int d = 0; d < dim; d++)
	coords[d] = distribution(generator);
	
      rbox.append(QhullPoint(dim,coords));
    }

  qhull.runQhull(rbox,"");

  QhullFacetList facets = qhull.facetList();

  QhullVertexList vl = qhull.vertexList();
  for (auto it = vl.begin();
       it != vl.end();
       ++it)
    {
      for (int d = 0; d < dim; d++)
      	cout << it->point().coordinates()[d] << " ";
      cout << endl;
    }

  // cout << facets;
  
  // for (auto it = facets.begin();
  //      it != facets.end();
  //      ++it)
  //   {
  //     cout << "Facet " << it->id() << ":" << endl;
  //     cout << "Normal: ";
  //     for (int d = 0; d < dim; d++)
  // 	cout << it->hyperplane().coordinates()[d] << " ";
  //     cout << endl << "Offset: " << it->hyperplane().offset() << endl << endl;
  //   }

  // Create a new qhull object and intersect half spaces
  RboxPoints rbox2;
  Qhull qhull2;

  vector<coordT> feasPnt(3,0);
  feasPnt[0] = 0.5-1e-9;
  feasPnt[1] = 0.5;
  feasPnt[2] = 0.5;
  coordT coords2[dim+1];
  for (auto it = facets.begin();
       it != facets.end();
       ++it)
    {
      for (int d = 0; d < dim; d++)
	coords2[d] =it->hyperplane().coordinates()[d];
      coords2[dim] = it->hyperplane().offset();
      rbox2.append(QhullPoint(dim+1,coords2));
    }
  coords2[0]=1;
  coords2[1]=0;
  coords2[2]=0;
  coords2[3]=-0.5;
  rbox2.append(QhullPoint(dim+1,coords2));

  qhull2.setFeasiblePoint(Coordinates(feasPnt));
  qhull2.runQhull(rbox2,"H");
  // qhull2.outputQhull();

  cout << endl << "Here come the points!" << endl << endl;
  QhullPoints points = qhull2.points();
  
  for (auto it = points.begin();
       it != points.end();
       ++it)
    {
      for (int d = 0; d < dim; d++)
      	cout << it->coordinates()[d] << " ";
      cout << endl;
    }

  return 0;
}
