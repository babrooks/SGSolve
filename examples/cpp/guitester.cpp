// One state prisoner's dilemma
#include "sg.hpp"
#include "sgsimulator.hpp"

//! @example

int main ()
{
  SGGame game;

  // SGGame::load(game,"../viewer/yuliysexample.sgm");
  // SGGame::load(game,"./test2.sgm");
  try
    {
      // SGGame::load(game,"../src/MATLAB/games/koch_ne=11_c2e=5_d=0.90_p=1.00.sgm");
      SGGame::load(game,"../src/MATLAB/games/koch_ne=5_c2e=75_d=0.90_p=12.00.sgm");

      SGEnv env;
      env.setParam(SGEnv::PRINTTOCOUT,true);
      env.setParam(SGEnv::IMPROVETOL,1e-13);
  
      cout << "Building solver" << endl;
      SGSolver solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      cout << "Saving data... ";
      SGSolution::save(solver.getSolution(),"sgtest.sln");
      cout << "Done!" << endl;
      




      // SGSolution soln;
      // // SGSolution::load(soln,"./sgtest.sln");
      // SGSolution::load(soln,"../src/MATLAB/solutions/koch_ne=11_c2e=5_d=0.90_p=0.00.sln");

      // SGSimulator sim(soln);
      
      // sim.initialize();

      // sim.simulate(100,0,soln.iterations.back().iteration);
      
    }
  catch (SGException e)
    {
      cout << "I caught an exception." << endl;
      cout << e.what() << endl;
    }


  return 0;
}
