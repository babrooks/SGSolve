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
  if (env.getParam(SGEnv::STOREITERATIONS) == 2)
    storeIterations = true;
  
  while (approx.generate(storeIterations) > env.getParam(SGEnv::ERRORTOL)
	 && approx.numIterations < env.getParam(SGEnv::MAXITERATIONS))
    {};

  if (env.getParam(SGEnv::STOREITERATIONS) == 1)
    {
      int lastRev = approx.numRevolutions;
      while (approx.numRevolutions == lastRev
	     && approx.numIterations < env.getParam(SGEnv::MAXITERATIONS))
	{
	  approx.generate(true);
	}
    }

  // Add the extreme tuples array to soln.
  for (vector<SGTuple>::const_iterator tuple
	 = approx.extremeTuples.begin();
       tuple != approx.extremeTuples.end();
       ++tuple)
    soln.push_back(*tuple);

  approx.end();

} // solve
