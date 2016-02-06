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

#include "sgsolver.hpp"

SGSolver::SGSolver(const SGEnv & _env,
		   const SGGame & _game):
  env(_env),
  game(_game),
  soln(_game)
{}

void SGSolver::solve()
{

  SGApprox approx (env,game,soln);

  approx.initialize();

  bool storeIterations = false;
  if (env.getParam(SG::STOREITERATIONS) == 2)
    storeIterations = true;
  
  while (approx.generate(storeIterations) > env.getParam(SG::ERRORTOL)
	 && approx.getNumIterations() < env.getParam(SG::MAXITERATIONS))
    {};

  if (env.getParam(SG::STOREITERATIONS) == 1)
    {
      int lastRev = approx.getNumRevolutions();
      while (approx.getNumRevolutions() == lastRev
	     && approx.getNumIterations() < env.getParam(SG::MAXITERATIONS))
	{
	  approx.generate(true);
	}
    }

  // Add the extreme tuples array to soln.
  for (vector<SGTuple>::const_iterator tuple
	 = approx.getExtremeTuples().begin();
       tuple != approx.getExtremeTuples().end();
       ++tuple)
    soln.push_back(*tuple);

  approx.end();

} // solve
