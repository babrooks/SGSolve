// Same as guitester but uses the jyc algorithm
//! @example
#include "sg.hpp"
#include "sgjycsolver.hpp"
#include <ctime>

int main ()
{
  SGGame game;

  SGGame::load(game,"../viewer/yuliysexample.sgm");

  cout << "Building solver" << endl;
  SGJYCSolver solver(game,100);

  std::clock_t start = std::clock();

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  cout << duration << " seconds elapsed" << endl;

  return 0;
}
