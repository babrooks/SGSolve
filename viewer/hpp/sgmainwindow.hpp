#ifndef SGMAINWINDOW_HPP
#define SGMAINWINDOW_HPP

#include <QMainWindow>
#include "sggamehandler.hpp"
#include "sgsolutionhandler.hpp"
// #include "Q_DebugStream.h"

#include "sg.hpp"
#include "sgapprox.hpp"

using namespace std;

class SGSolverWorker : public QObject
{
  Q_OBJECT;

private:
  SGEnv env;
  SGSolution soln;
  SGApprox approx;
  QTextEdit * logTextEdit;

public:
  enum STATUS
    {
      FAILED,
      CONVERGED,
      NOTCONVERGED
    } status;

  SGSolverWorker(const SGGame & game,
		 QTextEdit * _logTextEdit):
    soln(game), approx(env,game,soln),
    logTextEdit(_logTextEdit)
  {
    env.setParam(SGEnv::PRINTTOCOUT,false);

    approx.initialize();
  }

  STATUS getStatus() const { return status; }

public slots:

  void iterate()
  {
    try
      {
	if (approx.generate() > env.errorTol
	    && approx.numIterations < env.maxIterations)
	  {
	    status = NOTCONVERGED;
	    emit resultReady(false);
	  }
	else
	  {
    
	    // Add the extreme tuples array to soln.
	    for (vector<SGTuple>::const_iterator tuple
		   = approx.extremeTuples.begin();
		 tuple != approx.extremeTuples.end();
		 ++tuple)
	      soln.push_back(*tuple);

	    approx.end();
	    
	    status = CONVERGED;
	    emit resultReady(true);
	  }
      }
    catch (exception & e)
      {
	// Add the extreme tuples array to soln.
	qDebug() << "solve failed" << endl;

	for (vector<SGTuple>::const_iterator tuple
	       = approx.extremeTuples.begin();
	     tuple != approx.extremeTuples.end();
	     ++tuple)
	  soln.push_back(*tuple);

	approx.end();
	
	status = FAILED;
	emit resultReady(true);
	
	// emit exceptionCaught();
      }
  }

  const SGSolution & getSolution() const
  { return soln; }

  const SGApprox & getApprox() const
  { return approx; }
  
signals:
  void resultReady(bool);
  void iterationFinished();
  void exceptionCaught();  
};

class SGMainWindow : public QMainWindow
{
  Q_OBJECT;

private:
  // Panels
  QWidget * topPanel;
  QScrollArea * topRightScrollArea;
  QWidget * botPanel;

  SGSolverWorker * solverWorker;
  QThread solverThread;
  
  // Buttons
  QPushButton * solveButton;
  QPushButton * cancelButton;

  // Tab widget
  QTabWidget * tabWidget;
  
  // Actions
  QAction * loadSolutionAction;
  QAction * loadGameAction;
  QAction * saveSolutionAction;
  QAction * saveGameAction;

  QAction * screenShotAction;

  SGSolutionHandler * solutionHandler;
  SGGameHandler * gameHandler;

  QTextEdit * logTextEdit;

  QTime timer;

  bool cancelSolveFlag;

  QString path;

public:
  SGMainWindow();
  
  ~SGMainWindow()
  {
    solverThread.quit();
    solverThread.wait();
  }
  
protected:
  virtual void keyPressEvent(QKeyEvent * event);

signals:
  void startIteration();
					       
public slots:
  void loadSolution();
  void saveSolution();
  void loadGame();
  void saveGame();

  void screenShot();
  
  void solveGame();
  void cancelSolve();
  void iterationFinished(bool);
  void solverException();
};


#endif
