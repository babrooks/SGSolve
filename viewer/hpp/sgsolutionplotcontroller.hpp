#ifndef SGSOLUTIONPLOTCONTROLLER_HPP
#define SGSOLUTIONPLOTCONTROLLER_HPP

#include <QObject>
#include <QComboBox>
#include "sgsolution.hpp"

class SGSolutionPlotController : public QObject
{
  Q_OBJECT;

private:
  int state;
  int action;
  int iteration;
  list<SGIteration>::const_iterator currentIter;
  
  SGSolution * soln;

  bool solutionSet;

  QComboBox * stateCombo;
  QComboBox * actionCombo;
public:
  SGSolutionPlotController(QComboBox * _stateCombo,
			   QComboBox *_actionCombo):
    solutionSet(false), action(-1),state(-1),iteration(0),
    stateCombo(_stateCombo), actionCombo(_actionCombo)
  {
    connect(actionCombo,SIGNAL(currentIndexChanged(int)),
	    this,SLOT(changeAction(int)));
  }

  int getState() const { return state; }
  int getAction() const { return action; }
  int getIteration() const { return iteration; }
  bool hasSolution() const { return solutionSet; }
  const SGSolution * getSolution() const { return soln; }
  const list<SGIteration>::const_iterator getCurrentIter() const
  { return currentIter; }
  
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
  bool setAction(int newAction) 
  {
    if (solutionSet
	&& state>=0
	&& newAction>=-1
	&& newAction <= currentIter->actions[state].size())
      {
	action = newAction;
	if (action>-1)
	  emit actionChanged();
	return true;
      }
    return false;
  } // setAction
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
    setAction(index-1);
  }


}; // SGSolutionPlotController

#endif
