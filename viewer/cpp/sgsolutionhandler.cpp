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

  controller = new SGPlotController(stateCombo,actionCombo);
  connect(controller,SIGNAL(actionChanged()),
	  this,SLOT(replotSlot()));

  SGStateComboModel * stateComboModel
    = new SGStateComboModel(controller);
  SGActionComboModel * actionComboModel
    = new SGActionComboModel(controller);

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
} // constructor

void SGSolutionHandler::setSolution(const SGSolution & newSoln)
{
  soln = newSoln;
  controller->setSolution(&soln);
  
  // Initialize iter pointer
  startIter = soln.iterations.begin();
  endIter = soln.iterations.end();
  endIter--;
  pivotIter = endIter;
  controller->setIteration(pivotIter->iteration);
  
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
  detailPlot->clearItems();
  // QCPAbstractItem * item;
  // while ((item = detailPlot->item())!=0)
  //   delete item;
  
  int state = controller->getState();
  int action = controller->getAction();
  int actionIndex = controller->getActionIndex();

  if (state >= 0 && action >= 0 && endIter->iteration>=0)
    {
      detailPlot->setState(state);
  
      const SGBaseAction & actionObject = pivotIter->actions[state][actionIndex];

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
      
      QCPCurve * expCurve = new QCPCurve(detailPlot->xAxis,
					 detailPlot->yAxis);
      expCurve->setData(expSetT,expSetX,expSetY);
      expCurve->setPen(QPen(Qt::red));
      detailPlot->addPlottable(expCurve);

      QCPRange xrange = getBounds(expSetX),
	yrange = getBounds(expSetY);

      // Non-binding direction
      double delta = soln.game.getDelta();
      SGPoint expPivot = pivotIter->pivot.expectation(soln.game.getProbabilities()
						      [state][action]);
      SGPoint stagePayoffs = soln.game.getPayoffs()[state][action];
      SGPoint nonBindingPayoff = (1-delta)*stagePayoffs + delta*expPivot;

      QCPItemLine * nonBindingGenLine
	= sgToQCPItemLine(detailPlot,stagePayoffs,
			  expPivot-stagePayoffs);
      nonBindingGenLine->setPen(QPen(Qt::DashLine));
      detailPlot->addItem(nonBindingGenLine);

      QCPItemLine * nonBindingDirection
	= sgToQCPItemLine(detailPlot,pivotIter->pivot[state],
			  nonBindingPayoff-pivotIter->pivot[state]);
      nonBindingDirection->setHead(QCPLineEnding::esSpikeArrow);
      detailPlot->addItem(nonBindingDirection);
      
      // Binding directions
      for (vector<SGTuple>::const_iterator tuple
	     = actionObject.getBindingContinuations().begin();
	   tuple != actionObject.getBindingContinuations().end();
	   ++tuple)
	{
	  for (int pointIndex = 0; pointIndex < tuple->size(); pointIndex++)
	    {
	      SGPoint continuationValue = (*tuple)[pointIndex];
	      SGPoint bindingPayoff = (1-delta)*stagePayoffs
		+ delta*continuationValue;
	      QCPItemLine * bindingGenCurve
		= sgToQCPItemLine(detailPlot,stagePayoffs,
				  continuationValue - stagePayoffs);
	      bindingGenCurve->setPen(QPen(Qt::DashLine));
	      detailPlot->addItem(bindingGenCurve);

	      QCPItemLine * bindingDirection
		= sgToQCPItemLine(detailPlot,pivotIter->pivot[state],
				  bindingPayoff - pivotIter->pivot[state]);
	      bindingDirection->setHead(QCPLineEnding::esSpikeArrow);
	      detailPlot->addItem(bindingDirection);
				   
	    } // for 
	} // for tuple

      plotSolution(detailPlot,state,true);

      // Add action
      QVector<double> actionPointX(1), actionPointY(1);
      actionPointX[0] = stagePayoffs[0];
      actionPointY[0] = stagePayoffs[1];
      
      detailPlot->addGraph();
      detailPlot->graph(1)->setData(actionPointX,actionPointY);
      detailPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,
							    QPen(Qt::black),
							    QBrush(Qt::black),
							    8));
      xrange.expand(detailPlot->getNominalXRange());
      yrange.expand(detailPlot->getNominalYRange());

      // Add IC region
      const SGPoint & minIC = pivotIter->actions[state][actionIndex].getMinICPayoffs();

      QCPCurve * ICCurveH = vectorToQCPCurve(detailPlot,minIC,
					     SGPoint(0,yrange.upper-minIC[1]));
      QCPCurve * ICCurveV = vectorToQCPCurve(detailPlot,minIC,
					     SGPoint(xrange.upper-minIC[0],0));
      ICCurveH->setPen(QPen(Qt::magenta));
      ICCurveV->setPen(QPen(Qt::magenta));
      detailPlot->addPlottable(ICCurveH);
      detailPlot->addPlottable(ICCurveV);

      detailPlot->setRanges(xrange,yrange);

      detailPlot->getTitle()->setText(generatePlotTitle(state,action,true));
    }
  else if (state < 0)
    {
      plotSolution(detailPlot,pivotIter->bestState,false);
    }
  else
    plotSolution(detailPlot,0,false);
  

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
  QCPCurve * directionCurve = vectorToQCPCurve(plot,
					       pivotIter->pivot[state],
					       pivotIter->direction*1.5*payoffBound/pivotIter->direction.norm());
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

  plot->getTitle()->setText(generatePlotTitle(state,
					      pivotIter->actionTuple[state],
					      false));
} // plotSolution

QString SGSolutionHandler::generatePlotTitle(int state, int action,
					     bool addIterRev)
{
  // Update the title
  QString titleString = QString("");
  if (addIterRev)
    {
      titleString += QString("Iteration: ");
      titleString += QString::number(pivotIter->iteration);
      titleString += QString(", Revolution: ");
      titleString += QString::number(pivotIter->revolution);
      titleString += QString(", ");
    }
  if (pivotIter->iteration >=0
      && detailedTitlesAction->isChecked())
    {
      titleString += QString("S");
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
  
  return titleString;
} // generatePlotTitle

QCPCurve * SGSolutionHandler::vectorToQCPCurve(SGCustomPlot * plot,
					       const SGPoint & point,
					       const SGPoint & dir)
{
  double norm = dir.norm();

  QVector<double> X(2), Y(2), T(2);
  X[0] = point[0];
  Y[0] = point[1];
  T[0] = 0;
  X[1] = point[0]+dir[0];
  Y[1] = point[1]+dir[1];
  T[1] = 1;
  QCPCurve * curve = new QCPCurve(plot->xAxis,
				  plot->yAxis);
  curve->setData(T,X,Y);
  
  return curve;
} // vectorToQCPCurve

QCPItemLine * SGSolutionHandler::sgToQCPItemLine(SGCustomPlot * plot,
						 const SGPoint & point,
						 const SGPoint & dir)
{
  QCPItemLine * line = new QCPItemLine(plot);
  line->start->setCoords(point[0],point[1]);
  line->end->setCoords(point[0]+dir[0],point[1]+dir[1]);
    
  return line;
} // sgToQCPItemLine

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
