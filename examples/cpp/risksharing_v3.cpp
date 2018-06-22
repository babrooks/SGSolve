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

int main()
{
  double delta = 0.6;
  int numEndowments = 5;
  int c2e = 3;
  int numSims = 1e3;
  int numSimIters = 1e5;
  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

  {
    double persistence = 0;
    RiskSharingGame rsg(delta,numEndowments,
			c2e,persistence,endowmentMode);
    SGEnv env;
    env.setParam(SG::STOREITERATIONS,1);
    env.setParam(SG::STOREACTIONS,false);
    SGGame game(rsg);
    SGSolver_V3 solver(game);
    solver.solve();

  }

  return 0;
}
