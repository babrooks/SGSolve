//! One state prisoner's dilemma
//! @example

#include "sg.hpp"

int main ()
{
  double delta = 0.7;
  
  int action, state, player;
  int numPlayers = 2;
  int numStates = 1;

  vector<bool> unconstrained(2,false);

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,2));
  vector<int> numActions_total(numStates,4);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(4,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 0;
  payoffs[0][1][0] = -1;
  payoffs[0][2][0] = 3;
  payoffs[0][3][0] = 2;
  payoffs[0][0][1] = 0;
  payoffs[0][1][1] = 3;
  payoffs[0][2][1] = -1;
  payoffs[0][3][1] = 2;

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(4,vector<double>(numStates,1.0)));

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities,
	      unconstrained);

  SGEnv env;

  env.setParam(SG::DIRECTIONTOL,1e-12); // 1e-13
  env.setParam(SG::NORMTOL,1e-12); // 1e-12
  env.setParam(SG::LEVELTOL,1e-12); // 1e-12x
  env.setParam(SG::IMPROVETOL,1e-13); // 1e-14
  
  
  cout << "Building solver" << endl;
  SGSolver solver(env,game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  cout << "Saving data... ";
  SGSolution::save(solver.getSolution(),"sgtest.sln");
  cout << "Done!" << endl;

  SGGame::save(game,"pd.sgm");


  return 0;
}
