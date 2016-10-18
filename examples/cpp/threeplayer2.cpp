// Code for three player JYC style algorithm

#include "sgcommon.hpp"
#include "gurobi_c++.h"
#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include <random>
#include <time.h>
#include <chrono>

using namespace orgQhull;

// Return true if b is clockwise relative to a.
bool secondIsBelow(const vector<int> & indices,
		   const vector<double> & a, const vector<double> & b)
{ return (-a[indices[1]]*b[indices[0]]+a[indices[0]]*b[indices[1]]<=0); }

// Compute the intersection between hyperplanes a and b and return the
// level of the intersection in the clockwise tangent direction for a.
double intersect(int p, const vector<double> & minCV,
		 const vector<double> & a,const vector<double> & b,
		 vector<double> & intsctn)
{
  int numPlayers = a.size()-1;
  int p0 = (p+1)%numPlayers, p1 = (p+2)%numPlayers;

  double aRHS = -(a.back()+a[p]*minCV[p]);
  double bRHS = -(b.back()+b[p]*minCV[p]);
  double det = a[p0]*b[p1] - b[p0]*a[p1];
  if (abs(det) < 1e-12)
    return numeric_limits<double>::max(); // Close to singular
  
  // Apply Cramer's rule
  intsctn.resize(2);
  intsctn[0] = -(bRHS*a[p1]-b[p1]*aRHS)/det;
  intsctn[1] = (bRHS*a[p0]-b[p0]*aRHS)/det;

  // cout << "intsctn: " << intsctn[0] << " " << intsctn[1] << endl;
  // Now compute the level
  
  return (intsctn[0] * a[p1] - intsctn[1] * a[p0]);
}

class ASSolver
{
  // private:
public:
  // Game data
  int numPlayers;
  vector<int> numActions;
  int numActions_total;
  vector< vector<double> > G;
  vector< vector<double> > gains;
  double delta;

  // Parameters
  double convTol;
  bool printOn;
  int maxIter;
  int ItLimDefault;
  double minAngleIncrement;

  // Algorithm data
  vector< vector<double> > hyperplanes;
  double volumeChange;
  double volume;
  int iter;
  int numAvailableActions;
  vector<double> threats;
  
  // Outputs
  list< vector<double> > rawPnts;
  list<int> rawActions;
  list<int> rawConstrs;
  vector< vector<double> > extPnts;
  vector<int> extPntIndex;

  // Helper functions
  void computeGains();

  void setDefaultParameters()
  {
    maxIter = 150;
    convTol = 1e-7;
    ItLimDefault = 2100000000;
    minAngleIncrement = 1e-6;
    printOn = true;
  }

public:
  // Constructor
  ASSolver(const vector<int> & _numActions,
	   const vector<vector<double> > & _G,
	   double _delta):
    G(_G), delta(_delta), numActions(_numActions)
  {
    setDefaultParameters();

    numPlayers = numActions.size();

    numActions_total = 1;
    for (int p = 0; p < numPlayers; p++)
      numActions_total *= numActions[p];

    computeGains();
  }
  
  ASSolver()
  {
    setDefaultParameters();
  }

  // Generate random game
  void random(int _numPlayers, int _numActions_total, double _delta);

  int solve();
  void save(ofstream & ofs) const;

  void setPrint(bool tf)
  { printOn = tf; }
  
  bool collapsed() const
  { return (!numAvailableActions); }
  bool converged() const
  { return ((iter<=maxIter)&&(numAvailableActions>0)&&(!extPnts.empty())); }
  int availableActions() const
  { return numAvailableActions; }
  const vector< vector<double> > & getExtPnts() const
  { return extPnts; }
  const list< vector<double> > & getRawPnts() const
  { return rawPnts; }
  const list<int> & getRawActions() const
  { return rawActions; }
};

void randomSurvey(int numActions_total, 
		  int numTrials,
		  double delta);

void example()
{
  double delta = 0.6;
  
  stringstream ss;
  ss << "threeplayer2_fouraction"
     << setprecision(3)  << ".dat";
  ofstream saveOFS(ss.str().c_str());

  vector<int> numActions = {1,1,1};
  int numActions_total = 4;

  vector< vector<double> > G={{1.5,-0.5,-1},
  			     {-1,1.5,-0.5},
  			      {-0.5,-1,1.5},
  			      {1,1,0}};
  vector< vector<double> > gains={{0,0.5,0.5},
  				  {0.5,0,0.5},
  				  {0.5,0.5,0},
  				  {0,0,1.3}};

  // vector< vector<double> > G={{1,-1,-1},
  // 			     {-1,1,-1},
  // 			      {-1,-1,1},
  // 			      {0.5,0.5,0.5}};
  // vector< vector<double> > gains={{0,0.25,0.25},
  // 				  {0.25,0,0.25},
  // 				  {0.25,0.25,-2},
  // 				  {0,0,0}};

  ASSolver solver;
  solver.numPlayers=3;
  solver.delta=delta;
  solver.numActions = numActions;
  solver.numActions_total = numActions_total;
  solver.G = G;
  solver.gains = gains;
  
  solver.solve();
  solver.save(saveOFS);

  saveOFS.close();
}

int main ()
{
  example();

  return 0;

  
  randomSurvey(25,1e2,0.6);
  return 0;

  int numPlayers = 3;
  
  // Game specification
  vector<int> numActions(numPlayers,3);
  int numActions_total = 1;
  for (int p = 0; p < numPlayers; p++)
    numActions_total *= numActions[p];

  // Discount factor
  double delta = 0.8;
  
  // Flow payoffs
  vector< vector<double> > G(numActions_total,
			     vector<double>(numPlayers,0));

  int iter = 0;

  stringstream ss;
  ss << "threeplayer2_"
     << setprecision(3) << delta << ".dat";
  ofstream saveOFS(ss.str().c_str());

  stringstream logss;
  char buffer[10];
  time_t rawtime; time(&rawtime); 
  struct tm* timeinfo; timeinfo = localtime(&rawtime);
  strftime(buffer,10,"%m-%d-%y",timeinfo);
  logss << "threeplayer2_" << buffer << ".log";
  ofstream logOFS(logss.str().c_str());

  int maxExtPnt = 0;
  int avgExtPnt;
  int numTrials = 1;
  int numConverged;
  int numSetCollapsed;

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

  ASSolver solver(numActions,G,delta);
  solver.solve();
  solver.save(saveOFS);

  // Clean up
  saveOFS.close();
  logOFS.close();

  return 0;

}

void randomSurvey(int numActions_total, 
		  int numTrials,
		  double delta)
{
  int numPlayers = 3; 

  int numConverged = 0;
  int numSetCollapsed = 0;
  int globalMaxExt = 0;
  
  vector<double> avgExtPnts(numActions_total+1,0);
  vector<double> avgRawPnts(numActions_total+1,0);
  vector<int> maxExtPnts(numActions_total+1,0);
  vector<int> maxRawPnts(numActions_total+1,0);
  vector<int> count(numActions_total+1,0);

  for (int trial = 0; trial < numTrials; trial++)
    {
      ASSolver solver;
      solver.setPrint(false);
      solver.random(numPlayers,numActions_total,delta);
      solver.solve();

      if (solver.converged())
	{
	  int a = solver.availableActions();
	  avgExtPnts[a] += solver.getExtPnts().size();
	  avgRawPnts[a] += solver.getRawPnts().size();
	  maxExtPnts[a] = max((int) maxExtPnts[a],
			      (int) solver.getExtPnts().size());
	  maxRawPnts[a] = max((int) maxExtPnts[a],
			      (int) solver.getRawPnts().size());
	  if (solver.getExtPnts().size() > globalMaxExt)
	    {
	      globalMaxExt = solver.getExtPnts().size();
	      ofstream ofs ("threeplayer_largeextpnts.dat");
	      solver.save(ofs);
	      ofs.close();
	    }
	  count[a] ++;
	}
      
      cout << "trial: " << trial
           << ", converged: " << (solver.converged())
           << ", remaining a's: " << solver.availableActions()
           << ", raw points: " << solver.getRawPnts().size()
           << ", ext points: " << solver.getExtPnts().size()
           << endl;

    } // for random game

  cout << endl
       << "Total number of trials: " << numTrials << endl
       << "Discount factor: " << delta << endl
       << "Maximum number of action profiles: " << numActions_total
       << endl << endl;
  
  cout << "Distribution: " << endl;
  string availLabel = "Available actions",
    countLabel = "Count",
    maxRawPntsLabel = "Max raw points",
    avgRawPntsLabel = "Avg raw points",
    maxExtPntsLabel = "Max extreme points",
    avgExtPntsLabel = "Avg extreme points",
    tab = "   ";
  
  cout << availLabel << tab 
       << countLabel << tab
       << maxRawPntsLabel << tab
       << avgRawPntsLabel << tab
       << maxExtPntsLabel << tab
       << avgExtPntsLabel << endl;
  
  double globalAvgRaw = 0;
  double globalAvgExt = 0;
  int globalCount = 0;
  int globalMaxRaw = 0;
  int globalAvgActions = 0;

  for (int a = 0; a < numActions_total+1; a++)
    {
      if (a>0)
	{
	  globalAvgActions += count[a]*a;
	  globalCount += count[a];
	  globalAvgRaw += avgRawPnts[a];
	  globalAvgExt += avgExtPnts[a];
	  globalMaxRaw = max(globalMaxRaw,maxRawPnts[a]);
	}
      avgRawPnts[a] /= count[a];
      avgExtPnts[a] /= count[a];

      cout << setw(availLabel.size()) << a << tab
	   << setw(countLabel.size()) << count[a] << tab
	   << setw(maxRawPntsLabel.size()) << maxRawPnts[a] << tab
	   << setw(avgRawPntsLabel.size()) << avgRawPnts[a] << tab
	   << setw(maxExtPntsLabel.size()) << maxExtPnts[a] << tab
	   << setw(avgExtPntsLabel.size()) << avgExtPnts[a] << endl;
    }

  cout << "Total number converged: " << globalCount
       << ", max ext: " << globalMaxExt
       << ", avg ext: " << globalAvgExt/globalCount
       << ", max ext: " << globalMaxExt
       << ", avg ext: " << globalAvgExt/globalCount
       << ", avg actions: " << globalAvgActions/globalCount
       << endl;
} // randomSurvey


void ASSolver::computeGains()
{
  gains.clear();
  gains.resize(numActions_total);

  for (int a = 0; a < numActions_total; a++)
    {
      int remainder = a;
      int scale = 1;
      gains[a].resize(numPlayers);
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
} // computeGains

void ASSolver::save(ofstream & ofs) const
{
  // Print numbers of actions
  ofs << setprecision(15) << numActions_total << " "
      << delta << " " << extPnts.size() << " "
      <<  0 << " " << 0 << endl;

  for (int p = 0; p < numPlayers; p++)
    ofs << numActions[p] << " ";
  ofs << 0 << " " << 0 << endl;

  // Print payoffs and gains from deviating
  for (int a = 0; a < numActions_total; a++)
    {
      for (int p = 0; p < numPlayers; p++)
	ofs << G[a][p] << " ";
      ofs << 0 << " " << 0 << endl;
    }
  for (int a = 0; a < numActions_total; a++)
    {
      for (int p = 0; p < numPlayers; p++)
	ofs << gains[a][p] << " ";
      ofs << 0 << " " << 0 << endl;
    }

  auto iit = extPntIndex.begin();
  for (auto eit = extPnts.begin();
       eit != extPnts.end();
       ++eit, ++iit)
    {
      for (int p = 0; p < numPlayers; p++)
	ofs << (*eit)[p] << " ";
      ofs << (*iit) << " " << 0 << endl;
    }

  auto ait = rawActions.begin(),
    cit = rawConstrs.begin();
  for (auto rit = rawPnts.begin();
       rit != rawPnts.end();
       ++rit, ++ait, ++cit)
    {
      for (int p = 0; p < numPlayers; p++)
	ofs << (*rit)[p] << " ";
      ofs << (*ait) << " "
	  << (*cit) << endl;
    }
} // save

int ASSolver::solve()
{

  rawPnts.clear();
  rawActions.clear();
  rawConstrs.clear();
  extPntIndex.clear();

  bool notEnoughPoints = false;
  iter = 0;

  Qhull feasQhull;
  RboxPoints feasRbox;
  coordT c_payoffs[numPlayers];
  threats = vector<double> (numPlayers,numeric_limits<double>::max());
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

  hyperplanes.clear();
  hyperplanes.reserve(fl.size()+numPlayers);
  vector<double> hyperplane(numPlayers+1,0);
  for (int p = 0; p < numPlayers; p++)
    {
      hyperplanes.push_back(vector<double>(numPlayers+1,0));
      hyperplanes.back()[p]=-1;
    }
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
      env.set(GRB_IntParam_Method,0);
      env.set(GRB_DoubleParam_MarkowitzTol,1e-4);
      // env.set(GRB_IntParam_Quad,1);
      //env.set(GRB_IntParam_NormAdjust,3);
      // env.set(GRB_IntParam_SimplexPricing,3);
      

      // Main loop
      if (printOn)
	cout << "Starting main loop..." << endl;

      volumeChange = 1;
      volume = numeric_limits<double>::max();
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
	  int numVars = numPlayers;
	  GRBVar * vars = model.addVars(numVars);
	  model.update();
	  for (int p = 0; p < numPlayers; p++)
	    vars[p].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
	  
	  for (auto it = hyperplanes.begin();
	       it != hyperplanes.end();
	       ++it)
	    {
	      GRBLinExpr lhs = 0;
	      for (int p = 0; p < numPlayers; p ++)
		lhs += vars[p] * (*it)[p];

	      model.addConstr(lhs <= -(*it).back());
	    }
	  model.update();
	  GRBConstr * constrs = model.getConstrs();
	  
	  RboxPoints rbox;
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
		  constrs[p].set(GRB_DoubleAttr_RHS,-minCV[p]);
		  hyperplanes[p].back() = minCV[p];
		}

	      if (gIsIC)
		{
		  for (int p = 0; p < numPlayers; p++)
		    {
		      c_payoffs[p] = G[a][p];
		      newThreats[p] = min(newThreats[p],c_payoffs[p]);
		    }
		  
		  rbox.append(QhullPoint(numPlayers,c_payoffs));
		  if (!keepIterating)
		    {
		      rawPnts.push_back(vector<double>(c_payoffs,
						       c_payoffs+sizeof(c_payoffs)/sizeof(double)));
		      rawActions.push_back(a);
		      rawConstrs.push_back(numPlayers);
		    }
		  continue;
		}
	      
	      bool thisActionFeasible = false;

	      // Iterate over binding constraints
	      for (int p = 0; p < numPlayers; p++)
		{
		  if (G[a][p] >= minCV[p])
		    continue;

		  // Fix binding constraint
		  constrs[p].set(GRB_CharAttr_Sense,'=');

		  // Now map out the convex hull
		  vector<double> dir(2,1.0);
		  // dir[0] = 1.0;
		  int pp1 = (p+1)%numPlayers, pp2 = (p+2)%numPlayers;
		  vector<int> indices(2);
		  indices[0] = pp1; indices[1] = pp2;
		  model.setObjective(dir[0] * vars[pp1]+ dir[1] * vars[pp2],
				     GRB_MAXIMIZE);
		  model.optimize();
		  if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL)
		    continue;

		  thisActionFeasible = true;

		  for (int pp = 0; pp < numPlayers; pp++)
		    {
		      c_payoffs[pp] = (1-delta)*G[a][pp]
			+ delta*vars[pp].get(GRB_DoubleAttr_X);

		      newThreats[pp] = min(newThreats[pp],c_payoffs[pp]);
		    }
		  rbox.append(QhullPoint(numPlayers,c_payoffs));

		  if (!keepIterating)
		    {
		      rawPnts.push_back(vector<double>(c_payoffs,
						       c_payoffs+sizeof(c_payoffs)/sizeof(double)));
		      rawActions.push_back(a);
		      rawConstrs.push_back(p);
		    }
		  

		  // Identify the active constraints
		  vector< vector<vector<double> >::const_iterator > activeConstr(0);

		  int c = 0;
		  for (auto it = hyperplanes.begin();
		       it != hyperplanes.end();
		       ++it)
		    {
		      if (c==p)
			{
			  c++;
			  continue;
			}

		      double level = 0;
		      for (int pp = 0; pp < numPlayers; pp++)
			level += (*it)[pp]*vars[pp].get(GRB_DoubleAttr_X);
		      if (abs(level + it->back()) < 1e-12)
			activeConstr.push_back(it);
		      
		      c++;
		    }
		  assert(activeConstr.size() >= numPlayers-1);

		  // Determine which of the active constraints is CW and
		  // which is CCW.
		  vector<vector<double> >::const_iterator cwDir(activeConstr[0]),
		    ccwDir(activeConstr[0]);
		    
		  for (int k = 1;
		       k < activeConstr.size();
		       k++)
		    {
		      if (secondIsBelow(indices,*cwDir,*(activeConstr[k])))
			cwDir = activeConstr[k];
		      if (secondIsBelow(indices,*(activeConstr[k]),*ccwDir))
			ccwDir = activeConstr[k];
		    }

		  vector<double> intsctn(2);
		  double initialLevel = intersect(p,minCV,*cwDir,*ccwDir,intsctn);
		  
		  int numCHSteps = 0;
		  while (cwDir != ccwDir && numCHSteps < 2*hyperplanes.size())
		    {
		      // Determine how far we can go in the new direction
		      // by intersecting with every half plane.
		      double bestLevel = numeric_limits<double>::max();
		      vector< vector<double> >::const_iterator bestDir = hyperplanes.end();
		      vector<double> bestIntsctn(2,0);
		      
		      for (auto it = hyperplanes.begin();
			   it != hyperplanes.end();
			   ++it)
			{
			  if ((*cwDir)[pp2]*(*it)[pp1] - (*cwDir)[pp1]*(*it)[pp2] < 0)
			    continue;

			  double newLevel = intersect(p,minCV,*cwDir,*it,intsctn);
			  if (newLevel < bestLevel)
			    {
			      bestLevel = newLevel;
			      bestDir = it;
			      bestIntsctn = intsctn;
			    }
			}
		      c_payoffs[pp1] = (1-delta)*G[a][pp1] + delta*bestIntsctn[0];
		      c_payoffs[pp2] = (1-delta)*G[a][pp2] + delta*bestIntsctn[1];

		      newThreats[pp1] = min(newThreats[pp1],c_payoffs[pp1]);
		      newThreats[pp2] = min(newThreats[pp2],c_payoffs[pp2]);
		      
		      rbox.append(QhullPoint(numPlayers,c_payoffs));
		      if (!keepIterating)
		  	{
		  	  rawPnts.push_back(vector<double>(c_payoffs,
		  					   c_payoffs+sizeof(c_payoffs)/sizeof(double)));
		  	  rawActions.push_back(a);
		  	  rawConstrs.push_back(p);
		  	}


		      cwDir = bestDir;

		      // cout << "cwDir: ";
		      // for (int pp = 0; pp < numPlayers+1; pp++)
		      // 	cout << (*cwDir)[pp] << " ";
		      // cout << endl;
		      numCHSteps++;
		    } 
		  
		  // Reset contraint
		  // vars[p].set(GRB_DoubleAttr_UB,GRB_INFINITY);
		  constrs[p].set(GRB_CharAttr_Sense,'<');
		  // constrs[p].set(GRB_CharAttr_Sense,'>');
		} // for player
	      if (!thisActionFeasible)
		{
		  isAvailable[a]=false;
		  numAvailableActions --;
		}
	    } // for actions

	  delete[] vars;
	  delete[] constrs;
	  
	  Qhull qhull;

	  if (!numAvailableActions)
	    {
	      if (printOn)
		cout << "No actions available. Exiting..." << endl;
	      keepIterating = false;
	    }
	  else
	    {
	      qhull.setFactorEpsilon(1e-2);
	      qhull.runQhull(rbox,"");
	      fl = qhull.facetList();
	      hyperplanes.clear();
	      hyperplanes.reserve(numPlayers+fl.size());
	      for (int p = 0; p < numPlayers; p++)
		{
		  hyperplanes.push_back(vector<double>(numPlayers+1,0));
		  hyperplanes.back()[p]=-1;
		}
	      for (auto it = fl.begin();
		   it != fl.end();
		   ++it)
		{
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
	      extPnts.clear();
	      extPnts.reserve(vl.count());
	      vector<double> payoffs(numPlayers,0);
	      for (auto it = vl.begin();
		   it != vl.end();
		   ++it)
		{
		  for (int p = 0; p < numPlayers; p++)
		    payoffs[p] = it->point().coordinates()[p];
		  extPnts.push_back(payoffs);
		  extPntIndex.push_back(it->point().id());
		}
	    }	      
	      
	  iter ++;
	} while (keepIterating);

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
      else
	cout << e.what() << endl;
    }
  catch (...)
    {
      cout << "Unknown exception caught." << endl;
    }
  return iter;
} // ASSolver::solve()

void ASSolver::random(int _numPlayers,
		      int _numActions_total,
		      double _delta)
{
  delta = _delta;
  numPlayers = _numPlayers;
  numActions_total = _numActions_total;
  numActions=vector<int>(numPlayers,0);

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);
  uniform_real_distribution<coordT> distribution(0.0,10.0);
  
  G = vector< vector<double> > (numActions_total,
				vector<double>(numPlayers,0));
  gains=G;

  for (int a = 0; a < numActions_total; a++)
    {
      for (int p = 0; p < numPlayers; p++)
	{
	  G[a][p] = distribution(generator);
	  gains[a][p] = distribution(generator);
	}
    }
  
} // random
