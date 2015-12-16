#include <QtWidgets>
#include "sgmainwindow.hpp"

SGMainWindow::SGMainWindow()
{
  timer.start();
  
  env = new SGEnv();

  env->setParam(SGEnv::PRINTTOCOUT,false);

  path = QString("./");
  
  gameHandler = new SGGameHandler();

  solutionHandler = new SGSolutionHandler(this);

  // Menu bar
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  QAction * loadSolutionAction = new QAction(tr("&Load solution"),this);
  QAction * loadGameAction = new QAction(tr("Load &game"),this);
  QAction * saveSolutionAction = new QAction(tr("&Save solution"),this);
  QAction * saveGameAction = new QAction(tr("Save game"),this);
  QAction * quitAction = new QAction(tr("&Quit"),this);
  fileMenu->addAction(loadSolutionAction);
  fileMenu->addAction(loadGameAction);
  fileMenu->addSeparator();
  fileMenu->addAction(saveSolutionAction);
  fileMenu->addAction(saveGameAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);
  loadSolutionAction->setShortcut(tr("Ctrl+L"));
  saveSolutionAction->setShortcut(tr("Ctrl+S"));
  loadGameAction->setShortcut(tr("Ctrl+G"));
  quitAction->setShortcut(tr("Alt+W"));

  QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
  QAction * screenShotAction = new QAction(tr("&Save a screen shot"),this);
  viewMenu->addAction(solutionHandler->getDetailedTitlesAction());
  viewMenu->addAction(solutionHandler->getEqualizeAxesAction());
  viewMenu->addSeparator();
  viewMenu->addAction(screenShotAction);

  QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));
  QAction * settingsAction = new QAction(tr("&Settings"),this);
  toolsMenu->addAction(settingsAction);

  QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
  QAction * aboutAction = new QAction(tr("&About"),this);
  helpMenu->addAction(aboutAction);

  QWidget * mainPanel = new QWidget();
  QHBoxLayout * mainLayout = new QHBoxLayout();
  
  QWidget * gameTab = new QWidget();
  QWidget * solutionTab = new QWidget();
  QWidget * logTab = new QWidget();
  
  solutionTab->setLayout(solutionHandler->getLayout());


  // Game tab
  gameTab->setLayout(gameHandler->getLayout());

  // Connections
  connect(loadSolutionAction,SIGNAL(triggered()),
	  this,SLOT(loadSolution()));
  connect(saveSolutionAction,SIGNAL(triggered()),
	  this,SLOT(saveSolution()));
  connect(loadGameAction,SIGNAL(triggered()),
	  this,SLOT(loadGame()));
  connect(saveGameAction,SIGNAL(triggered()),
	  this,SLOT(saveGame()));
  connect(quitAction,SIGNAL(triggered()),
	  this,SLOT(quitProgram()));
  connect(screenShotAction,SIGNAL(triggered()),
	  this,SLOT(screenShot()));
  connect(aboutAction,SIGNAL(triggered()),
	  this,SLOT(displayAbout()));
  connect(gameHandler->getSolveButton(),SIGNAL(clicked()),
	  this,SLOT(solveGame()));
  connect(gameHandler->getCancelButton(),SIGNAL(clicked()),
	  this,SLOT(cancelSolve()));
  connect(settingsAction,SIGNAL(triggered()),
	  this,SLOT(changeSettings()));
  
  // Log tab
  QHBoxLayout * logEditLayout = new QHBoxLayout();
  logTextEdit = new QTextEdit();
  logEditLayout->addWidget(logTextEdit);
  logTab->setLayout(logEditLayout);

  // // redirect output to log edit
  // new Q_DebugStream (std::cout,logTextEdit);
  // Q_DebugStream::registerQDebugMessageHandler();
  
  // Main widget
  tabWidget = new QTabWidget();
  tabWidget->addTab(gameTab,"Game");
  tabWidget->addTab(solutionTab,"Solution");
  tabWidget->addTab(logTab,"Log");
  mainLayout->addWidget(tabWidget);

  mainPanel->setLayout(mainLayout);
  
  setCentralWidget(mainPanel);
  setWindowState(Qt::WindowMaximized);
  
  setWindowTitle(tr("SGViewer"));

} // constructor

void SGMainWindow::loadSolution()
{
  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a solution file"),
						 "./",
						 tr("SGViewer solution files (*.sln)"));

  if (newPath.isEmpty())
    return;

  path = newPath;

  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      SGSolution soln;
      
      SGSolution::load(soln,newPath_c);

      gameHandler->setGame(soln.game);
      solutionHandler->setSolution(soln);
      
      tabWidget->setCurrentIndex(1);

      QFileInfo info(path);
      
      QString newWindowTitle(tr("SGViewer - "));
      newWindowTitle += info.fileName();
      setWindowTitle(newWindowTitle);
      
    }
  catch (std::exception & e)
    {
      qDebug() << "Load solution didnt work :(" << endl;
    }
} // loadSolution

void SGMainWindow::saveSolution()
{
  QString newPath = QFileDialog::getSaveFileName(this,tr("Select a solution file"),
						 "./",
						 tr("SGViewer solution files (*.sln)"));

  if (newPath.isEmpty())
    return;

  QFileInfo fi(newPath);
  path = fi.canonicalPath();


  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      SGSolution::save(solutionHandler->getSolution(),
		       newPath_c);
    }
  catch (std::exception & e)
    {
      qDebug() << "Save solution didnt work :(" << endl;
    }
} // saveSolution

void SGMainWindow::loadGame()
{
  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a game file"),
						 path,
						 tr("SGViewer game files (*.sgm)"));

  if (newPath.isEmpty())
    return;

  QFileInfo fi(newPath);
  path = fi.canonicalPath();



  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      SGGame game;
      
      SGGame::load(game,newPath_c);

      gameHandler->setGame(game);

      tabWidget->setCurrentIndex(0);

      QString newWindowTitle(tr("SGViewer - "));
      newWindowTitle += fi.fileName();
      setWindowTitle(newWindowTitle);
    }
  catch (std::exception & e)
    {
      qDebug() << "Load game didnt work :(" << endl;
    }
} // loadGame

void SGMainWindow::saveGame()
{
  QString newPath = QFileDialog::getSaveFileName(this,tr("Select a game file"),
						 "./",
						 tr("SGViewer game files (*.sgm)"));

  if (newPath.isEmpty())
    return;

  QFileInfo fi(newPath);
  path = fi.canonicalPath();


  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      SGGame::save(gameHandler->getGame(),
		   newPath_c);
    }
  catch (std::exception & e)
    {
      qDebug() << "Save game didnt work :(" << endl;
    }
} // saveGame

void SGMainWindow::quitProgram()
{
  QApplication::quit();
}

void SGMainWindow::solveGame()
{
  tabWidget->setCurrentIndex(2);
  
  try
    {

      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Starting a new computation..."));
      logTextEdit->append(QString(""));
      
      cancelSolveFlag = false;
      
      solverWorker = new SGSolverWorker(*env,
					gameHandler->getGame(),
					logTextEdit);
      solverWorker->moveToThread(&solverThread);
      connect(this,SIGNAL(startIteration()),
	      solverWorker,SLOT(iterate()));
      connect(solverWorker,SIGNAL(resultReady(bool)),
      	      this,SLOT(iterationFinished(bool)));
      connect(solverWorker,SIGNAL(exceptionCaught()),
	      this,SLOT(solverException()));
      solverThread.start();
      
      timer.restart();
      
      emit startIteration();
    }
  catch (exception & e)
    {
      QMessageBox::critical(this,tr("Solver failed"),
			    tr("SGSolver was not able to solve your game.\nMaybe no pure strategy equilibria exist?"),
			    QMessageBox::Ok);
    }
} // solveGame

void SGMainWindow::cancelSolve()
{
  cancelSolveFlag = true;
} // cancelSolve

void SGMainWindow::iterationFinished(bool tf)
{
  string str;

  switch (solverWorker->getStatus())
    {
    case SGSolverWorker::NOTCONVERGED:
      
      if (cancelSolveFlag)
	{
	  str = solverWorker->getApprox().progressString();
	  logTextEdit->append(QString(str.c_str()));
	  logTextEdit->append(QString(""));
	  logTextEdit->append(QString("Computation canceled."));

	  tabWidget->setCurrentIndex(2);
	  
	  break;
	}
      else
	{
	  if (solverWorker->getApprox().passedNorth())
	    {
	      string str = solverWorker->getApprox().progressString();
	      logTextEdit->append(QString(str.c_str()));
	    }

	  emit startIteration();

	  return;
	}

    case SGSolverWorker::CONVERGED:
      str = solverWorker->getApprox().progressString();
      logTextEdit->append(QString(str.c_str()));
      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Computation complete!"));


      break;

    case SGSolverWorker::FAILED:
      str = solverWorker->getApprox().progressString();
      logTextEdit->append(QString(str.c_str()));
      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Computation failed."));

      break;
      
    }

  solutionHandler->setSolution(solverWorker->getSolution());

  int telapsed = timer.elapsed();
      
  QString timeString("Time elapsed: ");

  int hrs = telapsed/(60*60*1000);
  telapsed -= hrs*60*60*1000;
  int mins = telapsed/(60*1000);
  telapsed -= mins*60*1000;
  double secs = telapsed/1000.0;
      
  if (hrs)
    {
      timeString += QString::number(hrs);
      timeString += " hours, ";
      timeString += QString::number(mins);
      timeString += QString(" minutes, and ");
    }
  else if (mins)
    {
	  
      timeString += QString::number(mins);
      timeString += QString(" minutes and ");
    }

  timeString += QString::number(secs);
  timeString += QString(" seconds");
	
  logTextEdit->append(timeString);

  tabWidget->setCurrentIndex(1);
  
  delete solverWorker;

} // iterationFinished

void SGMainWindow::solverException()
{
  logTextEdit->append(QString("Unknown exception caught: Possibly no pure strategy equilibria exist."));

  delete solverWorker;
} // solverException

void SGMainWindow::keyPressEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_T
      && event->modifiers() == Qt::ControlModifier)
    tabWidget->setCurrentIndex((tabWidget->currentIndex()+1)%tabWidget->count());
  else if (event->key() == Qt::Key_B
	   && event->modifiers() == Qt::ControlModifier)
    {
      solutionHandler->moveForwards();
    }
  else if (event->key() == Qt::Key_F
	   && event->modifiers() == Qt::ControlModifier)
    {
      solutionHandler->moveBackwards();
    }
} // keyPressEvent


void SGMainWindow::screenShot()
{
  QString newPath = QFileDialog::getSaveFileName(this, tr("Save PNG"),
						 path, tr("PNG files (*.png)"));
  
  if (newPath.isEmpty())
    return;

  QFileInfo fi(newPath);
  path = fi.canonicalPath();

  grab().save(newPath);
} // screenShot

void SGMainWindow::displayAbout()
{
  QMessageBox aboutBox;

  aboutBox.setWindowTitle("About");
  aboutBox.setText("About SGViewer");
  aboutBox.setInformativeText("SGViewer was designed by Dilip Abreu, Ben Brooks, and Yuliy Sannikov. It implements the algorithm for solving stochastic games described in Abreu, Brooks, and Sannikov (2015). Inuiries can be directed to:\n\nBen Brooks\nChicago, IL\nbabrooks@uchicago.edu\n3/23/2013");

  aboutBox.exec();
}

void SGMainWindow::changeSettings()
{
  settingsHandler = new SGSettingsHandler(this,env);

  settingsHandler->adjustSize();
  settingsHandler->move(this->pos()
			+this->rect().center()
			-settingsHandler->pos()
			-settingsHandler->rect().center());
  connect(settingsHandler,SIGNAL(closeSettingsHandler()),
	  this,SLOT(settingsHandlerClosed()));
  settingsHandler->show();
}

void SGMainWindow::settingsHandlerClosed()
{
  delete settingsHandler;
}

