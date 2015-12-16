#ifndef SGPLOTCONTROLLER_HPP
#define SGPLOTCONTROLLER_HPP

#include <QObject>
#include <QComboBox>
#include "sgsolution.hpp"

class SGPlotController : public QObject
{
  Q_OBJECT;

private:
  int state;
  int action;
  int actionIndex;
  int iteration;
  list<SGIteration>::const_iterator currentIter;
  
  SGSolution * soln;
  
  bool solutionSet;
  bool plotAllDirections;

  QComboBox * stateCombo;
  QComboBox * actionCombo;
public:
  SGPlotController(QComboBox * _stateCombo,
			   QComboBox *_actionCombo):
    solutionSet(false), action(-1),state(-1),iteration(0),
    stateCombo(_stateCombo), actionCombo(_actionCombo),
    plotAllDirections(false)
  {
    connect(actionCombo,SIGNAL(currentIndexChanged(int)),
	    this,SLOT(changeAction(int)));
  }

  int getState() const { return state; }
  int getAction() const { return action; }
  int getActionIndex() const { return actionIndex; }
  int getIteration() const { return iteration; }
  bool getPlotAllDirections() const { return plotAllDirections; }
  bool hasSolution() const { return solutionSet; }
  const SGSolution * getSolution() const { return soln; }
  const list<SGIteration>::const_iterator getCurrentIter() const
  { return currentIter; }
  
  void setPlotAllDirections(bool tf) { plotAllDirections = tf; }
  void setSolution(SGSolution * newSoln)
  { 
    action=-1;
    state=-1;
    soln = newSoln; 
    currentIter = soln->iterations.end();
    --currentIter;
    iteration=currentIter->iteration;
    solutionSet = true;
    emit solutionChanged();
    stateCombo->setCurrentIndex(0);
    actionCombo->setCurrentIndex(0);
  }
  bool setState(int newState) 
  {
    if (solutionSet
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

  bool setActionIndex(int newActionIndex) 
  {
    if (solutionSet
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

  bool setAction(int newAction)
  {
    
    return false;
  }

  bool setIteration(int newIter)
  {
    if (solutionSet
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

	state = -1;
	action = -1;
	stateCombo->setCurrentIndex(0);
	actionCombo->setCurrentIndex(0);
	emit stateChanged();

	return true;
      }
    return false;
  } // setIteration

signals:
  void solutionChanged();

  void stateChanged();
  void actionChanged();

  void nextAction();
  void prevAction();

public slots:
  void changeAction(int index)
  {
    setActionIndex(index-1);
  }
  

}; // SGPlotController

#endif
