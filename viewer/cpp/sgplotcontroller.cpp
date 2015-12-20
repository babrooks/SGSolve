#include "sgplotcontroller.hpp"

SGPlotController::SGPlotController(QComboBox * _stateCombo,
				   QComboBox *_actionCombo,
				   QScrollBar * _iterSlider,
				   QScrollBar * _startSlider,
				   QComboBox * _solutionModeCombo):
  solnLoaded(false), action(-1),state(-1),iteration(0),
  stateCombo(_stateCombo), actionCombo(_actionCombo),
  solutionModeCombo(_solutionModeCombo),
  iterSlider(_iterSlider), startSlider(_startSlider),
  mode(Progress)
{
  connect(actionCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeAction(int)));
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeMode(int)));
} // Constructor

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
      stateCombo->setCurrentIndex(state+1);
      action = -1;
      actionCombo->setCurrentIndex(0);
      return true;
    }
  return false;
} // setState

bool SGPlotController::setAction(int newAction)
{
  if (newAction >= 0 && newAction < currentIter->actions[state].size())
    {
      action = newAction;
      int newActionIndex = 0;
      while (currentIter->actions[state][newActionIndex].getAction()!=action
	     && newActionIndex < currentIter->actions[state].size())
	newActionIndex++;
      actionIndex = newActionIndex;
      actionCombo->setCurrentIndex(newActionIndex+1);

      return true;
    }
  return false;
} // setAction

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

      setState(currentIter->bestState);
      setAction(currentIter->bestAction);

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

  emit iterationChanged();
} // setCurrentIteration

void SGPlotController::synchronizeSliders()
{
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
} // synchronizeSliders

void SGPlotController::iterSliderUpdate(int value)
{
  if (!solnLoaded)
    return;

  synchronizeSliders();

  setState(currentIter->bestState);
  setAction(currentIter->bestAction);

  plotMode = Directions;
  
  emit iterationChanged();
} // iterSliderUpdate

void SGPlotController::setSliderRanges(int start, int end)
{
  // Temporaily disconnect so we don't trigger plotSolution.
  bool iterSliderBlock = iterSlider->blockSignals(true);
  bool startSliderBlock = startSlider->blockSignals(true);
  
  iterSlider->setRange(start,end);
  iterSlider->setValue(end);
  startSlider->setRange(start,end);
  startSlider->setValue(start);

  startSlider->setEnabled(mode==Progress);
  
  iterSlider->blockSignals(iterSliderBlock);
  startSlider->blockSignals(startSliderBlock);
} // setSliderRanges

void SGPlotController::prevAction()
{
  if (!solnLoaded)
    return;
    
  if ( (state == -1
	|| (state == 0 && action <= 0))
       && (iterSlider->minimum() < iterSlider->value()) )
    {
      iterSlider->setSliderPosition(std::max(iterSlider->minimum(),
					     iterSlider->value()-1));
      synchronizeSliders();
      setState(soln->game.getNumStates()-1);
      setAction(currentIter->actions[state].size()-1);
      emit iterationChanged();
    }
  else if (state > 0
	   && action <= 0)
    {
      setState(state-1);
      setAction(currentIter->actions[state].size()-1);
      emit iterationChanged();
    }
  else 
    {
      setAction(action-1);
      emit iterationChanged();
    }
} // prevAction

void SGPlotController::nextAction()
{
  if (!solnLoaded)
    return;
    
  if (state==-1)
    {
      setState(0);
    }
  else if (state>-1 && action+1 < currentIter->actions[state].size())
    {
      setAction(action+1);
      emit iterationChanged();
    }
  else if (state>-1 && state+1 < soln->game.getNumStates()
	   && action+1==currentIter->actions[state].size())
    {
      setState(state+1);
      setAction(0);
      emit iterationChanged();
    }	
  else if (iterSlider->value() < iterSlider->maximum())
    {
      iterSlider->setSliderPosition(std::min(iterSlider->maximum(),
					     iterSlider->value()+1));
      synchronizeSliders();
      setState(0);
      setAction(0);
      emit iterationChanged();
    }
} // nextAction

void SGPlotController::changeMode(int newMode)
{
  if (newMode == 0)
    {
      mode = Progress;

      startIter = soln->iterations.begin();
    }
  else if (newMode == 1)
    {
      mode = Final;
      
      startIter = startOfLastRev;

      if (currentIter->iteration < startIter->iteration)
	{
	  currentIter = soln->iterations.end();
	  currentIter--;
	}
    }

  setSliderRanges(startIter->iteration,soln->iterations.back().iteration);
  
  iterSlider->setValue(endIter->iteration);
  iterSliderUpdate(endIter->iteration);
} // changeMode
