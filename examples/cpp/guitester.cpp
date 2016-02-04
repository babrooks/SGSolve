//! Utility for loading an external game and then solving it
//! @example

#include "sg.hpp"
#include "sgsimulator.hpp"

int main ()
{
  SGGame game;

  try
    {
      // SGGame::load(game,"./games/test2.sgm");
      SGGame::load(game,"./games/rsg_ne=5_c2e=25_d=0.85_p=0.sgm");

      SGEnv env;
      env.setParam(SG::PRINTTOCOUT,true);
      env.setParam(SG::IMPROVETOL,1e-13);
  
      cout << "Building solver" << endl;
      SGSolver solver(env,game);

      cout << "Starting solve routine" << endl;
      solver.solve();
  
      cout << "Saving data... ";
      SGSolution::save(solver.getSolution(),"sgtest.sln");
      cout << "Done!" << endl;
      
      SGSolution soln;
      // SGSolution::load(soln,"./sgtest.sln");
      SGSolution::load(soln,"../src/MATLAB/solutions/koch_ne=11_c2e=5_d=0.90_p=0.00.sln");

      SGSimulator sim(soln);
      
      sim.initialize();

      sim.simulate(100,100,0,soln.getIterations().back().getIteration());
      
    }
  catch (SGException e)
    {
      cout << "I caught an exception." << endl;
      cout << e.what() << endl;
    }


  return 0;
}
