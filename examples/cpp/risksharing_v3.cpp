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

//! Kocherlakota style risk sharing model
//! @example
#include "risksharing.hpp"
#include "sgsimulator.hpp"
#include "sgsolver_v3.hpp"
#include "sgsolver_v4.hpp"
#include <ctime>

int main()
{
  double delta = 0.6;
  int numEndowments = 9;
  int c2e = 5;
  int numSims = 1e3;
  int numSimIters = 1e5;
  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

  {
    double persistence = 0;
    RiskSharingGame rsg(delta,numEndowments,
			c2e,persistence,endowmentMode);
    SGEnv env;
    env.setParam(SG::STOREITERATIONS,2);
    env.setParam(SG::MAXITERATIONS,1e6);
    env.setParam(SG::MAXPOLICYITERATIONS,1e2);
    env.setParam(SG::STOREACTIONS,true);
    env.setParam(SG::ERRORTOL,1e-6);
    SGGame game(rsg);

    clock_t start;
    double duration;

    
    start = clock();
    SGSolver solver1(env,game);
    solver1.solve();
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Twist time elapsed: "<< duration << " seconds" << endl;
    // SGSolution soln = solver1.getSolution();
    // SGSolution::save(soln,"risksharing_v1.sln");
    

    // start = clock();
    // SGSolver_V3 solver3(game);
    // solver3.solve();
    // duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout << "Gurobi implementation time elapsed: "<< duration << " seconds" << endl;



    // start = clock();

    // SGSolver_V4 solver4(env,game);
    // solver4.solve();

    // duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout << fixed << "Fixed direction time elapsed: "<< duration << " seconds" << endl;

    // SGSolution_V2 soln2 = solver4.getSolution();
    // SGSolution_V2::save(soln2,"risksharing_v3.sln2");

    
    start = clock();

    SGSolver_V4 solver5(env,game);
    solver5.solve_endogenous();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << "Endogenous direction time elapsed: "<< duration << " seconds" << endl;

    SGSolution_V2 soln3 = solver5.getSolution();
    SGSolution_V2::save(soln3,"risksharing_v3_endogenous.sln2");
    
    
    
  }

  return 0;
}
