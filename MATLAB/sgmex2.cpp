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

#include "mex.h"
#include "matrix.h"
#include <cstring>
#include <fstream>
#include <string>
#include <cmath>
#include "sgsolution_maxminmax.hpp"

SGSolution_MaxMinMax soln;
list<SGIteration_MaxMinMax>::const_iterator currentIteration;
int currentIterationIndex;
int numStates;
int numPlayers;
bool solnLoaded;

void mexFunction(int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{
  char * optionStr, * inputFileName, *outputFileName, *paramStr;
  double *outputPtr, *inputPtr;
  vector <double*> outputPtrs(2);
  bool *logicalPtr;
  mwIndex cellCounter;
  int state, action, player,
    rows, cols, outputCounter;
  vector< vector<int> > numActions;
  vector<SGPoint> payoffs;
  vector< vector<double> > probabilities;
  vector<string> fieldNames;
	
  // Check inputs
  if (nrhs<1)
    mexErrMsgTxt("At least one input argument required.");
  if (!mxIsChar(prhs[0]))
    mexErrMsgTxt("First argument must be a character array.");
	
  // First argument is always an option string.
  optionStr = mxArrayToString(prhs[0]);
	
  // Load
  if (!strcmp(optionStr, "LoadSolution")) {
    // ppemex('Load',filename)
    if (nrhs<2)
      mexErrMsgTxt("Must provide input file name.");
    if (!mxIsChar(prhs[1]))
      mexErrMsgTxt("Input file name must be a character array.");
		
    inputFileName = mxArrayToString(prhs[1]);
		
    try
      {
	SGSolution_MaxMinMax::load(soln, inputFileName);
	currentIterationIndex = 0;
	currentIteration = soln.getIterations().begin();

	numStates = soln.getGame().getNumStates();
	numPlayers = soln.getGame().getNumPlayers();
	
	solnLoaded = true;
      }
    catch (SGException & e)
      {
	if (e.getType() == SG::FAILED_OPEN)
	  mexErrMsgTxt("Could not open file");
      }
    catch (exception & e)
      {
	cout << e.what() << endl;
	mexErrMsgTxt("Caught boost archive exception. Retrieved preceding error message.");
      }
    catch(...)
      {
	mexErrMsgTxt("Unknown exception caught");
      }
  } // LoadSolution
	
  // SolutionIsLoaded
  else if (!strcmp(optionStr,"SolutionIsLoaded"))
    {
      if (nlhs != 1)
	mexErrMsgTxt("Must have one output argument");
		
      plhs[0] = mxCreateLogicalScalar(solnLoaded);
		
    } // SolutionIsLoaded

  else
    {
      if (!solnLoaded)
	mexErrMsgTxt("No soln loaded.");
		
      // GetDelta
      else if (!strcmp(optionStr,"GetDelta"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputPtr[0] = soln.getGame().getDelta();
	} // GetDelta
		
      // GetNumStates
      else if (!strcmp(optionStr,"GetNumStates"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputPtr[0] = numStates;
	} // GetNumStates
		
      // GetNumActions
      else if (!strcmp(optionStr,"GetNumActions"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  plhs[0] = mxCreateDoubleMatrix(numStates,
					 numPlayers,mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputCounter = 0;
			
	  numActions = soln.getGame().getNumActions();
			
	  for (player = 0; player < numPlayers; player++)
	    {
	      for (state = 0; state < numStates; state++)
		{
		  outputPtr[outputCounter] = numActions[state][player];
		  outputCounter++;
		} // state
	    } // player
	} // GetNumActions
		
      // GetProbabilities
      else if (!strcmp(optionStr,"GetProbabilities"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  else if (nrhs!=2)
	    mexErrMsgTxt("One argument required.");
	  else if (!mxIsDouble(prhs[1]))
	    mexErrMsgTxt("Inputs must be doubles.");
			
	  rows = mxGetM(prhs[1]); cols = mxGetM(prhs[1]);
	  inputPtr = mxGetPr(prhs[1]);
			
	  if (rows!=1 || cols!=1)
	    mexErrMsgTxt("Input must be scalar double.");
	  state = static_cast<int>(inputPtr[0]);
			
	  if (state < 0 || state > numStates-1)
	    mexErrMsgTxt("State is out of range.");
	  probabilities =
	    vector< vector<double > >(soln.getGame().getProbabilities()[state]);
			
	  plhs[0] = mxCreateDoubleMatrix(probabilities.size(),
					 probabilities[0].size(),mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputCounter = 0;
	  for (state = 0; state < numStates; state++)
	    {
	      for (action = 0; action < probabilities.size(); action++)
		{
		  outputPtr[outputCounter] = probabilities[action][state];
					
		  outputCounter++;
		} // state
	    } // action
	} // GetProbabilities
		
      // GetPayoffs
	  else if (!strcmp(optionStr,"GetPayoffs"))
	{
	  double * outputPtr0;
			
	  if (nlhs!=1)
	    mexErrMsgTxt("Must have one output.");
	  else if (nrhs!=2)
	    mexErrMsgTxt("One arguments required, state.");
	  else if (!mxIsDouble(prhs[1]))
	    mexErrMsgTxt("Inputs must be doubles.");
			
	  rows = mxGetM(prhs[1]); cols = mxGetM(prhs[1]);
	  inputPtr = mxGetPr(prhs[1]);
	  if (rows!=1 || cols!=1)
	    mexErrMsgTxt("Input must be scalar double.");
	  state = static_cast<int>(inputPtr[0]);
	  if (state < 0 || state > numStates-1)
	    mexErrMsgTxt("State is out of range.");

	  // int player = static_cast<int>(inputPtr[1]);
	  // if (player < 0 || player > numPlayers-1)
	  //   mexErrMsgTxt("Player is out of range.");
			
	  numActions = soln.getGame().getNumActions();
	  payoffs = vector<SGPoint>(soln.getGame().getPayoffs()[state]);

	  int numActions_total = 1;
	  for (int p = 0; p < numPlayers; p++)
	    numActions_total *= numActions[state][p];
	  plhs[0] = mxCreateDoubleMatrix(numActions_total,numPlayers,mxREAL);
			
	  outputPtr0 = mxGetPr(plhs[0]);
	  outputCounter = 0;
	  for (int p = 0; p < numPlayers; p++)
	    {
	      for (int a = 0; a < numActions_total; a++)
		{
		  outputPtr0[outputCounter]
		    = payoffs[a][p];
					
		  outputCounter++;
		} // action
	    } // player
	} // GetPayoffs
		
      // Iter++
      else if (!strcmp(optionStr,"Iter++"))
	{
	  if (nlhs!=0)
	    mexErrMsgTxt("No outputs supported.");
	  else if (nrhs!=1 && nrhs!=2)
	    mexErrMsgTxt("Only zero or one arguments supported.");
			
	  currentIteration++;
	  if (currentIteration == soln.getIterations().end())
	    currentIteration--;
	  else
	    currentIterationIndex++;
	} // Iter++
		
      // Iter--
      else if (!strcmp(optionStr,"Iter--"))
	{
	  if (nlhs!=0)
	    mexErrMsgTxt("No outputs supported.");
	  else if (nrhs!=1 && nrhs!=2)
	    mexErrMsgTxt("Only zero or one arguments supported.");
			
	  if (currentIteration != soln.getIterations().begin())
	    {
	      currentIteration--;
	      currentIterationIndex--;
	    }
	} // Iter--
		
      // IterToBeginning
      else if (!strcmp(optionStr,"IterToBeginning"))
	{
	  if (nlhs!=0)
	    mexErrMsgTxt("No outputs supported.");
			
	  currentIteration = soln.getIterations().begin();
	  currentIterationIndex = 0;
	} // IterToBeginning
		
      // IterToEnd
      else if (!strcmp(optionStr,"IterToEnd"))
	{
	  if (nlhs!=0)
	    mexErrMsgTxt("No outputs supported.");
			
	  currentIteration = soln.getIterations().end();
	  currentIteration--;
	  currentIterationIndex = soln.getIterations().size()-1;
	} // IterToEnd
		
      // GetCurentIteration
      else if (!strcmp(optionStr,"GetCurrentIteration"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
			
	  mxArray *fieldOutput;
	  SGPoint minICPayoffs, payoffs;
	  int nFields = 6;
	  const char *fieldNames[] = {"pivots",
				      "directions",
				      "levels",
				      "actionTuples",
				      "regimeTuples",
				      "actions"};
	  
	  plhs[0] = mxCreateStructMatrix(1,1,nFields,fieldNames);
			
	  int currentField = 0;

	  // Pivots
	  fieldOutput = mxCreateDoubleMatrix(currentIteration->getSteps().size(),
					     numPlayers*numStates,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (state=0; state<numStates; state++)
	    {
	      for (player=0; player<numPlayers; player++)
		{
		  for (list<SGStep>::const_iterator step = currentIteration->getSteps().cbegin();
		       step != currentIteration->getSteps().cend();
		       step++)
		    {
		      outputPtr[outputCounter] = step->getPivot()[state][player];
		      outputCounter++;
		    }
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
						
	  // Directions
	  fieldOutput = mxCreateDoubleMatrix(currentIteration->getSteps().size(),
					     numPlayers,
					     mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (player=0; player<numPlayers; player++)
	    {
	      for (list<SGStep>::const_iterator step = currentIteration->getSteps().cbegin();
		   step != currentIteration->getSteps().cend();
		   step++)
		{
		  outputPtr[outputCounter] = step->getHyperplane().getNormal()[player];
		  outputCounter++;
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
	  
	  // Levels
	  fieldOutput = mxCreateDoubleMatrix(currentIteration->getSteps().size(),
					     numStates,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (state=0; state<numStates; state++)
	    {
	      for (list<SGStep>::const_iterator step = currentIteration->getSteps().cbegin();
		   step != currentIteration->getSteps().cend();
		   step++)
		{
		  outputPtr[outputCounter] = step->getHyperplane().getLevels()[state];
		  outputCounter++;
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // Action tuples
	  fieldOutput = mxCreateDoubleMatrix(currentIteration->getSteps().size(),
					     numStates,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (state=0; state<numStates; state++)
	    {
	      for (list<SGStep>::const_iterator step = currentIteration->getSteps().cbegin();
		   step != currentIteration->getSteps().cend();
		   step++)
		{
		  outputPtr[outputCounter] = step->getActionTuple()[state];
		  outputCounter++;
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // Regime tuples
	  fieldOutput = mxCreateDoubleMatrix(currentIteration->getSteps().size(),
					     numStates,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (state=0; state<numStates; state++)
	    {
	      for (list<SGStep>::const_iterator step = currentIteration->getSteps().cbegin();
		   step != currentIteration->getSteps().cend();
		   step++)
		{
		  if (step->getRegimeTuple()[state]==SG::NonBinding)
		    outputPtr[outputCounter] = 1;
		  else
		    outputPtr[outputCounter] = 0;
		  outputCounter++;
		} // step
	    } // state
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);

	  // Actions
	  fieldOutput = mxCreateCellMatrix(numStates,1);
	  int nActionFields = 4;
	  const char *actionFieldNames[] = {"state",
					    "action",
					    "minIC",
					    "points"};
	  for (state = 0; state < numStates; state++)
	    {
	      mxArray * stateActionsPtr
		= mxCreateCellMatrix(currentIteration->getActions()[state].size(),1);
	      int actionCtr = 0;
	      for (auto ait = currentIteration->getActions()[state].cbegin();
		   ait != currentIteration->getActions()[state].cend();
		   ait ++)
		{
		  mxArray * actionStructPtr
		    = mxCreateStructMatrix(1,1,nActionFields,
					   actionFieldNames);
		  int actionField = 0;
		  mxArray * actionFieldOutput;

		  // State
		  actionFieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
		  outputPtr = mxGetPr(actionFieldOutput);
		  outputPtr[0] = ait->getState();
		  mxSetFieldByNumber(actionStructPtr,0,actionField++,
				     actionFieldOutput);
		  
		  // Action
		  actionFieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
		  outputPtr = mxGetPr(actionFieldOutput);
		  outputPtr[0] = ait->getAction();;
		  mxSetFieldByNumber(actionStructPtr,0,actionField++,
				     actionFieldOutput);
		  
		  // minIC
		  actionFieldOutput = mxCreateDoubleMatrix(numPlayers,1,mxREAL);
		  outputPtr = mxGetPr(actionFieldOutput);
		  for (player = 0; player < numPlayers; player++)
		    outputPtr[player] = ait->getMinICPayoffs()[player];
		  mxSetFieldByNumber(actionStructPtr,0,actionField++,
				     actionFieldOutput);
		  
		  // Points

		  // A cell array, with one cell for each player. Each
		  // cell contains the tuple of extreme binding
		  // payoffs for that player. The tuple is represented
		  // as a k * n matrix where k is the number of
		  // extreme points and n is the number of players.
		  actionFieldOutput = mxCreateCellMatrix(numPlayers,1);
		  for (player = 0; player < numPlayers; player++)
		    {
		      // Create point matrix for each player
		      mxArray * extPntsPtr
			= mxCreateDoubleMatrix(ait->getPoints()[player].size(),
					       numPlayers,mxREAL);
		      outputPtr = mxGetPr(extPntsPtr);
		      outputCounter = 0;
		      for (int p = 0; p < numPlayers; p++)
			{
			  for (int ep = 0; ep < ait->getPoints()[player].size();
			       ep++)
			    {
			      outputPtr[outputCounter]
				= ait->getPoints()[player][ep][p];
			      outputCounter++;
			    } // extreme point
			} // player
		      
		      mxSetCell(actionFieldOutput,player,extPntsPtr);
		    }
		  mxSetFieldByNumber(actionStructPtr,0,actionField++,
				     actionFieldOutput);

		  // // BndryDirs

		  // actionFieldOutput = mxCreateCellMatrix(numPlayers,1);
		  // for (player = 0; player < numPlayers; player++)
		  //   {
		  //     // Create point matrix for each player
		  //     mxArray * extPntsPtr
		  // 	= mxCreateDoubleMatrix(ait->getBndryDirs()[player].size(),
		  // 			       numPlayers,mxREAL);
		  //     outputPtr = mxGetPr(extPntsPtr);
		  //     outputCounter = 0;
		  //     for (int p = 0; p < numPlayers; p++)
		  // 	{
		  // 	  for (int ep = 0; ep < ait->getPoints()[player].size();
		  // 	       ep++)
		  // 	    {
		  // 	      outputPtr[outputCounter]
		  // 		= ait->getPoints()[player][ep][p];
		  // 	      outputCounter++;
		  // 	    } // extreme point
		  // 	} // player
		      
		  //     mxSetCell(actionFieldOutput,player,extPntsPtr);
		  //   }
		  // mxSetFieldByNumber(actionStructPtr,0,actionField++,
		  // 		     actionFieldOutput);
		  
		  mxSetCell(stateActionsPtr,actionCtr,actionStructPtr);
		  actionCtr++;
		} // ait
	      
	      mxSetCell(fieldOutput,state,stateActionsPtr);
	    } // state
	  
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);

	  
	} // GetCurentIteration
		
      else
	{
	  mexErrMsgTxt("Not a valid option string.");
	}
		
		
    }
	
} // mexFunction
