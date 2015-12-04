#include "sgsimulationhandler.hpp"

SGSimulationHandler::SGSimulationHandler(QWidget * parent, 
					 const SGSolution & _soln,
					 const SGPoint & _point,
					 int _state,
					 list<SGIteration>::const_iterator _startOfLastRev)
  : QWidget(parent), soln(_soln), 
    point(_point), state(_state),
    startOfLastRev(_startOfLastRev),
    sim(soln)
{
  setWindowFlags(Qt::Window);
  setWindowTitle(tr("SGViewer: Equilibrium simulation"));
  // setWindowModality(Qt::WindowModal);
  
  setMinimumSize(parent->size()/2);

  QMenuBar * menuBar = new QMenuBar();
  menuBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  QMenu * fileMenu = menuBar->addMenu(tr("&File"));
  QAction * closeAction = new QAction(tr("&Close simulation"),this);
  fileMenu->addAction(closeAction);
  closeAction->setShortcut(tr("Alt+W"));

  QVBoxLayout * mainLayout = new QVBoxLayout();
  mainLayout->addWidget(menuBar);

  setLayout(mainLayout);

  QHBoxLayout * ewLayout = new QHBoxLayout();
  
  QFormLayout * controlLayout = new QFormLayout();
  iterationEdit = new QLineEdit(QString("100"));
  iterationEdit->setFixedWidth(100);
  controlLayout->addRow(new QLabel(tr("Number of periods:")),
			iterationEdit);

  simulateButton = new QPushButton(tr("Simulate"));

  iterationEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
  simulateButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

  controlLayout->addRow(simulateButton);

  QSplitter * editSplitter = new QSplitter();
  editSplitter->setOrientation(Qt::Vertical);
  
  textEdit = new QTextEdit();
  textEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);
  QSizePolicy policy = textEdit->sizePolicy();
  policy.setVerticalStretch(1);
  textEdit->setSizePolicy(policy);
  // controlLayout->addRow(textEdit);
  editSplitter->addWidget(textEdit);

  transitionTableEdit = new QTextEdit();
  transitionTableEdit->setLineWrapMode(QTextEdit::NoWrap);
  transitionTableEdit->setSizePolicy(policy);
  // controlLayout->addRow(transitionTableEdit);
  editSplitter->addWidget(transitionTableEdit);

  editSplitter->setSizePolicy(policy);

  controlLayout->addRow(editSplitter);

  // simulateButton->setFixedWidth(150);

  // controlLayout->addRow(new QSpacerItem(5,5));
  controlLayout->addRow(new QLabel(tr(" ")));


  longRunPayoffEdit = new QLineEdit();
  longRunPayoffEdit->setReadOnly(true);
  longRunPayoffEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
  longRunPayoffEdit->setFixedWidth(200);
  controlLayout->addRow(new QLabel(tr("Long run payoffs:")),
			longRunPayoffEdit);

  timeEdit = new QLineEdit();
  timeEdit->setReadOnly(true);
  timeEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
  timeEdit->setFixedWidth(200);
  controlLayout->addRow(new QLabel(tr("Elapsed time (s):")),
			timeEdit);
  
  distrPlot = new SGSimulationPlot(soln.game.getNumStates());
  distrPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  QScrollArea * scrollArea = new QScrollArea();
  QWidget * scrollWidget = new QWidget();
  QHBoxLayout * scrollLayout = new QHBoxLayout();
  scrollLayout->addWidget(distrPlot);
  scrollWidget->setLayout(scrollLayout);
  scrollArea->setWidget(scrollWidget);

  scrollArea->setWidgetResizable(true);
  scrollArea->setSizePolicy(QSizePolicy::Expanding,
  				    QSizePolicy::Preferred);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  QSplitter * ewSplitter = new QSplitter();
  QWidget * controlWidget = new QWidget();
  controlWidget->setLayout(controlLayout);
  ewSplitter->addWidget(controlWidget);
  ewSplitter->addWidget(scrollArea);
  
  mainLayout->addWidget(ewSplitter);

  connect(closeAction,SIGNAL(triggered()),
	  this,SLOT(close()));
  connect(simulateButton,SIGNAL(clicked()),
	  this,SLOT(simulate()));

  // Set up the simulator
  // Find the point that is closest for the given state.
  double minDistance = numeric_limits<double>::max();

  initialTuple = startOfLastRev->numExtremeTuples;

  for (list<SGIteration>::const_iterator iter = startOfLastRev;
       iter != soln.iterations.end();
       ++iter)
    {
      double newDistance = ( (iter->pivot[state] - point)
			     *(iter->pivot[state] - point) );
      if (newDistance < minDistance-1e-7)
	{
	  minDistance = newDistance;
	  
	  initialTuple = iter->numExtremeTuples;
	} // if
    } // for
   
  sim.initialize();
  sim.setLogFlag(true);

  // Set up the plots
  stateBars = new QCPBars(distrPlot->xAxis,
				    distrPlot->yAxis);
  distrPlot->addPlottable(stateBars);
  distrPlot->plotLayout()->insertRow(0);
  distrPlot->plotLayout()
    ->addElement(0,0,new QCPPlotTitle(distrPlot,
				      QString("State distribution")));
  
  // Prep x axis with state labels
  QVector<double> ticks;
  QVector<QString> labels;
  
  int numStateTicks = min(soln.game.getNumStates(),20);

  for (int state = 0; state < soln.game.getNumStates(); state+=soln.game.getNumStates()/numStateTicks)
    {
      ticks << state+1;
      labels << QString("S")+QString::number(state);
    } // for state
  distrPlot->xAxis->setAutoTicks(false);
  distrPlot->xAxis->setAutoTickLabels(false);
  distrPlot->xAxis->setTickVector(ticks);
  distrPlot->xAxis->setTickVectorLabels(labels);
  distrPlot->xAxis->setTickLabelRotation(60);
  distrPlot->xAxis->setSubTickCount(0);
  distrPlot->xAxis->setTickLength(0,4);
  distrPlot->xAxis->setRange(0,soln.game.getNumStates()+1);

  // Tuple distribution
  distrPlot->plotLayout()
    ->addElement(2,0,new QCPPlotTitle(distrPlot,
				      QString("Tuple distribution")));
  tupleDistrRect = new QCPAxisRect(distrPlot);
  distrPlot->plotLayout()->addElement(3,0,tupleDistrRect);
  distrPlot->addGraph();

  // Prep x axis with tuple labels
  QVector<double> tupleTicks;
  QVector<QString> tupleLabels;
  int numTuplesInLastRev = soln.iterations.size()-sim.getStartOfLastRev();
  int numTupleTicks = min(numTuplesInLastRev,20);
  for (int tuple = 0; tuple < numTuplesInLastRev; 
       tuple+= numTuplesInLastRev/numTupleTicks)
    {
      if (!(tuple%(numTuplesInLastRev/10)))
	{
	  tupleTicks << sim.getStartOfLastRev()+tuple;
	  tupleLabels << QString("T")+QString::number(sim.getStartOfLastRev()+tuple);
	}      
    } // for tuple
  tupleBars = new QCPBars(tupleDistrRect->axis(QCPAxis::atBottom),
			  tupleDistrRect->axis(QCPAxis::atLeft));
  distrPlot->addPlottable(tupleBars);

  tupleDistrRect->axis(QCPAxis::atBottom)->setAutoTicks(false);
  tupleDistrRect->axis(QCPAxis::atBottom)->setAutoTickLabels(false);
  tupleDistrRect->axis(QCPAxis::atBottom)->setTickVector(tupleTicks);
  tupleDistrRect->axis(QCPAxis::atBottom)->setTickVectorLabels(tupleLabels);
  tupleDistrRect->axis(QCPAxis::atBottom)->setTickLabelRotation(60);
  tupleDistrRect->axis(QCPAxis::atBottom)->setSubTickCount(0);
  tupleDistrRect->axis(QCPAxis::atBottom)->setTickLength(0,4);
  tupleDistrRect->axis(QCPAxis::atBottom)->setRange(sim.getStartOfLastRev(),
						    soln.iterations.back().iteration);

  // Action distributions
  actionBars = vector<QCPBars *>(soln.game.getNumStates());
  actionDistrRects = vector<QCPAxisRect *>(soln.game.getNumStates());
  for (int state = 0; state < soln.game.getNumStates(); state++)
    {
      distrPlot->plotLayout()->addElement(2*(state+2),0,
					  new QCPPlotTitle(distrPlot,
							   QString("Action distribution in S")
							   +QString::number(state)));

      actionDistrRects[state] = new QCPAxisRect(distrPlot);
      distrPlot->plotLayout()->addElement(2*(state+2)+1,0,actionDistrRects[state]);
      distrPlot->addGraph();

      QVector<double> actionTicks; 
      QVector<QString> actionLabels;

      for (int action = 0; action < soln.game.getNumActions_total()[state];
	   action++)
	{
	  actionTicks << action+1;
	  actionLabels << QString("A")+QString::number(action);
	}
      actionBars[state] = new QCPBars(actionDistrRects[state]->axis(QCPAxis::atBottom),
				      actionDistrRects[state]->axis(QCPAxis::atLeft));

      distrPlot->addPlottable(actionBars[state]);

      actionDistrRects[state]->axis(QCPAxis::atBottom)->setAutoTicks(false);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setAutoTickLabels(false);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setTickVector(actionTicks);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setTickVectorLabels(actionLabels);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setTickLabelRotation(60);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setSubTickCount(0);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setTickLength(0,4);
      actionDistrRects[state]->axis(QCPAxis::atBottom)->setRange(0,soln.game.getNumActions_total()[state]+1);
    } // for action


} // constructor

void SGSimulationHandler::simulate()
{
  QTime time;

  time.start();

  sim.simulate(iterationEdit->text().toInt(),
	       state,initialTuple);
  
  SGPoint payoffs = sim.getLongRunPayoffs();
  longRunPayoffEdit->setText(QString("(")
			     +QString::number(payoffs[0])
			     +QString(",")
			     +QString::number(payoffs[1])
			     +QString(")"));

  // Prep x axis with state labels
  QVector<double> stateDistr;
  QVector<double> ticks;
  double stateMax = 0.0;
  for (int state = 0; state < soln.game.getNumStates(); state++)
    {
      ticks << state+1;

      double tempProb = (1.0*sim.getStateDistr()[state])/(1.0*sim.getNumIter());
      stateDistr << tempProb;
      stateMax = max(stateMax,tempProb);
    } // for state
  distrPlot->yAxis->setRange(-0.05,stateMax+0.05);
  stateBars->setData(ticks,stateDistr);

  // Tuple distribution
  QVector<double> tupleDistr;
  QVector<double> tupleX;
  double tupleMax = 0.0;
  for (int tuple = 0; tuple < sim.getTupleDistr().size(); tuple++)
    {
      double tempProb = (1.0*sim.getTupleDistr()[tuple])/(1.0*sim.getNumIter());
      tupleDistr << tempProb;
      tupleX << sim.getStartOfLastRev()+tuple;
      tupleMax = max(tupleMax,tempProb);
    } // for tuple
  tupleDistrRect->axis(QCPAxis::atLeft)->setRange(0,tupleMax+0.05);

  tupleBars->setData(tupleX,tupleDistr);

  // Action distributions
  for (int state = 0; state < soln.game.getNumStates(); state++)
    {
      QVector<double> actionDistr;
      QVector<double> actionTicks; 
      double actionMax = 0.0;

      for (int action = 0; action < soln.game.getNumActions_total()[state];
	   action++)
	{
	  actionTicks << action+1;
	  double tempProb = (1.0*sim.getActionDistr()[state][action])/
	    (1.0*sim.getStateDistr()[state]);
	  actionDistr << tempProb;
	  actionMax = max(actionMax,tempProb);
	}
      actionDistrRects[state]->axis(QCPAxis::atLeft)->setRange(-0.05,actionMax+0.05);

      actionBars[state]->setData(actionTicks,actionDistr);
    } // for action

  textEdit->setText(QString::fromStdString(sim.getStringStream().str()));
  transitionTableEdit->setText(QString::fromStdString(sim.getTransitionTableStringStream().str()));
  timeEdit->setText(QString::number(time.elapsed()/1000.0));

  distrPlot->replot();
  
}
