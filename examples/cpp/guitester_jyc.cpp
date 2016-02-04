// Same as guitester but uses the jyc algorithm
//! @example
#include "sg.hpp"
#include "sgjycsolver.hpp"
#include <ctime>

int main ()
{
  try
    {
      SGGame game;
 
      SGGame::load(game,"./games/rsg_ne=5_c2e=25_d=0.85_p=0.sgm");

      cout << "Building solver" << endl;
      SGJYCSolver solver(game,100);

      std::clock_t start = std::clock();

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
      cout << duration << " seconds elapsed" << endl;
    }
  catch (SGException e)
    {
      cout << "Caught the following exception: " << endl
	   << e.what() << endl;
    }
  return 0;
}
