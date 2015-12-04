#ifndef _SGSIMULATOR_HPP
#define _SGSIMULATOR_HPP

#include "sgsolution.hpp"
#include <chrono>
#include <random>
#include <utility>



class SGSimulator
{
  typedef pair< list<SGIteration>::const_iterator, double > transitionPair;

private:
  const SGSolution & soln;

  list<SGIteration>::const_iterator startOfLastRev;

  vector< vector< list<transitionPair> > > transitionTable;

  vector< vector<int> > actionDistr;
  vector<int> tupleDistr;
  vector<int> stateDistr;

  int numIter;

  bool logFlag;

  double weightTol;
  
  std::stringstream ss;

  std::stringstream transitionTableSS;
  
public:
  SGSimulator(const SGSolution & _soln): 
    soln(_soln), logFlag(false), weightTol(1e-4)
  {}

  const vector< vector<int> > getActionDistr() const { return actionDistr; }
  const vector<int> getStateDistr() const { return stateDistr; } 
  const vector<int> getTupleDistr() const { return tupleDistr; } 
  int getStartOfLastRev() const { return startOfLastRev->iteration; }
  int getNumIter() const { return numIter; } 

  void setLogFlag(bool newFlag) { logFlag = newFlag; };
  
  const std::stringstream & getStringStream() const {return ss;}

  void initialize ();

  void simulate(int _numIter, int initialState, int initialTuple);

  //! Returns the long run action distribution
  SGPoint getLongRunPayoffs()
  {
    SGPoint payoffs(0.0,0.0);
    for (int state = 0; 
	 state < soln.game.getNumStates(); 
	 state++)
      {
	for (int action = 0;
	     action < soln.game.getNumActions_total()[state];
	     action++)
	  {
	    payoffs += (1.0*actionDistr[state][action])/(1.0*numIter)
	      * soln.game.getPayoffs()[state][action];
	  }
      } // state

    return payoffs;
  }

}; // SGSimulator

#endif
