#ifndef SGMAINWINDOW_HPP
#define SGMAINWINDOW_HPP

#include <QMainWindow>
#include "sggamehandler.hpp"
#include "sgsolutionhandler.hpp"
#include "sgsolverworker.hpp"
#include "sgsettingshandler.hpp"
#include "sg.hpp"
#include "risksharing.hpp"
#include "sgrisksharinghandler.hpp"

using namespace std;

//! Main window for SGViewer.
/*! This class constructs the tabs and layouts. It also contains
    signals and slots for handling certain high level functions, such
    as solving, saving, and loading saved content.

    Note that rather than using SGSolver, this class essentially
    replicates the behavior of the SGSolver class in its solveGame
    routine. The reason is that SGSolver runs straight through. We
    want to provide updates in the log window as the solver proceeds,
    which is facilitated by running SGSolverWorker one iteration at a
    time, printing the output when each revolution finishes, and then
    restarting the solve routine. Also allows the user to cancel a
    computation when the next iteration finishes.

    \ingroup viewer
 */
class SGMainWindow : public QMainWindow
{
  Q_OBJECT;

private:
  // Tab widget
  //! Widget containing game, solution, and log tabs.
  QTabWidget * tabWidget;

  //! Environment for computations
  SGEnv * env;

  //! Worker that handles solution.
  SGSolverWorker * solverWorker;
  //! Separate thread for running solve routines, so gui doesn't hang.
  QThread solverThread;

  //! The object for interfacing with the solution.
  SGSolutionHandler * solutionHandler;
  //! The object for interfacing with the game.
  SGGameHandler * gameHandler;

  //! The object for controling environment settings.
  SGSettingsHandler * settingsHandler;

  //! Text edit containing the log output.
  QTextEdit * logTextEdit;

  //! Times progress of the algorithm.
  QTime timer;

  //! Flag for canceling solve next time an iteration finishes.
  bool cancelSolveFlag;

  //! Most recent path for saves/loads.
  QString path;

public:
  //! Constructor
  /*! Initializes member objects and connects signals and slots. */
  SGMainWindow();
  
  //! Destructor
  /*! Just closes down the solver thread. Should other objects be
      destroyed? */
  ~SGMainWindow()
  {
    solverThread.quit();
    solverThread.wait();
  }
  
protected:
  //! Handles keyboard shortcuts.
  virtual void keyPressEvent(QKeyEvent * event);

signals:
  //! Signal for SGSolverThread to start next iteration.
  void startIteration();
					       
private slots:
  //! Triggers a solution load
  void loadSolution();
  //! Triggers a solution save.
  void saveSolution();
  //! Triggers a game load.
  void loadGame();
  //! Triggers a game save.
  void saveGame();

  //! Quits the program.
  void quitProgram();

  //! Changes settings for the SGEnv object
  void changeSettings();

  //! Changes settings for the SGEnv object
  void settingsHandlerClosed();

  //! Triggers a new screen shot.
  void screenShot();
  
  //! Initializes solve routine.
  void solveGame();
  //! Throws the cancel solution flag.
  void cancelSolve();
  //! Slot when iteration finishes.
  /*! Checks if the algorithm has converged or if maximum number of
      iterations has finished. If the algorithm has finished, plots
      the solution. If the algorithm has not converged and the cancel
      flag is false, the next iteration is started. If the cancel flag
      is thrown, will not trigger the next iteration. */
  void iterationFinished(bool);
  
  //! Triggers error message in log.
  /*! When the algorithm throws an exception, triggers an error
      message report. */
  void solverException();

  //! Displays an about message
  void displayAbout();

  //! Generate risk sharing game
  void generateRSG();

  //! Generate prisoner's dilemma game
  void generatePD();
};


#endif
