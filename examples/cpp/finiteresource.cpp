// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2019 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

//! One state prisoner's dilemma
//! @example
#include "sgsolver.hpp"

// Firms competing with a finite resource
class FiniteResource : public SGAbstractGame
{
private:
  int numQuantities;

  double A; // Demand intercept
  double B; // Demand slope

  double maxCost; // Maximum marginal cost of production
  double maxQuantity; // Maximum quantity

public:
  FiniteResource(double _delta, int _numStates, int _numQuantities,
		 double _A, double _B,
		 double _maxCost, double _maxQuantity ):
    SGAbstractGame(_delta,_numStates,
		   vector< vector<int> > (_numStates,vector<int>(2,_numQuantities))),
    numQuantities(_numQuantities),
    A(_A), B(_B),
    maxCost(_maxCost), maxQuantity(_maxQuantity)
  {}
    

  virtual SGPoint payoffs(int state,
			  const vector<int> & actions) const
  {
    double cost = maxCost*(1.0-static_cast<double>(state)/(numStates-1.0));

    SGPoint q;
    double Q = 0;
    for (int p = 0; p < 2; p++)
      {
	q[p] = maxQuantity*static_cast<double>(actions[p])/(numQuantities-1.0);
	Q += q[p];
      }

    double price = max(A-B*Q,0.0);
    
    return (price-cost)*q;
  }

  virtual double probability(int state,
			     const vector<int> & actions, int statep) const
  {
    // Lowest state is absorbing
    if (state == 0 && statep == 0)
      return 1.0;
    
    // State decreases by 1 with probability equal to total
    // production/maximum production
    int actionSum = actions[0]+actions[1];
    double p = static_cast<double>(actionSum) / (1.0*numQuantities);
    
    if (state>0 && statep == state-1)
      return p;
    else if (state>0 && state == statep)
      return 1.0-p;

    return 0;
  }
}; // FiniteResource

int main()
{
  double delta = 0.75;

  double A = 1.0;
  double B = 1.0;
  double maxQ = 1.0;
  double maxC = 0.8;

  double numQ = 20;
  double numC = 10;
  
  FiniteResource frgame (delta, numC, numQ,
		  A, B,
		  maxC, maxQ);

  SGGame game (frgame);

  SGGame::save(game,"finiteresource.sgm");
  
  // SGSolver solver(FiniteResource);

  // solver.solve();


  return 0.0;
};
