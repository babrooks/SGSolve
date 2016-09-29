// Code for three player JYC style algorithm

#include "sgcommon.hpp"
#include "gurobi_c++.h"
#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include <random>
#include <chrono>

using namespace orgQhull;



int main ()
{
  bool printOn = false;
  bool saveData = false;
  // bool printOn = true;
  // bool saveData = true;
  
  int numPlayers = 3;
  int ItLimDefault = 2100000000;
  double minAngleIncrement(1e-4);
  
  // Convergence threshold
  double convTol = 1e-10;
  int maxIter = 1e2;
  
  // Game specification
  vector<int> numActions(numPlayers,3);
  int numActions_total = 1;
  for (int p = 0; p < numPlayers; p++)
    numActions_total *= numActions[p];

  // Discount factor
  double delta = 0.6;
  
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);
  uniform_real_distribution<coordT> distribution(0.0,10.0);
  //uniform_int_distribution<int> distribution(1,10);

  // Flow payoffs
  numActions_total = 50;
  vector< vector<double> > G(numActions_total,
			     vector<double>(numPlayers,0));
  vector< vector<double> > gains(numActions_total,
				 vector<double>(numPlayers,0));

  int iter = 0;
  int numExtPnts=0;
  bool notEnoughPoints = false;

  ofstream ofs;

  ofstream surveylog("threeplayer2.log");

  int maxExtPnt = 0;
  int avgExtPnt = 0;
  int numConverged = 0;
  int numTrials = 1e3;
  int numSetCollapsed = 0;
  int numAvailableActions;

  int k = 0;

  for (numActions_total = 10; numActions_total <= 50; numActions_total += 5)
    {
      for (k = 0; k < numTrials; k++)
	{
	  for (int a = 0; a < numActions_total; a++)
	    {
	      for (int p = 0; p < numPlayers; p++)
		{
		  G[a][p] = distribution(generator);
		  gains[a][p] = distribution(generator);
		}
	    }
  
  
	  // // Row of G is a1 + a2*n1 + a3*n2*n1;
	  // G[0] = {20, 20, 20}; // (0,0,0)
	  // G[1] = {30, 10, 10}; // (1,0,0);
	  // G[2] = {25, 5, 5}; // (2,0,0);
	  // G[3] = {10, 30, 10}; // (0,1,0);
	  // G[4] = {15, 15, 7}; // (1,1,0);
	  // G[5] = {12, 10, 5}; // (2,1,0);
	  // G[6] = {5, 25, 5}; // (0,2,0);
	  // G[7] = {10, 12, 5}; // (1,2,0);
	  // G[8] = {7, 7, 3}; // (2,2,0);
	  // G[9] = {10, 10, 30}; // (0,0,1);
	  // G[10] = {15, 7, 15}; // (1,0,1);
	  // G[11] = {12, 5, 10}; // (2,0,1);
	  // G[12] = {7, 15, 15}; // (0,1,1);
	  // G[13] = {12, 12, 12}; // (1,1,1);
	  // G[14] = {9, 8, 8}; // (2,1,1);
	  // G[15] = {5, 12, 10}; // (0,2,1);
	  // G[16] = {8, 9, 8}; // (1,2,1);
	  // G[17] = {7, 7, 6}; // (2,2,1);
	  // G[18] = {5, 5, 25}; // (0,0,2);
	  // G[19] = {10, 5, 12}; // (1,0,2);
	  // G[20] = {7, 3, 7}; // (2,0,2);
	  // G[21] = {5, 10, 12}; // (0,1,2);
	  // G[22] = {8, 8, 9}; // (1,1,2);
	  // G[23] = {7, 6, 7}; // (2,1,2);
	  // G[24] = {3, 7, 7}; // (0,2,2);
	  // G[25] = {6, 7, 7}; // (1,2,2);
	  // G[26] = {5, 5, 5}; // (2,2,2);
  
	  // // Compute gains from deviating
	  // if (printOn)
	  // 	cout << "Computing gains from deviating..." << endl;
	  // for (int a = 0; a < numActions_total; a++)
	  // 	{
	  // 	  int remainder = a;
	  // 	  int scale = 1;
	  // 	  for (int p = 0; p < numPlayers; p++)
	  // 	    {
	  // 	      int ownAction = remainder % numActions[p];
	  // 	      for (int dev = 0; dev < numActions[p]; dev++)
	  // 		{
	  // 		  if (dev == ownAction)
	  // 		    continue;
	  // 		  int devAction = a+(dev-ownAction)*scale;
	  // 		  assert(devAction >= 0);
	  // 		  assert(devAction < numActions_total);
	  // 		  double tmp = G[devAction][p] - G[a][p];
	  // 		  if (tmp > gains[a][p])
	  // 		    gains[a][p] = tmp;
	  // 		} // for deviation

	  // 	      scale *= numActions[p];
	  // 	      remainder -= ownAction;
	  // 	      remainder /= numActions[p];
	  // 	    } // for player
	  // 	} // for action


	  if (saveData)
	    {
	      stringstream ss;
	      ss << "threeplayer2_"
		 << setprecision(3) << delta << ".dat";
	      ofs.open(ss.str().c_str());

	      // Print numbers of actions
	      ofs << numActions_total << " "
		  << delta << " " << 1 <<  endl;

	      for (int p = 0; p < numPlayers; p++)
		ofs << numActions[p] << " ";
	      ofs << endl;

	      // Print payoffs and gains from deviating
	      for (int a = 0; a < numActions_total; a++)
		{
		  for (int p = 0; p < numPlayers; p++)
		    ofs << G[a][p] << " ";
		  ofs << endl;
		}
	      for (int a = 0; a < numActions_total; a++)
		{
		  for (int p = 0; p < numPlayers; p++)
		    ofs << gains[a][p] << " ";
		  ofs << endl;
		}
	    }

	  Qhull feasQhull;
	  RboxPoints feasRbox;
	  coordT c_payoffs[numPlayers];
	  vector<double> threats(numPlayers,numeric_limits<double>::max());
	  for (int a = 0; a < numActions_total; a++)
	    {
	      for (int p = 0; p < numPlayers; p++)
		{
		  c_payoffs[p] = G[a][p];
		  threats[p] = min(threats[p],G[a][p]);
		}
	      feasRbox.append(QhullPoint(numPlayers,c_payoffs));
	    }
	  feasQhull.runQhull(feasRbox,"");
	  QhullFacetList fl = feasQhull.facetList();
	  list<vector<double> > hyperplanes;
	  vector<double> hyperplane(numPlayers+1,0);
	  for (auto it = fl.begin(); it != fl.end(); ++it)
	    {
	      if (it->hyperplane().dimension() != 3)
		cout << "Bad dimension: " << it->hyperplane().dimension() << endl;
	      for (int p = 0; p < numPlayers; p++)
		hyperplane[p] = it->hyperplane().coordinates()[p];
	      hyperplane.back() = it->hyperplane().offset();
	      hyperplanes.push_back(hyperplane);
	    }

	  QhullVertexList vl = feasQhull.vertexList();
  
	  vector<bool> isAvailable(numActions_total,true);
	  numAvailableActions = numActions_total;

	  try
	    {
	      GRBEnv env;
	      env.set(GRB_IntParam_OutputFlag,0);

	      // Main loop
	      if (printOn)
		cout << "Starting main loop..." << endl;

	      double volumeChange = 1;
	      double volume = numeric_limits<double>::max();
	      std::chrono::time_point<std::chrono::system_clock> start, end;
	      start = std::chrono::system_clock::now();

	      bool keepIterating = true;
	      vector<double> newThreats;
	      iter = 0;
	      do
		{
		  // Iterate one round past convergence
		  keepIterating = (volumeChange > convTol);

		  newThreats = vector<double>(numPlayers,numeric_limits<double>::max());
		  if (iter>maxIter)
		    keepIterating = false;
	  
		  // Print status update
		  end = std::chrono::system_clock::now();
		  std::chrono::duration<double> elapsed_seconds = end-start;
	  
		  GRBModel model(env);
		  GRBVar * vars = model.addVars(3);
		  model.update();
	  
		  for (auto it = hyperplanes.begin();
		       it != hyperplanes.end();
		       ++it)
		    {
		      GRBLinExpr lhs = 0;
		      for (int p = 0; p < numPlayers; p ++)
			lhs += vars[p] * (*it)[p];

		      model.addConstr(lhs <= -(*it).back());
		    }
	  
		  vector<GRBConstr> constrs(0);
		  for (int p = 0; p < numPlayers; p++)
		    constrs.push_back(model.addConstr(1.0*vars[p]>=0));
		  model.update();
	  
		  RboxPoints rbox;
		  Qhull qhull;
		  for (int a = 0; a < numActions_total; a++)
		    {
		      if (!isAvailable[a])
			continue;

		      vector<double> minCV(numPlayers,0);
		      bool gIsIC = true;
		      for (int p = 0; p < numPlayers; p++)
			{
			  minCV[p] = threats[p] + (1.0-delta)/delta*gains[a][p];
			  gIsIC = gIsIC && (G[a][p]>=minCV[p]);
			  constrs[p].set(GRB_DoubleAttr_RHS,minCV[p]);
			}

		      if (gIsIC)
			{
			  for (int p = 0; p < numPlayers; p++)
			    {
			      c_payoffs[p] = G[a][p];
			      newThreats[p] = min(newThreats[p],c_payoffs[p]);
			    }

			  rbox.append(QhullPoint(numPlayers,c_payoffs));
			  continue;
			}
	      
		      bool thisActionFeasible = false;

		      // Iterate over binding constraints
		      for (int p = 0; p < numPlayers; p++)
			{
			  // Fix binding constraint
			  constrs[p].set(GRB_CharAttr_Sense,'=');

			  c_payoffs[p] = (1-delta)*G[a][p] + delta*minCV[p];

			  // Now map out the convex hull
			  double w0 = 1.0, w1 = 0.0;
			  vector<double> rc0(numPlayers), rc1(numPlayers);
			  int pp1 = (p+1)%numPlayers, pp2 = (p+2)%numPlayers;
			  model.setObjective(w0 * vars[pp1]+ w1 * vars[pp2],
					     GRB_MAXIMIZE);
			  model.optimize();
			  if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL)
			    continue;

			  thisActionFeasible = true;
			  newThreats[p] = min(newThreats[p],c_payoffs[p]);

			  bool passedWest = false;
		  
			  int bndryMapIter = 0;
			  while (bndryMapIter < 1e6)
			    {

			      // Print progress.
			      // if (!(bndryMapIter%10))
			      // 	cout << setprecision(3) 
			      // 	     << "(w0,w1) = (" <<  w0 << "," << w1
			      // 	     << "), passedWest = " << passedWest
			      // 	     << ", bndryMapIter = " << bndryMapIter << endl;
			      c_payoffs[pp1] = (1-delta)*G[a][pp1]
				+ delta*vars[pp1].get(GRB_DoubleAttr_X);
			      c_payoffs[pp2] = (1-delta)*G[a][pp2]
				+ delta*vars[pp2].get(GRB_DoubleAttr_X);

			      newThreats[pp1] = min(newThreats[pp1],c_payoffs[pp1]);
			      newThreats[pp2] = min(newThreats[pp2],c_payoffs[pp2]);
		      
			      rbox.append(QhullPoint(numPlayers,c_payoffs));

			      // Check if we have passed west or gone all the way around.
			      if (!passedWest && w1 < 0)
				passedWest = true;
			      if (passedWest && w1 > 0)
				break;
		      
			      model.getEnv().set(GRB_IntParam_Method,0);
			      model.getEnv().set(GRB_DoubleParam_IterationLimit, 0);
		      
			      model.setObjective(1.0*vars[pp1],GRB_MAXIMIZE);
			      model.optimize();
			      for (int vc = 0; vc < numPlayers; vc++)
				rc0[vc] = vars[vc].get(GRB_DoubleAttr_RC);
		      
			      model.setObjective(1.0*vars[pp2],GRB_MAXIMIZE);
			      model.optimize();
			      for (int vc = 0; vc < numPlayers; vc++)
				rc1[vc] = vars[vc].get(GRB_DoubleAttr_RC);

			      double nw0 = w0 * cos(PI/4.0) - w1 * sin(PI/4.0),
				nw1 = w0* sin(PI/4.0) + w1 * cos(PI/4.0);
			      double t0, t1;
			      for (int vc = 0; vc < numPlayers; vc++)
				{
				  // Only look at variables with negative reduced costs under
				  // the current weights
				  if (w0*rc0[vc]+w1*rc1[vc]<0)
				    {
				      // Find unit clockwise normal from the reduced
				      // cost. This is the shallowest direction such that we
				      // would want to introduce this variable into the basis.
				      double d = sqrt(rc0[vc]*rc0[vc] + rc1[vc]*rc1[vc]);
				      if (d<1e-12)
					continue;
				      double t0 = rc1[vc]/d,
					t1 = -rc0[vc]/d;
	      
				      // If this direction is shallower than the current new
				      // direction, make it the new direction.
				      if (t0*nw1 - t1*nw0>0)
					{
					  nw0 = t0;
					  nw1 = t1;
					} // better direction
				    } // rc<0
				} // for vc
			      double md0 = w0 * cos(minAngleIncrement) - w1 * sin(minAngleIncrement),
				md1 = w0* sin(minAngleIncrement) + w1 * cos(minAngleIncrement);
			      if (nw0*md1 - nw1*md0>0)
				{
				  w0 = md0; 
				  w1 = md1;
				}
			      else
				{
				  w0 = nw0; 
				  w1 = nw1;
				}

			      model.getEnv().set(GRB_DoubleParam_IterationLimit,
						 ItLimDefault);
			      model.optimize();

			      bndryMapIter++;
			    } // Search for best directions
		    
		  
			  // Reset contraint
			  constrs[p].set(GRB_CharAttr_Sense,'>');
			} // for player
		      if (!thisActionFeasible)
			{
			  isAvailable[a]=false;
			  numAvailableActions --;
			}
		    } // for actions

		  delete[] vars;
	  

		  if (!numAvailableActions)
		    {
		      if (printOn)
			cout << "No actions available. Exiting..." << endl;
		      keepIterating = false;
		      numSetCollapsed ++;
		    }
		  else
		    {
		      qhull.runQhull(rbox,"");
		      fl = qhull.facetList();
		      hyperplanes.clear();
		      for (auto it = fl.begin();
			   it != fl.end();
			   ++it)
			{
			  if (it->hyperplane().dimension() != 3)
			    cout << "Bad dimension: " << it->hyperplane().dimension() << endl;
			  for (int p = 0; p < numPlayers; p++)
			    hyperplane[p] = it->hyperplane().coordinates()[p];
			  hyperplane.back() = it->hyperplane().offset();
			  hyperplanes.push_back(hyperplane);
			}

		      // Calculate change in volume
		      double newVolume = qhull.volume();
		      volumeChange = abs(volume-newVolume);
		      // assert(volume >= newVolume);
		      volume = newVolume;

		      for (int p = 0; p < numPlayers; p ++)
			threats[p] = max(newThreats[p],threats[p]);

		    }

		  if (printOn)
		    cout << "Iter: " << iter
			 << ", dist: " << setprecision(3) << volumeChange
			 << ", ext pnts: " << qhull.vertexList().count()
			 << ", b4 convhull: " << rbox.count() 
			 << ", time (s): " << elapsed_seconds.count()
			 << ", actions: " << numAvailableActions << endl;

		  if (!keepIterating)
		    {
		      notEnoughPoints = false;
		      vl = qhull.vertexList();
		      // Print the optimal payoffs
		      for (auto it = vl.begin();
			   it != vl.end();
			   ++it)
			{
			  if (saveData)
			    {
			      for (int p = 0; p < numPlayers; p++)
				ofs << it->point().coordinates()[p] << " ";
			      ofs << endl;
			    }
			}
		      numExtPnts = qhull.vertexList().count();
		    }	      
	      
		  iter ++;
		} while (keepIterating);


	      // Clean up
	      if (saveData)
		ofs.close();
	    }
	  catch (GRBException & e)
	    {
	      cout << "GRBException caught: " << e.getMessage() << endl;
	    }
	  catch (QhullError & e)
	    {
	      if (e.errorCode() == 6214) // Not enough points
		{
		  // cout << "Not enough points for convex hull." << endl;
		  notEnoughPoints = true;
		}
	    }
	  catch (...)
	    {
	      cout << "Unknown exception caught." << endl;
	    }


	  maxExtPnt = max(maxExtPnt,numExtPnts);
	  if (numAvailableActions>0)
	    {
	      avgExtPnt += numExtPnts;
	      numConverged += (iter <= maxIter);
	    }
	  
	  // cout << "trial: " << k
	  //      << ", converged: " << ((iter<=maxIter)&&(numAvailableActions>0)&&(!notEnoughPoints))
	  //      << ", actions remaining: " << numAvailableActions
	  //      << ", num extreme points: " << numExtPnts
	  //      << ", max num extreme points: " << maxExtPnt
	  //      << endl;

	} // for random game

      surveylog << "Total number of trials: " << numTrials << endl
		<< "Discount factor: " << delta << endl
		<< "Total number of action profiles: " << numActions_total << endl
		<< "Number of trials that converged: " << numConverged << endl
		<< "Percent converged: " << static_cast<double>(numConverged)/numTrials << endl
		<< "Average number of extreme points: " << static_cast<double>(avgExtPnt)/numTrials << endl
		<< "Max number of extreme points: " << maxExtPnt << endl;

    } // for numActions_total

  surveylog.close();

  return 0;
}
