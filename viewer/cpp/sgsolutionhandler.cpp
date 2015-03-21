#include "sgsolutionhandler.hpp"

SGSolutionHandler::SGSolutionHandler()
{
  detailPlot = new SGCustomPlot();
  detailPlot->setSizePolicy(QSizePolicy::Expanding,
			  QSizePolicy::Expanding);
  detailPlot->setInteraction(QCP::iRangeZoom,true);
  detailPlot->setInteraction(QCP::iRangeDrag,true);
  statePlots.push_back(new SGCustomPlot());

  detailPlot->addGraph();
  statePlots[0]->addGraph();

  statePlotsLayout = new QGridLayout();
  statePlotsLayout->addWidget(statePlots[0]);

  detailedTitlesAction = new QAction(tr("&Detailed plot titles"),this);
  detailedTitlesAction->setCheckable(true);
  
  iterSlider = new QScrollBar();
  iterSlider->setOrientation(Qt::Horizontal);
  startSlider = new QScrollBar();
  startSlider->setOrientation(Qt::Horizontal);
  actionSlider = new QScrollBar();
  actionSlider->setOrientation(Qt::Horizontal);
  stateSlider = new QScrollBar();
  stateSlider->setOrientation(Qt::Horizontal);

  QWidget * botSolutionPanel = new QWidget();

  // QPalette botPalette = botSolutionPanel->palette();
  // botPalette.setColor(QPalette::Background,Qt::red);
  // botSolutionPanel->setPalette(botPalette);
  // botSolutionPanel->setAutoFillBackground(true);

  QScrollArea * topRightScrollArea = new QScrollArea();
  
  QWidget * statePlotsWidget = new QWidget();
  statePlotsWidget->setLayout(statePlotsLayout);
  topRightScrollArea->setWidget(statePlotsWidget);

  topRightScrollArea->setWidgetResizable(true);
  topRightScrollArea->setSizePolicy(QSizePolicy::Expanding,
  				    QSizePolicy::Preferred);
  topRightScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  QHBoxLayout * topLayout = new QHBoxLayout();
  topLayout->addWidget(detailPlot);
  topLayout->addWidget(topRightScrollArea);

  QWidget * topSolutionPanel = new QWidget();
  topSolutionPanel->setLayout(topLayout);

  // Add sliders to bottom panel
  QFormLayout * botLayout = new QFormLayout(botSolutionPanel);
  botLayout->addRow(new QLabel(tr("End iteration:")),
		    iterSlider);
  botLayout->addRow(new QLabel(tr("Start iteration:")),
		    startSlider);
  botLayout->addRow(new QLabel(tr("Action:")),
		    actionSlider);
  botLayout->addRow(new QLabel(tr("State:")),
		    stateSlider);
  botSolutionPanel->setFixedHeight(100);
  botLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  
  layout = new QVBoxLayout();
  layout->addWidget(topSolutionPanel);
  layout->addWidget(botSolutionPanel);

  // Connect signals and slots
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(actionSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(actionUpdate(int)));
  connect(stateSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(actionUpdate(int)));
  
}

void SGSolutionHandler::setSolution(const SGSolution & newSoln)
{
  soln = newSoln;

  // Initialize iter pointer
  startIter = soln.iterations.begin();
  endIter = soln.iterations.end();
  endIter--;
      
  // Setup sliders
  int numStates = soln.game.getNumStates();

  // Temporaily disconnect so we don't trigger plotSolution.
  disconnect(iterSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));
  disconnect(startSlider,SIGNAL(valueChanged(int)),
	     this,SLOT(iterSliderUpdate(int)));
  iterSlider->setRange(soln.iterations.front().iteration,
		       soln.iterations.back().iteration);
  iterSlider->setValue(soln.iterations.size()-1);
  startSlider->setRange(soln.iterations.front().iteration,
			soln.iterations.back().iteration);
  startSlider->setValue(soln.iterations.front().iteration);
  connect(iterSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));
  connect(startSlider,SIGNAL(valueChanged(int)),
	  this,SLOT(iterSliderUpdate(int)));

  // Set up state plots
  QLayoutItem * item;
  while (item = statePlotsLayout->takeAt(0))
    delete item;

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
      statePlots[state] =  new SGCustomPlot();
      statePlots[state]->setInteraction(QCP::iRangeZoom,true);
      statePlots[state]->setInteraction(QCP::iRangeDrag,true);

      statePlotsLayout->addWidget(statePlots[state],state/2,state%2);
    }
  
  plotSolution(0,soln.extremeTuples.size()-1);

  solnLoaded = true;
} // setSolution

void SGSolutionHandler::plotSolution(int start, int end)
{
  int numStates = soln.game.getNumStates();

  detailPlot->clearPlottables();

  int bestState = endIter->bestState;

  int bestAction = endIter->actionTuple[bestState];

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
				     [bestState][bestAction]);
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

      double delta = soln.game.getDelta();
      QCPCurve * genCurve = new QCPCurve(detailPlot->xAxis,
					 detailPlot->yAxis);
      QVector<double> genLineX(2), genLineY(2), genLineT(2);
      genLineX[0] = soln.game.getPayoffs()[bestState][bestAction][0];
      genLineY[0] = soln.game.getPayoffs()[bestState][bestAction][1];
      genLineT[0] = 0;
      genLineX[1] = (endIter->pivot[bestState][0]-(1-delta)*genLineX[0])/delta;
      genLineY[1] = (endIter->pivot[bestState][1]-(1-delta)*genLineY[0])/delta;
      genLineT[1] = 1;

      xrange.expand(getBounds(genLineX));
      yrange.expand(getBounds(genLineY));

      genCurve->setData(genLineT,genLineX,genLineY);
      genCurve->setPen(QPen(Qt::DashLine));
      detailPlot->addPlottable(genCurve);

      // Add IC region
      QCPCurve * ICCurve = new QCPCurve(detailPlot->xAxis,
					detailPlot->yAxis);
      QVector<double> ICCurveX(3), ICCurveY(3), ICCurveT(3);

      SGPoint minIC;
      for (int player = 0; player < 2; player++)
	minIC[player] = SGAction::calculateMinIC(bestAction,bestState,player,
						 soln.game,endIter->threatTuple);

      ICCurveX[0] = xrange.upper;
      ICCurveY[0] = minIC[1];
      ICCurveT[0] = 0;
      ICCurveX[1] = minIC[0];
      ICCurveY[1] = minIC[1];
      ICCurveT[1] = 1;
      ICCurveX[2] = minIC[0];
      ICCurveY[2] = yrange.upper;
      ICCurveT[2] = 2;
  
      ICCurve->setData(ICCurveT,ICCurveX,ICCurveY);
      ICCurve->setPen(QPen(Qt::magenta));
      detailPlot->addPlottable(ICCurve);

      plotSolution(start,end,
		   detailPlot,bestState,false);

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
  
  
      xrange.expand(detailPlot->xAxis->range());
      yrange.expand(detailPlot->yAxis->range());

      detailPlot->setRanges(xrange,yrange);
    }
  else
    plotSolution(start,end,
		 detailPlot,0,false);
  
  // Update the title
  QString titleString = QString("Iteration: ");
  titleString += QString::number(endIter->iteration);
  if (endIter->iteration >=0
      && detailedTitlesAction->isChecked())
    {
      titleString += QString(", S");
      titleString += QString::number(bestState);

      vector<int> actions;
      indexToVector(bestAction,actions,
		    soln.game.getNumActions()[bestState]);
      titleString += QString(", (R");
      titleString += QString::number(actions[0]);
      titleString += QString(",C");
      titleString += QString::number(actions[1]);
      titleString += QString(")");

      if (endIter->nonBinding)
	titleString += "\n(Non-binding direction)";
      else
	titleString += "\n(Binding direction)";
    }
  detailPlot->getTitle()->setText(titleString);

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
      plotSolution(start,end,
		   statePlots[state],state,true);
      statePlots[state]->replot();
    }
  
} // plotSolution

void SGSolutionHandler::plotSolution(int start, int end,
				     SGCustomPlot * plot, int state,
				     bool addSquares)
{
  // if (start == 0)
  //   start = 4;

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
  // t.back() = t.size()-1;
  // x.back() = endIter->pivot[state][0];
  // y.back() = endIter->pivot[state][1];
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

  double norm = endIter->direction.norm();

  directionX[0] = endIter->pivot[state][0];
  directionY[0] = endIter->pivot[state][1];
  directionT[0] = 0;
  directionX[1] = endIter->pivot[state][0]
    +endIter->direction[0]*2*payoffBound/norm;
  directionY[1] = endIter->pivot[state][1]
    +endIter->direction[1]*2*payoffBound/norm;
  directionT[1] = 1;
  QCPCurve * directionCurve = new QCPCurve(plot->xAxis,
					   plot->yAxis);
  directionCurve->setData(directionT,directionX,directionY);
  QPen pen (Qt::darkGreen);
  pen.setStyle(Qt::DashLine);
  directionCurve->setPen(pen);
  plot->addPlottable(directionCurve);

  QVector<double> pivotPointX(1), pivotPointY(1);
  pivotPointX[0] = endIter->pivot[state][0];
  pivotPointY[0] = endIter->pivot[state][1];

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
      if (endIter->nonBindingStates[state])
	{
	  vector<int> actions;
	  indexToVector(endIter->actionTuple[state],actions,
			soln.game.getNumActions()[state]);
	  titleString += QString(", (R");
	  titleString += QString::number(actions[0]);
	  titleString += QString(",C");
	  titleString += QString::number(actions[1]);
	  titleString += QString(")");

	  titleString += "\n(Non-binding)";
	}
      else
	titleString += "\n(Binding)";
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
  int startTuple, endTuple;
  
  end = std::max(-1,std::max(start,end));
  iterSlider->setRange(std::max(start,-1),
		       soln.iterations.size()-1);

  while (endIter->iteration < end
	 && endIter!=soln.iterations.end())
    ++endIter;
  while (endIter->iteration > end
	 && endIter!=soln.iterations.begin())
    --endIter;
  if (endIter == soln.iterations.end())
    --endIter;
  
  if (start==-1)
    {
      startTuple = 0;
      startIter = soln.iterations.begin();
    }
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

      startTuple = startIter->numExtremeTuples-1;
    }

  endTuple = endIter->numExtremeTuples-1;

  plotSolution(startTuple,endTuple);
  
} // iterSliderUpdate

void SGSolutionHandler::actionUpdate(int value)
{

} // actionUpdate

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
