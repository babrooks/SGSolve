// Example from Abreu-Sannikov. Game with only one state.
#include "sg.hpp"

int main ()
{
  double delta = 0.3;
  
  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numStates = 1;

  vector< vector< int > > numActions(1,vector<int>(numPlayers,3));
  vector<int> numActions_total(1,9);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(1,vector< vector<double> >(9,vector<double>(numPlayers,0.0)));
  // payoffs[0][0][0] = 16;
  // payoffs[0][1][0] = 21;
  // payoffs[0][2][0] = 9;
  // payoffs[0][3][0] = 3;
  // payoffs[0][4][0] = 10;
  // payoffs[0][5][0] = 5;
  // payoffs[0][6][0] = 0;
  // payoffs[0][7][0] = -1;
  // payoffs[0][8][0] = -5;
  // payoffs[0][0][1] = 9;
  // payoffs[0][1][1] = 1;
  // payoffs[0][2][1] = 0;
  // payoffs[0][3][1] = 13;
  // payoffs[0][4][1] = 4;
  // payoffs[0][5][1] = -4;
  // payoffs[0][6][1] = 3;
  // payoffs[0][7][1] = 0;
  // payoffs[0][8][1] = -15;

  payoffs[0][0][0] = 18;
  payoffs[0][1][0] = 23;
  payoffs[0][2][0] = 11;
  payoffs[0][3][0] = 5;
  payoffs[0][4][0] = 12;
  payoffs[0][5][0] = 7;
  payoffs[0][6][0] = 2;
  payoffs[0][7][0] = 1;
  payoffs[0][8][0] = -3;
  payoffs[0][0][1] = 11;
  payoffs[0][1][1] = 3;
  payoffs[0][2][1] = 2;
  payoffs[0][3][1] = 15;
  payoffs[0][4][1] = 6;
  payoffs[0][5][1] = -2;
  payoffs[0][6][1] = 5;
  payoffs[0][7][1] = 2;
  payoffs[0][8][1] = -13;
  
  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(1,vector< vector<double> >(9,vector<double>(1,1.0)));

  // Equilibrium Actions
  list<int> actionsPerState;
  for (action = 0; action<9; action++)
    actionsPerState.push_back(action);

  vector< list<int> > equilibriumActions(1,actionsPerState);

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities,
	      equilibriumActions);

  cout << "Building solver" << endl;
  SGSolver solver(game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  cout << "Saving data... ";
  SGSolution::save(solver.getSolution(),"sgtest.sln");
  cout << "Done!" << endl;


  return 0;
}

