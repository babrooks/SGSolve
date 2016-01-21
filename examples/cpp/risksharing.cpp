#include "risksharing.hpp"
#include "sgsimulator.hpp"

int main()
{
  double delta = 0.85;
  int numEndowments = 9;
  int c2e = 25;
  int numSims = 1e3;
  int numSimIters = 1e5;
  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Endowment;

  stringstream nashname, lrpname, prename, name, gamename, solnname;
  prename << "_ne="
	  << numEndowments
	  << "_c2e="
	  << c2e
	  << "_d="
	  << setprecision(3) << delta;
  switch (endowmentMode)
    {
    case RiskSharingGame::Consumption:
      prename << "_cmode=C";
      break;
    case RiskSharingGame::Endowment:
      prename << "_cmode=E";
      break;
    }

  
  lrpname << "rsg_lrp" << prename.str() << ".log";
  nashname << "rsg_nash" << prename.str() << ".log";
  ofstream ofs_lrp(lrpname.str().c_str(),std::ofstream::out);
  ofstream ofs_nash(nashname.str().c_str(),std::ofstream::out);

  SGSolver * solver;
  
  for (double persistence = 0.0; persistence < 28.0; persistence += 0.25)
    {
      try
	{
	  cout << "Starting computation with p="
	       << setprecision(3) << persistence << "." << endl;
	  
	  RiskSharingGame rsg(delta,numEndowments,
			      c2e,persistence,endowmentMode);
	  SGGame game(rsg);

	  name.str(""); gamename.str(""); solnname.str("");
	  name << "_ne="
	       << numEndowments
	       << "_c2e="
	       << c2e
	       << "_d="
	       << setprecision(3) << delta
	       << "_p="
	       << setprecision(3) << persistence;
	  switch (endowmentMode)
	    {
	    case RiskSharingGame::Consumption:
	      name << "_cmode=C";
	      break;
	    case RiskSharingGame::Endowment:
	      name << "_cmode=E";
	      break;
	    }

	  gamename << "./games/rsg"
		   << name.str()
		   << ".sgm";
	  solnname << "./solutions/rsg"
		   << name.str()
		   << ".sln";

	  SGSolution soln;
	  try
	    {
	      cout << "Trying to load: " << solnname.str() << endl;
		
	      SGSolution::load(soln,solnname.str().c_str());
	    }
	  catch (std::exception & ep)
	    {
	      cout << "Caught the following exception:" << endl
		   << ep.what() << endl;
	      SGGame::save(game,gamename.str().c_str());

	      SGEnv env;
	      env.setParam(SG::STOREITERATIONS,1);
	      env.setParam(SG::STOREACTIONS,false);
	      solver = new SGSolver(env,game);

	      solver->solve();
	      soln = solver->getSolution();
	      SGSolution::save(soln,solnname.str().c_str());
	      delete solver;
	    }

	  // Locate the socially efficient and inefficient equilibria
	  int midPoint = (numEndowments-1)/2;
	  SGPoint northWest(1.0,1.0);
	  double bestLevel = -numeric_limits<double>::max(),
	    worstLevel = numeric_limits<double>::max();
	  list<SGIteration>::const_iterator bestIter, worstIter;
	  for (list<SGIteration>::const_iterator iter
		 = soln.getIterations().begin();
	       iter != soln.getIterations().end();
	       ++iter)
	    {
	      double tempLevel = northWest*(iter->getPivot()[midPoint]);
	      if (tempLevel < worstLevel)
		{
		  worstLevel = tempLevel;
		  worstIter = iter;
		}
	      if (tempLevel > bestLevel)
		{
		  bestLevel = tempLevel;
		  bestIter = iter;
		}
	    } // for iter

	  ofs_lrp << persistence << " ";

	  SGSimulator sim(soln);
	  sim.initialize();

	  sim.simulate(numSims,numSimIters,midPoint,bestIter->getIteration());
	  SGPoint lrp = sim.getLongRunPayoffs();
	  ofs_lrp << setprecision(6) << lrp[0] << " " << lrp[1] << " ";
	  cout << "Best long run payoffs: ("
	       << setprecision(6) << lrp[0] << ","
	       << lrp[1] << ")" << endl;
	    
	  sim.simulate(numSims,numSimIters,midPoint,worstIter->getIteration());
	  lrp = sim.getLongRunPayoffs();
	  ofs_lrp << setprecision(6) << lrp[0] << " " << lrp[1] << endl;
	  cout << "Autarky payoffs: ("
	       << setprecision(6) << lrp[0] << ","
	       << lrp[1] << ")" << endl;

	  // Calculate nash payoffs, where threat point is autarky
	  const SGTuple & threatTuple = soln.getIterations().back().getThreatTuple();
	  for (int e = 0; e < numEndowments; e++)
	    {
	      double nashObj = -numeric_limits<double>::max();
	      SGPoint nashPayoffs;
	      for (list<SGIteration>::const_iterator iter
		     = soln.getIterations().begin();
		   iter != soln.getIterations().end();
		   ++iter)
		{
		  double tempObj = ( iter->getPivot()[e][0] - threatTuple[e][0] )
		    * ( iter->getPivot()[e][1] - threatTuple[e][1] );
		  if (tempObj > nashObj)
		    {
		      nashObj = tempObj;
		      nashPayoffs = iter->getPivot()[e];
		    }
		} // for iter
	      ofs_nash << nashPayoffs[0] << " " << nashPayoffs[1] << " ";
	    } // for e
	  ofs_nash << endl;
	  
	}
      catch (SGException & e)
	{
	  if (e.getType() == SG::NO_ADMISSIBLE_DIRECTION)
	    continue;
	  else
	    throw;
	}
      catch (std::exception & e)
	{
	  SGSolution soln = solver->getSolution();
	  SGSolution::save(soln,solnname.str().c_str());
	  delete solver;
	  cout << "SGException caught. Received following error: "
	       << endl
	       << e.what()
	       << endl;
	  break;
	}
    } // for persistence
  ofs_lrp.close();
  ofs_nash.close();
  

  return 0;
}
