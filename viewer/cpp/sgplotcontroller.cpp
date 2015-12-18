#include "sgplotcontroller.hpp"

void SGPlotController::setSolution(SGSolution * newSoln)
{ 
  action=-1;
  state=-1;
  soln = newSoln; 
  currentIter = soln->iterations.end();
  --currentIter;
  iteration=currentIter->iteration;
  solnLoaded = true;
  stateCombo->setCurrentIndex(0);
  actionCombo->setCurrentIndex(0);

  // Initialize iter pointer
  startIter = soln->iterations.begin();
  endIter = soln->iterations.end();
  --endIter;
  setIteration(currentIter->iteration);
  
  // Setup sliders
  int numStates = soln->game.getNumStates();
  
  setSliderRanges(soln->iterations.front().iteration,
		  soln->iterations.back().iteration);

  startOfLastRev = endIter;
  while ((startOfLastRev--)->revolution
	 ==endIter->revolution) {}
  
  mode = Progress;
  startSlider->setEnabled(mode==Progress);
  disconnect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(changeMode(int)));
  solutionModeCombo->setCurrentIndex(mode);
  connect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeMode(int)));
  
  emit solutionChanged();
} // setSolution

bool SGPlotController::setState(int newState)
{
  if (solnLoaded
      && newState>=-1
      && state < soln->game.getNumStates())
    {
      state = newState;
      action = -1;
      actionCombo->setCurrentIndex(0);
      emit stateChanged();
      return true;
    }
  return false;
} // setState

bool SGPlotController::setActionIndex(int newActionIndex)
{
  if (solnLoaded
      && state>=0
      && newActionIndex>=-1
      && newActionIndex <= currentIter->actions[state].size())
    {
      actionIndex = newActionIndex;
      if (actionIndex>-1)
	{
	  action = currentIter->actions[state][actionIndex].getAction();
	  emit actionChanged();
	}
      else
	action = -1;
      return true;
    }
  return false;
} // setActionIndex

bool SGPlotController::setIteration(int newIter)
{
  if (solnLoaded
      && newIter>=0
      && newIter <= soln->iterations.size())
    {
      iteration = newIter;
      while (currentIter->iteration < iteration
	     && currentIter != --(soln->iterations.end()))
	++currentIter;
      while (currentIter->iteration > iteration
	     && currentIter != soln->iterations.begin())
	--currentIter;

      state = currentIter->bestState;
      stateCombo->setCurrentIndex(state+1);
      action = currentIter->bestAction;
      int newActionIndex = 0;
      while (currentIter->actions[state][newActionIndex].getAction()!=action
      	     && newActionIndex < currentIter->actions[state].size())
      	newActionIndex++;
      actionIndex = newActionIndex;
      actionCombo->setCurrentIndex(newActionIndex);

      return true;
    }
  return false;
} // setIteration

void SGPlotController::setCurrentIteration(SGPoint point, int state)
{
  double minDistance = numeric_limits<double>::max();

  int tupleC = 0;
  
  currentIter = startIter;
  for (list<SGTuple>::const_iterator tuple = soln->extremeTuples.begin();
       tuple != soln->extremeTuples.end();
       ++tuple)
    {
      if (tupleC >= endIter->numExtremeTuples-1)
	break;
      else if (tupleC >= startIter->numExtremeTuples-1)
	{
	  double newDistance = ((*tuple)[state] - point)*((*tuple)[state] - point);
	  if (newDistance < minDistance-1e-7)
	    {
	      minDistance = newDistance;
	      while (currentIter->numExtremeTuples <= tupleC)
		currentIter++;
	    } // if
	} // if
      
      tupleC++;
    } // for

  setState(state);
  setAction(currentIter->actionTuple[state]);
} // setCurrentIteration

void SGPlotController::iterSliderUpdate(int value)
{
  if (!solnLoaded)
    return;
      
  int start = startSlider->sliderPosition();
  int end = iterSlider->sliderPosition();
  
  end = std::max(-1,std::max(start,end));
  iterSlider->setMinimum(std::max(start,-1));
  
  while (currentIter->iteration < end
	 && currentIter!=soln->iterations.end())
    ++currentIter;
  while (currentIter->iteration > end
	 && currentIter!=soln->iterations.begin())
    --currentIter;
  if (currentIter == soln->iterations.end())
    --currentIter;
  
  if (mode==Progress)
    endIter = currentIter;
  else
    {
      endIter = soln->iterations.end();
      endIter--;
    }
  
  if (start==-1)
    startIter = soln->iterations.begin();
  else
    {
      while (startIter->iteration < start
	     && startIter!=soln->iterations.end())
	++startIter;
      while (startIter->iteration > start
	     && startIter!=soln->iterations.begin())
	--startIter;
      if (startIter == soln->iterations.end())
	--startIter;
    }

  if (currentIter == soln->iterations.begin())
    currentIter++;

  state = currentIter->bestState;
  stateCombo->setCurrentIndex(state+1);
  action = currentIter->bestAction;
  int newActionIndex = 0;
  while (currentIter->actions[state][newActionIndex].getAction()!=action
	 && newActionIndex < currentIter->actions[state].size())
    newActionIndex++;
  actionIndex = newActionIndex;
  actionCombo->setCurrentIndex(newActionIndex);
  
  emit iterationChanged();
} // iterSliderUpdate

void SGPlotController::setSliderRanges(int start, int end)
{
  // Temporaily disconnect so we don't trigger plotSolution.
  disconnect(iterSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));
  disconnect(startSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));
  
  iterSlider->setRange(start,end);
  iterSlider->setValue(end);
  startSlider->setRange(start,end);
  startSlider->setValue(start);

  startSlider->setEnabled(mode==Progress);
  
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
} // setSliderRanges
