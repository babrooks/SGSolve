// Kocherlakota example. Two individuals with stochastic endowments,
// iid uniform, who can make linear transfers to one another.
#include "sg.hpp"

int main ()
{
  double delta = 0.7;

  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numEndowments = 3,
    numStates = numEndowments * numEndowments;
  
  int actionsPerStatePlayer = 20;
  vector< vector< int > > numActions(numStates,
				     vector<int>(numPlayers,
						 actionsPerStatePlayer));
  vector<int> numActions_total(numStates,actionsPerStatePlayer*actionsPerStatePlayer);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,
	    vector< vector<double> >(actionsPerStatePlayer*actionsPerStatePlayer,
				     vector<double>(numPlayers,0.0)));
  for (state = 0; state < numStates; state++)
    {
      indexToVector(state,endowments,vector<int>(numPlayers,numEndowments));
      for (action = 0; action < numActions_total[state]; action++)
	{
	  indexToVector(action,actions,numActions[state]);
	  
	  // Actions encode the share of inccome being given away.
	  vector<double>share(2,0.0);
	  vector<double>income(2,0.0);
	  for (player=0; player<numPlayers; player++)
	    {
	      share[player] 
		= static_cast<double>(actions[player])/numActions[state][player];
	      income[player]
		= static_cast<double>(endowments[player])/numEndowments;
	    }
	  // Sqrt utility
	  payoffs[state][action][0] 
	    = pow((1.0-share[0])*income[0] + share[1]*income[1],0.5);
	  payoffs[state][action][1] 
	    = pow((1.0-share[1])*income[1] + share[0]*income[0],0.5);
	}
    }
  
  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,
		  vector< vector<double> >(actionsPerStatePlayer*actionsPerStatePlayer,
					   vector<double>(numStates,1.0/numStates)));

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);

  cout << "Building solver" << endl;
  SGEnv env;
  SGSolver solver(env,game);

  cout << "Starting solve routine" << endl;
  solver.solve();
  
  cout << "Done!" << endl;

  return 0;
}

