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

#ifndef _SGSOLVER_JYC_HPP
#define _SGSOLVER_JYC_HPP

#include "sgcommon.hpp"
#include "sgutilities.hpp"
#include "sggame.hpp"
#include "sgexception.hpp"
#include "gurobi_c++.h"

//! Class that implements the JYC algorithm using Gurobi
/*! This class implements the generalization of the algorithm of Judd,
  Yeltekin, and Conklin (2002) for solving stochastic games.

  This file has no associated cpp file, so that libsg does not have to
  link to Gurobi.

  Currently works on two and three player games.

  \ingroup src
*/
class SGSolver_JYC
{
private:
  //! Const reference to the game being solved.
  const SGGame & game;

  //! The Gurobi environment.
  GRBEnv env;
  //! The Gurobi model.
  GRBModel model;

  //! Payoff levels.
  vector< vector<double> > levels;

  //! List of gradients in which to bound the correspondence.
  vector<SGPoint> directions;

  //! Number of gradients
  int numDirections;

public:
  //! Constructor
  SGSolver_JYC(const SGGame & _game, int _numDirections):
    env(),
    model(env),
    game(_game),
    levels(vector< vector<double> > (game.getNumStates(),
				     vector<double>(_numDirections,0))),
    directions(_numDirections),
    numDirections(_numDirections)
  {
    env.set(GRB_IntParam_Method,2); // barrier
    env.set(GRB_IntParam_Crossover,0); // disable crossover
    env.set(GRB_DoubleParam_BarConvTol,1e-12);
    env.set(GRB_DoubleParam_OptimalityTol,1e-9);
    env.set(GRB_DoubleParam_MarkowitzTol,0.999);
    env.set(GRB_DoubleParam_FeasibilityTol,1e-9);
    env.set(GRB_IntParam_OutputFlag,0);
  }

  //! Returns the current game
  const SGGame & getGame() const { return game; }
  //! Returns the environment
  GRBEnv & getEnv() {return env; }
  //! Returns payoff levels
  const vector< vector<double> > & getLevels() const { return levels; }
  //! Return directions
  const vector<SGPoint> & getDirections() const { return directions; }
  //! Return numDirections
  int getNumDirections() const { return numDirections; }
  
  //! Solve routine
  void solve();

  //! Initializes the solver
  void initialize();
  
  //! Runs one iteration
  double iterate();
};

void SGSolver_JYC::solve()
{
  double errorTol = 1e-8;
  double error = 1.0;
  int numIterations = 0;

  initialize();
  
  while (error > errorTol)
    {
      error = iterate();
      cout << "Iteration: " << numIterations
	   << ", error: " << error << endl;

      numIterations++;
    }
}

void SGSolver_JYC::initialize()
{
  const int numPlayers = game.getNumPlayers();

  // Set directions to be equally spaced
  if (numPlayers==2)
    {
      for (int dir = 0; dir < numDirections; dir++)
	{
	  double theta = 2.0*dir/numDirections*PI;
	  directions[dir] = SGPoint(std::cos(theta),
				    std::sin(theta));
	}
    }
  else if (numPlayers==3)
    {
      int numDirsApprox = numDirections;
      numDirections = 0;
      directions.clear();
      
      double a = 4.0*PI/static_cast<double>(numDirsApprox);
      double d = sqrt(a);
      int Mpsi = round(PI/d);
      double dpsi = PI/static_cast<double>(Mpsi);
      double dphi = a/dpsi;

      // Initialize directions - approximately evenly spaced around the
      // sphere, with three negative coordinate directions at the end.
      vector< list<vector< double> > :: const_iterator> playerMinLvls(numPlayers);
      for (int mpsi = 0; mpsi < Mpsi; mpsi++)
	{
	  double psi = PI*(static_cast<double>(mpsi)+0.5)/static_cast<double>(Mpsi);
	  int Mphi = round(2.0*PI*sin(psi)/dpsi);

	  for (int mphi = 0; mphi < Mphi; mphi++)
	    {
	      double phi = 2.0*PI*static_cast<double>(mphi)/static_cast<double>(Mphi);

	      SGPoint newDir(numPlayers,0.0);
	      newDir[0]=sin(psi)*cos(phi);
	      newDir[1]=sin(psi)*sin(phi);
	      newDir[2]=cos(psi);
	      directions.push_back(newDir);

	      numDirections++;	  
	    }
	}

      levels= vector< vector<double> > (game.getNumStates(),
					vector<double>(numDirections,0));      
    } // 3 players

  // Variables 
  GRBVar *  var = model.addVars(2*numPlayers*game.getNumStates()); // One variable for each
					      // player/state in
					      // equilibrium and one
					      // variable for each
					      // player/state as the
					      // threat
  for (int varCtr = 0; varCtr < model.get(GRB_IntAttr_NumVars); varCtr++)
    {
      var[varCtr].set(GRB_DoubleAttr_LB,-1e20);
    }

  // First numPlayers*numStates variables correspond to eq payoffs, second
  // numPlayers*numStates are threats.
  model.update();

  // Calculate initial levels. 
  SGPoint NE, SW;
  game.getPayoffBounds(NE,SW);
  SGPoint NW(SW[0],NE[1]), SE(NE[0],SW[1]);

  for (int dir = 0; dir < numDirections; dir++)
    {
      levels[0][dir] = NE*directions[dir];
      levels[0][dir] = std::max(levels[0][dir],
				SE*directions[dir]);
      levels[0][dir] = std::max(levels[0][dir],
				SW*directions[dir]);
      levels[0][dir] = std::max(levels[0][dir],
				NW*directions[dir]);

      for (int state = 1; state < game.getNumStates(); state++)
	levels[state][dir] = levels[0][dir];
    } // direction

  // Add feasibility constraints.
  for (int state = 0; state < game.getNumStates(); state++)
    {
      // Equilibrium payoffs
      for (int dir = 0; dir < numDirections; dir++)
	{
	  GRBLinExpr lhs = 0;
	  for (int p = 0; p < numPlayers; p++)
	    {
	      lhs += var[numPlayers*state+p]*directions[dir][p];
	    }
	  model.addConstr(lhs <= levels[state][dir]);
	} // direction

      // Threat points
      for (int dir = 0; dir < numDirections; dir++)
	{
	  GRBLinExpr lhs = 0;
	  for (int p = 0; p < numPlayers; p++)
	    {
	      lhs += var[numPlayers*game.getNumStates()
			 +numPlayers*state+p]*directions[dir][p];
	    }
	  model.addConstr(lhs <= levels[state][dir]);
	} // direction
    } // state
  model.update();
  
  delete[] var;
}

double SGSolver_JYC::iterate()
{
  const int numPlayers = game.getNumPlayers();

  vector<int> actions, deviations;
  int deviation;

  const vector< vector< SGPoint> > & payoffs = game.getPayoffs();
  const vector< vector< vector<double> > > & prob = game.getProbabilities();
  const vector< vector<int> > & numActions = game.getNumActions();
  const vector< int > & numActions_total = game.getNumActions_total();
  const vector< vector<bool> > & eqActions = game.getEquilibriumActions();
  int numStates = game.getNumStates();
  double delta = game.getDelta();

  GRBVar * var = model.getVars();

  GRBConstr * constr = model.getConstrs();

  // Update feasibility constraints.
  for (int state = 0; state < numStates; state++)
    {
      // Equilibrium payoffs
      for (int dir = 0; dir < numDirections; dir++)
  	{
  	  constr[2*state*numDirections+dir].set(GRB_DoubleAttr_RHS,levels[state][dir]);
  	  constr[(2*state+1)*numDirections+dir].set(GRB_DoubleAttr_RHS,levels[state][dir]);
  	} // direction
    } // state

  vector< vector<double> > 
    newLevels(numStates,
	      vector<double>(numDirections,
			     -numeric_limits<double>::max()));
  for (int state = 0; state < numStates; state++)
    {
      for (int action = 0; action < numActions_total[state]; action++)
	{

	  if (!eqActions[state].empty() && !eqActions[state][action])
	    continue;
	      
	  indexToVector(action,actions,numActions[state]);
	  
	  deviations = actions;

	  // Implement incentive constraints for this action
	  for (int player = 0; player < numPlayers; player ++)
	    {
	      
	      GRBLinExpr lhs = 0;
	      lhs += (1-delta)*payoffs[state][action][player];
	      for (int sp = 0; sp < numStates; sp++)
	  	lhs += delta*prob[state][action][sp]*var[numPlayers*sp+player];

	      for (int dev = 0; dev < numActions[state][player]; dev++)
	  	{
	  	  if (dev == actions[player])
	  	    continue;
		  
	  	  deviations[player] = dev;
	  	  deviation = vectorToIndex(deviations,
	  				    numActions[state]);

	  	  GRBLinExpr rhs = 0; // deviation payoff
	  	  rhs += (1-delta)*payoffs[state][deviation][player];
	  	  for (int sp = 0; sp < numStates; sp++)
	  	    {
	  	      rhs += delta*prob[state][deviation][sp]*var[numPlayers*numStates
	  						       +numPlayers*sp+player];
	  	    }
		  
	  	  model.addConstr(lhs >= rhs);
	  	} // dev

	      deviations[player] = actions[player];
	    } // player

	  model.update();

	  for (int dir = 0; dir < numDirections; dir++)
	    {
	      GRBLinExpr obj = 0;
	      for (int sp = 0; sp < numStates; sp++)
		{
		  for (int player = 0; player < numPlayers; player++)
		    obj += directions[dir][player]*prob[state][action][sp]
		      *var[numPlayers*sp+player];
		  
		}
	      
	      model.setObjective(obj);
	      model.getEnv().set(GRB_IntParam_OutputFlag,0);
	      model.set(GRB_IntAttr_ModelSense,-1); // maximize
	      model.optimize();

	      if (model.get(GRB_IntAttr_Status)==GRB_OPTIMAL)
		{
		  double val = (1-delta)*payoffs[state][action]*directions[dir]
		    + delta * model.get(GRB_DoubleAttr_ObjVal);

		  if (val > newLevels[state][dir])
		    newLevels[state][dir] = val;
		}
	      else
		break;
	    } // direction

	  // Remove IC constraints.
	  constr = model.getConstrs();
	  for (int constrCtr = numPlayers*numStates*numDirections; 
	       constrCtr < model.get(GRB_IntAttr_NumConstrs);
	       constrCtr++)
	    model.remove(constr[constrCtr]);

	  model.update();
	  // delete[] constr;
	} // action
    } // state

  delete[] var;

  double dist = 0.0;
  for (int state = 0; state < numStates; state++)
    {
      for (int dir = 0; dir < numDirections; dir++)
	dist = std::max(dist,abs(levels[state][dir]-newLevels[state][dir]));
    } // state
  levels = newLevels;

  return dist;
} // iterate

#endif 
