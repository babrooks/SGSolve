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

//! Kocherlakota style risk sharing model, where we merge two of the
//! players from the three player example.
//! @example
#include "sgrisksharing_3player_merged.hpp"
#include "sgsolver.hpp"
#include "sgsolver_maxminmax.hpp"
#include <ctime>

int main()
{
  double delta = 0.6;
  int numEndowments = 3;
  int c2e = 3;

  {
    double persistence = 0;
    RiskSharingGame_3Player_Merged rsg(delta,numEndowments,c2e);
    SGEnv env;
    // env.setParam(SG::STOREITERATIONS,1);
    env.setParam(SG::MAXITERATIONS,1e6);
    // env.setParam(SG::MAXPOLICYITERATIONS,1e2);
    // env.setParam(SG::STOREACTIONS,true);
    // env.setParam(SG::ERRORTOL,1e-6);
    SGGame game(rsg);

    clock_t start;
    double duration;

    start = clock();

    SGSolver_MaxMinMax solver(env,game);
    solver.solve();

    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << "Endogenous direction time elapsed: "<< duration << " seconds" << endl;
    
    SGSolution_MaxMinMax soln = solver.getSolution();
    SGSolution_MaxMinMax::save(soln,"./solutions/risksharing_3player_merged.sln2");
    
    SGGame::save(game,"./games/risksharing_3player_merged.sgm");  
  }

  return 0;
}
