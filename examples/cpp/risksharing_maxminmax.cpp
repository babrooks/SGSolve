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
#include "sgsolver_pencilsharpening.hpp"
#include "sgsolver_jyc.hpp"
#include "sgsolver_maxminmax_grb.hpp"
#include "sgsolver_maxminmax.hpp"
#include <ctime>

void runModels(const int numEndowments,
	       const int c2e,
	       const delta,
	       bool maxminmax,
	       bool maxminmaxfixed,
	       int jycnumdirs);

int main()
{
  double delta = 0.7;

  // Run the benchmarks reported in ABS (2019)
  runModels(2,200,0.4,false,true,0);
  runModels(2,200,0.7,false,true,0);
  
  // Max-min-max fixed and endogenous
  runModels(2,20,0.7,false,true,0);
  runModels(2,40,0.7,false,true,0);
  runModels(5,20,0.7,false,true,0);
  runModels(9,15,0.7,false,true,0);

  return 0;
  
  // JYC 100
  runModels(2,20,0.7,false,false,100);
  runModels(2,40,0.7,false,false,100);
  runModels(5,20,0.7,false,false,100);
  runModels(9,15,0.7,false,false,100);

  // JYC 200
  runModels(2,20,0.7,false,false,200);
  runModels(2,40,0.7,false,false,200);
  runModels(5,20,0.7,false,false,200);
  runModels(9,15,0.7,false,false,200);
  
  return 0;
} // main

void runModels(const int numEndowments,
	       const int c2e,
	       const delta,
	       bool maxminmax,
	       bool maxminmaxfixed,
	       int jycnumdirs)
{
  double persistence = 0;
  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

  RiskSharingGame rsg(delta,numEndowments,
		      c2e,persistence,endowmentMode);
  SGEnv env;
  env.setParam(SG::STOREITERATIONS,2);
  env.setParam(SG::ERRORTOL,1e-5);

  env.setParam(SG::SUBGENFACTOR,0.0);
  if (env.getParam(SG::SUBGENFACTOR)>0)
    env.setParam(SG::ERRORTOL,
		 env.getParam(SG::SUBGENFACTOR)/10);
  // env.setParam(SG::MAXITERATIONS,20);
  SGGame game(rsg);

  clock_t start;
  double duration;

  if (maxminmaxfixed)
    {

      start = clock();

      SGSolver_MaxMinMax solver4(env,game);
      try
	{
	  solver4.solve_fixed();
	}
      catch(std::exception & e)
	{
	  cout << e.what() << endl;
	}

      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << fixed << "Fixed direction time elapsed: "<< duration << " seconds" << endl;

      SGSolution_MaxMinMax soln2 = solver4.getSolution();
      SGSolution_MaxMinMax::save(soln2,"./solutions/risksharing_maxminmax_fixed.sln");
    }
  if (maxminmax)
    {
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
      ss << ".sln";
      SGSolution_MaxMinMax::save(soln3,ss.str().c_str());
    }
  if (jycnumdirs>0)
    {
      start = clock();
      SGSolver_JYC jycsolver(game,jycnumdirs);
      jycsolver.solve();
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      cout << "JYC implementation time elapsed with " << numDirections
	   << " directions: "<< duration << " seconds" << endl;
    }
} // runModels
