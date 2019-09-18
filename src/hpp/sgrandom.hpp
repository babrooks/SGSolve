#ifndef _SGRANDOM_HPP
#define _SGRANDOM_HPP

#include "sg.hpp" 
#include "sggame.hpp"

//! function to generate random games
SGGame generateRandomGame(int numPlayers, int numStates, int numactions);

//! Class to generate random games 
class SGRandomGame : public SGGame 
{
private: 
  int numactions;
public: 
  SGRandomGame(int numPlayers,
	     int numStates,
	     int numactions): 
    SGGame(generateRandomGame(numPlayers, numStates, numactions)), 
    numactions(numactions) {}
}; 
/*
//! Class to generate random games 
class SGRandomGame : public SGGame 
{
private: 
  int numactions;
public: 
  SGRandomGame(int numplayers,
	       int numstates,
	       int numactions) 
  {
    SGGame();
    
    numPlayers = numplayers;
    numStates = numstates;
    	  
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); 
    std::default_random_engine generator (seed); 
   
    std::uniform_real_distribution<double> distribution(.1,.9);
    delta = distribution(generator);

    unsigned int c;
    unconstrained.resize(numPlayers);
    for(c=0;c<numPlayers;c++)
    {
      unconstrained[c] = false;
    }

    numActions.resize(numStates);
    unsigned int s,p;
    for(s=0;s<numStates;s++)
    {
      numActions[s].resize(numPlayers);
      for(p=0;p<numPlayers;p++)
      {
	numActions[s][p]=numactions;
      }
    }

    numActions_total.resize(numStates);
    for(s=0;s<numStates;s++)
    {
      numActions_total[s]=pow(numactions, numPlayers);
    }   

    // Payoffs 
    unsigned int i,j,k,l;
    SGPoint sgpoint(numPlayers);
    payoffs.resize(numStates);
    for(i=0;i<numStates;i++)
    {
      payoffs[i].resize(numActions_total[i]);
      for(j=0;j<numActions_total[i];j++)
      { 
	payoffs[i][j] = sgpoint;
        for(k=0;k<numPlayers;k++)
	  {
	    std::uniform_real_distribution<double> distribution(0,10);	                       payoffs[i][j][k] = distribution(generator);
	  }
       }
     }
	 	
    // Transition probabilities 
    double prob_sum;
    probabilities.resize(numStates);
    for(i=0;i<numStates;i++)
    { 
      probabilities[i].resize(pow(numactions,numPlayers));
      for(j=0;j<numActions_total[i];j++)
      { 
	probabilities[i][j].resize(numStates);      
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
   }

};
*/
#endif
