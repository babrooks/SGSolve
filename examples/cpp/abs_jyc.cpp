#include "sg.hpp"
#include "sgjycsolver.hpp"

int main()
{
  double delta = 0.75;

  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numEndowments = 5,
    numStates = numEndowments;
  
  int numTransfers = 30;
  vector< vector< int > > numActions(numStates, vector<int>(2,1));
  vector<int> numActions_total(numStates);

  // Payoffs
  vector< vector< vector<double> > > payoffs(numStates);
  vector< vector< vector<double> > > probabilities(numStates);

  for (state = 0; state < numStates; state++)
    {
      double e0 = state/(numStates-1.0);
      double e1 = 1.0 - e0;
      vector< list<int> > eqActions(numStates);

      if (state == numStates/2)
	{
	  numActions_total[state] = 1;
	  payoffs[state]
	    = vector< vector<double> > (1,vector<double>(2,std::sqrt(e0)));
	  probabilities[state]
	    = vector< vector<double> >(1,vector<double>(numStates,
							1.0/numStates));
	}
      else
	{
	  numActions_total[state] = numTransfers;
	  probabilities[state]
	    = vector< vector<double> >(numTransfers,
				       vector<double>(numStates,
						      1.0/numStates));
	  payoffs[state]
	    = vector< vector<double> >(numTransfers,
				       vector<double>(2));
	  for (action = 0; action < numTransfers; action++)
	    {
	      double c0 = e0 + (e1 - e0)*action/(numTransfers-1.0);
	      double c1 = 1.0 - c0;
	      payoffs[state][action][0] = std::sqrt(c0);
	      payoffs[state][action][1] = std::sqrt(c1);
	    }

	  if (state > numStates/2)
	    numActions[state][0] = numTransfers;
	  else if (state < numStates/2)
	    numActions[state][1] = numTransfers;
	}
    }      

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);

  SGGame::save(game,"kocherlakota2.sgm");
  
  std::clock_t start = std::clock();

  int numDirections = 50;

  cout << "K=" << numEndowments
       << ",M=" << numTransfers
       << ",J=" << numDirections << endl;
  SGJYCSolver solver_jyc(game,numDirections);
  solver_jyc.solve();

  double duration_jyc = (std::clock() - start) / (double) CLOCKS_PER_SEC;

  // Now solve the game with ABS to find the iteration at which we
  // pass JYC
  SGEnv env;
  SGSolver solver_abs(env,game);
  solver_abs.solve();
  
  SGSolution soln = solver_abs.getSolution();
  int lastIterOutsideJYC;
  int lastRevOutsideJYC;
  for (list<SGIteration>::const_reverse_iterator iter
	 = soln.iterations.rbegin();
       iter != soln.iterations.rend();
       ++iter)
    {
      bool insideJYC = true;
      for (state = 0; state < numStates; state++)
	{
	  for (int dir = 0; dir < numDirections; dir++)
	    {
	      if (solver_jyc.directions[dir]*(iter->pivot[state])
		  >solver_jyc.bounds[state][dir])
		{
		  cout << "Here I am! I'm at iteration "
		       << iter->iteration
		       << " and revolution "
		       << iter->revolution << endl;
		  insideJYC = false;
		  break;
		}
	    } // for dir
	  if (!insideJYC)
	    break;
	} // for state
      if (!insideJYC)
	{
	  lastIterOutsideJYC = iter->iteration;
	  lastRevOutsideJYC = iter->revolution;
	  break;
	}
    } // for tuple
  
  // Now run until the end of the revolution after lastOutsideJYC.
  SGApprox approx(env,game,soln);
  
  start = std::clock();
  approx.initialize();
  while (approx.getNumRevolutions() < lastRevOutsideJYC+2) 
    approx.generate();
  
  double duration_abs = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  cout << "JYC took " << duration_jyc << " seconds." << endl;
  cout << "ABS took " << duration_abs << " seconds to overtake JYC." << endl;
  

}
