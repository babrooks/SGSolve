// Example from Abreu-Sannikov. Game with only one state.
#include "sg.hpp"

//! @example

int main ()
{
  double delta = 0.9;
  
  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numStates = 2;

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,3));
  vector<int> numActions_total(numStates,9);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(9,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 16;
  payoffs[0][1][0] = 21;
  payoffs[0][2][0] = 9;
  payoffs[0][3][0] = 3;
  payoffs[0][4][0] = 10;
  payoffs[0][5][0] = 5;
  payoffs[0][6][0] = 1;
  payoffs[0][7][0] = -1;
  payoffs[0][8][0] = -5;
  payoffs[0][0][1] = 9;
  payoffs[0][1][1] = 1;
  payoffs[0][2][1] = 0;
  payoffs[0][3][1] = 13;
  payoffs[0][4][1] = 4;
  payoffs[0][5][1] = -4;
  payoffs[0][6][1] = 3;
  payoffs[0][7][1] = 0;
  payoffs[0][8][1] = -10;

  for (action=0; action<9; action++)
    {
      payoffs[1][action][0] = payoffs[0][action][0]-5;
      payoffs[0][action][0] += 5;
      payoffs[1][action][1] = payoffs[0][action][1]+5;
      payoffs[0][action][1] -= 5;
    } // for action

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(9,vector<double>(numStates,0.0)));
  double persistence = 0.3;
  for (state=0; state<numStates; state++)
    {
      for (action=0; action<9; action++)
	{
	  probabilities[state][action][state] = persistence;
	  probabilities[state][action][1-state] = 1-persistence;
	} // for action
    } // for state
  
  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);
  
  SGEnv env;
  
  cout << "Building solver" << endl;
  SGSolver solver(env,game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  cout << "Saving data... ";
  SGSolution::save(solver.getSolution(),"sgtest.sln");
  cout << "Done!" << endl;


  return 0;
}
