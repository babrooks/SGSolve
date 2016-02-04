//! This example compares the runtimes of the pencil sharpening
//! algorithm and generalized JYC.
//! @example
#include "sg.hpp"
#include "sgjycsolver.hpp"
#include "risksharing.hpp"

int main()
{
  std::vector<int> D   = {100, 100, 200, 100, 200, 400, 100, 200, 400, 600};
  std::vector<int> NE  = {3,   3,   3,   3,   3,   3,   5,   5,   5,   5};
  std::vector<int> C2E = {15,  25,  25,  50,  50,  50,  25,  25,  25,  25};
  
  for (int k = 0; k < D.size(); k++)
    {
      int numDirections=D[k];
      int numEndowments=NE[k];
      int c2e = C2E[k];
      cout << endl << "nD = " << numDirections
	   << ", ne = " << numEndowments
	   << ", c2e = " << c2e << endl << endl;
      
      int action, state, player;

      ofstream ofs;
      ofs.open("./logs/abs_jyc.log");

      double delta = 0.85;
      RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

      double persistence = 0;
      try
	{
	  RiskSharingGame rsg(delta,numEndowments,
			      c2e,persistence,endowmentMode);
	  SGGame game(rsg);

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
	  env.setParam(SG::STOREITERATIONS,2);
	  env.setParam(SG::STOREACTIONS,false);
	  solver_abs.solve();

	  double duration_abs = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  
	  SGSolution soln = solver_abs.getSolution();

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
	  env.setParam(SG::STOREITERATIONS,0);
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
      catch (SGException e)
	{
	  cout << "Caught the following exception: " << endl
	       << e.what() << endl;
	} // try/catch
    } // for

  return 0;
}
