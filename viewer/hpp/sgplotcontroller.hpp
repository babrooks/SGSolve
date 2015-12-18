#ifndef SGPLOTCONTROLLER_HPP
#define SGPLOTCONTROLLER_HPP

#include <QObject>
#include <QComboBox>
#include <QScrollBar>
#include "sgsolution.hpp"

class SGPlotController : public QObject
{
  Q_OBJECT;

private:
  int state;
  int action;
  int actionIndex;
  int iteration;
  
  SGSolution * soln;
  
  //! Solution mode
  enum SolutionMode {Progress, Final} mode;
  
  //! Points to the first iteration from which to plot.
  list<SGIteration>::const_iterator startIter;
  //! Points to the last iteration to which to plot.
  list<SGIteration>::const_iterator endIter;
  //! Points to the pivot iteration.
  list<SGIteration>::const_iterator currentIter;
  //! Points to the pivot iteration.
  list<SGIteration>::const_iterator startOfLastRev;

  bool solnLoaded;
  bool plotAllDirections;

  QComboBox * stateCombo;
  QComboBox * actionCombo;

  QComboBox * solutionModeCombo;

  QScrollBar * iterSlider;
  QScrollBar * startSlider;
public:
  SGPlotController(QComboBox * _stateCombo,
		   QComboBox *_actionCombo,
		   QScrollBar * _iterSlider,
		   QScrollBar * _startSlider,
		   QComboBox * _solutionModeCombo):
    solnLoaded(false), action(-1),state(-1),iteration(0),
    stateCombo(_stateCombo), actionCombo(_actionCombo),
    solutionModeCombo(_solutionModeCombo),
    iterSlider(_iterSlider), startSlider(_startSlider),
    plotAllDirections(false),
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
  }

  int getState() const { return state; }
  int getAction() const { return action; }
  int getActionIndex() const { return actionIndex; }
  int getIteration() const { return iteration; }
  SolutionMode getMode () const { return mode; }
  const SGIteration & getCurrentIter() const { return *currentIter; }
  const SGIteration & getStartIter() const { return *startIter; }
  const SGIteration & getEndIter() const { return *endIter; }
  const SGIteration & getStartOfLastRev() const { return *startOfLastRev; }
  int getStartSliderPosition() const { return startSlider->sliderPosition(); }
  bool getPlotAllDirections() const { return plotAllDirections; }
  bool hasSolution() const { return solnLoaded; }
  const SGSolution * getSolution() const { return soln; }
  
  void setPlotAllDirections(bool tf) { plotAllDirections = tf; }
  void setSolution(SGSolution * newSoln);
  bool setState(int newState);

  bool setActionIndex(int newActionIndex);

  bool setAction(int newAction)
  {
    
    return false;
  }

  bool setIteration(int newIter);

  void moveForwards()
  {
    if (!solnLoaded)
      return;
      
    iterSlider->setSliderPosition(std::max(iterSlider->minimum(),
					   iterSlider->value()-1));
    iterSliderUpdate(iterSlider->value());
  }

  void moveBackwards()
  {
    if (!solnLoaded)
      return;
      
    iterSlider->setSliderPosition(std::min(iterSlider->maximum(),
					   iterSlider->value()+1));
    iterSliderUpdate(iterSlider->value());
  }

  void setCurrentIteration(SGPoint point, int state);

signals:
  void solutionChanged();

  void stateChanged();
  void actionChanged();

  void iterationChanged();

public slots:
  //! Toggles the solution mode
  void changeMode(int newMode)
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

  void iterSliderUpdate(int value);

  void actionUpdate(int value)
  {

  } // actionUpdate

  void changeAction(int index)
  {
    setActionIndex(index-1);
  }
  
  void setSliderRanges(int start, int end);

  void prevAction()
  {

  } // prevAction

  void nextAction()
  {

  } // nextAction

  


}; // SGPlotController



#endif
