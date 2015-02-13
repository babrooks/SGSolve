#ifndef SGMAINWINDOW_HPP
#define SGMAINWINDOW_HPP

#include <QMainWindow>
#include "sggamehandler.hpp"
#include "sgsolutionhandler.hpp"
#include "sgsolverworker.hpp"
#include "sg.hpp"

using namespace std;

class SGMainWindow : public QMainWindow
{
  Q_OBJECT;

private:
  // Panels
  QWidget * topPanel;
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
