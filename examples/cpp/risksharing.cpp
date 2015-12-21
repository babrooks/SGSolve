#include "risksharing.hpp"

int main()
{
  double delta = 0.9;
  int numEndowments = 5;
  int c2e = 20;
  double persistence = 2.0;
  
  RiskSharingGame rsg(delta,numEndowments,c2e,persistence);

  SGGame game(rsg);
  SGGame::save(game,"./rsgtest.sgm");

  return 0;
  SGEnv env;
  SGSolver solver(env,game);

  solver.solve();

  SGSolution soln = solver.getSolution();
  SGSolution::save(soln,"rsgtest.sln");

  return 0;
}
