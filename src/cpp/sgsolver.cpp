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

  while (approx.generate() > env.errorTol
	 && approx.numIterations < env.maxIterations)
    {};

  // Add the extreme tuples array to soln.
  for (vector<SGTuple>::const_iterator tuple
	 = approx.extremeTuples.begin();
       tuple != approx.extremeTuples.end();
       ++tuple)
    soln.push_back(*tuple);

  approx.end();

} // solve
