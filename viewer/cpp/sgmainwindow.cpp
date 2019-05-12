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

#include "sgmainwindow.hpp"

SGMainWindow::SGMainWindow()
{
  timer.start();
  
  settings = new QSettings("Benjamin Brooks", "SGViewer", this);
  settingsLoader();

  path = QDir::currentPath();

  env = new SGEnv();

  env->setParam(SG::PRINTTOCOUT,false);

  gameHandler = new SGGameHandler();

  solutionHandler = new SGSolutionHandler(this);
  solutionHandler_V2 = new SGSolutionHandler_V2(this);

  // Menu bar
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  QAction * loadSolutionAction = new QAction(tr("&Load solution"),this);
  QAction * loadSolutionAction_V2 = new QAction(tr("&Load solution V2"),this);
  QAction * loadGameAction = new QAction(tr("Load &game"),this);
  QAction * saveGameAction = new QAction(tr("Save game"),this);
  QAction * saveSolutionAction = new QAction(tr("&Save solution"),this);
  QAction * quitAction = new QAction(tr("&Quit"),this);
  fileMenu->addAction(loadSolutionAction);
  fileMenu->addAction(loadSolutionAction_V2);
  fileMenu->addAction(loadGameAction);
  fileMenu->addSeparator();
  fileMenu->addAction(saveSolutionAction);
  fileMenu->addAction(saveGameAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);
  loadSolutionAction->setShortcut(tr("Ctrl+L"));
  loadSolutionAction_V2->setShortcut(tr("Ctrl+Shift+L"));
  saveSolutionAction->setShortcut(tr("Ctrl+S"));
  loadGameAction->setShortcut(tr("Ctrl+G"));
  quitAction->setShortcut(tr("Alt+W"));


  QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
  QAction * screenShotAction = new QAction(tr("&Save a screen shot"),this);
  viewMenu->addAction(solutionHandler->getDetailedTitlesAction());
  viewMenu->addAction(solutionHandler->getEqualizeAxesAction());
  viewMenu->addSeparator();
  QAction * plotSettingsAction = new QAction(tr("&Plot settings"),this);
  viewMenu->addAction(plotSettingsAction);
  viewMenu->addSeparator();
  viewMenu->addAction(screenShotAction);

  QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));
  QAction * settingsAction = new QAction(tr("&Settings"),this);
  toolsMenu->addAction(settingsAction);
  QMenu * gamesMenu = toolsMenu->addMenu(tr("Special &games"));
  QAction * rsgAction = new QAction(tr("&Risk sharing"),this);
  QAction * pdAction = new QAction(tr("&Prisoners' Dilemmas"),this);
  QAction * bosAction = new QAction(tr("&Battle of the Sexes"),this);
  gamesMenu->addAction(rsgAction);
  gamesMenu->addAction(pdAction);
  gamesMenu->addAction(bosAction);
  toolsMenu->addSeparator();
  QAction * solveAction = new QAction(tr("&Solve game"),this);
  QAction * cancelAction = new QAction(tr("&Cancel computation"),this);
  toolsMenu->addAction(solveAction);
  toolsMenu->addAction(cancelAction);

  QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
  QAction * legendAction = new QAction(tr("&Legend"),this);
  helpMenu->addAction(legendAction);
  QAction * aboutAction = new QAction(tr("&About"),this);
  helpMenu->addAction(aboutAction);

  QWidget * mainPanel = new QWidget();
  QHBoxLayout * mainLayout = new QHBoxLayout();
  
  QWidget * gameTab = new QWidget();
  QWidget * solutionTab = new QWidget();
  QWidget * solutionTab2 = new QWidget();
  QWidget * logTab = new QWidget();
  
  solutionTab->setLayout(solutionHandler->getLayout());
  solutionTab2->setLayout(solutionHandler_V2->getLayout());


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
  connect(legendAction,SIGNAL(triggered()),
	  this,SLOT(displayLegend()));
  connect(aboutAction,SIGNAL(triggered()),
	  this,SLOT(displayAbout()));
  connect(gameHandler->getSolveButton(),SIGNAL(clicked()),
      this,SLOT(solveGame()));
  connect(gameHandler->getSolveButton_V2(),SIGNAL(clicked()),
      this,SLOT(solveGame_V2()));
  connect(gameHandler->getCancelButton(),SIGNAL(clicked()),
	  this,SLOT(cancelSolve()));
  connect(solveAction,SIGNAL(triggered()),
	  this,SLOT(solveGame()));
  connect(cancelAction,SIGNAL(triggered()),
	  this,SLOT(cancelSolve()));
  connect(settingsAction,SIGNAL(triggered()),
	  this,SLOT(changeSettings()));
  connect(plotSettingsAction,SIGNAL(triggered()),
	  this,SLOT(changePlotSettings()));
  connect(rsgAction,SIGNAL(triggered()),
	  this,SLOT(generateRSG()));
  connect(pdAction,SIGNAL(triggered()),
	  this,SLOT(generatePD()));
  connect(bosAction,SIGNAL(triggered()),
	  this,SLOT(generateBoS()));
  
  connect(loadSolutionAction_V2,SIGNAL(triggered()),
	  this,SLOT(loadSolution_V2()));

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
  tabWidget->addTab(solutionTab,"Solution - Pencil Sharpening");
  tabWidget->addTab(solutionTab2,"Solution - ABS 2018");
  tabWidget->addTab(logTab,"Log");
  mainLayout->addWidget(tabWidget);

  mainPanel->setLayout(mainLayout);
  
  setCentralWidget(mainPanel);
  setWindowState(Qt::WindowMaximized);

  generatePD();
  
  setWindowTitle(tr("SGViewer"));

} // constructor

void SGMainWindow::settingsLoader()
{
    path = settings->value("LastDir", QDir::homePath()).toString();
}

void SGMainWindow::settingsSaver()
{
    settings->setValue("LastDir", path);
}

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

      gameHandler->setGame(soln.getGame());
      solutionHandler->setSolution(soln);
      
      tabWidget->setCurrentIndex(1);

      QFileInfo info(path);
      
      QString newWindowTitle(tr("SGViewer - "));
      newWindowTitle += info.fileName();
      setWindowTitle(newWindowTitle);
      
    }
  catch (...)
    {
      qDebug() << "Load solution didnt work :(" << endl;
      QErrorMessage em(this);
      em.showMessage(QString("Load solution didnt work :("));
    }
} // loadSolution

void SGMainWindow::loadSolution_V2()
{
  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a solution file"),
                         path,
						 tr("SGViewer solution files (*.sln2)"));

  if (newPath.isEmpty())
    return;

  path = newPath;
  QFileInfo info(path);

  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      SGSolution_MaxMinMax soln;
      
      SGSolution_MaxMinMax::load(soln,newPath_c);

      if (soln.getIterations().empty())
	{
	  qDebug() << "Solution has no iterations." << endl;
	  throw(1);
	}
      
      gameHandler->setGame(soln.getGame());
      solutionHandler_V2->setSolution(soln);
      
      tabWidget->setCurrentIndex(2);

      QString newWindowTitle(tr("SGViewer - "));
      newWindowTitle += info.fileName();
      setWindowTitle(newWindowTitle);
      
    }
  catch (std::exception & e)
    {
      qDebug() << "Load solution didnt work :(" << endl;
      QErrorMessage em(this);
      em.showMessage(QString("Load solution didnt work :("));

      qDebug() << "Error message:" << e.what() << endl;
    }
} // loadSolution_V2

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
  tabWidget->setCurrentIndex(3);

  try
    {

      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Starting a new computation with pencil sharpening..."));
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

void SGMainWindow::solveGame_V2()
{
  tabWidget->setCurrentIndex(3);

  try
    {

      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Starting a new computation with Max-Min-Max..."));
      logTextEdit->append(QString(""));

      cancelSolveFlag = false;

//      Needs to be updated to use a new solver worker.

     solverWorker_v2 = new SGSolverWorker_V2(*env,
                   gameHandler->getGame(),
                   logTextEdit);
     solverWorker_v2->moveToThread(&solverThread);
     connect(this,SIGNAL(startIteration_V2()),
         solverWorker_v2,SLOT(iterate()));
     connect(solverWorker_v2,SIGNAL(resultReady(bool)),
             this,SLOT(iterationFinished_V2(bool)));
     connect(solverWorker_v2,SIGNAL(exceptionCaught()),
         this,SLOT(solverException_V2()));
     solverThread.start();

     timer.restart();

     emit startIteration_V2();
    }
  catch (exception & e)
    {
      QMessageBox::critical(this,tr("Solver failed"),
                tr("SGSolver was not able to solve your game.\nMaybe no pure strategy equilibria exist?"),
                QMessageBox::Ok);
    }
} // solveGame_V2

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

	  tabWidget->setCurrentIndex(3);
	  
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

void SGMainWindow::iterationFinished_V2(bool tf)
{
  string str;

  switch (solverWorker_v2->getStatus())
    {
    case SGSolverWorker::NOTCONVERGED:
      
      if (cancelSolveFlag)
	{
	  str = solverWorker_v2->getSolver().progressString();
	  logTextEdit->append(QString(str.c_str()));
	  logTextEdit->append(QString(""));
	  logTextEdit->append(QString("Computation canceled."));

	  tabWidget->setCurrentIndex(3);
	  
	  break;
	}
      else
	{
	  string str = solverWorker_v2->getSolver().progressString();
	  logTextEdit->append(QString(str.c_str()));

	  emit startIteration_V2();

	  return;
	}

    case SGSolverWorker::CONVERGED:
      str = solverWorker_v2->getSolver().progressString();
      logTextEdit->append(QString(str.c_str()));
      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Computation complete!"));


      break;

    case SGSolverWorker::FAILED:
      str = solverWorker_v2->getSolver().progressString();
      logTextEdit->append(QString(str.c_str()));
      logTextEdit->append(QString(""));
      logTextEdit->append(QString("Computation failed."));

      break;
      
    }

  solutionHandler_V2->setSolution(solverWorker_v2->getSolution());

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

  tabWidget->setCurrentIndex(2);
  
  delete solverWorker_v2;

} // iterationFinished_V2

void SGMainWindow::solverException()
{
  logTextEdit->append(QString("Unknown exception caught: Possibly no pure strategy equilibria exist."));

  delete solverWorker;
} // solverException

void SGMainWindow::solverException_V2()
{
  logTextEdit->append(QString("Unknown exception caught: Possibly no pure strategy equilibria exist."));

  delete solverWorker_v2;
} // solverException

void SGMainWindow::keyPressEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_T
      && event->modifiers() == Qt::ControlModifier)
    tabWidget->setCurrentIndex((tabWidget->currentIndex()+1)%tabWidget->count());
  else if (event->key() == Qt::Key_B
	   && event->modifiers() == Qt::ControlModifier)
    {
      // Move iteration forwards
      solutionHandler->moveForwards();
    }
  else if (event->key() == Qt::Key_F
	   && event->modifiers() == Qt::ControlModifier)
    {
      // Move iteration backwards
      solutionHandler->moveBackwards();
    }
  else if (event->key() == Qt::Key_P
	   && event->modifiers() == Qt::ControlModifier)
    {
      // previous action
      solutionHandler->prevAction();
    }
  else if (event->key() == Qt::Key_N
	   && event->modifiers() == Qt::ControlModifier)
    {
      // next action
      solutionHandler->nextAction();
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
  aboutBox.setInformativeText("\n\nSGViewer is part of the SGSolve library for stochastic games\n\nCopyright (C) 2016 Benjamin A. Brooks\n\nSGSolve implements the algorithm for solving stochastic games developed by in Abreu, Brooks, and Sannikov (2016). Inquiries can be directed to:\n\nBen Brooks\nChicago, IL\nben@benjaminbrooks.net\nFebruary 5, 2016");

  aboutBox.exec();
} // displayAbout

void SGMainWindow::displayLegend()
{
  SGLegend legend(this);
  
  legend.exec();
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


void SGMainWindow::changePlotSettings()
{
  plotSettingsHandler = new SGPlotSettingsHandler(this,
						  solutionHandler_V2->getPlotSettingsPtr());

  plotSettingsHandler->adjustSize();
  plotSettingsHandler->move(this->pos()
			    +this->rect().center()
			    -plotSettingsHandler->pos()
			    -plotSettingsHandler->rect().center());
  connect(plotSettingsHandler,SIGNAL(closePlotSettingsHandler()),
	  this,SLOT(plotSettingsHandlerClosed()));
  plotSettingsHandler->show();
}

void SGMainWindow::plotSettingsHandlerClosed()
{
  delete plotSettingsHandler;
}

void SGMainWindow::generateRSG()
{
  double delta = 0.85;
  int numEndowments = 5;
  int c2e = 5;
  double persistence = 0;

  RiskSharingGame::EndowmentMode endowmentMode = RiskSharingGame::Consumption;

  SGRiskSharingHandler rsh(this,numEndowments,c2e,persistence);
  if (rsh.exec()==QDialog::Accepted)
    {
  
      RiskSharingGame rsg(delta,numEndowments,
			  c2e,persistence,endowmentMode);

      SGGame game(rsg);
      gameHandler->setGame(game);

      tabWidget->setCurrentIndex(0);

      QString newWindowTitle(tr("SGViewer - Risk sharing"));
      setWindowTitle(newWindowTitle);
    }
} // generateRSG

void SGMainWindow::generatePD()
{
  double delta = 0.7;
  
  int action, state, player;
  int numPlayers = 2;
  int numStates = 2;

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,2));
  vector<int> numActions_total(numStates,4);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(4,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 0;
  payoffs[0][1][0] = -1;
  payoffs[0][2][0] = 2;
  payoffs[0][3][0] = 1;
  payoffs[0][0][1] = 0;
  payoffs[0][1][1] = 2;
  payoffs[0][2][1] = -1;
  payoffs[0][3][1] = 1;

  payoffs[1][0][0] = 2;
  payoffs[1][1][0] = 1;
  payoffs[1][2][0] = 4;
  payoffs[1][3][0] = 3;
  payoffs[1][0][1] = 2;
  payoffs[1][1][1] = 4;
  payoffs[1][2][1] = 1;
  payoffs[1][3][1] = 3;

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(4,vector<double>(numStates,1.0)));
  probabilities[0][0][0] = 1.0/3.0;
  probabilities[0][1][0] = 0.5;
  probabilities[0][2][0] = 0.5;
  probabilities[0][3][0] = 1.0/3.0;
  probabilities[1][0][0] = 2.0/3.0;
  probabilities[1][1][0] = 0.5;
  probabilities[1][2][0] = 0.5;
  probabilities[1][3][0] = 2.0/3.0;
  for (int s = 0; s < 2; s ++)
    {
      for (int a = 0; a < 4; a ++)
	probabilities[s][a][1] = 1.0-probabilities[s][a][0];
    }

  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);
  
  gameHandler->setGame(game);

  tabWidget->setCurrentIndex(0);

  QString newWindowTitle(tr("SGViewer - Prisoners' dilemma"));
  setWindowTitle(newWindowTitle);

} // generatePD

void SGMainWindow::generateBoS()
{
  double delta = 0.4;
  
  int action, state, player;
  int numPlayers = 2;
  int numStates = 1;

  vector< vector< int > > numActions(numStates,vector<int>(numPlayers,2));
  vector<int> numActions_total(numStates,4);

  // Payoffs
  vector< vector< vector<double> > > 
    payoffs(numStates,vector< vector<double> >(4,vector<double>(numPlayers,0.0)));
  payoffs[0][0][0] = 0;
  payoffs[0][1][0] = 1;
  payoffs[0][2][0] = 3;
  payoffs[0][3][0] = 0;
  payoffs[0][0][1] = 0;
  payoffs[0][1][1] = 3;
  payoffs[0][2][1] = 1;
  payoffs[0][3][1] = 0;

  // Transition probabilities
  vector< vector< vector<double> > >
    probabilities(numStates,vector< vector<double> >(4,vector<double>(numStates,1.0)));

  SGGame game(delta,
	      numStates,
	      numActions,
	      payoffs,
	      probabilities);
  
  gameHandler->setGame(game);

  tabWidget->setCurrentIndex(0);

  QString newWindowTitle(tr("SGViewer - Battle of the Sexes"));
  setWindowTitle(newWindowTitle);

} // generateBoS
