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

//! Wiseman's model of monopoly with predation
//! @example
#include "sgpredation.hpp"
#include "sgsimulator.hpp"
#include "sgsolver_v4.hpp"
#include <ctime>

int main()
{
  double delta = 0.98;
  int numFirmStates = 3;
  int numQuantities = 30;

  {
    double persistence = 0;
    SGPredationGame pg(delta,numFirmStates,numQuantities);
    SGEnv env;
    env.setParam(SG::STOREITERATIONS,1);
    // env.setParam(SG::MAXITERATIONS,1e6);
    // env.setParam(SG::MAXPOLICYITERATIONS,1e2);
    // env.setParam(SG::STOREACTIONS,true);

    // env.setParam(SG::ICTOL,1e-9);
    // env.setParam(SG::INTERSECTTOL,true);
    
    env.setParam(SG::ERRORTOL,1e-8);
    SGGame game(pg);

    clock_t start;
    double duration;

    
    start = clock();
    SGSolver_V4 solver(env,game);
    solver.solve_endogenous();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << "Endogenous direction time elapsed: "<< duration << " seconds" << endl;

    SGSolution_V2 soln = solver.getSolution();
    stringstream ss;
    ss << "./solutions/predationgame_numq=" << numQuantities
       << "_numF=" << numFirmStates
       << "_delta=" << delta
       << ".sln2";
    SGSolution_V2::save(soln,ss.str().c_str());
    
  }

  return 0;
}
