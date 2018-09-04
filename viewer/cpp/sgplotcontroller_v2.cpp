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
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

#include "sgplotcontroller_v2.hpp"

SGPlotController_V2::SGPlotController_V2(QComboBox * _stateCombo,
				   QComboBox *_actionCombo,
				   QScrollBar * _iterSlider,
				   QScrollBar * _stepSlider,
				   QComboBox * _solutionModeCombo):
  solnLoaded(false), action(-1),state(-1),iteration(0),
  stateCombo(_stateCombo), actionCombo(_actionCombo),
  solutionModeCombo(_solutionModeCombo),
  iterSlider(_iterSlider), stepSlider(_stepSlider),
  mode(Progress)
{
  connect(actionCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeAction(int)));
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(stepSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeMode(int)));
} // Constructor

void SGPlotController_V2::setSolution(SGSolution_V2 * newSoln)
{ 
  action=-1;
  state=-1;
  soln = newSoln; 
  currentIter = soln->getIterations().cend();
  --currentIter;
  iteration=currentIter->getIteration();
  currentStep = currentIter->getSteps().cbegin();
  solnLoaded = true;

  bool iterSliderBlock = iterSlider->blockSignals(true);
  bool stepSliderBlock = stepSlider->blockSignals(true);
  bool solutionModeComboBlock = solutionModeCombo->blockSignals(true);

  // Setup sliders
  int numStates = soln->getGame().getNumStates();
  
  iterSlider->setRange(soln->getIterations().front().getIteration(),soln->getIterations().back().getIteration());
  iterSlider->setValue(soln->getIterations().back().getIteration());
  stepSlider->setRange(0,soln->getIterations().back().getSteps().size()-1);
  stepSlider->setValue(0);
  // setSliderRanges(soln->getIterations().front().getIteration(),
  // 		  soln->getIterations().back().getIteration());

  // startOfLastRev = endIter;
  // while ((startOfLastRev--)->getRevolution()
  // 	 ==endIter->getRevolution()) {}
  
  mode = Progress;
  stepSlider->setEnabled(mode==Progress);

  solutionModeCombo->setCurrentIndex(mode);
  
  iterSlider->blockSignals(iterSliderBlock);
  stepSlider->blockSignals(stepSliderBlock);
  solutionModeCombo->blockSignals(solutionModeComboBlock);

  // Has to be last because this triggers replot
  setIteration(currentIter->getIteration());

  emit solutionChanged();
} // setSolution

bool SGPlotController_V2::setState(int newState)
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

bool SGPlotController_V2::setAction(int newAction)
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

bool SGPlotController_V2::setActionIndex(int newActionIndex)
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

bool SGPlotController_V2::setIteration(int newIter)
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

      currentStep = currentIter->getSteps().cbegin();

      setState(0);
      setAction(0);

      return true;
    }
  return false;
} // setIteration

void SGPlotController_V2::setCurrentDirection(SGPoint point, int state)
{
    double minDistance = numeric_limits<double>::max();

    for (auto step = currentIter->getSteps().cbegin();
         step != currentIter->getSteps().cend();
         ++step)
    {
        double newDistance = (step->getPivot()[state] - point)*((step->getPivot())[state] - point);
        if (newDistance < minDistance-1e-7)
        {
            minDistance = newDistance;
            currentStep = step;
        } // if
    } // for

    setState(state);
    setActionIndex(currentStep->getActionTuple()[state]);

    emit iterationChanged();
} // setCurrentDirection

void SGPlotController_V2::synchronizeSliders()
{
    synchronizeIterSlider();
    synchronizeStepSlider();
} // synchronizeSliders

void SGPlotController_V2::synchronizeIterSlider()
{
    int iter = iterSlider->sliderPosition();
    while (currentIter->getIteration() < iter
           && currentIter!=soln->getIterations().cend())
        ++currentIter;
    while (currentIter->getIteration() > iter
           && currentIter!=soln->getIterations().cbegin())
        --currentIter;
    if (currentIter == soln->getIterations().cend())
        --currentIter;

    stepSlider->setRange(0,currentIter->getSteps().size()-1);

} // synchronizeIterSlider

void SGPlotController_V2::synchronizeStepSlider()
{
    int dir = stepSlider->sliderPosition();
    if (dir > currentIter->getSteps().size()-1)
        dir = currentIter->getSteps().size()-1;
    stepSlider->setValue(dir);
    currentStep = currentIter->getSteps().cbegin();
    for (int d = 0; d < dir; d++)
    {
        if (currentStep++ == currentIter->getSteps().cend())
        {
            currentStep--;
            break;
        }
    }
} // synchronizeStepSlider

void SGPlotController_V2::iterSliderUpdate(int value)
{
  if (!solnLoaded)
    return;

  synchronizeSliders();

  // setState(currentIter->getBestState());
  // setAction(currentIter->getBestAction());
//  setState(0);
  setActionIndex(currentStep->getActionTuple()[state]);

  plotMode = Directions;
  
  emit iterationChanged();
} // iterSliderUpdate

void SGPlotController_V2::prevAction()
{
  if (!solnLoaded)
    return;
    
  if ( (state == -1
    || (state == 0 && actionIndex <= 0))
       && (stepSlider->minimum() < stepSlider->value()) )
    {
      stepSlider->setSliderPosition(std::max(stepSlider->minimum(),
                         stepSlider->value()-1));
      synchronizeStepSlider();
      setState(soln->getGame().getNumStates()-1);
      setActionIndex(currentIter->getActions()[state].size()-1);
      emit iterationChanged();
    }
  else if ( (state == -1
    || (state == 0 && actionIndex <= 0))
       && (iterSlider->minimum() < iterSlider->value()) )
    {
      iterSlider->setSliderPosition(std::max(iterSlider->minimum(),
                         iterSlider->value()-1));
      synchronizeIterSlider();
      stepSlider->setSliderPosition(stepSlider->maximum());
      synchronizeStepSlider();
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

void SGPlotController_V2::nextAction()
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
  else if (stepSlider->value() < stepSlider->maximum())
    {
      stepSlider->setSliderPosition(std::min(stepSlider->maximum(),
                         stepSlider->value()+1));
      synchronizeStepSlider();
      setState(0);
      setActionIndex(0);
      emit iterationChanged();
    }
  else if (iterSlider->value() < iterSlider->maximum())
    {
      iterSlider->setSliderPosition(std::min(iterSlider->maximum(),
                         iterSlider->value()+1));
      synchronizeIterSlider();
      stepSlider->setSliderPosition(0);
      synchronizeStepSlider();
      setState(0);
      setActionIndex(0);
      emit iterationChanged();
    }
} // nextAction

void SGPlotController_V2::changeMode(int newMode)
{
  if (newMode == 0)
    {
      mode = Progress;

      currentIter = soln->getIterations().cbegin();
      currentStep = currentIter->getSteps().cbegin();
    }
  else if (newMode == 1)
    {
      // mode = Final;
      
      // startIter = startOfLastRev;

      // if (currentIter->getIteration() < startIter->getIteration())
      // 	{
      // 	  currentIter = soln->getIterations().end();
      // 	  currentIter--;
      // 	}
    }

  // setSliderRanges(startIter->getIteration(),soln->getIterations().back().getIteration());
  
  // iterSlider->setValue(endIter->getIteration());
  // iterSliderUpdate(endIter->getIteration());
} // changeMode

void SGPlotController_V2::changeAction(int newAction)
{
  setActionIndex(newAction-1);
}


// void SGPlotController_V2::setSliderRanges(int start, int end)
// {
//   // Temporaily disconnect so we don't trigger plotSolution.
//   bool iterSliderBlock = iterSlider->blockSignals(true);
//   bool stepSliderBlock = stepSlider->blockSignals(true);
  
//   iterSlider->setRange(start,end);
//   iterSlider->setValue(end);
//   stepSlider->setRange(start,end);
//   stepSlider->setValue(start);

//   stepSlider->setEnabled(mode==Progress);
  
//   iterSlider->blockSignals(iterSliderBlock);
//   stepSlider->blockSignals(stepSliderBlock);
// } // setSliderRanges

