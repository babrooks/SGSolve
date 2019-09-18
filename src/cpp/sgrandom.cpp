#include "sgrandom.hpp"

SGGame generateRandomGame(int numPlayers, int numStates, int numactions)
{	  
	  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	  std::default_random_engine generator (seed); 

	  std::uniform_real_distribution<double> distribution(.1,.9);
	  double delta = distribution(generator);

	  vector<bool> unconstrained(numPlayers,false); 

	  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,numactions));
	  vector<int> numActions_total(numStates,pow(numactions, numPlayers));

	  // Payoffs 

	  vector< vector< vector<double> > >
		  payoffs(numStates,vector< vector<double> >(pow(numactions, numPlayers),vector<double>(numPlayers,0.0))); 
	  unsigned int i,j,k,l;
	  for(i=0;i<numStates;i++)
	  {
	    for(j=0;j<numActions_total[i];j++)
	    {
	       for(k=0;k<numPlayers;k++)
	       {
	         std::uniform_real_distribution<double> distribution(0,10);	                    
		 payoffs[i][j][k] = distribution(generator);
	       }
	    }
	  }
	 	
	  // Transition probabilities 
	  vector < vector< vector<double> > >
		  probabilities(numStates,vector< vector<double> >(pow(numactions, numPlayers), vector<double>(numStates,1.0))); // 1.0 as initial value, but then randomize
	  // randomize transition probabilities 
	  double prob_sum;
	  for(i=0;i<numStates;i++)
	  { 
	    for(j=0;j<numActions_total[i];j++)
	    { 
	      prob_sum = 0.0;
	      for(k=0;k<numStates;k++)
	      {
		std::uniform_real_distribution<double> distribution(0,1.0);
		probabilities[i][j][k] = distribution(generator);
		// normalize probabilities
		prob_sum += probabilities[i][j][k];
	      }
	      for(k=0;k<numStates;k++)
	      {
		probabilities[i][j][k] = probabilities[i][j][k]/prob_sum;
	      } 
	    }
	  }
	  return SGGame(delta,
		          numStates,
		          numActions,
		          payoffs,
		          probabilities,
		          unconstrained);
}

