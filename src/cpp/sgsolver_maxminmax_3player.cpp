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

#include "sgsolver_maxminmax_3player.hpp"



SGSolver_MaxMinMax_3Player::SGSolver_MaxMinMax_3Player(const SGEnv & _env,
						       const SGGame & _game):
  env(_env),
  game(_game),
  soln(_game),
  numPlayers(_game.getNumPlayers()),
  numStates(_game.getNumStates()),
  delta(_game.getDelta()),
  payoffs(_game.getPayoffs()),
  eqActions(_game.getEquilibriumActions()),
  probabilities(_game.getProbabilities()),
  numActions(_game.getNumActions()),
  numActions_totalByState(_game.getNumActions_total()),
  debugMode(false)
{
  
}

void SGSolver_MaxMinMax_3Player::solve_fixed(const int numDirsApprox,
					     const bool dropRedundant,
					     const bool addEndogenous)
{
  cout << "Initializing solver..." << endl;

  initialize();
  
  // Initialize directions
  int numDirections = 0;

  double a = 4.0*PI/static_cast<double>(numDirsApprox);
  double d = sqrt(a);
  int Mpsi = round(PI/d);
  double dpsi = PI/static_cast<double>(Mpsi);
  double dphi = a/dpsi;

  // Initialize directions - approximately evenly spaced around the
  // sphere, with three negative coordinate directions at the end.
  vector< list<vector< double> > :: const_iterator> playerMinLvls(numPlayers);
  for (int mpsi = 0; mpsi < Mpsi; mpsi++)
    {
      double psi = PI*(static_cast<double>(mpsi)+0.5)/static_cast<double>(Mpsi);
      int Mphi = round(2.0*PI*sin(psi)/dpsi);

      for (int mphi = 0; mphi < Mphi; mphi++)
	{
	  double phi = 2.0*PI*static_cast<double>(mphi)/static_cast<double>(Mphi);

	  SGPoint newDir(numPlayers,0.0);
	  newDir[0]=sin(psi)*cos(phi);
	  newDir[1]=sin(psi)*sin(phi);
	  newDir[2]=cos(psi);
	  directions.push_back(newDir);
	  levels.push_back(vector<double>(numStates,0));

	  numDirections++;	  
	}
    }

  threatDirections.clear();
  for (int player = 0; player <numPlayers; player++)
    {
      SGPoint newDir(numPlayers,0.0);
      newDir[player] = -1.0;

      threatDirections.push_back(newDir);
    }

  // numDirections += 6;
  // SGPoint faceDir(3,2.0/3.0);
  // faceDir[0] = -1.0/3.0;
  // directions.push_front(faceDir);
  // directions.push_front(-1.0*faceDir);
  // faceDir[0] = 2.0/3.0; faceDir[1] = -1.0/3.0;
  // directions.push_front(faceDir);
  // directions.push_front(-1.0*faceDir);
  // faceDir[1] = 2.0/3.0; faceDir[2] = -1.0/3.0;
  // directions.push_front(faceDir);
  // directions.push_front(-1.0*faceDir);
  // levels.push_front(vector<double>(numStates,0));
  // levels.push_front(vector<double>(numStates,0));
  // levels.push_front(vector<double>(numStates,0));
  // levels.push_front(vector<double>(numStates,0));
  // levels.push_front(vector<double>(numStates,0));
  // levels.push_front(vector<double>(numStates,0));
  

  cout << "Starting main loop..." << endl;

  numRedundDirs = 0;
  numEndogDirs = 0;
  
  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      iterate_fixed(numDirections,dropRedundant,addEndogenous);
      cout << progressString() << endl;

      numIter++;
    } // while

  // Finish with one round of endogenous iteration.
  // iterate_endogenous();

  cout << "Converged!" << endl;

  cout << "Overall added " << numEndogDirs
       << " endogenous directions and removed " << numRedundDirs
       << " redundant directions." << endl;

} // solve_fixed

double SGSolver_MaxMinMax_3Player::iterate_fixed(const int maxDirections,
						 const bool dropRedundant,
						 const bool addEndogenous)
{
  int addEndogFreq = 5;
  int dropAfterThisIter = addEndogFreq;
  
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::normal_distribution<double> normDistr(0.0,1.0);
  std::uniform_int_distribution<int> intDistr(0.0,directions.size());

  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  SGIteration_MaxMinMax iter;

  vector<SGActionIter> actionTuple(numStates);
  // Pick the initial actions arbitrarily
  for (int state = 0; state < numStates; state++)
    actionTuple[state] = actions[state].begin();
      
  vector<SG::Regime> regimeTuple(numStates,SG::Binding);

  // Reset the trimmed points for the actions
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  ait->updateTrim();
	      
	  // WARNING: NO GUARANTEE THAT THIS POINT IS FEASIBLE
	  if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							   [ait->getAction()]))))
	    {
	      actions[state].erase(ait++);
	      continue;
	    }
	      
	} // for ait
    } // for state

  if (env.getParam(SG::STOREITERATIONS))
    iter = SGIteration_MaxMinMax(actions,threatTuple);
      
  // Reset the error level
  errorLevel = 0;

  {
    list<vector< double> >::iterator lvl;
    int dirCnt;
    list<SGPoint>::const_iterator dir;

    // Iterate through directions
    for (dir = directions.cbegin(),
	   lvl = levels.begin(),
	   dirCnt = 0;
	 dir != directions.cend(),
	   lvl != levels.end();
	 dir++, lvl++, dirCnt ++ )
      {
	// Compute optimal level in this direction
	vector<double> newLevels(numStates,0);
	SGPoint currDir = *dir;

	optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
		       actions,feasibleTuple);
	for (int s = 0; s < numStates; s++)
	  assert(pivot[s].size()==numPlayers);

	// SGProductPolicy optPolicies(numStates);
	// computeOptimalPolicies(optPolicies,pivot,currDir,actions);

	for (int state = 0; state < numStates; state++)
	  {
	    // Update the levels and the error level with the
	    // movement in this direction
	    newLevels[state] = pivot[state]*currDir;
	    errorLevel = max(errorLevel,abs(newLevels[state]-(*lvl)[state]));
	    (*lvl)[state] = newLevels[state];
	  } // for state

	if (env.getParam(SG::STOREITERATIONS))
	  iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
				SGHyperplane(currDir,newLevels),actions));
	    
      } // for dir lvl dirCntr
  } // Computing new levels

  // If there are fewer directions than the max, add a new face
  // direction to the front
    
  int endogDirCnt = 0;
  int currNumDirs = directions.size();
  if (addEndogenous
      && directions.size() < maxDirections
      && ( (numIter%addEndogFreq) == addEndogFreq-1))
    {
      for (int k = 0; k < maxDirections-currNumDirs; k++)
	// for (int k = 0; k < 40; k++)
      // while (directions.size() < maxDirections)
	{
	  // Construct a new face
	  SGPoint startDir = SGPoint(numPlayers,0.0); 
	  SGPoint rotateDir = SGPoint(numPlayers,0.0);

	  for (int p = 0; p  < numPlayers; p++)
	    {
	      startDir[p] = normDistr(generator);
	      rotateDir[p] = -1.0*startDir[p]+0.05*normDistr(generator);
	    }

	  
	  // if (rotateDir.norm() > 1e-6)
	  //   rotateDir /= rotateDir.norm();

	  optimizePolicy(pivot,actionTuple,regimeTuple,startDir,
			 actions,feasibleTuple);

	  double bestLevel = 0.0;
  
	  SGPoint optSubDir(numPlayers,0.0);
	  bestLevel = sensitivity(optSubDir,
				  pivot,
				  startDir,
				  rotateDir,
				  actions);
	  if (bestLevel > 1e9 || bestLevel < 1e-9
	      || optSubDir.norm() < 1e-6)
	    {
	      continue;
	    }
	  
	  SGPoint newDir = startDir+ bestLevel*rotateDir;

	  // Need the actual direction of the new substitution here
	  rotateDir = SGPoint::cross(newDir,optSubDir);
	  // assert(rotateDir.norm() > 1e-6);
  
	  bestLevel = sensitivity(optSubDir,
				  pivot,
				  newDir,
				  rotateDir,
				  actions);
	  SGPoint faceDir = newDir + bestLevel*rotateDir;

	  if (bestLevel > 1e9 || bestLevel < 1e-9
	      || faceDir.norm() < 1e-6)
	    {
	      continue;
	    }
  
	  // faceDir /= faceDir.norm();
	  if (faceDir.norm() >= 1e-6)
	    faceDir /= faceDir.norm();

	  bool alreadyPresent = false;
	  for (auto dit = directions.cbegin();
	       dit != directions.cend();
	       dit++)
	    {
	      if (SGPoint::distance(*dit,faceDir)<1e-5)
		{
		  alreadyPresent = true;
		  break;
		}
	    }

	  if (alreadyPresent)
	    continue;

	  endogDirCnt++;
	  optimizePolicy(pivot,actionTuple,regimeTuple,faceDir,
			 actions,feasibleTuple);
	  directions.push_front(faceDir);
	  levels.push_front( vector<double>(numStates,0.0));
	  for (int s = 0; s < numStates; s++)
	    levels.front()[s] = pivot[s]*faceDir;
	  
	  if (directions.size()==maxDirections)
	    break;
	}
    }

  if (endogDirCnt)
    {
      cout << "Added " << endogDirCnt << " endogenous directions." << endl;
    }

  // Update the the threat tuple
  {
    list<SGPoint>::const_iterator dit = threatDirections.cbegin();
    for (int player = 0; player < numPlayers; player++)
      {
	optimizePolicy(pivot,actionTuple,regimeTuple,*dit,
		       actions,feasibleTuple);
      
	vector<double> newLevels(numStates,0);
	for (int state = 0; state < numStates; state++)
	  {
	    newLevels[state] = (pivot[state]*(*dit));
	    errorLevel = max(errorLevel,
			     abs(threatTuple[state][player] + newLevels[state]));
	    threatTuple[state][player] = -1.0*newLevels[state];
	  }
	if (env.getParam(SG::STOREITERATIONS))
	  iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
				SGHyperplane(*dit,newLevels),actions));

	++dit;
      }
  }
  // cout << "New threat tuple: " << threatTuple << endl;

  if (env.getParam(SG::STOREITERATIONS)==2
      || (env.getParam(SG::STOREITERATIONS)==1
	  && ( errorLevel < env.getParam(SG::ERRORTOL)
	       || numIter+1 >= env.getParam(SG::MAXITERATIONS) ) ) )
    soln.push_back(iter); // Important to do this before updating
  // the threat point and minIC of the
  // actions

  findFeasibleTuple(feasibleTuple,actions);

  // cout << "Computing new binding payoffs..." << endl;

  // Recalculate minimum IC continuation payoffs
  int redundDirCnt = 0;
  {
    // Reset the trimmed points
    for (int state = 0; state < numStates; state++)
      {
	for (auto ait = actions[state].begin();
	     ait != actions[state].end();
	     ++ait)
	  {
	    ait->calculateMinIC(game,threatTuple);
	    ait->resetTrimmedPoints(payoffUB);
	  }
      }

    // Go through the half spaces in a random order.
    std::vector<int> order(directions.size(),0);
    vector<bool> redundant(directions.size(),true);
    std::uniform_int_distribution<int> intDistr2(0.0,directions.size()-1);
    for (int d =0; d < order.size(); d++)
      {
    	order[d] = d;
      }
    for (int d =0; d < order.size(); d++)
      {
    	int k = intDistr2(generator);
    	std::swap(order[d],order[k]);
      }
    
    list<SGPoint>::iterator dir = directions.begin();
    list< vector< double> >::iterator lvl = levels.begin();
    int dirCnt = 0;
    // while (dir != directions.end())
    for (int d = 0; d < order.size(); d++)
      {
	while (dirCnt < order[d])
	  {
	    dir++;
	    lvl++;
	    dirCnt++;
	  }
	while (dirCnt > order[d])
	  {
	    dir--;
	    lvl--;
	    dirCnt--;
	  }
	
	for (int state = 0; state < numStates; state++)
	  {
	    for (auto ait = actions[state].begin();
		 ait != actions[state].end();
		 ++ait)
	      {
		// Compute the expected level

		// cout  << "direction: " << dirCnt++ << endl;

		double expLevel = 0;
		for (int sp = 0; sp < numStates; sp++)
		  expLevel += probabilities[state][ait->getAction()][sp]
		    * (*lvl)[sp];
		    
		// Trim the action
		if(ait->trim(*dir,expLevel))
		  {
		    redundant[order[d]]=false;
		  }
	      } // for ait
	  } // for state
      } // whiledir

    dir = threatDirections.begin();
    dirCnt = 0;
    while (dir != threatDirections.end())
      {
	for (int state = 0; state < numStates; state++)
	  {
	    for (auto ait = actions[state].begin();
		 ait != actions[state].end();
		 ++ait)
	      {
		// Compute the expected level

		// cout  << "direction: " << dirCnt++ << endl;

		double expLevel = 0;
		for (int sp = 0; sp < numStates; sp++)
		  expLevel += probabilities[state][ait->getAction()][sp]
		    * threatTuple[sp][dirCnt];
		    
		// Trim the action
		ait->trim(*dir,expLevel);
	      } // for ait
	  } // for state
	dir++;
	dirCnt++;
      }

    // Every once in awhile, don't drop directions, to make sure we
    // get an accurate measure of the hausdorff distance.
    if (dropRedundant
	&& numIter > dropAfterThisIter
	&& numIter%addEndogFreq!= 1) 
      {
	dir=directions.begin();
	lvl = levels.begin();
	for (int d = 0; d < order.size(); d++)
	  {
	    if (redundant[d])
	      {
		redundDirCnt++;
		directions.erase(dir++);
		levels.erase(lvl++);
	      }
	    else
	      {
		dir++;
		lvl++;
	      }
	  }
      }
    
    if (redundDirCnt)
      cout << "Dropped " << redundDirCnt << " redundant directions." << endl;
  }

  numRedundDirs += redundDirCnt;
  numEndogDirs += endogDirCnt;
  
  if (endogDirCnt)
    errorLevel = 1.0;
  

  return errorLevel;
} // iterate_fixed

void SGSolver_MaxMinMax_3Player::solve_endogenous()
{
  initialize();
  
  // Main loop

  while (errorLevel > env.getParam(SG::ERRORTOL)
	 && numIter < env.getParam(SG::MAXITERATIONS) )
    {
      iterate_endogenous();

      cout << progressString() << endl;
    } // while --- main loop

  numIter++;
  cout << "Converged!" << endl;
  
} // solve_endogenous

std::string SGSolver_MaxMinMax_3Player::progressString() const
{
  std::stringstream ss;
  // Print summary of iteration
  ss << "Iter: " << numIter
     << ", errorLvl: " << scientific << errorLevel
     << ", remaining actions: ( ";
  for (int state = 0; state < numStates; state++)
    ss << actions[state].size() << " ";
  ss << ")"
     << ", numDirections = " << directions.size();

  return ss.str();
}


double SGSolver_MaxMinMax_3Player::iterate_endogenous()
{
  SGTuple pivot = threatTuple;
  SGTuple feasibleTuple = threatTuple; // A payoff tuple that is feasible for APS

  SGIteration_MaxMinMax iter;
  
  // Clear the directions and levels
  list<SGPoint> newDirections;
  list< vector<double> > newLevels;

  
  vector<SGActionIter> actionTuple(numStates);
  // Pick the initial actions arbitrarily
  for (int state = 0; state < numStates; state++)
    actionTuple[state] = actions[state].cbegin();
      
  vector<SG::Regime> regimeTuple(numStates,SG::Binding);

  if (env.getParam(SG::STOREITERATIONS))
    iter = SGIteration_MaxMinMax (actions,threatTuple);

  
  // Construct initial face
  SGPoint currDir = SGPoint(3,1.0); // Start pointing due east

  optimizePolicy(pivot,actionTuple,regimeTuple,currDir,
		 actions,feasibleTuple);

  SGPoint rotateDir = SGPoint(3,0.0); // rotate towards minimizing player 1's payoff
  rotateDir[0]=currDir[1];
  rotateDir[1]=-currDir[0];
  double bestLevel = 0.0;

  assert(rotateDir.norm()>1e-6);
  assert(currDir.norm()>1e-6);
  
  SGPoint optSubDir(3,0.0);
  bestLevel = sensitivity(optSubDir,
			  pivot,
			  currDir,
			  rotateDir,
			  actions);

  SGPoint newDir = currDir+ bestLevel*rotateDir;

  // assert(optSubDir.norm() > 1e-6);
  // assert(newDir.norm() > 1e-6);
  newDir /= newDir.norm();
  
  // Need the actual direction of the new substitution here
  rotateDir = SGPoint::cross(newDir,optSubDir);
  // assert(rotateDir.norm() > 1e-6);
  
  bestLevel = sensitivity(optSubDir,
			  pivot,
			  newDir,
			  rotateDir,
			  actions);
  
  SGPoint faceDir = 1.0/(bestLevel+1.0)*newDir
    + bestLevel/(bestLevel+1.0)*rotateDir;
  // assert(optSubDir.norm() > 1e-6);
  // assert(faceDir.norm() > 1e-6);
  faceDir /= faceDir.norm();
  SGProductPolicy initialFace(numStates,faceDir);
  computeOptimalPolicies(initialFace,pivot,faceDir,actions);

  std::unordered_set<std::string> foundFaces;
  foundFaces.insert(initialFace.hash());
  std::queue<SGProductPolicy> unexploredFaces;
  unexploredFaces.push(initialFace);
  if (env.getParam(SG::STOREITERATIONS))
    iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
			  SGHyperplane(initialFace.getDir(),
				       initialFace.getLevels()),
			  actions));
  // cout << "Initial face dir: " << faceDir << endl;
  
  //cout << "New threat tuple: " << newThreatTuple << endl;
  
  // Now run new computeOptimalPolicies to obtain a new face
  
  list<SGPoint> foundSubDirs;
  list<SGTuple> foundPivots;

  SGPoint faceDir0(3,-2.0/3.0), faceDir1(faceDir0), faceDir2(faceDir0);
  faceDir0[0] = 1.0/3.0;
  faceDir1[1] = 1.0/3.0;
  faceDir2[2] = 1.0/3.0;
    
  int totalEdgeCount = 0;
  while (!unexploredFaces.empty())
    {
      faceDir = unexploredFaces.front().getDir();

      debugMode = (numIter==1 || numIter==2 || numIter==3);
      debugMode = false;
      // debugMode = true;
      
      // cout << "faceDir: " << faceDir
      // 	   << ", unexploredFaces: " << unexploredFaces.size()
      // 	   << ", foundFaces: " << foundFaces.size() << endl;

      if (debugMode)
	{
	  cout << unexploredFaces.front().hash() << endl;
	  cout << "faceDir: " << faceDir;
	  for (int s = 0; s < numStates; s++)
	    cout << " " << unexploredFaces.front().getLevels()[s];
	  cout << endl;
	}

      
      int edgeCount = 0;
      int newFaceCount = 0;
      // Pop first face off
      SGEdgePolicy edge(unexploredFaces.front());
      foundSubDirs.clear();
      foundPivots.clear();
      do
	{
	  // Compute optimal level in this direction
	  for (int s = 0; s < numStates; s++)
	    {
	      actionTuple[s] = edge.getPolicies()[s]->getAction();
	      edge.getPolicies()[s]->getAction()->getAction();
	      
	      regimeTuple[s] = edge.getPolicies()[s]->getRegime();
	      if (regimeTuple[s] == SG::Binding)
		{
		  const int player = edge.getPolicies()[s]->getBindingPlayer();
		  const int point = edge.getPolicies()[s]->getBindingPoint();
		  pivot[s] = (1-delta)*payoffs[s][actionTuple[s]->getAction()]
		    + delta*actionTuple[s]->getPoints()[player][point];
		}
	    }
	  policyToPayoffs(pivot,actionTuple,regimeTuple);

	  // Find the orthogonal direction to the current direction
	  // and this edge to find a new face direction. If it's been
	  // explored before, do nothing. Otherwise, add it to the
	  // queue. For now, just try rotating in both directions.
	  const int subState = edge.getSubState();
	  const SGActionIter subAction = edge.getSubPolicy()->getAction();
	  const int subActionIndex = subAction->getAction();
	  SGPoint subDir = SGPoint(3,0.0);
	  if (edge.getSubPolicy()->getRegime() == SG::NonBinding)
	     subDir = (1-delta)*payoffs[subState][subActionIndex]
	      + delta*pivot.expectation(probabilities[subState][subActionIndex])-pivot[subState];
	  else
	    {
	      const int subPlayer = edge.getSubPolicy()->getBindingPlayer();
	      const int subPoint = edge.getSubPolicy()->getBindingPoint();
	      subDir = (1-delta)*payoffs[subState][subActionIndex]
		+ delta*subAction->getPoints()[subPlayer][subPoint]-pivot[subState];
	    }

	  rotateDir = SGPoint::cross(faceDir,subDir);

	  // Avoid searching the same edge twice.
	  bool newEdge = true;
	  auto sit = foundSubDirs.cbegin();
	  auto pit = foundPivots.cbegin();
	  while( sit != foundSubDirs.cend())
	    {
	      bool sameEdge = SGPoint::distance(subDir,*sit)<1e-8;
	      
	      for (int s = 0; s < numStates; s++)
		{
		  if (abs((pivot[s]*rotateDir -(*pit)[s]*rotateDir))>1e-8)
		    {
		      sameEdge = false;
		      break;
		    }
		}

	      if (sameEdge)
		{
		  newEdge = false;
		  break;
		}
		
	      ++ sit; ++ pit;
	    }
	  
	  if (false && debugMode)
	    {
	      cout << edge.hash() << endl;
	      cout << "subDir: " <<  subDir << ", lvls:";
	      for (int s = 0; s < numStates; s++)
	      	cout << " " << pivot[s]*rotateDir;
	      cout << ", rotateDir: " << rotateDir
	      	   << ", new: " << newEdge;
	      cout << endl;
	    }
	  std::string critEdgeHash("E_s0a1NBs1a1NB_subs0a4NB");

	  if (newEdge)
	    {
	      foundSubDirs.push_back(subDir);
	      foundPivots.push_back(pivot);
	    }
	  else
	    continue;

	  if (subDir.norm() < 1e-10)
	    {
	      //cout << "Warning: Null substitution direction." << endl;
	      edgeCount++;
	      continue;
	    }
	  
	  for (double weight: {-1.0,1.0})
	    {
	      bestLevel = sensitivity(optSubDir,
				      pivot,
				      faceDir,
				      weight*rotateDir,
				      actions);
	      SGPoint newDir = faceDir;
	      newDir *= 1.0/(bestLevel+1.0);
	      newDir.plusWithWeight(rotateDir,weight*bestLevel/(bestLevel+1.0));
	      newDir /= newDir.norm();

	      // Skip this face if it's a thread direction.
	      bool isThreatDir = false;
	      for (int p = 0; p < numPlayers; p++)
		{
		  if (abs(newDir[p]+1.0)<1e-8)
		    isThreatDir = true;
		}
	      if (isThreatDir)
		{
		  if (debugMode)
		    cout << "Skipping a thread direction." << endl;
		  continue;
		}
	      

	      if (debugMode)
		{
		  // cout << setprecision(8) << min(bestLevel,9999.0) << " " << newDir << endl;

		  SGPoint dir0(3,-0.622), dir1(dir0), dir2(dir0);
		  dir0[0] = 0.47565;
		  dir1[1] = 0.47565;
		  dir2[2] = 0.47565;

		  if (SGPoint::distance(newDir,dir0)<1e-4
		      || SGPoint::distance(newDir,dir1)<1e-4
		      || SGPoint::distance(newDir,dir2)<1e-4)
		    cout << "************************************I FOUND THE CRITICAL DIRECTION!!!" << endl;
		}

	      SGProductPolicy newFace(numStates, newDir);
	      if (computeOptimalPolicies(newFace,pivot,newDir,actions))
		{
		  if (newFace.isempty()
		      || newFace.dimension() < 2)
		    {
		      continue;

		      // Exception not currently thrown
		      throw(SGException(SG::NOOPTIMALPOLICIES));
		    }
		  
		  // Did not find a strict improvement.
		  string hashKey = newFace.hash();

		  // If the new face is unexplored, add it to the
		  // queue, and add the hash key to the set of found
		  // faces.
		  if (!foundFaces.count(hashKey))
		    {
		      ++newFaceCount;

		      if (env.getParam(SG::STOREITERATIONS))
			iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
					      SGHyperplane(newFace.getDir(),
							   newFace.getLevels()),
					      actions));
		      
		      foundFaces.insert(hashKey);
		      unexploredFaces.push(newFace);
		    }
		}
	    } // for weight
	  
	  ++ edgeCount;
	} while( ++edge );
      if (debugMode)
      	cout << "edgeCount: " << edgeCount
      	     << ", newFaceCount: " << newFaceCount
      	     << ", unexplored faces: " << unexploredFaces.size()
      	     << ", found faces: " << foundFaces.size()
      	     << endl;

      totalEdgeCount += edgeCount;

      newDirections.push_back(unexploredFaces.front().getDir());
      newLevels.push_back(unexploredFaces.front().getLevels());
      unexploredFaces.pop();
      
    } // while !unexploredFaces.empty()

  cout << "Total edge count for iteration " << numIter << ": " << totalEdgeCount << endl << endl;

  // Compute new threat points/faces
  SGTuple newThreatTuple(threatTuple);
  for (int p = 0; p < numPlayers; p++)
    {
      SGPoint threatDir (numPlayers,0.0);
      threatDir[p] = -1.0;

      optimizePolicy(pivot,actionTuple,regimeTuple,threatDir,
		     actions,feasibleTuple);
      for (int s = 0; s < numStates; s++)
	newThreatTuple[s][p] = pivot[s][p];

      SGProductPolicy threatFace (numStates,threatDir);
      computeOptimalPolicies(threatFace,pivot,threatDir,actions);
      newDirections.push_back(threatDir);
      newLevels.push_back(threatFace.getLevels());
      
      if (env.getParam(SG::STOREITERATIONS))
	iter.push_back(SGStep(actionTuple,regimeTuple,pivot,
			      SGHyperplane(threatDir,
					   threatFace.getLevels()),
			      actions));
    }

  // Recompute the error level
  errorLevel = 0;
  {
    // Rather heavy handed, but do this for now.

    auto dir0 = directions.cbegin(), dir1 = newDirections.cbegin();
    auto lvl0 = levels.cbegin(), lvl1 = newLevels.cbegin();
    while (dir1 != newDirections.cend()
	   && lvl1 != newLevels.cend())
      {
	dir0 = directions.cbegin();
	lvl0 = levels.cbegin();
	double minDist = numeric_limits<double>::max();
	while (dir0 != directions.cend()
	       && lvl0 != levels.cend() )
	  {
	    double tmp = 0;
	    for (int state = 0; state < numStates; state++)
	      tmp = max(tmp,abs((*lvl0)[state]-(*lvl1)[state]));
		  
	    minDist = min(minDist,SGPoint::distance(*dir0,*dir1)+tmp);
		  
	    dir0++;
	    lvl0++;
	  }
	errorLevel = max(errorLevel,minDist);
	      
	dir1++;
	lvl1++;
      }
    dir0 = directions.cbegin(); dir1 = newDirections.cbegin();
    lvl0 = levels.cbegin(); lvl1 = newLevels.cbegin();
    while (dir0 != directions.cend()
	   && lvl0 != levels.cend())
      {
	dir1 = newDirections.cbegin();
	lvl1 = newLevels.cbegin();
	double minDist = numeric_limits<double>::max();
	while (dir1 != newDirections.cend()
	       && lvl1 != newLevels.cend() )
	  {
	    double tmp = 0;
	    for (int state = 0; state < numStates; state++)
	      tmp = max(tmp,abs((*lvl0)[state]-(*lvl1)[state]));
		  
	    minDist = min(minDist,SGPoint::distance(*dir0,*dir1)+tmp);
		  
	    dir1++;
	    lvl1++;
	  }
	errorLevel = max(errorLevel,minDist);
	      
	dir0++;
	lvl0++;
      }
    errorLevel = max(errorLevel,SGTuple::distance(threatTuple,newThreatTuple));
  } // Compute new error level
  
  if (env.getParam(SG::STOREITERATIONS)==2
      || (env.getParam(SG::STOREITERATIONS)==1
	  && ( errorLevel < env.getParam(SG::ERRORTOL)
	       || numIter+1 >= env.getParam(SG::MAXITERATIONS) ) ) )
    soln.push_back(iter); // Important to do this before updating the threat point and minIC of the actions

  findFeasibleTuple(feasibleTuple,actions);
      
  // Update the the threat tuple, directions, levels
  threatTuple = newThreatTuple;
  directions = newDirections;
  levels = newLevels;
      
  // Recalculate minimum IC continuation payoffs
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  ait->calculateMinIC(game,threatTuple);
	  ait->resetTrimmedPoints(payoffUB);

	  // Trim the actions
	  auto dir = directions.cbegin();
	  auto lvl = levels.cbegin();
	  while (dir != directions.cend()
		 && lvl != levels.cend())
	    {
	      // Trim the action
	      double expLevel = 0;
	      for (int sp = 0; sp < numStates; sp++)
		expLevel += probabilities[state][ait->getAction()][sp]
		  * (*lvl)[sp];

	      ait->trim(*dir,expLevel);

	      dir++;
	      lvl++;
	    } // for dir, lvl
	  ait->updateTrim();
	  // ait->mergeDuplicatePoints(1e-8);

	  // Delete the action if not supportable
	  if (!(ait->supportable(feasibleTuple.expectation(probabilities[state]
							   [ait->getAction()]))))
	    {
	      actions[state].erase(ait++);
	      continue;
	    }
	} // for ait
	  
    } // for state

  return errorLevel;
  
} // iterate_endogenous

void SGSolver_MaxMinMax_3Player::initialize()
{
  errorLevel = 1;
  numIter = 0;
  
  game.getPayoffBounds(payoffUB,payoffLB);

  threatTuple = SGTuple(numStates,payoffLB);

  // Clear the solution
  soln.clear();
  directions.clear();
  levels.clear();
  
  // Initialize actions with a big box as the feasible set
  actions.clear();
  actions = vector< list<SGAction_MaxMinMax> > (numStates);

  int numActions_grandTotal = 0;
  int numEqActions_grandTotal = 0;
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = eqActions[state].cbegin();
	   ait != eqActions[state].cend();
	   ait++)
	{
	  actions[state].push_back(SGAction_MaxMinMax(env,numPlayers,state,*ait));
	  actions[state].back().calculateMinIC(game,threatTuple);
	  actions[state].back().resetTrimmedPoints(payoffUB);
	  actions[state].back().updateTrim();
	  // actions[state].back().mergeDuplicatePoints(1e-8);
	}
      numEqActions_grandTotal += eqActions[state].size();
      int numActions_total = 1;
      for (int p = 0; p < numPlayers; p++)
	numActions_total *= game.getNumActions()[state][p];
      numActions_grandTotal += numActions_total;
    } // for state

  cout << "The game has " << numActions_grandTotal << " total action profiles across all states, of which " << numEqActions_grandTotal << " are allowed in equilibrium." << endl;
} // initialize

void SGSolver_MaxMinMax_3Player::optimizePolicy(SGTuple & pivot,
						vector<SGActionIter> & actionTuple,
						vector<SG::Regime> & regimeTuple,
						const SGPoint & currDir,
						const vector<list<SGAction_MaxMinMax> > & actions,
						const SGTuple & feasibleTuple) const
{
  // TODO Breaks if the initial pivot is outside the feasible set.

  // Do policy iteration to find the optimal pivot.
  
  double pivotError = 1.0;
  int numPolicyIters = 0;
  // Iterate until convergence
  // SGTuple newPivot(numStates,SGPoint(3,0.0));
  SGTuple newPivot(pivot);

  vector<SGActionIter> newActionTuple(actionTuple);
  vector<SG::Regime> newRegimeTuple(regimeTuple);

  vector<bool> bestAPSNotBinding(numStates,false);
  SGTuple bestBindingPayoffs(numStates,SGPoint(3,0.0));

  // policy iteration
  do
    {
      pivotError = 0;

      // Look in each state for improvements
      for (int state = 0; state < numStates; state++)
	{
	  double bestLevel = -numeric_limits<double>::max();

	  if (numPolicyIters > 0)
	    bestLevel = pivot[state]*currDir;
	  
	  for (auto ait = actions[state].cbegin();
	       ait != actions[state].cend();
	       ++ait)
	    {
	      int actionIndex = ait->getAction();
	      
	      // Procedure to find an improvement to the policy
	      // function
	      SGPoint nonBindingPayoff = payoffs[state][ait->getAction()];
	      nonBindingPayoff *= (1-delta);
	      nonBindingPayoff.plusWithWeight(pivot.expectation(probabilities[state][ait->getAction()]),delta);

	      bool APSNotBinding = false;
	      SGPoint bestAPSPayoff(numPlayers,0.0);

	      // Find which payoff is highest in current normal and
	      // break ties in favor of the clockwise 90 degree.
	      int bestBindingPlayer = -1;
	      int bestBindingPoint = -1;
	      double bestBindLvl = -numeric_limits<double>::max();
	      for (int p = 0; p < numPlayers; p++)
		{
		  for (int k = 0; k < ait->getPoints()[p].size(); k++)
		    {
		      double tmpLvl = ait->getPoints()[p][k]*currDir;
		      if (tmpLvl > bestBindLvl)
			{
			  bestBindLvl = tmpLvl;
			  bestBindingPlayer = p;
			  bestBindingPoint = k;
			}
		    } // point
		} // player

		  
	      if (bestBindingPlayer >= 0
		  && (ait->getBndryDir(bestBindingPlayer,bestBindingPoint)
		       *currDir > env.getParam(SG::ICTOL) ) // Can improve on the best
		  // binding payoff by moving in
		  // along the frontier
		  )
		{
		  APSNotBinding = true;
		}
	      else if (bestBindingPlayer >= 0)// Found a binding payoff
		bestAPSPayoff =  (1-delta)*payoffs[state][ait->getAction()]
		  + delta * ait->getPoints()[bestBindingPlayer][bestBindingPoint];

	      if ( APSNotBinding // NB bestAPSPayoff has only been
		   // set if bestAPSNotBinding == false
		   || (bestAPSPayoff*currDir >= nonBindingPayoff*currDir) )
		{
		  // ok to use non-binding payoff
		  if (nonBindingPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = nonBindingPayoff*currDir;
		      
		      bestAPSNotBinding[state] = APSNotBinding;
		      if (!APSNotBinding)
			{
			  bestBindingPayoffs[state] = bestAPSPayoff;
			}
		      
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::NonBinding;
	      	      newPivot[state] = nonBindingPayoff;
		      
		    }
		}
	      else if (bestBindingPlayer >= 0
		       && bestAPSPayoff*currDir < nonBindingPayoff*currDir)
		{
		  if (bestAPSPayoff*currDir > bestLevel)
		    {
	      	      bestLevel = bestAPSPayoff*currDir;
		      newActionTuple[state] = ait;
	      	      newRegimeTuple[state] = SG::Binding;
	      	      newPivot[state] = bestAPSPayoff;
		    }
		}
	    } // ait

	  assert(newPivot[state].size() == numPlayers);
	  assert(pivot[state].size() == numPlayers);

	  pivotError = max(pivotError,abs(bestLevel-pivot[state]*currDir));
	} // state

      pivot = newPivot;
      actionTuple = newActionTuple;
      regimeTuple = newRegimeTuple;
      
      // Inner loop to fix regimes
      bool anyViolation = false;
      do
	{
	  // Do Bellman iteration to find new fixed point
	  policyToPayoffs(pivot,actionTuple,regimeTuple);

	  anyViolation = false;

	  // Check if any reversals between best binding and non-binding

	  // First determine the maximum gap
	  vector<double> gaps(numStates,0);
	  double maxGap = 0;
	  for (int state = 0; state < numStates; state++)
	    {
	      if (!bestAPSNotBinding[state]
		  && regimeTuple[state] == SG::NonBinding)
		{
		  gaps[state] = pivot[state]*currDir - bestBindingPayoffs[state]*currDir;
		  if (gaps[state] > maxGap)
		    {
		      anyViolation = true;

		      maxGap = gaps[state];
		    }
		}
	    }
	  
	  for (int state = 0; state < numStates; state++)
	    {
	      if (!bestAPSNotBinding[state]
		  && regimeTuple[state] == SG::NonBinding
		  && gaps[state] >= delta*maxGap)
		{
		  pivot[state] = bestBindingPayoffs[state];
		  regimeTuple[state] = SG::Binding;
		}
	      assert(pivot[state].size() == numPlayers);
	    }
	} while (anyViolation);


      if (numPolicyIters == env.getParam(SG::MAXPOLICYITERATIONS)/2)
	cout << "Cycling detected at direction: " << currDir << endl;
      if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS)/2)
	{
	  cout << "Policy iter: " << numPolicyIters
	       << ", action tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << actionTuple[state]->getAction() << " ";
	  cout << "), regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << regimeTuple[state] << " ";
	  cout << "), new regime tuple: (";
	  for (int state = 0; state < numStates; state++)
	    cout << newRegimeTuple[state] << " ";
	  cout << "), pivot error: " << scientific << pivotError;
	  cout << endl;
	}

    } while (pivotError > env.getParam(SG::POLICYITERTOL)
	     && ++numPolicyIters < env.getParam(SG::MAXPOLICYITERATIONS));

  if (numPolicyIters >= env.getParam(SG::MAXPOLICYITERATIONS))
    cout << "WARNING: Maximum policy iterations reached." << endl;

} // optimizePolicy

bool SGSolver_MaxMinMax_3Player::computeOptimalPolicies(SGProductPolicy & optPolicies,
							const SGTuple & pivot,
							const SGPoint & currDir,
							const vector<list<SGAction_MaxMinMax> > & actions) const
{
  vector<bool> bestAPSNotBinding(numStates,false);

  const double improveTol = 1e-10;
  const double optTol = 1e-12;
  
  optPolicies.clear();

  // Find optimal substitutions
  for (int state = 0; state < numStates; state++)
    {
      double bestLevel = pivot[state]*currDir;
      optPolicies.setLevel(state,bestLevel);
      
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ++ait)
	{
	  // Procedure to find an improvement to the policy
	  // function
	  double payoffLvl = (1-delta)*(currDir*payoffs[state][ait->getAction()]);
	  double expPivotLvl = delta*(currDir*pivot.expectation(probabilities[state][ait->getAction()]));
	  double nonBindingLvl = payoffLvl + expPivotLvl;

	  bool APSNotBinding = false;

	  // Find which payoff is highest in current normal and
	  // break ties in favor of the clockwise 90 degree.
	  int bestBindingPlayer = -1;
	  int bestBindingPoint = -1;
	  double bestBindLvl = -numeric_limits<double>::max();
	  for (int p = 0; p < numPlayers; p++)
	    {
	      for (int k = 0; k < ait->getPoints()[p].size(); k++)
		{
		  double bindLvl = payoffLvl + delta*(ait->getPoints()[p][k]*currDir);
		  if (bindLvl > bestBindLvl)
		    {
		      bestBindLvl = bindLvl;
		      bestBindingPlayer = p;
		      bestBindingPoint = k;
		    }

		  if (bindLvl <= nonBindingLvl+1e-10)
		    {
		      if (bindLvl - bestLevel > improveTol)
			return false;
		      if (bindLvl-bestLevel > -optTol)
			{
			  optPolicies.insertPolicy(state,
						   SGPolicy(ait,SG::Binding,p,k));
			}
		    }
		  
		} // point
	    } // player
	  
	  if (bestBindingPlayer < 0 // didn't find a binding payoff
	      || (ait->getBndryDir(bestBindingPlayer,bestBindingPoint)
		  *currDir > env.getParam(SG::ICTOL) ) // Can improve
							// on the best
							// binding
							// payoff by
							// moving in
							// along the
							// frontier
	      )
	    {
	      APSNotBinding = true;
	    }

	  if ( APSNotBinding // NB bestAPSPayoff has only been
	       // set if bestAPSNotBinding ==
	       // false
	       || (nonBindingLvl <= bestBindLvl + 1e-10) ) 
	    {
	      // ok to use non-binding payoff
	      if (nonBindingLvl-bestLevel > improveTol)
		return false;
	      else if (nonBindingLvl-bestLevel>-optTol)
		{
		  bestAPSNotBinding[state] = APSNotBinding;
		  optPolicies.insertPolicy(state,
					   SGPolicy(ait,SG::NonBinding));

		}
	    }
	} // ait
    } // state

  return true;
} // computeOptimalPolicies

double SGSolver_MaxMinMax_3Player::sensitivity(SGPoint & optSubDir,
					       const SGTuple & pivot,
					       const SGPoint & currDir,
					       const SGPoint & newDir,
					       const vector<list<SGAction_MaxMinMax> > & actions) const
{
  const double minIndiffLvl = 1e-13; // A level of 1e-6 eliminates
				    // spurious edges on the
				    // contribution examples for
				    // delta=1/3,2/3
  
  double nonBindingIndiffLvl = -1;
  double bindingIndiffLvl = -1;
  double bestLevel = numeric_limits<double>::max();
  bool availSubFound = false;

  // Look in each state for improvements
  for (int state = 0; state < numStates; state++)
    {
      for (auto ait = actions[state].cbegin();
	   ait != actions[state].cend();
	   ++ait)
	{
	  // Find the smallest weight on newDir such that this action
	  // improves in that direction. 

	  // SGPoint nonBindingPayoff = (1-delta)*payoffs[state]
	  //   [ait->getAction()]
	  //   + delta * pivot.expectation(probabilities[state][ait->getAction()]);
	  SGPoint nonBindingPayoff(3,0.0);
	  nonBindingPayoff.plusWithWeight(payoffs[state][ait->getAction()],
					  1.0-delta);
	  nonBindingPayoff.plusWithWeight(pivot.expectation(probabilities[state][ait->getAction()]),delta);

	  // Calculate the lvl at which indifferent to the pivot
	  double denom = newDir*nonBindingPayoff-newDir*pivot[state];
	  double numer = (pivot[state]*currDir-nonBindingPayoff*currDir);
	  if (abs(denom) > env.getParam(SG::NORMTOL))
	    {
	      nonBindingIndiffLvl = numer/denom;

	      if (nonBindingIndiffLvl < bestLevel
		  && nonBindingIndiffLvl > minIndiffLvl)
		{
		  SGPoint indiffDir = currDir;
		  indiffDir.plusWithWeight(newDir,nonBindingIndiffLvl);

		  // See if a binding payoff is higher in the
		  // indifference direction
		  double bestBindLvl = -numeric_limits<double>::max();
		  int bestBindingPlayer = -1;
		  int bestBindingPoint = 0;
		  for (int p = 0; p < numPlayers; p++)
		    {
		      for (int k = 0; k < ait->getPoints()[p].size(); k++)
			{
			  double tmpLvl = ait->getPoints()[p][k]*indiffDir;
			  if (tmpLvl > bestBindLvl)
			    {
			      bestBindLvl = tmpLvl;
			      bestBindingPlayer = p;
			      bestBindingPoint = k;
			    }
			} // point
		    } // player

		  bool bestAPSNotBinding = false;
		  if (bestBindingPlayer < 0 // didn't find a binding payoff
		      || (ait->getBndryDir(bestBindingPlayer,bestBindingPoint)
			  *indiffDir > env.getParam(SG::IMPROVETOL))
		      )
		    {
		      // Can improve on the best binding payoff by
		      // moving in along the frontier
		      bestAPSNotBinding = true;
		    }

		  if ( bestAPSNotBinding // NB bestAPSPayoff has only been
		       // set if bestAPSNotBinding == true
		       || bestBindLvl > nonBindingPayoff*indiffDir 
		       -env.getParam(SG::IMPROVETOL))
		    {
		      // If we get to here, non-binding regime is
		      // available in the indifferent direction, and
		      // this direction is smaller than the best level
		      // found so far.

		      availSubFound = true;
		      bestLevel = nonBindingIndiffLvl;
		      optSubDir = nonBindingPayoff - pivot[state];
		    }
		} // Non-binding indifference level is smaller than best level
	    } // Positive level of indifference

	  double bestBindIndiffLvl = numeric_limits<double>::max();
	  SGPoint bestBindPnt(numPlayers,0.0);

	  // Now check the binding directions
	  for (int p = 0; p < numPlayers; p++)
	    {
	      for (int k = 0; k < ait->getPoints()[p].size(); k++)
		{
		  SGPoint bindingPayoff = payoffs[state][ait->getAction()];
		  bindingPayoff *= 1.0-delta;
		  bindingPayoff.plusWithWeight(ait->getPoints()[p][k],delta);
		  double denom = newDir*bindingPayoff-newDir*pivot[state];
		  double numer = pivot[state]*currDir-bindingPayoff*currDir;
		  if (abs(denom) > env.getParam(SG::NORMTOL))
		    {
		      
		      bindingIndiffLvl = numer/denom;

		      if (bindingIndiffLvl < bestLevel
			  && bindingIndiffLvl > minIndiffLvl)
			{
			  SGPoint indiffDir = currDir + newDir * bindingIndiffLvl;

			  bool isBestBinding = true;
			  for (int pp = 0; pp < numPlayers; pp++)
			    {
			      for (int kp = 0; kp < ait->getPoints()[pp].size(); kp++)
				{
				  if (ait->getPoints()[pp][kp]*indiffDir
				      > ait->getPoints()[p][k]*indiffDir
				      +env.getParam(SG::IMPROVETOL)) 
				    isBestBinding = false;
				}
			    }

			  if (isBestBinding
			      && (nonBindingPayoff*indiffDir
				  >= bindingPayoff*indiffDir
				  -env.getParam(SG::IMPROVETOL)))
			    {
			      availSubFound = true;
			      bestLevel = bindingIndiffLvl;
			      optSubDir = bindingPayoff - pivot[state];

			      bestBindIndiffLvl = bindingIndiffLvl;
			      bestBindPnt = bindingPayoff;
			    } // Binding payoff is available
			} // Smaller than the current bestLvl
		    } // Denominator is positive
		} // point
	    } // player

	} // ait

    } // state

  // if (!availSubFound)
  //   cout << "WARNING: Sensitivity analysis did not find an available substitution." << endl;

  return std::max(bestLevel,0.0);

} // sensitivity


void SGSolver_MaxMinMax_3Player::findFeasibleTuple(SGTuple & feasibleTuple,
				    const vector<list<SGAction_MaxMinMax> > & actions) const
{
  // Update the APS-feasible tuple

  // These are just in case we cannot find binding APS payofs. NB:
  // This is not the same actionTuple and regimeTuple that determine
  // the pivot.
  vector<SGActionIter> actionTuple(numStates);
  vector<SG::Regime> regimeTuple(numStates,SG::Binding);

  bool anyNonBinding = false;
  for (int state = 0; state < numStates; state++)
    {
      // Search for an action with feasible binding continuations
      // and just pick one. If we don't find any action with
      // feasible binding continuations, it either means (i) the
      // game has no pure strategy SPNE, or (ii) any feasible
      // payoff tuple (within the set of remaining actions) is an
      // APS payoff.
      bool foundFeasiblePlayer = false;

      if (actions[state].size()==0)
	throw(SGException(SG::NOACTIONS));
	
      for (auto ait = actions[state].begin();
	   ait != actions[state].end();
	   ait++)
	{
	  int feasiblePlayer = -1;
	  for (int player = 0; player < numPlayers; player++)
	    {
	      if (ait->getPoints()[player].size()>0)
		{
		  feasiblePlayer = player;
		  break;
		}
	    }
	  if (feasiblePlayer >= 0)
	    {
	      feasibleTuple[state] = (1-delta)*payoffs[state][ait->getAction()]
		+delta*ait->getPoints()[feasiblePlayer][0];
	      foundFeasiblePlayer = true;
	      break;
	    }
	} // for ait
      if (!foundFeasiblePlayer)
	{
	  regimeTuple[state] = SG::NonBinding;
	  actionTuple[state] = actions[state].begin();
	  anyNonBinding = true;
	}
    } // for state

  bool notAllIC = anyNonBinding;
  while (notAllIC)
    {
      // Have to do Bellman iteration to find the new pivot.
      policyToPayoffs(feasibleTuple,actionTuple,regimeTuple);
      notAllIC = false;
      
      // Check if resulting tuple is IC.
      for (int state = 0; state < numStates; state++)
	{
	  if (!(feasibleTuple.expectation(probabilities[state]
					  [actionTuple[state]->getAction()])
		>= actionTuple[state]->getMinICPayoffs()))
	    {
	      notAllIC = true;
	      // Try advancing the action and recomputing
	      if ((++actionTuple[state])==actions[state].end())
		{
		  throw(SGException(SG::NOFEASIBLETUPLE));
		}
	    }
	} // for state
    } // if anyNonBinding

} // findFeasibleTuple

void SGSolver_MaxMinMax_3Player::policyToPayoffs(SGTuple & pivot,
						 const vector<SGActionIter> & actionTuple,
						 const vector<SG::Regime> & regimeTuple) const
{
  // Do Bellman iteration to find new fixed point
  int updatePivotPasses = 0;
  double bellmanPivotGap = 0;
  SGTuple newPivot(pivot);

  do
    {
      for (int state = 0; state < numStates; state++)
	{
	  if (regimeTuple[state] == SG::NonBinding)
	    {
	      newPivot[state] = payoffs[state][actionTuple[state]->getAction()];
	      newPivot[state] *= 1.0-delta;
	      newPivot[state].plusWithWeight(pivot.expectation(probabilities[state]
							       [actionTuple[state]->getAction()]),
					     delta);
	    }
	}
      bellmanPivotGap = SGTuple::distance(newPivot,pivot);
      pivot = newPivot;
    } while (bellmanPivotGap > env.getParam(SG::UPDATEPIVOTTOL)
	     && (++updatePivotPasses < env.getParam(SG::MAXUPDATEPIVOTPASSES) ));
  
  if (updatePivotPasses == env.getParam(SG::MAXUPDATEPIVOTPASSES) )
    cout << "WARNING: Maximum pivot update passes reached." << endl;
  
} // policyToPayoffs
