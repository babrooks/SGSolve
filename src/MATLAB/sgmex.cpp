#include "mex.h"
#include "matrix.h"
#include <cstring>
#include <fstream>
#include <string>
#include <cmath>
#include "sgsolution.hpp"
#include "sgapprox.hpp"
#include "sgsimulator.hpp"

SGSolution soln;
list<SGIteration>::iterator currentIteration;
int currentIterationIndex;
bool solnLoaded;

bool createGame(int nlhs, mxArray *plhs[],
	      int nrhs, const mxArray *prhs[],
	      SGGame & game);

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
	SGSolution::load(soln, inputFileName);
	currentIterationIndex = 0;
	currentIteration = soln.iterations.begin();
			
	solnLoaded = true;
      }
    catch (SGException & e)
      {
	if (e.getType() == SGException::FAILED_OPEN)
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

  else if ( !strcmp(optionStr,"Simulate") )
    {
      if (!solnLoaded)
	mexErrMsgTxt("Load a solution first.");
      if (nlhs != 3)
	mexErrMsgTxt("Must have three output arguments.");
      if (nrhs != 4)
	mexErrMsgTxt("Must have four arguments.");

      int numIter = mxGetScalar(prhs[1]);
      int initialState = mxGetScalar(prhs[2]);
      int initialTuple = mxGetScalar(prhs[3]);

      if (numIter<=0)
	mexErrMsgTxt("Number of iterations must be at least 1.");
      if (initialState < 0 | initialState >= soln.game.getNumStates())
	mexErrMsgTxt("Initial state is out of bounds.");

      try
	{
	  SGSimulator sim(soln);

	  sim.initialize();

	  if (initialTuple < sim.getStartOfLastRev()
	      | initialTuple > soln.iterations.back().iteration )
	    mexErrMsgTxt("Initial tuple must be in the last iteration.");

	  sim.simulate(numIter,initialState,initialTuple);

	  mexPrintf("Simulation completed.\n");
	  mexEvalString("drawnow;");
	  
	  vector< vector<int> > actionDistr = sim.getActionDistr();
	  plhs[0] = mxCreateCellMatrix(soln.game.getNumStates(),1);
	  for (state = 0; state < actionDistr.size(); state++)
	    {
	      mxArray * distrPtr = mxCreateDoubleMatrix(actionDistr[state].size(),1,mxREAL);
	      outputPtr = mxGetPr(distrPtr);
	      for (action = 0; action < actionDistr[state].size(); action++)
		outputPtr[action] = actionDistr[state][action];
	      mxSetCell(plhs[0],state,distrPtr);
	    } // state
	  plhs[1] = mxCreateDoubleMatrix(sim.getTupleDistr().size(),1,mxREAL);
	  outputPtr = mxGetPr(plhs[1]);
	  for (int tuple = 0; tuple < sim.getTupleDistr().size(); tuple++)
	    outputPtr[tuple] = sim.getTupleDistr()[tuple];

	  SGPoint lrPayoffs = sim.getLongRunPayoffs();
	  plhs[2] = mxCreateDoubleMatrix(1,2,mxREAL);
	  outputPtr = mxGetPr(plhs[2]);
	  outputPtr[0] = lrPayoffs[0];
	  outputPtr[1] = lrPayoffs[1];
	  
	}
      catch (exception e)
	{
	  cout << e.what();
	}
      catch (SGException e)
	{
	  cout << e.what();
	}
      catch (...)
	{
	  mexErrMsgTxt("Simulation failed: Unknown exception caught.");
	}

    } // Simulate

  // SaveGame
  else if (!strcmp(optionStr,"SaveGame"))
    {
      SGGame game;
      if (createGame(nlhs,plhs,nrhs,prhs,game))
	mexErrMsgTxt("Unable to create game.");


      if (nrhs<6)
	mexErrMsgTxt("Must provide an output file name");
      if (!mxIsChar(prhs[5]))
	mexErrMsgTxt("Input file name must be a character array.");
      
      outputFileName = mxArrayToString(prhs[5]);
			
      try
	{
	  SGGame::save(game,outputFileName);
	  mexPrintf("Game saved to %s.\n",outputFileName);
	}
      catch (SGException & e)
	{

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
    } // Save Game
  

  
  // Solve
  else if (!strcmp(optionStr,"Solve"))
    {
      try
	{
	  SGGame game;
	  if (createGame(nlhs,plhs,nrhs,prhs,game))
	    mexErrMsgTxt("Unable to create game.");

	  SGEnv env;

	  vector<bool> unconstrained(2,false);

	  for (int k=5; k<nrhs; k+=2)
	    {
	      paramStr = mxArrayToString(prhs[k]);
	      if (nrhs<k+1)
		break;
				
	      if (!strcmp(paramStr,"errorTol"))
		env.setParam(SGEnv::ERRORTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"directionTol"))
		env.setParam(SGEnv::DIRECTIONTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"pastThreatTol"))
		env.setParam(SGEnv::PASTTHREATTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"updatePivotTol"))
		env.setParam(SGEnv::UPDATEPIVOTTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"ICTol"))
		env.setParam(SGEnv::ICTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"normTol"))
		env.setParam(SGEnv::NORMTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"flatTol"))
		env.setParam(SGEnv::FLATTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"levelTol"))
		env.setParam(SGEnv::LEVELTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"improveTol"))
		env.setParam(SGEnv::IMPROVETOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"backBendingTol"))
		env.setParam(SGEnv::BACKBENDINGTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"movementTol"))
		env.setParam(SGEnv::MOVEMENTTOL,mxGetScalar(prhs[k+1]));
	      else if (!strcmp(paramStr,"unconstrained"))
		{
		  mxLogical *unconstrPtr = mxGetLogicals(prhs[k+1]);
		  if (!mxIsLogical(prhs[k+1]))
		    mexErrMsgTxt("unconstrained value must be a logical array.");
		  if (mxGetNumberOfElements(prhs[k+1]) != 2)
		    mexErrMsgTxt("unconstrained value must have two elements.");
		  unconstrained[0] = unconstrPtr[0];
		  unconstrained[1] = unconstrPtr[1];

		  game.setConstrained(unconstrained);
		}
	    }

	  env.setParam(SGEnv::PRINTTOCOUT,true)
;	  env.setParam(SGEnv::STOREITERATIONS,true);

	  soln = SGSolution(game);
	  SGApprox approx(env,game,soln);

	  approx.initialize();

	  while (approx.generate() > env.getParam(SGEnv::ERRORTOL)
		 && approx.getNumIterations() < env.getParam(SGEnv::MAXITERATIONS))
	    {
	      if (approx.passedNorth())
		{
		  string str = approx.progressString();
		  mexPrintf(str.c_str());
		  mexPrintf("\n");
		  mexEvalString("drawnow;");
		}
	    };
	  
	  // Add the extreme tuples array to soln.
	  for (vector<SGTuple>::const_iterator tuple
		 = approx.getExtremeTuples().begin();
	       tuple != approx.getExtremeTuples().end();
	       ++tuple)
	    soln.push_back(*tuple);

	  approx.end();

	  // SGSolver solver(env,game);

	  // solver.solve();
	  
	  mexPrintf("Game solved.\n");
	  mexEvalString("drawnow;");

	  // soln = solver.getSolution();
	  solnLoaded = true;

	  currentIteration = soln.iterations.end();
	  currentIteration--;
	  currentIterationIndex = soln.iterations.size()-1;
	}
      catch (SGException & e)
	{
	  cout << e.what() << endl;
	  mexErrMsgTxt("SGException caught. Exiting.");
	}
      catch (exception & e)
	{
	  cout << e.what() << endl;
	  mexErrMsgTxt("std::exception caught. Exiting.");
	}
      catch(...)
	{
	  mexErrMsgTxt("Unknown exception caught");
	}
		
    } // Solve
	
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
	  outputPtr[0] = soln.game.getDelta();
	} // GetDelta
		
      // GetNumStates
      else if (!strcmp(optionStr,"GetNumStates"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputPtr[0] = soln.game.getNumStates();
	} // GetNumStates
		
      // GetNumActions
      else if (!strcmp(optionStr,"GetNumActions"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
	  plhs[0] = mxCreateDoubleMatrix(soln.game.getNumStates(),
					 soln.game.getNumPlayers(),mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputCounter = 0;
			
	  numActions = soln.game.getNumActions();
			
	  for (player = 0; player < soln.game.getNumPlayers(); player++)
	    {
	      for (state = 0; state < soln.game.getNumStates(); state++)
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
			
	  if (state < 0 || state > soln.game.getNumStates()-1)
	    mexErrMsgTxt("State is out of range.");
	  probabilities =
	    vector< vector<double > >(soln.game.getProbabilities()[state]);
			
	  plhs[0] = mxCreateDoubleMatrix(probabilities.size(),
					 probabilities[0].size(),mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputCounter = 0;
	  for (state = 0; state < soln.game.getNumStates(); state++)
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
	  if (state < 0 || state > soln.game.getNumStates()-1)
	    mexErrMsgTxt("State is out of range.");
			
	  numActions = soln.game.getNumActions();
	  payoffs = vector<SGPoint>(soln.game.getPayoffs()[state]);
			
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
	  if (currentIteration == soln.iterations.end())
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
			
	  if (currentIteration != soln.iterations.begin())
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
			
	  currentIteration = soln.iterations.begin();
	  currentIterationIndex = 0;
	} // IterToBeginning
		
      // IterToEnd
      else if (!strcmp(optionStr,"IterToEnd"))
	{
	  if (nlhs!=0)
	    mexErrMsgTxt("No outputs supported.");
			
	  currentIteration = soln.iterations.end();
	  currentIteration--;
	  currentIterationIndex = soln.iterations.size()-1;
	} // IterToEnd
		
      // GetCurentIteration
      else if (!strcmp(optionStr,"GetCurrentIteration"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
			
	  mxArray *fieldOutput;
	  SGPoint minICPayoffs, payoffs;
	  int nFields = 8;
			
	  const char *fieldNames[] = {"iteration",
				      "revolution",
				      "pivot",
				      "direction",
				      "actions",
				      "nonBinding",
				      "bestState",
				      "numExtremeTuples"};
	  
	  plhs[0] = mxCreateStructMatrix(1,1,nFields,fieldNames);
			
	  int currentField = 0;
	  // Iteration
	  fieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputPtr[0] = currentIteration->iteration;
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // Revolution
	  fieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputPtr[0] = currentIteration->revolution;
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // Pivot
	  fieldOutput = mxCreateDoubleMatrix(soln.game.getNumStates(),
					     soln.game.getNumPlayers(),
					     mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputCounter=0;
	  for (player=0; player<soln.game.getNumPlayers(); player++)
	    {
	      for (state=0; state<soln.game.getNumStates(); state++)
		{
		  outputPtr[outputCounter]
		    = currentIteration->pivot[state][player];
		  outputCounter++;
		}
	    }
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
						
	  // Direction
	  fieldOutput = mxCreateDoubleMatrix(1,
					     soln.game.getNumPlayers(),
					     mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  for (player=0; player<soln.game.getNumPlayers(); player++)
	    outputPtr[player]
	      = currentIteration->direction[player];
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // action
	  fieldOutput = mxCreateDoubleMatrix(1,soln.game.getNumStates(),mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  for (state = 0; state < soln.game.getNumStates(); state++)
	    outputPtr[state] = currentIteration->actionTuple[state];
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // nonBinding
	  fieldOutput = mxCreateDoubleMatrix(1,soln.game.getNumStates(),mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  for (state = 0; state < soln.game.getNumStates(); state++)
	    outputPtr[state] = static_cast<double>(currentIteration->regimeTuple[state]==SG::NonBinding);
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
	  
	  // state
	  fieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputPtr[0] = currentIteration->bestState;
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	  // numExtremeTuples
	  fieldOutput = mxCreateDoubleMatrix(1,1,mxREAL);
	  outputPtr = mxGetPr(fieldOutput);
	  outputPtr[0] = currentIteration->numExtremeTuples;
	  mxSetFieldByNumber(plhs[0],0,currentField++,fieldOutput);
			
	} // GetCurentIteration
		
      // GetTuples
      else if  (!strcmp(optionStr,"GetTuples"))
	{
	  if (nlhs!=1)
	    mexErrMsgTxt("One output required.");
			
	  plhs[0] = mxCreateDoubleMatrix(soln.extremeTuples.size(),
					 soln.game.getNumPlayers()
					 *soln.game.getNumStates(),mxREAL);
	  outputPtr = mxGetPr(plhs[0]);
	  outputCounter=0;
	  for (state=0; state<soln.game.getNumStates(); state++)
	    {
	      for (player=0; player<soln.game.getNumPlayers(); player++)
		{
		  for (list<SGTuple>::iterator tuple = soln.extremeTuples.begin();
		       tuple != soln.extremeTuples.end();
		       tuple++)
		    {
		      outputPtr[outputCounter] = (*tuple)[state][player];
		      outputCounter++;
		    }
		}
	    }
	} // GetTuples

      // SaveSolution
      else if (!strcmp(optionStr,"SaveSolution"))
	{
	  if (!solnLoaded)
	    mexErrMsgTxt("Load soln or solve a game before saving");
	  if (nrhs!=2)
	    mexErrMsgTxt("Must provide an output file name");
	  if (!mxIsChar(prhs[1]))
	    mexErrMsgTxt("Input file name must be a character array.");
			
	  outputFileName = mxArrayToString(prhs[1]);
			
	  try
	    {
	      SGSolution::save(soln, outputFileName);
	    }
	  catch (SGException & e)
	    {

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
			
	} // Save
		
		
      else
	{
	  mexErrMsgTxt("Not a valid option string.");
	}
		
		
    }
	
}

bool createGame(int nlhs, mxArray *plhs[],
		int nrhs, const mxArray *prhs[],
		SGGame & game)
{
  if (nrhs<5)
    mexErrMsgTxt("Must provide arguments of delta, player 1's payoffs, player 2's payoffs, and transition probabilities.");
  if (!mxIsDouble(prhs[1]))
    mexErrMsgTxt("Second argument must be discount factor.");
  if (!(mxIsCell(prhs[2]) && mxIsCell(prhs[3])))
    mexErrMsgTxt("Third and fourth argument must be cell arrays of payoff matrices");
  if (!mxIsCell(prhs[4]))
    mexErrMsgTxt("Fifth argument must be transition probabilities");
		
  double delta = mxGetScalar(prhs[1]);
  if (!(delta > 0.0 && delta < 1.0))
    mexErrMsgTxt("Delta must be a number between 0 and 1");
		
  int numStates = mxGetNumberOfElements(prhs[2]);
  if (numStates<1)
    mexErrMsgTxt("Number of states must be at least 1");
  if (mxGetNumberOfElements(prhs[3])!= numStates
      || mxGetNumberOfElements(prhs[4])!= numStates)
    mexErrMsgTxt("G1, G2, and P must have the same number of elements");
		
  int numPlayers = 2;
		
  vector<vector< int> > numActions(numStates,vector<int>(2,0));
  vector<int> numActions_total(numStates,0);
  for (int state = 0; state < numStates; state++)
    {
      numActions[state][0] = mxGetM(mxGetCell(prhs[2],state));
      numActions[state][1] = mxGetN(mxGetCell(prhs[2],state));
      numActions_total[state] = numActions[state][0] * numActions[state][1];
			
      if (mxGetM(mxGetCell(prhs[3],state))!=numActions[state][0]
	  || mxGetN(mxGetCell(prhs[3],state))!=numActions[state][1])
	mexErrMsgTxt("Payoff matrices for corresponding states must have the same size");
    } // state
		
  vector< vector< vector<double> > > newPayoffs(numStates);
  vector< vector< vector<double> > > newProbabilities(numStates);
  for (int state = 0; state < numStates; state++)
    {
      newPayoffs[state] = vector< vector<double> >(numActions_total[state],
						   vector<double> (numPlayers,0.0));
      newProbabilities[state] = vector< vector<double> >(numActions_total[state],
							 vector<double>(numStates,0.0));
			
    }
		
  for (int player = 0; player < numPlayers; player++)
    {
      for (int state = 0; state < numStates; state ++)
	{
	  double * pr = mxGetPr(mxGetCell(prhs[2+player],state));
				
	  for (int action = 0; action < numActions_total[state]; action++)
	    newPayoffs[state][action][player] = pr[action];
	} // state
    } // player
		
		
  for (int state = 0; state < numStates; state++)
    {
      if (mxGetM(mxGetCell(prhs[4],state)) != numActions_total[state])
	mexErrMsgTxt("Number of rows in probability transition matrices must equal number of action profiles in that state");
      if (mxGetN(mxGetCell(prhs[4],state)) != numStates)
	mexErrMsgTxt("Number of columns in probability transition matrices must equal the number of states.");
			
      double * pr = mxGetPr(mxGetCell(prhs[4],state));
			
      for (int action = 0; action < numActions_total[state]; action++)
	{
	  double probSum = 0;
				
	  for (int statep = 0; statep < numStates; statep ++)
	    {
	      probSum += pr[action+statep*numActions_total[state]];
	      newProbabilities[state][action][statep]
		= pr[action + statep*numActions_total[state]];
	    }
	  if (abs(probSum-1.0)>1e-9)
	    {
	      cout << probSum << endl;
	      mexErrMsgTxt("Transition probabilities must sum to 1");
	    }
	} // action
    } // state

  game = SGGame(delta,
		numStates,
		numActions,
		newPayoffs,
		newProbabilities);

  return false;
}
