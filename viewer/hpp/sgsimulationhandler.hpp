#ifndef SGSIMULATIONHANDLER_HPP
#define SGSIMULATIONHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include "sg.hpp"
#include "sgsimulator.hpp"
#include "sgsimulationplot.hpp"
#include "qcustomplot.h"

class SGSimulationHandler : public QWidget
{
  Q_OBJECT;

private:
  const SGSolution & soln;
  const SGPoint & point;
  const int state;
  const list<SGIteration>::const_iterator startOfLastRev;

  SGSimulator sim;
  int initialTuple;

  QLineEdit * iterationEdit;
  QLineEdit * longRunPayoffEdit;
  QLineEdit * timeEdit;
  QPushButton * simulateButton;
  SGSimulationPlot * distrPlot;
  QTextEdit * textEdit;
  QTextEdit * transitionTableEdit;

  QCPBars * stateBars;
  QCPBars * tupleBars;
  vector<QCPBars *> actionBars;

  QCPAxisRect * stateDistrRect;
  QCPAxisRect * tupleDistrRect;
  vector<QCPAxisRect *> actionDistrRects;
  

public:
  SGSimulationHandler(QWidget * parent, 
		      const SGSolution & _soln, 
		      const SGPoint & _point,
		      int _state,
		      list<SGIteration>::const_iterator _startOfLastRev);

  virtual ~SGSimulationHandler() {}

private slots:
  void simulate();
  
  // virtual QSize minimumSizeHint() const { return QSize(500,700); }
};

#endif
