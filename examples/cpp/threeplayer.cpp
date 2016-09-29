// Code for three player JYC style algorithm

#include "sgcommon.hpp"
#include "gurobi_c++.h"

int main ()
{
  int numPlayers = 3;
  int approxNumGradients = 2000;
  
  // Convergence threshold
  double convTol = 1e-6;
  
  // Game specification
  vector<int> numActions(numPlayers,3);
  int numActions_total = 1;
  for (int p = 0; p < numPlayers; p++)
    numActions_total *= numActions[p];

  // Discount factor
  double delta = 0.3;
  
  // Flow payoffs
  vector< vector<double> > G(numActions_total,
			     vector<double>(numPlayers,0));
  // Row of G is a1 + a2*n1 + a3*n2*n1;
  G[0] = {20, 20, 20}; // (0,0,0)
  G[1] = {30, 10, 10}; // (1,0,0);
  G[2] = {25, 5, 5}; // (2,0,0);
  G[3] = {10, 30, 10}; // (0,1,0);
  G[4] = {15, 15, 7}; // (1,1,0);
  G[5] = {12, 10, 5}; // (2,1,0);
  G[6] = {5, 25, 5}; // (0,2,0);
  G[7] = {10, 12, 5}; // (1,2,0);
  G[8] = {7, 7, 3}; // (2,2,0);
  G[9] = {10, 10, 30}; // (0,0,1);
  G[10] = {15, 7, 15}; // (1,0,1);
  G[11] = {12, 5, 10}; // (2,0,1);
  G[12] = {7, 15, 15}; // (0,1,1);
  G[13] = {12, 12, 12}; // (1,1,1);
  G[14] = {9, 8, 8}; // (2,1,1);
  G[15] = {5, 12, 10}; // (0,2,1);
  G[16] = {8, 9, 8}; // (1,2,1);
  G[17] = {7, 7, 6}; // (2,2,1);
  G[18] = {5, 5, 25}; // (0,0,2);
  G[19] = {10, 5, 12}; // (1,0,2);
  G[20] = {7, 3, 7}; // (2,0,2);
  G[21] = {5, 10, 12}; // (0,1,2);
  G[22] = {8, 8, 9}; // (1,1,2);
  G[23] = {7, 6, 7}; // (2,1,2);
  G[24] = {3, 7, 7}; // (0,2,2);
  G[25] = {6, 7, 7}; // (1,2,2);
  G[26] = {5, 5, 5}; // (2,2,2);
  
  // Compute gains from deviating
  cout << "Computing gains from deviating..." << endl;
  vector< vector<double> > gains(numActions_total,
				 vector<double>(numPlayers,0));
  for (int a = 0; a < numActions_total; a++)
    {
      int remainder = a;
      int scale = 1;
      for (int p = 0; p < numPlayers; p++)
	{
	  int ownAction = remainder % numActions[p];
	  for (int dev = 0; dev < numActions[p]; dev++)
	    {
	      if (dev == ownAction)
		continue;
	      int devAction = a+(dev-ownAction)*scale;
	      assert(devAction >= 0);
	      assert(devAction < numActions_total);
	      double tmp = G[devAction][p] - G[a][p];
	      if (tmp > gains[a][p])
		gains[a][p] = tmp;
	    } // for deviation

	  scale *= numActions[p];
	  remainder -= ownAction;
	  remainder /= numActions[p];
	} // for player
    } // for action

  cout << "Setting up gradients... ";
  // Set up the list of gradients
  double dist = sqrt(4*PI/approxNumGradients);
  list< vector<double> > gradients;
  
  int numLevels = PI/dist;
  // Add in threat directions
  for (int p = 0; p < numPlayers; p++)
    {
      gradients.push_back(vector<double>(numPlayers,0));
      gradients.back()[p] = -1;
    }
  for (double phi=0; phi<PI; phi+=PI/(numLevels-1.0))
    {
      int numPointsInLevel = 2*PI*sin(phi)/dist;
      for (double psi = 0; psi < 2*PI; psi+=2*PI/numPointsInLevel)
	{
	  gradients.push_back(vector<double>(3));
	  gradients.back()[0] = sin(phi)*cos(psi);
	  gradients.back()[1] = sin(phi)*sin(psi);
	  gradients.back()[2] = cos(phi);
	} // psi
    } // phi


  int numGradients = gradients.size();
  cout << numGradients << " gradients selected." << endl;

  stringstream ss;
  ss << "threeplayer_" << numGradients << "_"
     << setprecision(3) << delta << ".dat";
  ofstream ofs (ss.str().c_str());

  // Print numbers of actions
  ofs << numActions_total << " "
      << numGradients << " "
      << delta << endl;

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
  
  // Print  gradients
  for (list<vector<double> >::const_iterator it
	 = gradients.begin();
       it != gradients.end();
       ++it)
    {
      for (int p = 0; p < numPlayers; p++)
	ofs << (*it)[p] << " ";
      ofs << endl;
    }

  cout << "Computing feasible set..." << endl;
  // Compute feasible set
  vector<double> feasibleRHS(numGradients,
			     -std::numeric_limits<double>::max());
  {
    int g = 0;
    for (list<vector<double> >::const_iterator it = gradients.begin();
	 it != gradients.end();
	 ++it)
      {
	for (int a = 0; a < numActions_total; a++)
	  {
	    double tmp = 0;
	    for (int p = 0; p < numPlayers; p ++)
	      tmp += (*it)[p] * G[a][p];
	    if (tmp > feasibleRHS[g])
	      feasibleRHS[g] = tmp;
	  } // action
	g ++;
      } // gradient
  }

  vector<bool> isAvailable(numActions_total,true);
  int numAvailableActions = numActions_total;
  
  try
    {
      GRBEnv env;
      env.set(GRB_IntParam_OutputFlag,0);
      vector<GRBModel*> models(numActions_total);
      vector<GRBVar*> vars(numActions_total);
      vector<GRBConstr *> constrs(numActions_total);
      GRBModel feasModel(env);
      GRBVar * feasVars = feasModel.addVars(3);
      feasModel.update();

      cout << "Setting up feasible model..." << endl;
      {
	int g = 0;
	for (list<vector<double> >::const_iterator it = gradients.begin();
	     it != gradients.end();
	     it++)
	  {
	    GRBLinExpr lhs = 0;
	    for (int p = 0; p < numPlayers; p ++)
	      lhs += feasVars[p] * (*it)[p];
	    feasModel.addConstr(lhs <= feasibleRHS[g]);
	    
	    g++;
	  } // gradient
      }
      feasModel.update();
      GRBConstr * feasConstrs = feasModel.getConstrs();

      cout << "Setting up models..." << endl;
      // Set up the models with feasibility and IC constraints
      for (int a = 0; a < numActions_total; a++)
	{
	  models[a] = new GRBModel(env);
	  vars[a] = models[a]->addVars(3);
	  models[a]->update();
	  int g = 0;
	  for (list<vector<double> >::const_iterator it = gradients.begin();
	       it != gradients.end();
	       it++)
	    {
	      GRBLinExpr lhs = 0;
	      for (int p = 0; p < numPlayers; p ++)
		lhs += vars[a][p] * (*it)[p];
	      models[a]->addConstr(lhs <= feasibleRHS[g]);
	      
	      g++;
	    } // gradient

	  for (int p = 0; p < numPlayers; p++)
	    models[a]->addConstr(vars[a][p] >= 0);

	  models[a]->update();
	  constrs[a] = models[a]->getConstrs();
	} // action

      // Main loop
      cout << "Starting main loop..." << endl;
      vector<double> rhs = feasibleRHS;
      double rhsDistance = 1;
      vector<double> threats(numPlayers);
      int iter = 0;
      std::chrono::time_point<std::chrono::system_clock> start, end;
      start = std::chrono::system_clock::now();

      bool keepIterating = true;
      vector<vector<double> > optPayoffs(numGradients,vector<double>(numPlayers,0));
      vector<int> optActions(numGradients,0);
      vector<int> optRegimes(numGradients,0);
      vector<double> optLevels(numGradients,0);
      vector<double> bestCV(numPlayers,0);
      do
	{
	  // Iterate one round past convergence
	  keepIterating = (rhsDistance > convTol);
	  
	  // Print status update
	  end = std::chrono::system_clock::now();
	  std::chrono::duration<double> elapsed_seconds = end-start;
	  
	  cout << "Iteration: " << iter
	       << ", distance: " << setprecision(4) << rhsDistance
	       << ", elapsed time (s): " << elapsed_seconds.count()
	       << ", available actions: " << numAvailableActions << endl;
	  
	  // Calculate new threat point
	  for (int g = 0; g < numGradients; g ++)
	    feasConstrs[g].set(GRB_DoubleAttr_RHS,rhs[g]);
	  for (int p = 0; p < numPlayers; p++)
	    {
	      feasModel.setObjective(1.0*feasVars[p],GRB_MINIMIZE);
	      feasModel.optimize();
	      threats[p] = feasModel.get(GRB_DoubleAttr_ObjVal);
	    }

	  // New support function
	  vector<double> tmpRHS(rhs.size(),-std::numeric_limits<double>::max());

	  for (int a = 0; a < numActions_total; a++)
	    {
	      if (!isAvailable[a])
		continue;
	      
	      // Update the feasibility and IC constraints
	      for (int g = 0; g < numGradients; g ++)
		constrs[a][g].set(GRB_DoubleAttr_RHS,rhs[g]);

	      vector<double> minCV(numPlayers,0);
	      bool gIsIC = true;
	      for (int p = 0; p < numPlayers; p++)
		{
		  minCV[p] = threats[p] + (1.0-delta)/delta*gains[a][p];
		  constrs[a][numGradients+p].set(GRB_DoubleAttr_RHS,minCV[p]);
		  gIsIC = gIsIC && (G[a][p]>=minCV[p]);
		}

	      if (gIsIC)
		{
		  int g = 0;
		  // cout << "actions " << a << " are IC." << endl;
		  for (list<vector<double> >::const_iterator it
			 = gradients.begin();
		       it != gradients.end();
		       ++it)
		    {
		      double gLevel = 0;
		      GRBLinExpr obj = 0;
		      for (int pp = 0; pp < numPlayers; pp++)
			gLevel += (*it)[pp] * G[a][pp];

		      if (gLevel > tmpRHS[g])
			{
			  tmpRHS[g] = gLevel;
			  if (!keepIterating)
			    {
			      optActions[g] = a;
			      optLevels[g] = gLevel;
			      optPayoffs[g] = G[a];
			      optRegimes[g] = -1;
			    } // Last iteration---save payoffs
			} // New best level
		      g++;
		    }
		  continue;
		}
	      
	      bool thisActionFeasible = false;

	      // Iterate over binding constraints
	      for (int p = 0; p < numPlayers; p++)
		{
		  if (G[a][p] > minCV[p])
		    continue;
		  
		  // Fix binding constraint
		  constrs[a][numGradients+p].set(GRB_CharAttr_Sense,'=');

		  // Now iterate through directions
		  int g = 0;
		  for (list<vector<double> >::const_iterator it
			 = gradients.begin();
		       it != gradients.end();
		       ++it)
		    {
		      double gLevel = 0;
		      GRBLinExpr obj = 0;
		      for (int pp = 0; pp < numPlayers; pp++)
			{
			  gLevel += (*it)[pp] * G[a][pp];
			  obj += (*it)[pp] * vars[a][pp];
			}
		      models[a]->setObjective(obj,GRB_MAXIMIZE);
		      models[a]->optimize();
		      
		      if (models[a]->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
			{
			  thisActionFeasible = true;

			  double cvLevel = models[a]->get(GRB_DoubleAttr_ObjVal);
			  double tmpLevel
			    = min(gLevel,(1-delta) * gLevel + delta * cvLevel);
		  
			  if (tmpLevel > tmpRHS[g])
			    {
			      tmpRHS[g] = tmpLevel;
			      // if (a==18 && gLevel < cvLevel)
			      // 	{
			      // 	  for (int pp = 0; pp < numPlayers; pp++)
			      // 	    cout << vars[a][pp].get(GRB_DoubleAttr_X) << " ";
			      // 	  cout << endl;
			      // 	  for (int pp = 0; pp < numPlayers; pp++)
			      // 	    cout << G[a][pp] << " ";
			      // 	  cout << endl;
			      // 	  for (int pp = 0; pp < numPlayers; pp++)
			      // 	    cout << (*it)[pp] << " ";
			      // 	  cout << endl;
				  
			      // 	  cout << constrs[a][numGradients].get(GRB_DoubleAttr_RHS) << " "
			      // 	       << vars[a][0].get(GRB_DoubleAttr_X) << " "
			      // 	       << g << " " << p << " "
			      // 	       << cvLevel << " " << gLevel << endl;
			      // 	}
			      if (!keepIterating)
				{
				  optActions[g] = a;
				  optLevels[g] = tmpLevel;
				  if (gLevel < cvLevel)
				    {
				      optPayoffs[g] = G[a];
				      optRegimes[g] = -1;
				    }
				  else
				    {
				      for (int pp = 0; pp < numPlayers; pp++)
					optPayoffs[g][pp] = (1-delta)*G[a][pp]
					  + delta * vars[a][pp].get(GRB_DoubleAttr_X);
				      optRegimes[g] = p;
				    }
				} // Last iteration---save payoffs
			    } // New best level
			  
			} // Optimization succeded

		      g ++;
		    } // for gradient

		  // Reset contraint
		  constrs[a][numGradients+p].set(GRB_CharAttr_Sense,'>');
		} // for player
	      if (!thisActionFeasible)
		{
		  isAvailable[a]=false;
		  numAvailableActions --;
		}
	    } // for actions

	  // Calculate new distance and replace rhs
	  rhsDistance = 0;
	  for (int g = 0; g < numGradients; g++)
	    {
	      if (rhs[g] - tmpRHS[g] > rhsDistance)
		rhsDistance = rhs[g] - tmpRHS[g];
	      rhs[g] = tmpRHS[g];
	    } // for gradient
	  iter ++;
	} while (keepIterating);


      // Compute and print feasible payoffs
      vector<vector<double> > feasiblePayoffs(numGradients,vector<double>(3,0));
      {
	int g = 0;
	for (list<vector<double> >::const_iterator it = gradients.begin();
	     it != gradients.end();
	     ++it)
	  {
	    GRBLinExpr obj = 0;
	    for (int p = 0; p < numPlayers; p ++)
	      obj += (*it)[p] * feasVars[p];
	    feasModel.setObjective(obj,GRB_MAXIMIZE);
	    feasModel.optimize();
	    for (int p = 0; p < numPlayers; p++)
	      {
		feasiblePayoffs[g][p] = feasVars[p].get(GRB_DoubleAttr_X);
		ofs << feasiblePayoffs[g][p] << " ";
	      }
	    ofs << endl;
	    g++;
	  }
      }
      
      // Print the optimal payoffs
      for (int g = 0; g < numGradients; g ++)
	{
	  for (int p = 0; p < numPlayers; p++)
	    ofs << optPayoffs[g][p] << " ";
	  ofs << endl;
	} // gradient
      // Print optimal actions, regimes, levels
      for (int g = 0; g < numGradients; g ++)
	{
	  ofs << optActions[g] << " "
	      << optRegimes[g] << " "
	      << optLevels[g] << endl;
	} // gradient
      // Print the feasible payoffs
      
      

      // Clean up
      ofs.close();
      delete[] feasVars;
      delete[] feasConstrs;
      for (int a = 0; a < numActions_total; a++)
	{
	  delete[] vars[a];
	  delete[] constrs[a];
	  delete models[a];
	}
      
    }
  catch (GRBException & e)
    {
      cout << "GRBException caught: " << e.getMessage() << endl;
    }
  catch (...)
    {
      cout << "Unknown exception caught." << endl;
    }

}
