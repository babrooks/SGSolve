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
#include "sgsolution_v2.hpp"

SGSolution_V2 soln;
list<SGIteration_V2>::const_iterator currentIteration;
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
	SGSolution_V2::load(soln, inputFileName);
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
	  int a0, a1;
	  double * outputPtr0, *outputPtr1;
			
	  if (nlhs!=2)
	    mexErrMsgTxt("Two outputs required.");
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
			
	  numActions = soln.getGame().getNumActions();
	  payoffs = vector<SGPoint>(soln.getGame().getPayoffs()[state]);
			
	  plhs[0] = mxCreateDoubleMatrix(numActions[state][0],
					 numActions[state][1],mxREAL);
	  plhs[1] = mxCreateDoubleMatrix(numActions[state][0],
					 numActions[state][1],mxREAL);
			
	  outputPtr0 = mxGetPr(plhs[0]);
	  outputPtr1 = mxGetPr(plhs[1]);
	  outputCounter = 0;
	  for (a1 = 0; a1 < numActions[state][1]; a1++)
	    {
	      for (a0 = 0; a0 < numActions[state][0]; a0++)
		{
		  outputPtr0[outputCounter]
		    = payoffs[a0+a1*numActions[state][0]][0];
		  outputPtr1[outputCounter]
		    = payoffs[a0+a1*numActions[state][0]][1];
					
		  outputCounter++;
		} // state
	    } // action
			
			
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
	  int nFields = 5;
			
	  const char *fieldNames[] = {"pivots",
				      "directions",
				      "levels",
				      "actions",
				      "regimes"};
	  
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
			
	  // actions
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
			
	  // Regimes
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
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
	  
	} // GetCurentIteration
		
      else
	{
	  mexErrMsgTxt("Not a valid option string.");
	}
		
		
    }
	
} // mexFunction
