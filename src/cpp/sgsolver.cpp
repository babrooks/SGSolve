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
