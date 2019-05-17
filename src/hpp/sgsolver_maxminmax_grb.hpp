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

#ifndef _SGSOLVER_MAXMINMAX_GRB_HPP
#define _SGSOLVER_MAXMINMAX_GRB_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sggame.hpp"
#include "sgexception.hpp"
#include "gurobi_c++.h"

//! Class that implements a version of the max-min-max algorithm using Gurobi
/*! This file has no associated cpp file, so that libsg does not have
  to link to Gurobi.

  \ingroup src
*/
class SGSolver_MaxMinMax_GRB
{
private:
  //! Const reference to the game being solved.
  const SGGame & game;

  //! Minimum rotation
  double minRotation = -3.14159*1e-4;

  //! Payoff bound
  double payoffBound = 0;

  //! Feasible actions
  vector< vector<bool> > eqActions;
  vector<bool> feasibleActions;
  int numFeasibleActions;

  int numActions_grandTotal;
  vector<int> gaToS;
  vector<int> gaToA;

  //! Payoff bounds.
  list< vector<double> > bounds;

  //! List of gradients in which to bound the correspondence.
  list<SGPoint> directions;

  SGTuple threatTuple;
  
  //! Number of gradients
  int numDirections;

  //! Game elements
  const vector< vector< SGPoint> > & payoffs;
  const vector< vector< vector<double> > > & prob;
  const vector< vector<int> > & numActions;
  const vector< int > & numActions_total;
  const int numStates;
  const double delta;
  const int numPlayers;

  // Parameters
  const double regimeChangeTol = 1e-9;
  const double pseudoConstrTol = 1e-3;
  const double convTol = 1e-6;
  const int maxIter = 1e3;
  
  
public:
  enum SGRegimeStatus
    {
      SG_RECURSIVE,
      SG_FIXED
    };
  enum SGQuadrant
    {
      SG_NORTHEAST,
      SG_SOUTHEAST,
      SG_SOUTHWEST,
      SG_NORTHWEST
    };
  enum SGSolverMode
    {
      SG_FEASIBLE,
      SG_MAXMINMAX,
      SG_APS
    };
    
  enum SGICStatus
    {
      SG_NONE,
      SG_BINDING0,
      SG_BINDING1,
      SG_BINDING01
    };
    
  //! Constructor
  SGSolver_MaxMinMax_GRB(const SGGame & _game):
    game(_game),
    payoffs(_game.getPayoffs()),
    prob(_game.getProbabilities()),
    numActions(_game.getNumActions()),
    numActions_total(_game.getNumActions_total()),
    numStates(_game.getNumStates()),
    delta(_game.getDelta()),
    numPlayers(2),
    bounds(),
    directions(),
    numDirections()
  {
  }

  //! Returns the current game
  const SGGame & getGame() const { return game; }
  // //! Returns the environment
  // GRBEnv & getEnv() {return env; }
  //! Returns payoff bounds
  const list< vector<double> > & getBounds() const { return bounds; }
  //! Return directions
  const list<SGPoint> & getDirections() const { return directions; }
  //! Return numDirections
  int getNumDirections() const { return numDirections; }
  
  //! Solve routine
  void solve();

  //! Initializes the solver
  void initialize();
  
  //! Runs one iteration
  double iterate(const SGSolverMode mode,
		 int & steps);

  void addBoundingHyperplane(SGPoint & currDir,
			     GRBConstr & xConstr,
			     GRBConstr & yConstr,
			     GRBVar * valueFn,
			     int numStates,
			     list<SGPoint> & newDirections,
			     list< vector<double> > & newBounds,
			     GRBModel & model,
			     const bool addDirection);
  void printIteration(ofstream & ofs, int numIter);

};


void SGSolver_MaxMinMax_GRB::solve()
{
  double movement = 1.0;
  int numIter = 0;
  int steps = 0;

  SGSolverMode mode = SG_MAXMINMAX;
  // mode = SG_FEASIBLE;
  // mode = SG_APS;

  initialize();

  threatTuple = SGTuple (game.getNumStates(),-GRB_INFINITY);

  eqActions = game.getEquilibriumActions();
  
  ofstream ofs;
  ofs.open("sgsolver_v3.log");
  
  try
    {
      // First round to compute the feasible set
      iterate(SG_FEASIBLE,steps);
      printIteration(ofs, numIter);

      // Now implement the ABS operator
      while (movement > convTol
	     && numIter < maxIter)
	{
	  movement = iterate(mode,steps);

	  cout << "Iteration: " << numIter
	       << ", directions: " << directions.size()
	       << ", steps: " << steps
	       << ", movement: " << movement
	       << endl;

	  numIter++;

	  // Save the bounds in a log file
	  printIteration(ofs, numIter);
	} // while
    }
  catch (GRBException & e)
    {
      cout << "GRB Exception caught: " << e.getMessage() << endl;
    }

  ofs.close();
  
  cout << "Converged!" << endl;
}

void SGSolver_MaxMinMax_GRB::initialize()
{
  payoffBound = 0;
  for (int s = 0; s < numStates; s++)
    {
      for (int a = 0; a < numActions_total[s]; a++)
	{
	  for (int p = 0; p < numPlayers; p++)
	    {
	      payoffBound = max(payoffBound,abs(payoffs[s][a][p]));
	    } // for p
	} // for a 
    } // for s

  payoffBound *= 1e2;
}

double SGSolver_MaxMinMax_GRB::iterate(const SGSolverMode mode, int & steps)
{
  GRBEnv env;
  // env.set(GRB_IntParam_Method,0); // primal simplex
  // env.set(GRB_IntParam_Method,1); // dual simplex
  env.set(GRB_IntParam_Method,5); // deterministic concurrent simplex; needs newer version of gurobi
  // env.set(GRB_IntParam_Crossover,0); // disable crossover
  // env.set(GRB_DoubleParam_BarConvTol,1e-12);
  env.set(GRB_DoubleParam_OptimalityTol,1e-9);
  env.set(GRB_DoubleParam_MarkowitzTol,0.999);
  env.set(GRB_DoubleParam_FeasibilityTol,1e-9);
  // env.set(GRB_IntParam_DualReductions,0);
  env.set(GRB_IntParam_OutputFlag,0);
  GRBModel model(env);

  vector<int> numEqActions(numStates,0);

  vector<int> actions, deviations;
  int deviation;

  numActions_grandTotal = 0;
  for (int s = 0; s < game.getNumStates(); s++)
    {
      if (eqActions[s].empty())
	{
	  numActions_grandTotal += numActions_total[s];
	  numEqActions[s] = numActions_total[s];
	}
      else
	{
	  for (int a = 0; a < eqActions[s].size(); a++)
	    {
	      if (eqActions[s][a])
		{
		  numActions_grandTotal ++;
		  numEqActions[s] ++;
		}
	    }
	}
    }
  feasibleActions = vector<bool>(numActions_grandTotal,true);
  numFeasibleActions = numActions_grandTotal;

  gaToS = vector<int>  (numActions_grandTotal,0);
  gaToA = vector<int>  (numActions_grandTotal,0);
  {
    int ga = 0;
    for (int s = 0; s < numStates; s++)
      {
	for (int a = 0; a < numActions_total[s]; a++)
	  {
	    if (eqActions[s][a])
	      {
		gaToS[ga] = s;
		gaToA[ga] = a;
		++ga;
	      }
	  } // for a
      } // for s
  }
  
  const int numDirections = directions.size();
  if (mode != SG_FEASIBLE)
    assert(numDirections>0);

  GRBVar * valueFn = model.addVars(numStates);
  GRBVar * contVals = model.addVars(numActions_grandTotal);
  GRBVar * valueFunSlacks = model.addVars(numActions_grandTotal);
  GRBVar * pseudoContVals = model.addVars(numActions_grandTotal);
  GRBVar * recursiveContValSlacks = model.addVars(numActions_grandTotal);
  GRBVar * APSContValSlacks = model.addVars(numActions_grandTotal);
  GRBVar * APSContValVar = model.addVars(numActions_grandTotal);
  GRBVar * feasMult = model.addVars(numActions_grandTotal*numDirections);
  GRBVar * ICMult = model.addVars(numActions_grandTotal*numPlayers);
  GRBVar * currDirVar = model.addVars(2);

  model.update();

  SGPoint currDir(0,1);
  currDirVar[0].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
  currDirVar[1].set(GRB_DoubleAttr_LB,-GRB_INFINITY);

  GRBConstr xConstr = model.addConstr(currDirVar[0] == 0.0);
  GRBConstr yConstr = model.addConstr(currDirVar[1] == 1.0);
  vector< vector<GRBConstr> > valueFnConstr (numStates);
  for (int s = 0; s < numStates; s++)
    {
      valueFnConstr[s] = vector<GRBConstr> (numEqActions[s]);
    }

  vector<GRBLinExpr> recursiveContVal(numActions_grandTotal,0);
  vector<SGRegimeStatus> regimeStatus(numActions_grandTotal,SG_RECURSIVE);
  vector<int> optActions(numStates,-1);

  // This tracks which IC constraints are binding. 
  vector<SGICStatus> optICStatuses(numStates,SG_NONE);
  vector<SGPoint> optPayoffs(numStates,0);

  for (int ga = 0; ga < numActions_grandTotal; ga++)
    {
      contVals[ga].set(GRB_DoubleAttr_LB,-2*payoffBound);
      pseudoContVals[ga].set(GRB_DoubleAttr_LB,-2*payoffBound);
      APSContValVar[ga].set(GRB_DoubleAttr_LB,-2*payoffBound);
    }
  for (int s = 0; s < numStates; s++)
    valueFn[s].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
  
  GRBLinExpr objective = 0;
  {
    int ga = 0; // grandAction
    for (int s = 0; s < numStates; s++)
      {
	objective += 1e10*valueFn[s];

	int actr = 0;
	// Add feasibility constraints
	for (int  a = 0; a < numActions_total[s]; a++)
	  {
	    if (!eqActions[s].empty() && !eqActions[s][a])
	      continue;
	    
	    for (int sp = 0; sp < numStates; sp++)
	      recursiveContVal[ga] += prob[s][a][sp]*valueFn[sp];

	    GRBLinExpr APSContVal = 0;
	    if (mode==SG_MAXMINMAX || mode==SG_APS)
	      {
		// Calculate minIC for each player.
		for (int p = 0; p < numPlayers; p++)
		  {
		    double minIC = SGAction::calculateMinIC(a,s,p,
							    game,threatTuple);
		    APSContVal -= ICMult[p+2*ga]*minIC;
		  }

		{
		  list< vector<double> >::const_iterator bnd;
		  int dirCtr;
		  for (bnd = bounds.begin(),
			 dirCtr = 0;
		       bnd != bounds.end();
		       ++bnd,++dirCtr)
		    {
		      double expBnd = 0;
		      for (int sp = 0; sp < numStates; sp++)
			expBnd += (*bnd)[sp] * prob[s][a][sp];
		      
		      APSContVal += expBnd * feasMult[ga+dirCtr*numActions_grandTotal];
		    } // for bnd
		}

		{
		  list<SGPoint>::const_iterator dir;
		  int dirCtr;
		  for (int p = 0; p < numPlayers; p++)
		    {
		      GRBLinExpr dualConstrLHS = 0;

		      dualConstrLHS += ICMult[p+2*ga];
		      dualConstrLHS += currDirVar[p];
		      for (dir = directions.begin(),
			     dirCtr = 0;
			   dir != directions.end();
			   ++dir,++dirCtr)
			{
			  dualConstrLHS -= (*dir)[p] * feasMult[ga+dirCtr*numActions_grandTotal];
			} // for dir
		      
		      model.addConstr(dualConstrLHS==0);
		    } // for p
		}
	    
		model.addConstr(pseudoContVals[ga] >= APSContVal);
		model.addConstr(contVals[ga] == APSContVal+APSContValSlacks[ga]);
		model.addConstr(APSContValVar[ga]==APSContVal);
	      } // if calculating subgame perfect
	    model.addConstr(pseudoContVals[ga]
			    >= recursiveContVal[ga]*(1.0+pseudoConstrTol));
	    model.addConstr(contVals[ga] == recursiveContVal[ga]+recursiveContValSlacks[ga]);

	    GRBLinExpr valueFnConstrRHS = 0;
	    for (int p = 0; p < numPlayers; p++)
	      valueFnConstrRHS += (1-delta)*payoffs[s][a][p]*currDirVar[p];

	    valueFnConstrRHS += delta*contVals[ga];
	    
	    valueFnConstr[s][actr] = model.addConstr(valueFn[s] == valueFnConstrRHS
						     +valueFunSlacks[ga]);

	    actr++;
	    
	    ++ga;
	  } // for a
      } // for s
  } // ga

  for (int ga = 0; ga < numActions_grandTotal; ga++)
    {
      if (mode == SG_APS)
	{
	  // Start with fixed constraints
	  regimeStatus[ga] = SG_FIXED;
	  APSContValSlacks[ga].set(GRB_DoubleAttr_LB,0);
	  recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
	}
      else
	{
	  // Start with recursive constraints
	  APSContValSlacks[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
	  recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,0);
	}
    } // for ga

  // Finish setting up objective
  for (int ga = 0; ga < numActions_grandTotal; ga++)
    {
      objective += pseudoContVals[ga];
      // objective += contVals[ga];
      objective += APSContValVar[ga];
    } // for ga

  model.setObjective(objective,GRB_MINIMIZE);

  list< vector<double> > newBounds(0);
  list<SGPoint> newDirections(0);
  SGTuple newThreatTuple(numStates);
    
  bool passNorth = false;
  bool newQuadrant = true;
  SGQuadrant quadrant = SG_NORTHEAST;

  // Main loop to find new directions/bounds
  steps = 0;
  do
    {
      // On each iteration, need to accomplish two main tasks.

      // (i) optimize the objective in the current direction. This
      // includes changing any regimes as needed.
      bool regimesSubOptimal = true;
      int regimeChangeIters = 0;

      vector<SGRegimeStatus> oldRegimeStatus(regimeStatus);
      vector<int> oldOptActions(optActions);
      vector<SGICStatus> oldOptICStatuses(optICStatuses);
      vector<SGPoint> oldOptPayoffs(optPayoffs);
	
      while (regimesSubOptimal
	     && regimeChangeIters < 10*numActions_grandTotal)
	{
	  model.optimize();

	  if (model.get(GRB_IntAttr_Status)!=GRB_OPTIMAL)
	    {
	      cout << "Warning: model not optimal. Code is: "
		   << model.get(GRB_IntAttr_Status) << endl;
	    }
	  if (model.get(GRB_IntAttr_Status)==GRB_UNBOUNDED)
	    {
	      cout << "Warning: Unbounded model" << endl;
	    }
	  if (model.get(GRB_IntAttr_Status)==GRB_INFEASIBLE)
	    {
	      cout << "Warning: Infeasible model" << endl;
	    }
	  
	  // If just computing the feasible set, skip the next step of
	  // updating regimes
	  if (mode!=SG_MAXMINMAX)
	    break;

	  // Calculate the maximum slack in the continuation values
	  double maxSlack = 0.0;
	  for (int ga = 0; ga < numActions_grandTotal; ga++)
	    {
	      double tmp = 0;
	      tmp = recursiveContVal[ga].getValue()
		    -APSContValVar[ga].get(GRB_DoubleAttr_X);
	      if (regimeStatus[ga]==SG_FIXED)
		{
		  tmp = -tmp;
		}
	      
	      if (tmp > maxSlack)
		maxSlack = tmp;
	    } // for ga

	  //cout << endl << currDir << endl;
	  // If no slack found, skip the regime changes
	  regimesSubOptimal = (maxSlack > regimeChangeTol);
	  if (!regimesSubOptimal)
	    {
	      // Change fixed regimes to recursive if optimal action
	      // has non-binding constraints
	      int ga = 0;
	      for (int s = 0; s < numStates; s++)
		{
		  int numOptA = 0;
		  // cout << endl << "s = " << s;
		  for (int  a = 0; a < numActions_total[s]; a++)
		    {
		      if (!eqActions[s].empty() && !eqActions[s][a])
			continue;
		      // cout << " (a,vbasis)=(" << a
		      // 	   << "," << valueFunSlacks[ga].get(GRB_IntAttr_VBasis) << ")";
		      if (valueFunSlacks[ga].get(GRB_IntAttr_VBasis) == -1)
			{
			  numOptA++;
			  optActions[s]=ga;
			  if (regimeStatus[ga] == SG_FIXED
			      && ICMult[2*ga].get(GRB_IntAttr_VBasis)==-1
			      && ICMult[1+2*ga].get(GRB_IntAttr_VBasis)==-1)
			    {
			      regimeStatus[ga] = SG_RECURSIVE;
			      optICStatuses[s] = SG_NONE;
			      APSContValSlacks[ga].set(GRB_DoubleAttr_LB,
						       -GRB_INFINITY);
			      recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,0.0);
			    } // if
			  else if (regimeStatus[ga] == SG_FIXED)
			    {
			      // Update the number of IC statuses
			      if (ICMult[1+2*ga].get(GRB_IntAttr_VBasis)==-1)
				optICStatuses[s] = SG_BINDING0;
			      else if (ICMult[2*ga].get(GRB_IntAttr_VBasis)==-1)
				optICStatuses[s] = SG_BINDING1;
			      else
				optICStatuses[s] = SG_BINDING01;
			    } // else if
			  else
			    optICStatuses[s] = SG_NONE;
			} // if
		      ++ga;
		    } // for a
		  //assert(numOptA>=1);
		} // for s
	      // Optimize one last time with the correct regimes.
	      model.optimize();
	      break;
	    } // if
	  else
	    {
	      // Switch regimes where the slack is greater than
	      // delta*maxSlack (since for these actions, changing all of
	      // the other regimes is insufficient to take up all the slack).
	      for (int ga = 0; ga < numActions_grandTotal; ga++)
		{
		  switch (regimeStatus[ga])
		    {
		    case SG_RECURSIVE:
		      if (recursiveContVal[ga].getValue()
			  -APSContValVar[ga].get(GRB_DoubleAttr_X)
			  >delta*maxSlack)
			{
			  regimeStatus[ga] = SG_FIXED;
			  APSContValSlacks[ga].set(GRB_DoubleAttr_LB,0.0);
			  recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,
							 -GRB_INFINITY);
			}
		  
		      break;
		  
		    case SG_FIXED:
		      // If we are in the fixed regime, have to change
		      // regime if there's any slack

		      // If this is the optimal action but no IC
		      // constraint binds, switch to recursive
		      if ( (-recursiveContVal[ga].getValue()
			    +APSContValVar[ga].get(GRB_DoubleAttr_X)
			    > regimeChangeTol))
			{
			  regimeStatus[ga] = SG_RECURSIVE;
			  APSContValSlacks[ga].set(GRB_DoubleAttr_LB,
						   -GRB_INFINITY);
			  recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,0.0);
			}
		      break;
		    } // switch
		} // for ga
	    } // else
	  regimeChangeIters++;
	} // while

      // We want to add a new direction only if the optimum changes
      // for the value function. This means either (a) changing which
      // constraint binds in some state s or (b) changing a regime for
      // a binding action. If either of these conditions is met,
      // change the following flag to true. This will hopefully help
      // cut down on spurious constraints.
      bool addDirection = newQuadrant;
      for (int s = 0; s < numStates; s++)
	{
	  // Check if the optimal policy changed, and if so, trip the
	  // addDirection flag and update the optimal action
	  if (optActions[s] != oldOptActions[s]
	      || regimeStatus[optActions[s]] != oldRegimeStatus[optActions[s]]
	      || optICStatuses[s] != oldOptICStatuses[s])
	    addDirection = true;
	} // for s
      addDirection = true;

      if (regimeChangeIters >= numActions_grandTotal)
	cout << "Warning: Too many regime changes. " << regimeChangeIters
	     << " changes but only " << numActions_grandTotal
	     << " action profiles." << endl;

      // (ii) Find how far we can rotate clockwise without violating
      // optimality. Add a new hyperplane for this face. Rotate the
      // objective and continue.
      double tmp = 0;
      switch (quadrant)
	{
	case SG_NORTHEAST:
	  // Increasing currDirVar[0] and decreasing currDirVar[1]
	  tmp = xConstr.get(GRB_DoubleAttr_SARHSUp);
	  currDir[0] = tmp;
	  	  
	  addBoundingHyperplane(currDir,xConstr,yConstr,valueFn,
				numStates,newDirections,newBounds,model,
				addDirection);
	  newQuadrant = false;
	  
	  if (currDir[1]<0)
	    {
	      quadrant=SG_SOUTHEAST;
	      newQuadrant = true;
	      // cout << "Starting southeast..." << endl;
	    }

	  break;
	  
	case SG_SOUTHEAST:
	  // Decreasing currDirVar[0] and decreasing currDirVar[1]
	  tmp = yConstr.get(GRB_DoubleAttr_SARHSLow);
	  currDir[1] = tmp;

	  addBoundingHyperplane(currDir,xConstr,yConstr,valueFn,
				numStates,newDirections,newBounds,model,
				addDirection);

	  newQuadrant = false;
	  if (currDir[0]<0)
	    {
	      quadrant=SG_SOUTHWEST;
	      newQuadrant = true;

	      // Last direction must have been due south... update
	      // threats for player 2
	      for (int s = 0; s < numStates; s++)
		newThreatTuple[s][1]=-newBounds.back()[s];
	      
	      // cout << "Starting southwest..." << endl;
	    }

	  break;
	  
	case SG_SOUTHWEST:
	  // Decreasing currDirVar[0] and increasing currDirVar[1]
	  tmp = xConstr.get(GRB_DoubleAttr_SARHSLow);
	  currDir[0] = tmp;

	  addBoundingHyperplane(currDir,xConstr,yConstr,valueFn,
				numStates,newDirections,newBounds,model,
				addDirection);

	  newQuadrant = false;
	  // cout << "Maximum RHS " << tmp << endl;
	  if (currDir[1]>0)
	    {
	      quadrant=SG_NORTHWEST;
	      newQuadrant = true;

	      // Last direction must have been due west... update
	      // threats for player 1
	      for (int s = 0; s < numStates; s++)
		newThreatTuple[s][0]=-newBounds.back()[s];
	      
	      // cout << "Starting northwest..." << endl;
	    }	      
	  
	  break;
	  
	case SG_NORTHWEST:
	  // Increasing currDirVar[0] and increasing currDirVar[1]
	  tmp = yConstr.get(GRB_DoubleAttr_SARHSUp);
	  currDir[1] = tmp;

	  addBoundingHyperplane(currDir,xConstr,yConstr,valueFn,
				numStates,newDirections,newBounds,model,
				addDirection);

	  newQuadrant = false;
	  
	  // cout << "Maximum RHS " << tmp << endl;
	  if (currDir[0]>0)
	    {
	      quadrant=SG_NORTHEAST;
	      newQuadrant = true;
	      // cout << "Passing north!" << endl;
	      passNorth = true;
	    }
	  break;
	  
	} // switch


      ++ steps;
    } while (!passNorth);
  // cout << "Done with iteration!" << endl;
  // cout << "New threat tuple: " << newThreatTuple << endl;
  
  delete [] valueFn;
  delete [] contVals;
  delete [] valueFunSlacks;
  delete [] pseudoContVals;
  delete [] recursiveContValSlacks;
  delete [] APSContValSlacks;
  delete [] APSContValVar;
  delete [] feasMult;
  delete [] ICMult;
  delete [] currDirVar;

  double dist = 1.0;
  if (directions.size() == newDirections.size())
    {
      dist = 0.0;
      list<SGPoint>::const_iterator dir;
      list< vector<double> >::const_iterator bnd;
      list<SGPoint>::const_iterator newDir;
      list< vector<double> >::const_iterator newBnd;
      for (dir = directions.begin(),
	     bnd = bounds.begin(),
	     newDir = newDirections.begin(),
	     newBnd = newBounds.begin();
	   dir != directions.end();
	   ++dir,++bnd,++newDir,++newBnd)
	{
	  dist = std::max(dist,SGPoint::distance(*dir,*newDir));
	  
	  for (int s = 0; s < numStates; s++)
	    dist = std::max(dist,abs((*bnd)[s]-(*newBnd)[s]));
	}
    } // if 
  bounds = newBounds;
  directions = newDirections;

  // for (int s = 0; s < numStates; s++)
  //   threatTuple[s].max(newThreatTuple[s]);
  threatTuple = newThreatTuple;
  
  return dist;
} // iterate

void SGSolver_MaxMinMax_GRB::addBoundingHyperplane(SGPoint & currDir,
					GRBConstr & xConstr,
					GRBConstr & yConstr,
					GRBVar * valueFn,
					int numStates,
					list<SGPoint> & newDirections,
					list< vector<double> > & newBounds,
					GRBModel & model,
					const bool addDirection)
{
  int roundScale = 1e7;
  
  currDir.normalize();
  if (addDirection)
    {
      xConstr.set(GRB_DoubleAttr_RHS,currDir[0]);
      yConstr.set(GRB_DoubleAttr_RHS,currDir[1]);

      // const double defaultLimit = model.getEnv().get(GRB_DoubleParam_IterationLimit);
      // model.getEnv().set(GRB_DoubleParam_IterationLimit,1);
      // model.optimize();
      // model.getEnv().set(GRB_DoubleParam_IterationLimit,defaultLimit);
      model.optimize();
      
      // currDir.roundPoint(1.0/roundScale);

      if (newDirections.size() > 2)
	{
	  // Check if colinear with last two hyperplanes Want to know if
	  // there exist a, b such that a*(d,h)+b*(d',h')=(d'',h''). We
	  // can solve the matrix equation a*d+b*d' = d'', and then check
	  // if a*h+b*h'=h''.
	  std::list<SGPoint>::const_reverse_iterator d0 = newDirections.rbegin();
	  std::list<SGPoint>::const_reverse_iterator d1 = d0++;
	  double det = (*d0)[0]*(*d1)[1]-(*d0)[1]*(*d1)[0];

	  if (det == 0 && SGPoint::distance(*d0,*d1)==0)
	    {
	      cout << "Warning: Repeated direction. " << endl
	  	   << (*d0) << " " << (*d1) << " " << currDir << endl;
	    }
	  
	  double a = (currDir[0]*(*d1)[1]-currDir[1]*(*d1)[0])/det;
	  double b = ((*d0)[0]*currDir[1]-(*d0)[1]*currDir[0])/det;

	  std::list<vector<double> >::const_reverse_iterator h0 = newBounds.rbegin();
	  std::list<vector<double> >::const_reverse_iterator h1 = h0++;
	  double distSum = 0;
	  for (int s = 0; s < numStates; s++)
	    distSum += abs(a*(*h0)[s]+b*(*h1)[s] - valueFn[s].get(GRB_DoubleAttr_X));
	  // cout << "Checking for colinearity" << endl;
	  // cout << distSum << endl;
	  if (distSum < 1e-14)
	    {
	      // If colinear, drop the previous hyperplane/bound.
	      newBounds.pop_back();
	      newDirections.pop_back();
	      // cout << "Colinear hyperplane found." << endl;
	    }
	}
      
      newDirections.push_back(currDir);
      newBounds.push_back(vector<double>(numStates,0));
      for (int s = 0; s < numStates; s++)
	{
	  double tmp  = valueFn[s].get(GRB_DoubleAttr_X);
	  // tmp  = round(tmp*roundScale)/roundScale;
	  assert(!isnan(tmp));
	  newBounds.back()[s] = tmp;
	}
    }

  // SGPoint oldDir = currDir;
  currDir.rotateCW(-minRotation);
  // cout << "Distance from rotation: " << setprecision(15) << SGPoint::distance(oldDir,currDir) << endl;
  xConstr.set(GRB_DoubleAttr_RHS,currDir[0]);
  yConstr.set(GRB_DoubleAttr_RHS,currDir[1]);
} // addBoundingHyperlpane

void SGSolver_MaxMinMax_GRB::printIteration(ofstream & ofs, int numIter)
{
  const int numStates = game.getNumStates();
  ofs << numIter << " " << directions.size();
  for (int s = 0; s < numStates; s++)
    ofs << " " << 0;
  ofs << endl;
  
  list<SGPoint>::const_iterator dir;
  list< vector<double> >::const_iterator bnd;
  for (dir = directions.begin(),
	 bnd = bounds.begin();
       dir != directions.end();
       ++dir,++bnd)
    {
      ofs << setprecision(12) << (*dir)[0] << " " << (*dir)[1];
      for (int s = 0; s < numStates; s++)
	ofs << " " << (*bnd)[s];
      ofs << endl;
    }
  
} // printIteration


#endif 
