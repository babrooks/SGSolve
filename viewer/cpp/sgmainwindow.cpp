#include <QtWidgets>
#include "sgmainwindow.hpp"

SGMainWindow::SGMainWindow()
{
  timer.start();

  path = QString("./");
  
  gameHandler = new SGGameHandler();
  gameHandler->setGame(SGGame());

  solutionHandler = new SGSolutionHandler();

  // Menu bar
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  loadSolutionAction = new QAction(tr("&Load solution"),this);
  loadGameAction = new QAction(tr("Load &game"),this);
  saveSolutionAction = new QAction(tr("&Save solution"),this);
  saveGameAction = new QAction(tr("Save game"),this);
  fileMenu->addAction(loadSolutionAction);
  fileMenu->addAction(loadGameAction);
  fileMenu->addSeparator();
  fileMenu->addAction(saveSolutionAction);
  fileMenu->addAction(saveGameAction);
  loadSolutionAction->setShortcut(tr("Ctrl+L"));
  saveSolutionAction->setShortcut(tr("Ctrl+S"));
  loadGameAction->setShortcut(tr("Ctrl+G"));

  QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
  screenShotAction = new QAction(tr("&Save a screen shot"),this);
  viewMenu->addAction(solutionHandler->detailedTitlesAction);
  viewMenu->addSeparator();
  viewMenu->addAction(screenShotAction);

  botPanel = new QWidget();

  // QPalette botPalette = botPanel->palette();
  // botPalette.setColor(QPalette::Background,Qt::red);
  // botPanel->setPalette(botPalette);
  // botPanel->setAutoFillBackground(true);

  topRightScrollArea = new QScrollArea();
  
  QWidget * statePlotsWidget = new QWidget();
  statePlotsWidget->setLayout(solutionHandler->statePlotsLayout);
  topRightScrollArea->setWidget(statePlotsWidget);

  topRightScrollArea->setWidgetResizable(true);
  topRightScrollArea->setSizePolicy(QSizePolicy::Expanding,
  				    QSizePolicy::Preferred);
  topRightScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  QHBoxLayout * topLayout = new QHBoxLayout();
  topLayout->addWidget(solutionHandler->detailPlot);
  topLayout->addWidget(topRightScrollArea);

  QWidget * topPanel = new QWidget();
  topPanel->setLayout(topLayout);

  // Add sliders to bottom panel

  QFormLayout * botLayout = new QFormLayout(botPanel);
  botLayout->addRow(new QLabel(tr("End iteration:")),
		    solutionHandler->iterSlider);
  botLayout->addRow(new QLabel(tr("Start iteration:")),
		    solutionHandler->startSlider);
  botLayout->addRow(new QLabel(tr("Action:")),
		    solutionHandler->actionSlider);
  botLayout->addRow(new QLabel(tr("State:")),
		    solutionHandler->stateSlider);
  botPanel->setFixedHeight(100);
  botLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  
  QVBoxLayout * solutionLayout = new QVBoxLayout();
  solutionLayout->addWidget(topPanel);
  solutionLayout->addWidget(botPanel);

  QWidget * mainPanel = new QWidget();
  QHBoxLayout * mainLayout = new QHBoxLayout();
  
  QWidget * gameTab = new QWidget();
  QWidget * solutionTab = new QWidget();
  QWidget * logTab = new QWidget();
  
  solutionTab->setLayout(solutionLayout);


  // Game tab
  QVBoxLayout * gameLayout = new QVBoxLayout();
  QHBoxLayout * controlLayout = new QHBoxLayout();
  QFormLayout * leftControlLayout = new QFormLayout();
  QFormLayout * centerControlLayout = new QFormLayout();
  QFormLayout * rightControlLayout = new QFormLayout();
  QHBoxLayout * tableLayout = new QHBoxLayout();
  QVBoxLayout * payoffLayout = new QVBoxLayout();
  QVBoxLayout * probabilityLayout = new QVBoxLayout();
  
  solveButton = new QPushButton(tr("Solve"));
  solveButton->setSizePolicy(QSizePolicy::Fixed,
			     QSizePolicy::Preferred);
  solveButton->resize(300,solveButton->height());

  cancelButton = new QPushButton(tr("Cancel"));
  cancelButton->setSizePolicy(QSizePolicy::Fixed,
			      QSizePolicy::Preferred);
  cancelButton->resize(300,cancelButton->height());

  QHBoxLayout * deltaLayout = new QHBoxLayout();
  deltaLayout->addWidget(gameHandler->deltaEdit);
  deltaLayout->setSpacing(5);

  QHBoxLayout * currentStateLayout = new QHBoxLayout();
  currentStateLayout->addWidget(gameHandler->currentStateCombo);
  currentStateLayout->addWidget(gameHandler->prevStateButton);
  currentStateLayout->addWidget(gameHandler->nextStateButton);
  currentStateLayout->setSpacing(5);
  
  centerControlLayout->addRow(new QLabel(tr("Discount factor:")),
			      deltaLayout);
  centerControlLayout->addRow(new QLabel(tr("Error tolerance:")),
			      gameHandler->errorTolEdit);
  centerControlLayout->addRow(new QLabel(tr("Current state:")),
			      currentStateLayout);
  // leftControlLayout->addRow(gameHandler->removeStateButton,
  // 			    gameHandler->addStateButton);
  // leftControlLayout->setSpacing(0);

  for (int player = 0; player < 2; player ++)
    {
      QHBoxLayout * numActionsLayout = new QHBoxLayout();
      numActionsLayout->addWidget(gameHandler->numActionsEdits[player]);
      numActionsLayout->addWidget(gameHandler->removeActionButtons[player]);
      numActionsLayout->addWidget(gameHandler->addActionButtons[player]);
      numActionsLayout->setSpacing(5);

      QString numActionsLabel = QString(tr("Player "))
	+QString::number(player+1)
	+QString(tr("'s number of actions ("));
      if (player == 0)
	numActionsLabel += QString(tr("row"));
      else
	numActionsLabel += QString(tr("column"));
      numActionsLabel += QString(tr("):"));
      
      leftControlLayout->addRow(numActionsLabel,
				numActionsLayout);
      
    }

  QHBoxLayout * numStatesLayout = new QHBoxLayout();
  numStatesLayout->addWidget(gameHandler->numStatesEdit);
  numStatesLayout->addWidget(gameHandler->removeStateButton);
  numStatesLayout->addWidget(gameHandler->addStateButton);
  numStatesLayout->setSpacing(5);
  leftControlLayout->addRow(new QLabel(tr("Number of states:")),
			    numStatesLayout);
  leftControlLayout->setSpacing(5);

  
  rightControlLayout->addRow(gameHandler->feasibleCheckBox);
  rightControlLayout->addRow(solveButton);
  rightControlLayout->addRow(cancelButton);
  
  controlLayout->addLayout(leftControlLayout);
  controlLayout->addLayout(centerControlLayout);
  controlLayout->addLayout(rightControlLayout);

  payoffLayout->addWidget(new QLabel(tr("Stage payoffs:")));
  payoffLayout->addWidget(gameHandler->payoffTableView);

  probabilityLayout->addWidget(new QLabel(tr("Transition probabilities:")));
  probabilityLayout->addWidget(gameHandler->probabilityTableView);

  tableLayout->addLayout(payoffLayout);
  tableLayout->addLayout(probabilityLayout);

  // resizePayoffTable(0,2,0,2);
  // resizeProbabilityTable(0,2,0,2,0,1);
  
  gameLayout->addLayout(controlLayout);
  gameLayout->addLayout(tableLayout);

  gameTab->setLayout(gameLayout);

  // Connections
  connect(loadSolutionAction,SIGNAL(triggered()),
	  this,SLOT(loadSolution()));
  connect(saveSolutionAction,SIGNAL(triggered()),
	  this,SLOT(saveSolution()));
  connect(loadGameAction,SIGNAL(triggered()),
	  this,SLOT(loadGame()));
  connect(saveGameAction,SIGNAL(triggered()),
	  this,SLOT(saveGame()));
  connect(screenShotAction,SIGNAL(triggered()),
	  this,SLOT(screenShot()));
  connect(solveButton,SIGNAL(clicked()),
	  this,SLOT(solveGame()));
  connect(cancelButton,SIGNAL(clicked()),
	  this,SLOT(cancelSolve()));

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

      gameHandler->setGame(soln.game);
      solutionHandler->setSolution(soln);
      
      tabWidget->setCurrentIndex(1);
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
}

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
    }
  catch (std::exception & e)
    {
      qDebug() << "Load game didnt work :(" << endl;
    }
}

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

      solverWorker = new SGSolverWorker(gameHandler->getGame(),
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
}

void SGMainWindow::cancelSolve()
{
  cancelSolveFlag = true;
}

void SGMainWindow::iterationFinished(bool tf)
{
  string str;

  switch (solverWorker->getStatus())
    {
    case SGSolverWorker::NOTCONVERGED:
      
      if (cancelSolveFlag)
	{
	  logTextEdit->append("Computation canceled by user");
	  tabWidget->setCurrentIndex(2);
	}
      else
	{
	  if (solverWorker->getApprox().passedNorth())
	    {
	      string str = solverWorker->getApprox().progressString();
	      logTextEdit->append(QString(str.c_str()));
	    }

	  emit startIteration();
	}
      return;

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
}

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
}


void SGMainWindow::screenShot()
{
  QString newPath = QFileDialog::getSaveFileName(this, tr("Save PNG"),
						 path, tr("PNG files (*.png)"));
  
  if (newPath.isEmpty())
    return;

  QFileInfo fi(newPath);
  path = fi.canonicalPath();

  grab().save(newPath);
}
