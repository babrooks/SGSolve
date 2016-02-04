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
  currentIter = soln->getIterations().end();
  --currentIter;
  iteration=currentIter->getIteration();
  solnLoaded = true;

  // Initialize iter pointer
  startIter = soln->getIterations().begin();
  endIter = soln->getIterations().end();
  --endIter;

  bool iterSliderBlock = iterSlider->blockSignals(true);
  bool startSliderBlock = startSlider->blockSignals(true);
  bool solutionModeComboBlock = solutionModeCombo->blockSignals(true);

  // Setup sliders
  int numStates = soln->getGame().getNumStates();
  
  setSliderRanges(soln->getIterations().front().getIteration(),
		  soln->getIterations().back().getIteration());

  startOfLastRev = endIter;
  while ((startOfLastRev--)->getRevolution()
	 ==endIter->getRevolution()) {}
  
  mode = Progress;
  startSlider->setEnabled(mode==Progress);

  solutionModeCombo->setCurrentIndex(mode);
  
  iterSlider->blockSignals(iterSliderBlock);
  startSlider->blockSignals(startSliderBlock);
  solutionModeCombo->blockSignals(solutionModeComboBlock);

  // Has to be last because this triggers replot
  setIteration(currentIter->getIteration());

  emit solutionChanged();
} // setSolution

bool SGPlotController::setState(int newState)
{
  if (solnLoaded
      && newState>=-1
      && state < soln->getGame().getNumStates())
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
  if (newAction >= 0 && newAction < currentIter->getActions()[state].size())
    {
      action = newAction;
      int newActionIndex = 0;
      while (currentIter->getActions()[state][newActionIndex].getAction()!=action
	     && newActionIndex < currentIter->getActions()[state].size())
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
      && newActionIndex <= currentIter->getActions()[state].size())
    {
      actionIndex = newActionIndex;
      if (actionIndex>-1)
	{
	  action = currentIter->getActions()[state][actionIndex].getAction();
	  emit actionChanged();
	}
      else
	action = -1;

      bool actionComboBlock = actionCombo->blockSignals(true);
      actionCombo->setCurrentIndex(newActionIndex+1);
      actionCombo->blockSignals(actionComboBlock);
      return true;
    }
  return false;
} // setActionIndex

bool SGPlotController::setIteration(int newIter)
{
  if (solnLoaded
      && newIter>=0
      && newIter <= soln->getIterations().size())
    {
      iteration = newIter;
      while (currentIter->getIteration() < iteration
	     && currentIter != --(soln->getIterations().end()))
	++currentIter;
      while (currentIter->getIteration() > iteration
	     && currentIter != soln->getIterations().begin())
	--currentIter;

      setState(currentIter->getBestState());
      setAction(currentIter->getBestAction());

      return true;
    }
  return false;
} // setIteration

void SGPlotController::setCurrentIteration(SGPoint point, int state)
{
  double minDistance = numeric_limits<double>::max();

  int tupleC = 0;
  
  currentIter = startIter;
  for (list<SGTuple>::const_iterator tuple = soln->getExtremeTuples().begin();
       tuple != soln->getExtremeTuples().end();
       ++tuple)
    {
      if (tupleC >= endIter->getNumExtremeTuples()-1)
	break;
      else if (tupleC >= startIter->getNumExtremeTuples()-1)
	{
	  double newDistance = ((*tuple)[state] - point)*((*tuple)[state] - point);
	  if (newDistance < minDistance-1e-7)
	    {
	      minDistance = newDistance;
	      while (currentIter->getNumExtremeTuples() <= tupleC)
		currentIter++;
	    } // if
	} // if
      
      tupleC++;
    } // for
  
  setState(state);
  setAction(currentIter->getActionTuple()[state]);

  emit iterationChanged();
} // setCurrentIteration

void SGPlotController::synchronizeSliders()
{
  int start = startSlider->sliderPosition();
  int end = iterSlider->sliderPosition();
  
  end = std::max(-1,std::max(start,end));
  iterSlider->setMinimum(std::max(start,-1));
  
  while (currentIter->getIteration() < end
	 && currentIter!=soln->getIterations().end())
    ++currentIter;
  while (currentIter->getIteration() > end
	 && currentIter!=soln->getIterations().begin())
    --currentIter;
  if (currentIter == soln->getIterations().end())
    --currentIter;
  
  if (mode==Progress)
    endIter = currentIter;
  else
    {
      endIter = soln->getIterations().end();
      endIter--;
    }
  
  if (start==-1)
    startIter = soln->getIterations().begin();
  else
    {
      while (startIter->getIteration() < start
	     && startIter!=soln->getIterations().end())
	++startIter;
      while (startIter->getIteration() > start
	     && startIter!=soln->getIterations().begin())
	--startIter;
      if (startIter == soln->getIterations().end())
	--startIter;
    }

  if (currentIter == soln->getIterations().begin())
    currentIter++;
} // synchronizeSliders

void SGPlotController::iterSliderUpdate(int value)
{
  if (!solnLoaded)
    return;

  synchronizeSliders();

  setState(currentIter->getBestState());
  setAction(currentIter->getBestAction());

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
	|| (state == 0 && actionIndex <= 0))
       && (iterSlider->minimum() < iterSlider->value()) )
    {
      iterSlider->setSliderPosition(std::max(iterSlider->minimum(),
					     iterSlider->value()-1));
      synchronizeSliders();
      setState(soln->getGame().getNumStates()-1);
      setActionIndex(currentIter->getActions()[state].size()-1);
      emit iterationChanged();
    }
  else if (state > 0
	   && actionIndex <= 0)
    {
      setState(state-1);
      setActionIndex(currentIter->getActions()[state].size()-1);
      emit iterationChanged();
    }
  else 
    {
      setActionIndex(actionIndex-1);
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
  else if (state>-1 && actionIndex+1 < currentIter->getActions()[state].size())
    {
      setActionIndex(actionIndex+1);
      emit iterationChanged();
    }
  else if (state>-1 && state+1 < soln->getGame().getNumStates()
	   && actionIndex+1==currentIter->getActions()[state].size())
    {
      setState(state+1);
      setActionIndex(0);
      emit iterationChanged();
    }	
  else if (iterSlider->value() < iterSlider->maximum())
    {
      iterSlider->setSliderPosition(std::min(iterSlider->maximum(),
					     iterSlider->value()+1));
      synchronizeSliders();
      setState(0);
      setActionIndex(0);
      emit iterationChanged();
    }
} // nextAction

void SGPlotController::changeMode(int newMode)
{
  if (newMode == 0)
    {
      mode = Progress;

      startIter = soln->getIterations().begin();
    }
  else if (newMode == 1)
    {
      mode = Final;
      
      startIter = startOfLastRev;

      if (currentIter->getIteration() < startIter->getIteration())
	{
	  currentIter = soln->getIterations().end();
	  currentIter--;
	}
    }

  setSliderRanges(startIter->getIteration(),soln->getIterations().back().getIteration());
  
  iterSlider->setValue(endIter->getIteration());
  iterSliderUpdate(endIter->getIteration());
} // changeMode

void SGPlotController::changeAction(int newAction)
{
  setActionIndex(newAction-1);
}
