// Kocherlakota example. Two individuals with stochastic endowments,
// iid uniform, who can make linear transfers to one another.
#include "sg.hpp"

int main ()
{
  double delta = 0.6;

  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numEndowments = 10,
    numStates = numEndowments;
  
  int numTransfers = 100;
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

  cout << "K=" << numEndowments
       << ",M=" << numTransfers << endl;

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);

  // SGGame::save(game,"kocherlakota2.sgm");
  
  cout << "Building solver" << endl;
  SGEnv env;
  env.setParam(SGEnv::STOREITERATIONS,false);
  SGSolver solver(env,game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  cout << "Done!" << endl;

  return 0;
}

