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

//! Kocherlakota style risk sharing model
//! @example
#include "sgrisksharing.hpp"
#include "sgsolver_PencilSharpening.hpp"
#include "sgsolver_jyc.hpp"
#include "sgsolver_maxminmax_grb.hpp"
#include "sgsolver_maxminmax.hpp"
#include <ctime>

int main()
{
  double delta = 0.7;
  int numEndowments = 2;
  int c2e = 200;
  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

  {
    double persistence = 0;
    RiskSharingGame rsg(delta,numEndowments,
			c2e,persistence,endowmentMode);
    SGEnv env;
    env.setParam(SG::STOREITERATIONS,2);
    // env.setParam(SG::STOREACTIONS,true);
    env.setParam(SG::ERRORTOL,1e-5);

    env.setParam(SG::SUBGENFACTOR,0.0);
    if (env.getParam(SG::SUBGENFACTOR)>0)
      env.setParam(SG::ERRORTOL,
		   env.getParam(SG::SUBGENFACTOR)/10);
    SGGame game(rsg);

    clock_t start;
    double duration;

    // start = clock();
    // SGSolver solver1(env,game);
    // solver1.solve();
    // duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout << "Twist time elapsed: "<< duration << " seconds" << endl;
    // // SGSolution soln = solver1.getSolution();
    // // SGSolution::save(soln,"risksharing_v1.sln");

    start = clock();

    SGSolver_MaxMinMax solver4(env,game);
    solver4.solve_fixed();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << "Fixed direction time elapsed: "<< duration << " seconds" << endl;

    SGSolution_MaxMinMax soln2 = solver4.getSolution();
    SGSolution_MaxMinMax::save(soln2,"./solutions/risksharing_maxminmax_fixed.sln2");

    start = clock();

    SGSolver_MaxMinMax solver5(env,game);
    solver5.solve();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << "Endogenous direction time elapsed: "<< duration << " seconds" << endl;

    SGSolution_MaxMinMax soln3 = solver5.getSolution();
    stringstream ss;
    ss << "./solutions/risksharing_nume=" << numEndowments
       << "_c2e=" << c2e
       << "_delta=" << delta;
    if (env.getParam(SG::SUBGENFACTOR)>0)
      ss << "_sgf=" << env.getParam(SG::SUBGENFACTOR);
    ss << ".sln2";
    SGSolution_MaxMinMax::save(soln3,ss.str().c_str());
    
    start = clock();
    int numDirections = 100;
    SGSolver_JYC jycsolver(game,numDirections);
    jycsolver.solve();
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "JYC implementation time elapsed with " << numDirections
    	 << " directions: "<< duration << " seconds" << endl;

    start = clock();
    numDirections = 200;
    SGSolver_JYC jycsolver2(game,numDirections);
    jycsolver2.solve();
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "JYC implementation time elapsed with " << numDirections
    	 << " directions: "<< duration << " seconds" << endl;
  }

  return 0;
}
