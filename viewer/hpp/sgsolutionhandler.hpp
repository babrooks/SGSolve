#ifndef SGSOLUTIONHANDLER_HPP
#define SGSOLUTIONHANDLER_HPP

#include "sgsolution.hpp"
#include "sgcustomplot.hpp"

class SGSolutionHandler : public QObject
{
  Q_OBJECT

protected:
  // Solution
  SGSolution soln;

  double payoffBound;
  list<SGIteration>::const_iterator startIter;
  list<SGIteration>::const_iterator endIter;
  bool solnLoaded;
  
  // Plots
  SGCustomPlot * detailPlot;
  vector<SGCustomPlot *> statePlots;

  QGridLayout * statePlotsLayout;

  // Sliders
  QScrollBar * iterSlider;
  QScrollBar * startSlider;
  QScrollBar * actionSlider;
  QScrollBar * stateSlider;

  QAction * detailedTitlesAction;

  // Methods
  void plotSolution(int start, int end);
  void plotSolution(int start, int end,
		    SGCustomPlot * plot, int state,
		    bool addSquares);
  
  QCPRange getBounds(const QVector<double> & x) const;
public:
  SGSolutionHandler();

  void setSolution(const SGSolution & newSoln);
  const SGSolution & getSolution() const
  {return soln;}

  void moveForwards();
  void moveBackwards();
					       
public slots:
  void iterSliderUpdate(int value);
  void actionUpdate(int value);

  friend class SGMainWindow;
};

#endif
