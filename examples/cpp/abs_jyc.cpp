#include "sg.hpp"
#include "sgjycsolver.hpp"

int main()
{
  int numDirections = 100;

  int action, state, player;

  ofstream ofs;
  ofs.open("abs_jyc.log");

  SGGame game;
  // SGGame::load(game,"../src/MATLAB/games/koch_ne=5_c2e=25_d=0.90_p=4.00.sgm");
  SGGame::load(game,"rsgtest.sgm");
  game.setDiscountFactor(0.85);
  
  std::clock_t start = std::clock();

  SGJYCSolver solver_jyc(game,numDirections);
  solver_jyc.solve();

  double duration_jyc = (std::clock() - start) / (double) CLOCKS_PER_SEC;

  for (int dir = 0; dir < numDirections; dir++)
    {
      ofs << solver_jyc.getDirections()[dir][0] << " ";
    }
  ofs << endl;
  for (int dir = 0; dir < numDirections; dir++)
    {
      ofs << solver_jyc.getDirections()[dir][1] << " ";
    }
  ofs << endl;
  for (state = 0; state < game.getNumStates(); state++)
    {
      for (int dir = 0; dir < numDirections; dir++)
	{
	  ofs << solver_jyc.getBounds()[state][dir] << " ";
	}
      ofs << endl;
    } // for state

  ofs.close(); 

  // Now solve the game with ABS to find the iteration at which we
  // pass JYC
  SGEnv env;
  start = std::clock();
  SGSolver solver_abs(env,game);
  env.setParam(SGEnv::STOREITERATIONS,2);
  solver_abs.solve();

  double duration_abs = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  
  SGSolution soln = solver_abs.getSolution();
  // SGSolution::save(soln,"abs_jyc.sln");

  int lastIterOutsideJYC;
  int lastRevOutsideJYC;
  for (list<SGIteration>::const_reverse_iterator iter
	 = soln.getIterations().rbegin();
       iter != soln.getIterations().rend();
       ++iter)
    {
      bool insideJYC = true;
      for (state = 0; state < game.getNumStates(); state++)
	{
	  for (int dir = 0; dir < numDirections; dir++)
	    {
	      if (solver_jyc.getDirections()[dir]*(iter->getPivot()[state])
		  > solver_jyc.getBounds()[state][dir])
		{
		  cout << "Here I am! I'm at iteration "
		       << iter->getIteration()
		       << " and revolution "
		       << iter->getRevolution() << endl;
		  insideJYC = false;
		  break;
		}
	    } // for dir
	  if (!insideJYC)
	    break;
	} // for state
      if (!insideJYC)
	{
	  lastIterOutsideJYC = iter->getIteration();
	  lastRevOutsideJYC = iter->getRevolution();
	  break;
	}
    } // for tuple

  int firstIterInsideJYC;
  int firstRevInsideJYC;
  for (list<SGIteration>::const_iterator iter
	 = soln.getIterations().begin();
       iter != soln.getIterations().end();
       ++iter)
    {
      bool outsideJYC = true;
      for (state = 0; state < game.getNumStates(); state++)
	{
	  for (int dir = 0; dir < numDirections; dir++)
	    {
	      if (solver_jyc.getDirections()[dir]*(iter->getPivot()[state])
		  < solver_jyc.getBounds()[state][dir])
		{
		  cout << "Here I am! I'm at iteration "
		       << iter->getIteration()
		       << " and revolution "
		       << iter->getRevolution() << endl;
		  outsideJYC = false;
		  break;
		}
	    } // for dir
	  if (!outsideJYC)
	    break;
	} // for state
      if (!outsideJYC)
	{
	  firstIterInsideJYC = iter->getIteration();
	  firstRevInsideJYC = iter->getRevolution();
	  break;
	}
    } // for tuple

  
  // Now run until the end of the revolution after lastOutsideJYC.
  env.setParam(SGEnv::STOREITERATIONS,0);
  SGApprox approx(env,game,soln);
  
  start = std::clock();
  approx.initialize();
  while (approx.getNumRevolutions() < firstRevInsideJYC+1) 
    approx.generate();
  double overtakeTime = (std::clock() - start) / (double) CLOCKS_PER_SEC;

  start = std::clock();
  approx.initialize();
  while (approx.getNumRevolutions() < lastRevOutsideJYC+1) 
    approx.generate(false);
  double passTime = (std::clock() - start) / (double) CLOCKS_PER_SEC;

  cout << "JYC took " << duration_jyc << " seconds." << endl;
  cout << "ABS took " << duration_abs << " seconds." << endl;
  cout << "ABS took " << overtakeTime << " seconds to overtake JYC." << endl;
  cout << "ABS took " << passTime << " seconds to pass JYC." << endl;
  

}
