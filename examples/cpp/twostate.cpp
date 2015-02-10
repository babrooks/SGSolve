// Two state version of AS. Shift payoffs up by 2 in state #1.
#include "sg.hpp"
#include <chrono>
#include <ctime>

using namespace std::chrono;

inline double unif()
{ return rand() / double(RAND_MAX);}

int main ()
{
  double delta = 0.3;
  // double delta = 0.3;
  
  int action, state, player;
  int numPlayers = 2;
  vector<int> actions(2,0), endowments(2,0);

  int numStates = 2;

  vector< vector< int > > numActions(2,vector<int>(numPlayers,3));
  vector<int> numActions_total(2,9);

  srand((unsigned) time(NULL));

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(2,vector< vector<double> >(9,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 16;
  payoffs[0][1][0] = 21;
  payoffs[0][2][0] = 9;
  payoffs[0][3][0] = 3;
  payoffs[0][4][0] = 10;
  payoffs[0][5][0] = 5;
  payoffs[0][6][0] = 0;
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
  payoffs[0][8][1] = -15;
  
  for (action=0; action < payoffs[0].size(); action++)
    {
      for (player=0; player < numPlayers; player++)
	{
	  payoffs[1][action][player] = payoffs[0][action][player]+2.0;
	}
    }

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(2,vector< vector<double> >(9,vector<double>(2,0.5)));

  // for (state = 0; state < numStates; state++)
  //   {
  //     for (action = 0; action < 9; action++)
  // 	{
  // 	  double prob = unif();
  // 	  probabilities[state][action][0] = prob;
  // 	  probabilities[state][action][1] = 1.0-prob;
  // 	  cout << "probabilities[" << state << "][" << action << "][0] = " 
  // 	       << setprecision(15) << prob << ";" << endl;
  // 	  cout << "probabilities[" << state << "][" << action << "][1] = 1.0 -  " 
  // 	       << setprecision(15) << prob << ";" << endl;
  // 	}
  //   }
  probabilities[0][0][0] = 0.858891657953566;
  probabilities[0][0][1] = 1.0 -  0.858891657953566;
  probabilities[0][1][0] = 0.435153968369194;
  probabilities[0][1][1] = 1.0 -  0.435153968369194;
  probabilities[0][2][0] = 0.863878608152214;
  probabilities[0][2][1] = 1.0 -  0.863878608152214;
  probabilities[0][3][0] = 0.675496499834348;
  probabilities[0][3][1] = 1.0 -  0.675496499834348;
  probabilities[0][4][0] = 0.432588648252463;
  probabilities[0][4][1] = 1.0 -  0.432588648252463;
  probabilities[0][5][0] = 0.313656833634552;
  probabilities[0][5][1] = 1.0 -  0.313656833634552;
  probabilities[0][6][0] = 0.647560087799821;
  probabilities[0][6][1] = 1.0 -  0.647560087799821;
  probabilities[0][7][0] = 0.0526849339030147;
  probabilities[0][7][1] = 1.0 -  0.0526849339030147;
  probabilities[0][8][0] = 0.148281459299979;
  probabilities[0][8][1] = 1.0 -  0.148281459299979;
  probabilities[1][0][0] = 0.452111321246303;
  probabilities[1][0][1] = 1.0 -  0.452111321246303;
  probabilities[1][1][0] = 0.232975769430853;
  probabilities[1][1][1] = 1.0 -  0.232975769430853;
  probabilities[1][2][0] = 0.794963912011573;
  probabilities[1][2][1] = 1.0 -  0.794963912011573;
  probabilities[1][3][0] = 0.346808374089565;
  probabilities[1][3][1] = 1.0 -  0.346808374089565;
  probabilities[1][4][0] = 0.30224752440222;
  probabilities[1][4][1] = 1.0 -  0.30224752440222;
  probabilities[1][5][0] = 0.683172795308369;
  probabilities[1][5][1] = 1.0 -  0.683172795308369;
  probabilities[1][6][0] = 0.490227123485053;
  probabilities[1][6][1] = 1.0 -  0.490227123485053;
  probabilities[1][7][0] = 0.212482880434246;
  probabilities[1][7][1] = 1.0 -  0.212482880434246;
  probabilities[1][8][0] = 0.116095767410517;
  probabilities[1][8][1] = 1.0 -  0.116095767410517;

  // Problematic example:

  // probabilities[0][0][0] = 0.385342795581251;
  // probabilities[0][0][1] = 1.0 -  0.385342795581251;
  // probabilities[0][1][0] = 0.693178151591298;
  // probabilities[0][1][1] = 1.0 -  0.693178151591298;
  // probabilities[0][2][0] = 0.255834069222135;
  // probabilities[0][2][1] = 1.0 -  0.255834069222135;
  // probabilities[0][3][0] = 0.198884315415697;
  // probabilities[0][3][1] = 1.0 -  0.198884315415697;
  // probabilities[0][4][0] = 0.83761779723578;
  // probabilities[0][4][1] = 1.0 -  0.83761779723578;
  // probabilities[0][5][0] = 0.921670305971834;
  // probabilities[0][5][1] = 1.0 -  0.921670305971834;
  // probabilities[0][6][0] = 0.0488508446369557;
  // probabilities[0][6][1] = 1.0 -  0.0488508446369557;
  // probabilities[0][7][0] = 0.168088588476222;
  // probabilities[0][7][1] = 1.0 -  0.168088588476222;
  // probabilities[0][8][0] = 0.894221100906944;
  // probabilities[0][8][1] = 1.0 -  0.894221100906944;
  // probabilities[1][0][0] = 0.555679399778917;
  // probabilities[1][0][1] = 1.0 -  0.555679399778917;
  // probabilities[1][1][0] = 0.0514699835569924;
  // probabilities[1][1][1] = 1.0 -  0.0514699835569924;
  // probabilities[1][2][0] = 0.500087023479905;
  // probabilities[1][2][1] = 1.0 -  0.500087023479905;
  // probabilities[1][3][0] = 0.125726508035197;
  // probabilities[1][3][1] = 1.0 -  0.125726508035197;
  // probabilities[1][4][0] = 0.980429368084497;
  // probabilities[1][4][1] = 1.0 -  0.980429368084497;
  // probabilities[1][5][0] = 0.566904126930472;
  // probabilities[1][5][1] = 1.0 -  0.566904126930472;
  // probabilities[1][6][0] = 0.753654245638128;
  // probabilities[1][6][1] = 1.0 -  0.753654245638128;
  // probabilities[1][7][0] = 0.568363984380087;
  // probabilities[1][7][1] = 1.0 -  0.568363984380087;
  // probabilities[1][8][0] = 0.0112428283371231;
  // probabilities[1][8][1] = 1.0 -  0.0112428283371231;

  // vector< vector< vector<double> > > probabilities(2);
  // vector<double> allOn0(2,0), allOn1(2,0); allOn0[0]=1.0; allOn1[1]=1.0;
  // probabilities[0] = vector< vector<double> >(9,allOn0);
  // probabilities[1] = vector< vector<double> >(9,allOn1);

  // Equilibrium Actions
  list<int> actionsPerState;
  for (action = 0; action<9; action++)
    actionsPerState.push_back(action);

  vector< list<int> > equilibriumActions(2,actionsPerState);

  cout << "Constructing game object" << endl;
  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities,
	      equilibriumActions);

  cout << "Building solver" << endl;
  SGSolver solver(game);


  cout << "Starting solve routine... ";

  high_resolution_clock::time_point start = high_resolution_clock::now();
  solver.solve();
  high_resolution_clock::time_point end = high_resolution_clock::now();

  milliseconds diff =  duration_cast<milliseconds>(end-start);
 
  cout << "Done!" << endl 
       << "Took " << diff.count() << " milliseconds to compute." << endl;


  cout << "Saving solution... ";
  SGSolution::save(solver.getSolution(),"sgtest.sln");
  cout << "Done!" << endl;

  SGSolution soln;
  cout << "Loading soln... ";
  SGSolution::load(soln,"sgtest.sln");
  cout << "Done!" << endl;
  cout << soln.iterations.size() << " iterations were loaded." << endl;

  return 0;
}

