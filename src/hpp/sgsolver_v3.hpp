// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
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

#ifndef _SGSOLVER_V3_HPP
#define _SGSOLVER_V3_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sggame.hpp"
#include "sgexception.hpp"
#include "gurobi_c++.h"

//! Class that implements the JYC algorithm using Gurobi
/*! This file has no associated cpp file, so that libsg does not have
  to link to Gurobi.

  \ingroup src
*/
class SGSolver_V3
{
private:
  //! Const reference to the game being solved.
  const SGGame & game;

  //! Minimum rotation
  double minRotation = -3.14159*1e-4;

  // //! The Gurobi environment.
  // GRBEnv env;
  // //! The Gurobi model.
  // GRBModel model;

  // GRBVar * valueFun;

  // vector<GRBVar *> ICMult;
  // vector<GRBVar *> FeasMult;

  //! Payoff bounds.
  list< vector<double> > bounds;

  //! List of gradients in which to bound the correspondence.
  list<SGPoint> directions;

  SGTuple threatTuple;
  
  //! Number of gradients
  int numDirections;

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
    
  //! Constructor
  SGSolver_V3(const SGGame & _game):
    game(_game),
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
  double iterate(const SGSolverMode mode = SG_MAXMINMAX);

  void addBoundingHyperplane(SGPoint & currDir,
			     GRBConstr & xConstr,
			     GRBConstr & yConstr,
			     GRBVar * valueFn,
			     int numStates,
			     list<SGPoint> & newDirections,
			     list< vector<double> > & newBounds,
			     GRBModel & model);
  void printIteration(ofstream & ofs, int numIter);

};

void SGSolver_V3::solve()
{
  double convTol = 1e-6;
  int maxIter = 100;
  double movement = 1.0;
  int numIter = 0;

  SGSolverMode mode = SG_MAXMINMAX;
  // mode = SG_FEASIBLE;
  // mode = SG_APS;
  
  initialize();

  ofstream ofs;
  ofs.open("sgsolver_v3.log");
  
  try
    {
      // First round to compute the feasible set
      iterate(SG_FEASIBLE);
      printIteration(ofs, numIter);

      // Now implement the ABS operator
      while (movement > convTol
	     && numIter < maxIter)
	{
	  movement = iterate(mode);

	  cout << "Iteration: " << numIter
	       << ", movement: " << movement
	       << ", directions: " << directions.size()
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

void SGSolver_V3::initialize()
{
}

double SGSolver_V3::iterate(const SGSolverMode mode)
{
  double regimeChangeTol = 1e-6;

  GRBEnv env;
  // env.set(GRB_IntParam_Method,0); // primal simplex
  env.set(GRB_IntParam_Method,1); // dual simplex
  // env.set(GRB_IntParam_Method,5); // deterministic concurrent simplex; needs newer version of gurobi
  // env.set(GRB_IntParam_Crossover,0); // disable crossover
  // env.set(GRB_DoubleParam_BarConvTol,1e-12);
  env.set(GRB_DoubleParam_OptimalityTol,1e-9);
  // env.set(GRB_DoubleParam_MarkowitzTol,0.999);
  // env.set(GRB_DoubleParam_FeasibilityTol,1e-9);
  env.set(GRB_IntParam_OutputFlag,0);
  GRBModel model(env);

  vector<int> actions, deviations;
  int deviation;

  const vector< vector< SGPoint> > & payoffs = game.getPayoffs();
  const vector< vector< vector<double> > > & prob = game.getProbabilities();
  const vector< vector<int> > & numActions = game.getNumActions();
  const vector< int > & numActions_total = game.getNumActions_total();
  const int numStates = game.getNumStates();
  const double delta = game.getDelta();
  const int numPlayers = 2;

  int numActions_grandTotal = 0;
  for (int s = 0; s < numStates; s++)
    numActions_grandTotal += numActions_total[s];
  
  const int numDirections = directions.size();

  GRBVar * valueFn = model.addVars(numStates);
  GRBVar * regimeVals = model.addVars(numActions_grandTotal);
  GRBVar * recursiveContValSlacks = model.addVars(numActions_grandTotal);
  GRBVar * APSContValSlacks = model.addVars(numActions_grandTotal);
  GRBVar * feasMult = model.addVars(numActions_grandTotal*numStates*numDirections);
  GRBVar * ICMult = model.addVars(numActions_grandTotal*numPlayers);
  GRBVar * currDirVar = model.addVars(2);

  model.update();

  currDirVar[0].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
  currDirVar[1].set(GRB_DoubleAttr_LB,-GRB_INFINITY);

  vector<GRBLinExpr> recursiveContVal(numActions_grandTotal,0);
  vector<GRBLinExpr> APSContVal(numActions_grandTotal,0);
  vector<SGRegimeStatus> regimeStatus(numActions_grandTotal,SG_RECURSIVE);

  for (int ga = 0; ga < numActions_grandTotal; ga++)
    regimeVals[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
  for (int s = 0; s < numStates; s++)
    valueFn[s].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
  
  GRBLinExpr objective = 0;
  {
    int ga = 0; // grandAction
    for (int s = 0; s < numStates; s++)
      {
	objective += valueFn[s];

	// Add feasibility constraints
	for (int a = 0; a < numActions_total[s]; a++)
	  {
	    for (int sp = 0; sp < numStates; sp++)
	      recursiveContVal[ga] += prob[s][a][sp]*valueFn[sp];

	    if (mode==SG_MAXMINMAX || mode==SG_APS)
	      {
		// Calculate minIC for each player.
		for (int p = 0; p < numPlayers; p++)
		  {
		    double minIC = SGAction::calculateMinIC(a,s,p,
							    game,threatTuple);
		    APSContVal[ga] -= ICMult[p+2*ga]*minIC;
		  }

		for (int sp = 0; sp < numStates; sp++)
		  {
		    {
		      list<SGPoint>::const_iterator dir;
		      list< vector<double> >::const_iterator bnd;
		      int dirCtr;
		      for (dir = directions.begin(),
			     bnd = bounds.begin(),
			     dirCtr = 0;
			   dir != directions.end();
			   ++dir,++bnd,++dirCtr)
			{
			  APSContVal[ga] += ( feasMult[ga+dirCtr*numActions_grandTotal
						       + sp * numDirections * numActions_grandTotal]
					      * (*bnd)[sp]);
			} // for dir
		    }

		    {
		      list<SGPoint>::const_iterator dir;
		      list< vector<double> >::const_iterator bnd;
		      int dirCtr;
		      for (int p = 0; p < numPlayers; p++)
			{
			  GRBLinExpr dualConstrLHS = 0;

			  dualConstrLHS += prob[s][a][sp]*currDirVar[p]
			    +ICMult[p+2*ga]*prob[s][a][sp];
			  for (dir = directions.begin(),
				 bnd = bounds.begin(),
				 dirCtr = 0;
			       dir != directions.end();
			       ++dir,++bnd,++dirCtr)
			    {
			      dualConstrLHS -= (*dir)[p] * feasMult[ga+dirCtr*numActions_grandTotal
								    + sp * numDirections * numActions_grandTotal];
			    } // for dir

			  model.addConstr(dualConstrLHS==0);
			} // for p
		    }
		  } // for sp
	    
		model.addConstr(regimeVals[ga] == APSContVal[ga]+APSContValSlacks[ga]);
	      } // if calculating subgame perfect
	    model.addConstr(regimeVals[ga] == recursiveContVal[ga]+recursiveContValSlacks[ga]);

	    GRBLinExpr valueFnConstrRHS = 0;
	    for (int p = 0; p < numPlayers; p++)
	      valueFnConstrRHS += (1-delta)*payoffs[s][a][p]*currDirVar[p];

	    if (mode==SG_APS)
	      valueFnConstrRHS += delta*APSContVal[ga];
	    else
	      valueFnConstrRHS += delta*regimeVals[ga];
	    
	    model.addConstr(valueFn[s] >= valueFnConstrRHS);
	    
	    ++ga;
	  } // for a
      } // for s
  } // ga

  // Start with recursive constraints
  for (int ga = 0; ga < numActions_grandTotal; ga++)
    {
      APSContValSlacks[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
      recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,0);
    } // for ga

  // Finish setting up objective
  for (int ga = 0; ga < numActions_grandTotal; ga++)
    {
      objective += regimeVals[ga];
      objective += APSContVal[ga];
    } // for ga

  model.setObjective(objective,GRB_MINIMIZE);

  list< vector<double> > newBounds;
  list<SGPoint> newDirections;
  SGTuple newThreatTuple(numStates);
  
  SGPoint currDir(0,1);
  GRBConstr xConstr = model.addConstr(currDirVar[0] == 0.0);
  GRBConstr yConstr = model.addConstr(currDirVar[1] == 1.0);
  bool passNorth = false;
  SGQuadrant quadrant = SG_NORTHEAST;

  // Main loop to find new directions/bounds
  do
    {
      // cout << "Current direction = " << currDir
      // 	   << ", feasible flag = " << (mode==SG_FEASIBLE) << endl;
  
      // On each iteration, need to accomplish two main tasks.

      // (i) optimize the objective in the current direction. This
      // includes changing any regimes as needed.
      bool regimesSubOptimal = true;
      int regimeChangeIters = 0;
      while (regimesSubOptimal && regimeChangeIters < numActions_grandTotal)
	{
	  model.optimize();

	  // If just computing the feasible set, skip this next step
	  if (mode!=SG_MAXMINMAX)
	    break;

	  regimesSubOptimal = false;
	  for (int ga = 0; ga < numActions_grandTotal; ga++)
	    {
	      switch (regimeStatus[ga])
		{
		case SG_RECURSIVE:
		  if (APSContValSlacks[ga].get(GRB_DoubleAttr_X)
		      >regimeChangeTol)
		    {
		      regimeStatus[ga] = SG_FIXED;
		      APSContValSlacks[ga].set(GRB_DoubleAttr_LB,0);
		      recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
		      regimesSubOptimal = true;
		    }
		  
		  break;
		  
		case SG_FIXED:
		  if (recursiveContValSlacks[ga].get(GRB_DoubleAttr_X)
		      > regimeChangeTol)
		    {
		      regimeStatus[ga] = SG_RECURSIVE;
		      APSContValSlacks[ga].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
		      recursiveContValSlacks[ga].set(GRB_DoubleAttr_LB,0);
		      regimesSubOptimal = true;
		    }
		  break;
		} // switch
	    } // for ga
	  regimeChangeIters++;
	} // while
      if (regimeChangeIters >= numActions_grandTotal)
	cout << "Warning: Too many regime changes..." << endl;

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
				numStates,newDirections,newBounds,model);
	  
	  if (currDir[1]<0)
	    {
	      quadrant=SG_SOUTHEAST;
	      // cout << "Starting southeast..." << endl;
	    }

	  break;
	  
	case SG_SOUTHEAST:
	  // Decreasing currDirVar[0] and decreasing currDirVar[1]
	  tmp = yConstr.get(GRB_DoubleAttr_SARHSLow);
	  currDir[1] = tmp;

	  addBoundingHyperplane(currDir,xConstr,yConstr,valueFn,
				numStates,newDirections,newBounds,model);

	  if (currDir[0]<0)
	    {
	      quadrant=SG_SOUTHWEST;

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
				numStates,newDirections,newBounds,model);

	  // cout << "Maximum RHS " << tmp << endl;
	  if (currDir[1]>0)
	    {
	      quadrant=SG_NORTHWEST;

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
				numStates,newDirections,newBounds,model);
	  
	  // cout << "Maximum RHS " << tmp << endl;
	  if (currDir[0]>0)
	    {
	      quadrant=SG_NORTHEAST;
	      // cout << "Passing north!" << endl;
	      passNorth = true;
	    }
	  break;
	  
	} // switch

    } while (!passNorth);
  

  // cout << "Done with iteration!" << endl;
  cout << "New threat tuple: " << newThreatTuple << endl;
  
  delete[] valueFn;
  delete[] regimeVals;
  delete[] feasMult;
  delete[] ICMult;
  delete[] currDirVar;
  
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
  threatTuple = newThreatTuple;
  
  return dist;
} // iterate

void SGSolver_V3::addBoundingHyperplane(SGPoint & currDir,
					GRBConstr & xConstr,
					GRBConstr & yConstr,
					GRBVar * valueFn,
					int numStates,
					list<SGPoint> & newDirections,
					list< vector<double> > & newBounds,
					GRBModel & model)
{
  currDir.normalize();
  xConstr.set(GRB_DoubleAttr_RHS,currDir[0]);
  yConstr.set(GRB_DoubleAttr_RHS,currDir[1]);

  // const double defaultLimit = model.getEnv().get(GRB_DoubleParam_IterationLimit);
  // model.getEnv().set(GRB_DoubleParam_IterationLimit,1);
  // model.optimize();
  // model.getEnv().set(GRB_DoubleParam_IterationLimit,defaultLimit);
  model.optimize();
  
  newDirections.push_back(currDir);
  newBounds.push_back(vector<double>(numStates,0));
  for (int s = 0; s < numStates; s++)
    {
      newBounds.back()[s] = valueFn[s].get(GRB_DoubleAttr_X);
    }
  
  currDir.rotateCW(minRotation);
  xConstr.set(GRB_DoubleAttr_RHS,currDir[0]);
  yConstr.set(GRB_DoubleAttr_RHS,currDir[1]);
} // addBoundingHyperlpane

void SGSolver_V3::printIteration(ofstream & ofs, int numIter)
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
      ofs << (*dir)[0] << " " << (*dir)[1];
      for (int s = 0; s < numStates; s++)
	ofs << " " << (*bnd)[s];
      ofs << endl;
    }
  
} // printIteration


#endif 
