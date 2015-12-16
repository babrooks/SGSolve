#include "sgsolutionhandler.hpp"

SGSolutionHandler::SGSolutionHandler(QWidget * _parent): 
  mode(Progress), parent(_parent)
{
  // Set up the menu items. Added by SGMainWindow to the menubar.
  detailedTitlesAction = new QAction(tr("&Detailed plot titles"),this);
  detailedTitlesAction->setCheckable(true);
  detailedTitlesAction->setChecked(true);

  equalizeAxesAction = new QAction(tr("&Equalize axes scales"),this);
  equalizeAxesAction->setCheckable(true);
  equalizeAxesAction->setChecked(true);

  // Top control for solution mode.
  QHBoxLayout * controlLayout = new QHBoxLayout();
  QFormLayout * leftControlLayout = new QFormLayout();
  solutionModeCombo = new QComboBox();
  solutionModeCombo->addItem("Progress");
  solutionModeCombo->addItem("Final");
  solutionModeCombo->setSizePolicy(QSizePolicy::Maximum,
				   QSizePolicy::Preferred);
  connect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeMode(int)));
  leftControlLayout->addRow(new QLabel(tr("Display mode:")),
			    solutionModeCombo);
  controlLayout->addLayout(leftControlLayout);

  // Set up the detail plot
  detailPlot = new SGCustomPlot(0,true);
  detailPlot->setSizePolicy(QSizePolicy::Expanding,
			  QSizePolicy::Expanding);
  detailPlot->setInteraction(QCP::iRangeZoom,true);
  detailPlot->setInteraction(QCP::iRangeDrag,true);
  statePlots.push_back(new SGCustomPlot());

  detailPlot->addGraph();
  statePlots[0]->addGraph();

  // Layout the controls for the detail plot
  stateCombo = new QComboBox();
  actionCombo = new QComboBox();

  controller = new SGSolutionPlotController(stateCombo,actionCombo);
  connect(controller,SIGNAL(actionChanged()),
	  this,SLOT(replotSlot()));

  SGSolutionStateComboModel * stateComboModel
    = new SGSolutionStateComboModel(controller);
  SGSolutionActionComboModel * actionComboModel
    = new SGSolutionActionComboModel(controller);

  stateCombo->setModel(stateComboModel);
  actionCombo->setModel(actionComboModel);
  QPushButton * nextActionButton = new QPushButton("->");
  QPushButton * prevActionButton = new QPushButton("<-");
  
  QHBoxLayout * detailPlotControlLayout = new QHBoxLayout();
  QLabel * stateComboLabel = new QLabel(tr("State:"));
  stateComboLabel->setAlignment(Qt::AlignRight);
  detailPlotControlLayout->addWidget(stateComboLabel);
  detailPlotControlLayout->addWidget(stateCombo);
  QLabel * actionComboLabel = new QLabel(tr("Action:"));
  actionComboLabel->setAlignment(Qt::AlignRight);
  detailPlotControlLayout->addWidget(actionComboLabel);
  detailPlotControlLayout->addWidget(actionCombo);
  detailPlotControlLayout->addWidget(prevActionButton);
  detailPlotControlLayout->addWidget(nextActionButton);
  connect(stateCombo,SIGNAL(currentIndexChanged(int)),
	  stateComboModel,SLOT(stateChanged(int)));
  connect(nextActionButton,SIGNAL(clicked()),
	  this,SLOT(nextAction()));
  connect(prevActionButton,SIGNAL(clicked()),
	  this,SLOT(prevAction()));

  // Combine the detail plot and controls into a detail plot widget
  QWidget * detailPlotWidget = new QWidget();
  QVBoxLayout * detailPlotWidgetLayout = new QVBoxLayout();
  detailPlotWidgetLayout->addWidget(detailPlot);
  detailPlotWidgetLayout->addLayout(detailPlotControlLayout);
  detailPlotWidget->setLayout(detailPlotWidgetLayout);

  // Now layout the state plots
  statePlotsLayout = new QGridLayout();
  statePlotsLayout->addWidget(statePlots[0]);

  QScrollArea * topRightScrollArea = new QScrollArea();
  
  QWidget * statePlotsWidget = new QWidget();
  statePlotsWidget->setLayout(statePlotsLayout);
  topRightScrollArea->setWidget(statePlotsWidget);

  topRightScrollArea->setWidgetResizable(true);
  topRightScrollArea->setSizePolicy(QSizePolicy::Expanding,
  				    QSizePolicy::Preferred);
  topRightScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  QHBoxLayout * topLayout = new QHBoxLayout();
  topLayout->addWidget(detailPlotWidget);
  topLayout->addWidget(topRightScrollArea);

  QWidget * topSolutionPanel = new QWidget();
  topSolutionPanel->setLayout(topLayout);

  // Sliders at the bottom
  iterSlider = new QScrollBar();
  iterSlider->setOrientation(Qt::Horizontal);
  startSlider = new QScrollBar();
  startSlider->setOrientation(Qt::Horizontal);
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));

  QWidget * botSolutionPanel = new QWidget();

  // Add sliders to bottom panel
  QFormLayout * botLayout = new QFormLayout(botSolutionPanel);
  botLayout->addRow(new QLabel(tr("End iteration:")),
		    iterSlider);
  botLayout->addRow(new QLabel(tr("Start iteration:")),
		    startSlider);
  botSolutionPanel->setFixedHeight(60);
  botLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  
  layout = new QVBoxLayout();
  layout->addLayout(controlLayout);
  layout->addWidget(topSolutionPanel);
  layout->addWidget(botSolutionPanel);

  
}

void SGSolutionHandler::setSolution(const SGSolution & newSoln)
{
  soln = newSoln;
  controller->setSolution(&soln);
  
  // Initialize iter pointer
  startIter = soln.iterations.begin();
  endIter = soln.iterations.end();
  endIter--;
  pivotIter = endIter;
  
  startOfLastRev = endIter;
  while ((startOfLastRev--)->revolution
	 ==endIter->revolution) {}
  
  // Setup sliders
  int numStates = soln.game.getNumStates();
  
  setSliderRanges(soln.iterations.front().iteration,
		  soln.iterations.back().iteration);

  connect(detailPlot,SIGNAL(inspectPoint(SGPoint,int,bool)),
	  this,SLOT(inspectPoint(SGPoint,int,bool)) );
  connect(detailPlot,SIGNAL(simulateEquilibrium(SGPoint,int,bool)),
	  this,SLOT(simulateEquilibrium(SGPoint,int,bool)) );
  // Set up state plots
  QLayoutItem * item;
  while ((item = statePlotsLayout->takeAt(0))!=0)
    delete item->widget();
  
  statePlots.clear();
  statePlots = vector<SGCustomPlot *>(numStates);
  
  SGPoint UB, LB;
  soln.game.getPayoffBounds(UB,LB);
  payoffBound = std::max(UB[0]-LB[0],
			 UB[1]-LB[1]);

  for (int state = 0;
       state < numStates;
       state ++)
    {
      statePlots[state] =  new SGCustomPlot(state,false);
      statePlots[state]->setInteraction(QCP::iRangeZoom,true);
      statePlots[state]->setInteraction(QCP::iRangeDrag,true);

      connect(statePlots[state],SIGNAL(inspectPoint(SGPoint,int,bool)),
	      this,SLOT(inspectPoint(SGPoint,int,bool)) );
      connect(statePlots[state],SIGNAL(simulateEquilibrium(SGPoint,int,bool)),
	      this,SLOT(simulateEquilibrium(SGPoint,int,bool)) );
      statePlotsLayout->addWidget(statePlots[state],state/2,state%2);
    }

  mode = Progress;
  startSlider->setEnabled(mode==Progress);
  disconnect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(changeMode(int)));
  solutionModeCombo->setCurrentIndex(mode);
  connect(solutionModeCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(changeMode(int)));
  
  
  plotSolution();

  solnLoaded = true;
} // setSolution

void SGSolutionHandler::setSliderRanges(int start, int end)
{
  // Temporaily disconnect so we don't trigger plotSolution.
  disconnect(iterSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));
  disconnect(startSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));

  iterSlider->setRange(start,end);
  iterSlider->setValue(end);
  startSlider->setRange(start,end);
  startSlider->setValue(start);

  startSlider->setEnabled(mode==Progress);

  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
}

void SGSolutionHandler::plotSolution()
{
  int start = startSlider->sliderPosition();
  if (start == -1)
    start = 0;
  else
    start = startIter->numExtremeTuples-1;
  int end = endIter->numExtremeTuples-1;

  int numStates = soln.game.getNumStates();

  detailPlot->clearPlottables();
  
  int state = pivotIter->bestState;
  int action = pivotIter->actionTuple[pivotIter->bestState];
  if (controller->getAction()>-1)
    {
      state = controller->getState();
      action = controller->getAction();
    }

  detailPlot->setState(state);
  
  if (endIter->iteration>=0)
    {
      // Add expected set
      QVector<double> expSetX(end-start),
	expSetY(expSetX.size()),
	expSetT(expSetX.size());

      int tupleC = 0;

      for (list<SGTuple>::const_iterator tuple = soln.extremeTuples.begin();
	   tuple != soln.extremeTuples.end();
	   ++tuple)
	{
	  if (tupleC >= end)
	    break;
	  else if (tupleC >= start)
	    {
	      SGPoint expPoint
		= tuple->expectation(soln.game.getProbabilities()
				     [state][action]);
	      expSetX[tupleC-start] = expPoint[0];
	      expSetY[tupleC-start] = expPoint[1];
	      expSetT[tupleC-start] = tupleC;
	    }
      
	  tupleC++;
	}
      
      QCPCurve * newCurve = new QCPCurve(detailPlot->xAxis,
					 detailPlot->yAxis);
      newCurve->setData(expSetT,expSetX,expSetY);
      newCurve->setPen(QPen(Qt::red));
      detailPlot->addPlottable(newCurve);

      QCPRange xrange = getBounds(expSetX),
	yrange = getBounds(expSetY);

      // Line between stage payoffs and expected continuation value
      double delta = soln.game.getDelta();
      QCPCurve * genCurve = new QCPCurve(detailPlot->xAxis,
					 detailPlot->yAxis);
      QVector<double> genLineX(2), genLineY(2), genLineT(2);
      genLineX[0] = soln.game.getPayoffs()[state][action][0];
      genLineY[0] = soln.game.getPayoffs()[state][action][1];
      genLineT[0] = 0;
      genLineX[1] = (pivotIter->pivot[state][0]-(1-delta)*genLineX[0])/delta;
      genLineY[1] = (pivotIter->pivot[state][1]-(1-delta)*genLineY[0])/delta;
      genLineT[1] = 1;

      genCurve->setData(genLineT,genLineX,genLineY);
      genCurve->setPen(QPen(Qt::DashLine));
      detailPlot->addPlottable(genCurve);

      plotSolution(detailPlot,state,true);

      // Add action
      QVector<double> actionPointX(1), actionPointY(1);
      actionPointX[0] = genLineX[0];
      actionPointY[0] = genLineY[0];

      detailPlot->addGraph();
      detailPlot->graph(1)->setData(actionPointX,actionPointY);
      detailPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,
							    QPen(Qt::black),
							    QBrush(Qt::black),
							    8));
      xrange.expand(getBounds(genLineX));
      yrange.expand(getBounds(genLineY));
      xrange.expand(detailPlot->getNominalXRange());
      yrange.expand(detailPlot->getNominalYRange());
      // xrange.expand(detailPlot->xAxis->range());
      // yrange.expand(detailPlot->yAxis->range());

      // Add IC region
      QCPCurve * ICCurve = new QCPCurve(detailPlot->xAxis,
					detailPlot->yAxis);
      QVector<double> ICCurveX(3), ICCurveY(3), ICCurveT(3);

      SGPoint minIC;
      for (int player = 0; player < 2; player++)
	minIC[player] = SGAction::calculateMinIC(action,state,player,
						 soln.game,pivotIter->threatTuple);

      ICCurveX[0] = xrange.upper+1e2*(xrange.upper-xrange.lower);
      ICCurveY[0] = minIC[1];
      ICCurveT[0] = 0;
      ICCurveX[1] = minIC[0];
      ICCurveY[1] = minIC[1];
      ICCurveT[1] = 1;
      ICCurveX[2] = minIC[0];
      ICCurveY[2] = yrange.upper+1e2*(yrange.upper-yrange.lower);
      ICCurveT[2] = 2;
  
      ICCurve->setData(ICCurveT,ICCurveX,ICCurveY);
      ICCurve->setPen(QPen(Qt::magenta));
      detailPlot->addPlottable(ICCurve);

      detailPlot->setRanges(xrange,yrange);
    }
  else
    plotSolution(detailPlot,0,false);
  
  // Update the title
  QString titleString = QString("Iteration: ");
  titleString += QString::number(pivotIter->iteration);
  titleString += QString(", Revolution: ");
  titleString += QString::number(pivotIter->revolution);
  if (pivotIter->iteration >=0
      && detailedTitlesAction->isChecked())
    {
      titleString += QString(", S");
      titleString += QString::number(state);

      vector<int> actions;
      indexToVector(action,actions,
		    soln.game.getNumActions()[state]);
      titleString += QString(", (R");
      titleString += QString::number(actions[0]);
      titleString += QString(",C");
      titleString += QString::number(actions[1]);
      titleString += QString(")");

      switch (pivotIter->regime)
	{
	case SG::NonBinding:
	  titleString += "\n(Non-binding direction)";
	  break;

	case SG::Binding:
	  titleString += "\n(Binding direction)";
	  break;

	case SG::Binding0:
	  titleString += "\n(Binding 0 direction)";
	  break;

	case SG::Binding1:
	  titleString += "\n(Binding 1 direction)";
	  break;

	case SG::Binding01:
	  titleString += "\n(Binding 0 and 1 direction)";
	  break;
	}

    }
  detailPlot->getTitle()->setText(titleString);

  if (equalizeAxesAction->isChecked())
    detailPlot->equalizeAxesScales();
  detailPlot->replot();

  // Other states
  if (statePlots.size() != numStates)
    qDebug() << "Warning: Number of states ("
	     << numStates << ") does not match size of stateplots (" 
	     << statePlots.size() << ")." << endl;
  for (int state = 0;
       state < statePlots.size();
       state++)
    {
      statePlots[state]->clearPlottables();
      plotSolution(statePlots[state],state,true);

      if (equalizeAxesAction->isChecked())
	statePlots[state]->equalizeAxesScales();

      statePlots[state]->replot();
    }
  
} // plotSolution

void SGSolutionHandler::plotSolution(SGCustomPlot * plot, int state,
				     bool addSquares)
{
  // if (start == 0)
  //   start = 4;
  int start = startSlider->sliderPosition();
  if (start == -1)
    start = 0;
  else
    start = startIter->numExtremeTuples-1;
  int end = endIter->numExtremeTuples-1;

  QVector<double> x(end-start+1);
  QVector<double> y(x.size());
  QVector<double> t(x.size());
  
  assert(end>=start);

  int tupleC = 0;
  
  for (list<SGTuple>::const_iterator tuple = soln.extremeTuples.begin();
       tuple != soln.extremeTuples.end();
       ++tuple)
    {
      if (tupleC >= end)
	break;
      else if (tupleC >= start)
	{
	  x[tupleC-start] = (*tuple)[state][0];
	  y[tupleC-start] = (*tuple)[state][1];
	  t[tupleC-start] = tupleC;
	}
      
      tupleC++;
    }
  t[tupleC-start] = tupleC;
  x[tupleC-start] = endIter->pivot[state][0];
  y[tupleC-start] = endIter->pivot[state][1];
  
  QCPRange xrange = getBounds(x),
    yrange = getBounds(y);
  
  QCPCurve * newCurve = new QCPCurve(plot->xAxis,
				     plot->yAxis);
  newCurve->setData(t,x,y);
  plot->addPlottable(newCurve);

  // Add current pivot and current direction
  QVector <double> directionX(2), directionY(2), directionT(2);

  double norm = pivotIter->direction.norm();

  directionX[0] = pivotIter->pivot[state][0];
  directionY[0] = pivotIter->pivot[state][1];
  directionT[0] = 0;
  directionX[1] = pivotIter->pivot[state][0]
    +pivotIter->direction[0]*2*payoffBound/norm;
  directionY[1] = pivotIter->pivot[state][1]
    +pivotIter->direction[1]*2*payoffBound/norm;
  directionT[1] = 1;
  QCPCurve * directionCurve = new QCPCurve(plot->xAxis,
					   plot->yAxis);
  directionCurve->setData(directionT,directionX,directionY);
  QPen pen (Qt::darkGreen);
  pen.setStyle(Qt::DashLine);
  directionCurve->setPen(pen);
  plot->addPlottable(directionCurve);

  QVector<double> pivotPointX(1), pivotPointY(1);
  pivotPointX[0] = pivotIter->pivot[state][0];
  pivotPointY[0] = pivotIter->pivot[state][1];

  plot->addGraph();
  plot->graph(0)->setData(pivotPointX,pivotPointY);
  plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
						  QPen(Qt::blue),
						  QBrush(Qt::blue),
						  8));
  if (addSquares)
    {
      plot->addGraph();
      plot->graph(1)->setData(x,y);
      plot->graph(1)->setLineStyle(QCPGraph::lsNone);
      plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare,
						      QPen(Qt::blue),
						      QBrush(Qt::blue),
						      2));
    }
  
  plot->setRanges(xrange,yrange);

  QString titleString("S");
  titleString += QString::number(state,10);

  if (detailedTitlesAction->isChecked())
    {
      vector<int> actions;

      indexToVector(pivotIter->actionTuple[state],actions,
		    soln.game.getNumActions()[state]);
      titleString += QString(", (R");
      titleString += QString::number(actions[0]);
      titleString += QString(",C");
      titleString += QString::number(actions[1]);
      titleString += QString(")");

      switch (pivotIter->regimeTuple[state])
	{
	case SG::NonBinding:
	  titleString += "\n(Non-binding)";
	  break;

	case SG::Binding:
	  titleString += "\n(Binding direction)";
	  break;

	case SG::Binding0:
	  titleString += "\n(Binding 0 direction)";
	  break;

	case SG::Binding1:
	  titleString += "\n(Binding 1 direction)";
	  break;

	case SG::Binding01:
	  titleString += "\n(Binding 0 and 1 direction)";
	  break;
	} // switch

    }
  plot->getTitle()->setText(titleString);
} // plotSolution

QCPRange SGSolutionHandler::getBounds(const QVector<double> & x) const
{
  double paddFactor = 0.1;

  double xmin = numeric_limits<double>::max();
  double xmax = -numeric_limits<double>::max();
  
  for (int k = 0; k < x.size(); ++k)
    {
      xmin = std::min(xmin,x[k]);
      xmax = std::max(xmax,x[k]);
    }
  double padd = (xmax-xmin)*paddFactor;
  xmin -= padd; xmax += padd;

  return QCPRange(xmin,xmax);  
}

void SGSolutionHandler::iterSliderUpdate(int value)
{
  if (!solnLoaded)
    return;
      
  int start = startSlider->sliderPosition();
  int end = iterSlider->sliderPosition();
  
  end = std::max(-1,std::max(start,end));
  iterSlider->setMinimum(std::max(start,-1));
  
  while (pivotIter->iteration < end
	 && pivotIter!=soln.iterations.end())
    ++pivotIter;
  while (pivotIter->iteration > end
	 && pivotIter!=soln.iterations.begin())
    --pivotIter;
  if (pivotIter == soln.iterations.end())
    --pivotIter;
  
  if (mode==Progress)
    endIter = pivotIter;
  else
    {
      endIter = soln.iterations.end();
      endIter--;
    }
  
  if (start==-1)
    startIter = soln.iterations.begin();
  else
    {
      while (startIter->iteration < start
	     && startIter!=soln.iterations.end())
	++startIter;
      while (startIter->iteration > start
	     && startIter!=soln.iterations.begin())
	--startIter;
      if (startIter == soln.iterations.end())
	--startIter;
    }

  if (pivotIter == soln.iterations.begin())
    pivotIter++;

  controller->setIteration(pivotIter->iteration);
  
  plotSolution();
  
} // iterSliderUpdate

void SGSolutionHandler::actionUpdate(int value)
{

} // actionUpdate

void SGSolutionHandler::prevAction()
{

} // prevAction

void SGSolutionHandler::nextAction()
{

} // nextAction

void SGSolutionHandler::moveForwards()
{
  if (!solnLoaded)
    return;
      
  iterSlider->setSliderPosition(std::max(iterSlider->minimum(),
					 iterSlider->value()-1));
  iterSliderUpdate(iterSlider->value());
}

void SGSolutionHandler::moveBackwards()
{
  if (!solnLoaded)
    return;
      
  iterSlider->setSliderPosition(std::min(iterSlider->maximum(),
					 iterSlider->value()+1));
  iterSliderUpdate(iterSlider->value());
}


void SGSolutionHandler::changeMode(int newMode)
{
  int plotState = detailPlot->getState();
  if (newMode == 0)
    {
      mode = Progress;

      startIter = soln.iterations.begin();
    }
  else if (newMode == 1)
    {
      mode = Final;
      
      startIter = startOfLastRev;

      if (pivotIter->iteration < startIter->iteration)
	{
	  pivotIter = soln.iterations.end();
	  pivotIter--;
	  plotState = pivotIter->bestState;
	}
    }

  setSliderRanges(startIter->iteration,soln.iterations.back().iteration);
  
  iterSlider->setValue(endIter->iteration);
  iterSliderUpdate(endIter->iteration);
} // changeMode

void SGSolutionHandler::inspectPoint(SGPoint point,
				     int state, bool isDetailPlot)
{
  // Find the point that is closest for the given state.
  double minDistance = numeric_limits<double>::max();

  int tupleC = 0;
  
  pivotIter = startIter;
  for (list<SGTuple>::const_iterator tuple = soln.extremeTuples.begin();
       tuple != soln.extremeTuples.end();
       ++tuple)
    {
      if (tupleC >= endIter->numExtremeTuples-1)
	break;
      else if (tupleC >= startIter->numExtremeTuples-1)
	{
	  double newDistance = ((*tuple)[state] - point)*((*tuple)[state] - point);
	  if (newDistance < minDistance-1e-7)
	    {
	      minDistance = newDistance;
	      while (pivotIter->numExtremeTuples <= tupleC)
	      	pivotIter++;
	    } // if
	} // if
      
      tupleC++;
    } // for

  controller->setState(state);
  controller->setAction(pivotIter->actionTuple[state]);
} // inspectPoint


					       
void SGSolutionHandler::simulateEquilibrium(SGPoint point,
					    int state, bool isDetailPlot)
{
  simHandler = new SGSimulationHandler(parent,soln,
				       point,state,
				       startOfLastRev);

  simHandler->adjustSize();

  simHandler->move(parent->pos()
		   +parent->rect().center()
		   -simHandler->pos()
		   -simHandler->rect().center());
  
  simHandler->show();
} // simulateEquilibrium
