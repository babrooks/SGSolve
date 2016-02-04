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

#ifndef SGSOLVERWORKER_HPP
#define SGSOLVERWORKER_HPP

#include <QtWidgets>
#include "sg.hpp"
#include "sgapprox.hpp"

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
class SGSolverWorker : public QObject
{
  Q_OBJECT;

private:
  //! An environment object to hold settings.
  const SGEnv & env;
  //! Solution object used by SGApprox.
  SGSolution soln;
  //! The main object for performing calculations
  SGApprox approx;
  //! A pointer to the text edit in which to report progress.
  QTextEdit * logTextEdit;

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
  /*! Initializes the SGApprox object. Waits for instruction to begin
      iteration via the SGSolverWorker::iterate slot. */
  SGSolverWorker(const SGEnv & _env,
		 const SGGame & game,
		 QTextEdit * _logTextEdit):
    env(_env), soln(game), approx(env,game,soln),
    logTextEdit(_logTextEdit)
  {
    approx.initialize();
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
	// Add the extreme tuples array to soln.
	if (approx.getNumIterations()==0)
	  {
	    for (vector<SGTuple>::const_iterator tuple
	    	   = approx.getExtremeTuples().begin();
	    	 tuple != approx.getExtremeTuples().end();
	    	 ++tuple)
	      soln.push_back(*tuple);
	  }
	else
	  soln.push_back(approx.getExtremeTuples().back());
	
	if (approx.generate() > env.getParam(SG::ERRORTOL)
	    && approx.getNumIterations() < env.getParam(SG::MAXITERATIONS))
	  {
	    status = NOTCONVERGED;
	    emit resultReady(false);
	  }
	else
	  {
	    approx.end();
	    
	    status = CONVERGED;
	    emit resultReady(true);
	  }
      }
    catch (exception & e)
      {
	// Add the extreme tuples array to soln.
	qDebug() << "solve failed" << endl;

	approx.end();
	
	status = FAILED;
	emit resultReady(true);
	
	// emit exceptionCaught();
      }
  } // iterate

  //! Returns the SGSolution object.
  const SGSolution & getSolution() const
  { return soln; }

  //! Returns the SGApprox object.
  const SGApprox & getApprox() const
  { return approx; }
  
signals:
  //! Signal that gets emitted when iteration finishes.
  void resultReady(bool);
  //! Signal that gets emitted when an exception is caught.
  /*! Not currently used. */
  void exceptionCaught();  
};

#endif
