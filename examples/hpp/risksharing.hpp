#include "sg.hpp"

class RiskSharingGame : public SGAbstractGame
{
private:
  int numEndowments;
  int c2e;
  double persistence;
  vector<double> E;
  int midPoint;
  double cIncr;
  vector< vector<double> > stateProbSum;
  vector<int> numActions_total;

  double consumption(int e, int t) const
  {
    double alpha = (1.0*t)/(numActions_total[e]-1.0);
    
    if (e<midPoint)
      {
 	// Player 1 is transferring to player 0
	return E[e] + (1.0-alpha)*(0.5-E[e]);
      }
    else if (e>midPoint)
      {
 	// Player 0 is transferring to player 1
	return E[e] + alpha*(0.5-E[e]);
      }
    else
      return E[midPoint];
  }

  double cdf(double x) const
  {
    if (persistence>0)
      return 1-exp(-persistence*max(0.0,x));
    else
      return x;
  }

  double probHelper(int e, int t, int ep) const
  {
    double c = consumption(e,t);
    
    return ( cdf(E[ep]-c+cIncr) - cdf(E[ep]-c-cIncr)
	     + cdf(c-E[ep]+cIncr) - cdf(c-E[ep]-cIncr) );
  } // probHelper
  
public:
  RiskSharingGame(double delta,
		  int _numEndowments,
		  int _c2e,
		  double _persistence):
    numEndowments(_numEndowments),
    c2e(_c2e),
    persistence(_persistence),
    E(numEndowments,0),
    midPoint((_numEndowments-1)/2),
    stateProbSum(_numEndowments),
    numActions_total(_numEndowments,1),
    SGAbstractGame(delta,
		   _numEndowments,
		   vector< vector<int> > (_numEndowments,vector<int>(2,1)))
  {
    // A couple of checks on the number of endowments
    assert( (numEndowments%2) == 1 );
    assert( numEndowments>2 );

    // Set up the endowment grid and the numbers of actions
    for (int e = 0; e < numEndowments; e++)
      {
	E[e] = (1.0*e)/(numEndowments-1); // player 0's endowment

	cout << E[e] << endl;
	
	numActions_total[e] = abs(e-midPoint)*c2e+1;
	if (e < midPoint)
	  {
	    numActions[e][0] = 1;
	    numActions[e][1] = numActions_total[e];
	  }
	else if (e>midPoint)
	  {
	    numActions[e][1] = 1;
	    numActions[e][0] = numActions_total[e];
	  }
      } // for e

    cout << endl << E[midPoint] << endl;
    
    cIncr = (E[1]-E[0])/2.0;

    // Sum the pseudo probabilities for each state and action
    for (int e = 0; e < numEndowments; e++)
      {
	stateProbSum[e] = vector<double> (numActions_total[e],0);
	for (int t = 0; t < numActions_total[e]; t++)
	  {
	    for (int ep = 0; ep < numStates; ep++)
	      stateProbSum[e][t] += probHelper(e,t,ep);
	  } // for t
      } // for e
  } // constructor

  virtual SGPoint payoffs(int state, const vector<int> & actions) const
  {
    int t = max(actions[0],actions[1]);
    double c = consumption(state,t);
    return SGPoint(sqrt(c),sqrt(1-c));
  } // payoffs

  virtual double probability(int e, const vector<int> & actions, int ep) const
  {
    int t = max(actions[0],actions[1]);
    return probHelper(e,t,ep)/stateProbSum[e][t];
  } // probability
  
};
