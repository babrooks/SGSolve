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

//! Widget for forward simulating an equilibrium
/*! This widget is constructed by SGSolutionHandler when the user
    requests to simulate a particular equilibrium from the right-click
    context menu on one of the SGCustomPlot objects. This class
    constructs an SGSimulator object from the SGSolve library, and it
    also constructs plots and text edits for viewing the output of the
    simulation. The user can then input the number of periods for
    which to simulate the equilibrium. The widget will then use
    SGSimulator to run the simulation. The output is displayed in
    various formats. The first 200 periods of the simulation are
    displayed, along with the decomposition of all extremal equilibria
    into stage payoffs and a lottery over continuation equilibria. If
    there is more than one decomposition of the continuation value,
    one is selected arbitrarily. The widget also shows the
    distributions of tuples, states, actions, and average payoffs of
    the players over the course of the simulation. 

  \ingroup viewer
 */
class SGSimulationHandler : public QWidget
{
  Q_OBJECT;

private:
  //! Constant reference to the associated SGSolution object.
  const SGSolution & soln;
  //! Discounted payoffs in the simulated equilibrium.
  /*! The widget will simulate an equilibrium that generates payoffs
      closest to this point. */
  const SGPoint & point;
  //! Initial state for the simulation.
  const int state;

  //! The associated SGSimulator object.
  SGSimulator sim;
  //! The initial tuple for the simulation.
  int initialTuple;

  //! Displays the number of simulations to run.
  QLineEdit * simEdit;
  //! Displays the number of periods for which to simulate.
  QLineEdit * iterationEdit;
  //! Displays the average payoffs over the course of the simulation.
  QLineEdit * longRunPayoffEdit;
  //! Displays the time it took the simulation to run.
  QLineEdit * timeEdit;
  //! Displays the distribution of tuples, states, and action profiles.
  SGSimulationPlot * distrPlot;
  //! Displays the first 200 periods of the simulation.
  QTextEdit * textEdit;
  //! Displays the decomposition of extremal equilibria.
  QTextEdit * transitionTableEdit;

  //! Bars displaying for the distribution of states.
  QCPBars * stateBars;
  //! Bars displaying the distribution of tuples.
  QCPBars * tupleBars;
  //! Bars displaying the distributions of actions in each state.
  vector<QCPBars *> actionBars;

  //! The QCPAxisRect object associated with the state distributions.
  QCPAxisRect * stateDistrRect;
  //! The QCPAxisRect object associated with the tuple distributions.
  QCPAxisRect * tupleDistrRect;
  //! The QCPAxisRect objects associated with the action distributions.
  vector<QCPAxisRect *> actionDistrRects;

public:
  //! Constructor.
  SGSimulationHandler(QWidget * parent, 
		      const SGSolution & _soln, 
		      const SGPoint & _point,
		      int _state);

  //! Destructor.
  virtual ~SGSimulationHandler() {}

private slots:
  //! Triggers a new simulation
  /*! Causes the widget to run a new simulation for the number of
      periods in iterationsEdit. */
  void simulate();
  
};

#endif
