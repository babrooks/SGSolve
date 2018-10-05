// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2018 Benjamin A. Brooks
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

#ifndef SGSOLVERWORKER_V2_HPP
#define SGSOLVERWORKER_V2_HPP

#include <QtWidgets>
#include "sg.hpp"
#include "sgsolver_v4.hpp"

//! Class for implementing the twist algorithm within SGViewer
/*! If we just used the SGSolver class to solve the game, the
  algorithm would run continuously until convergence or an
  error. Since we want regular status updates and we want to print
  progress to the log window, we have to reimplement the solver
  routine. 

  This class uses SGApprox to calculate the solution. The main program
  assigns this object to a separate thread to preserve responsiveness
  of the algorithm. After each iteration, control is returned to the
  main process, which prints a status update if a revolution has been
  completed and cancels the algorithm if the cancel flag has been
  thrown. Communication between the SGSolverWorker and SGMainWindow is
  facilitated by signals and slots.

  \ingroup viewer
*/
class SGSolverWorker_V2 : public QObject
{
  Q_OBJECT;

private:
  //! An environment object to hold settings.
  const SGEnv & env;
  //! The main object for performing calculations
  SGSolver_V4 solver;
  //! A pointer to the text edit in which to report progress.
  QTextEdit * logTextEdit;
  //! Number of iterations
  int numIter;

public:
  //! Code for status at the end of the iteration.
  enum STATUS
    {
      FAILED, /*!< Could not complete the iteration. An error
                 occured. */
      CONVERGED, /*!< Algorithm completed the iteration and has
                    converged. */
      NOTCONVERGED /*!< Algorithm completed the iteration but has not
                      converged. */
    } status; /*!< Current status. */

  //! Constructor
  /*! Initializes the solver object. Waits for instruction to begin
      iteration via the SGSolverWorker_V2::iterate slot. */
  SGSolverWorker_V2(const SGEnv & _env,
		    const SGGame & game,
		    QTextEdit * _logTextEdit):
    env(_env), solver(env,game),
    logTextEdit(_logTextEdit)
  {
    numIter = 0;
    solver.initialize();
  } // constructor

  //! Returns the status of the worker
  STATUS getStatus() const { return status; }

public slots:

  //! Iterates. 
  /*! Runs one iteration of the twist algorithm and emits the
      resultReady signal. */
  void iterate()
  {
    try
      {
	if (solver.iterate_endogenous() > env.getParam(SG::ERRORTOL)
	    && numIter < env.getParam(SG::MAXITERATIONS))
	  {
	    status = NOTCONVERGED;
	    emit resultReady(false);
	  }
	else
	  {
	    status = CONVERGED;
	    emit resultReady(true);
	  }
      }
    catch (exception & e)
      {
	// Add the extreme tuples array to soln.
	qDebug() << "solve failed" << endl;

	status = FAILED;
	emit resultReady(true);
	
	// emit exceptionCaught();
      }
  } // iterate

  //! Returns the SGSolution object.
  const SGSolution_V2 & getSolution() const
  { return solver.getSolution(); }

  //! Returns the solver object.
  const SGSolver_V4 & getSolver() const
  { return solver; }
  
signals:
  //! Signal that gets emitted when iteration finishes.
  void resultReady(bool);
  //! Signal that gets emitted when an exception is caught.
  /*! Not currently used. */
  void exceptionCaught();  
};

#endif
