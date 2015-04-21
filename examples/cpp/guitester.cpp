// One state prisoner's dilemma
#include "sg.hpp"

//! @example

int main ()
{
  SGGame game;

  // SGGame::load(game,"../viewer/yuliysexample.sgm");
  SGGame::load(game,"./test2.sgm");

  SGEnv env;
  env.setParam(SGEnv::PRINTTOCOUT,true);
  env.setParam(SGEnv::IMPROVETOL,1e-13);
  
  cout << "Building solver" << endl;
  SGSolver solver(env,game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  // cout << "Saving data... ";
  // SGSolution::save(solver.getSolution(),"sgtest.sln");
  // cout << "Done!" << endl;


  return 0;
}
