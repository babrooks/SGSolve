// One state prisoner's dilemma
#include "sg.hpp"
#include "sgjycsolver.hpp"
#include <ctime>

//! @example

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
  // cout << "Saving data... ";
  // SGSolution::save(solver.getSolution(),"sgtest.sln");
  // cout << "Done!" << endl;


  return 0;
}
