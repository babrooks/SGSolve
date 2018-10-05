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

#include "sgsolutionhandler_v2.hpp"

SGSolutionHandler_V2::SGSolutionHandler_V2(QWidget * _parent): 
    parent(_parent),
    plotSettings()
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
    QComboBox * solutionModeCombo = new QComboBox();
    solutionModeCombo->addItem("Progress");
    solutionModeCombo->addItem("Final");

    solutionModeCombo->setSizePolicy(QSizePolicy::Maximum,
                                     QSizePolicy::Preferred);
    leftControlLayout->addRow(new QLabel(tr("Display mode:")),
                              solutionModeCombo);
    controlLayout->addLayout(leftControlLayout);

    // Sliders at the bottom
    QScrollBar * iterSlider = new QScrollBar();
    iterSlider->setOrientation(Qt::Horizontal);
    iterSlider->setToolTip(tr("The current iteration."));
    QScrollBar * stepSlider = new QScrollBar();
    stepSlider->setOrientation(Qt::Horizontal);
    stepSlider->setToolTip(tr("The current direction."));

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
    QComboBox * stateCombo = new QComboBox();
    stateCombo->setToolTip(tr("Current state"));
    QComboBox * actionCombo = new QComboBox();
    actionCombo->setToolTip(tr("Current action pair"));

    controller = new SGPlotController_V2(stateCombo,actionCombo,
                                         iterSlider,stepSlider,
                                         solutionModeCombo);
    connect(controller,SIGNAL(actionChanged()),
            this,SLOT(replotSlot()));
    connect(controller,SIGNAL(iterationChanged()),
            this,SLOT(replotSlot()));

    SGStateComboModel_V2 * stateComboModel
            = new SGStateComboModel_V2(controller);
    SGActionComboModel_V2 * actionComboModel
            = new SGActionComboModel_V2(controller);

    stateCombo->setModel(stateComboModel);
    actionCombo->setModel(actionComboModel);
    QPushButton * nextActionButton = new QPushButton("->");
    nextActionButton->setToolTip(tr("Next action"));
    QPushButton * prevActionButton = new QPushButton("<-");
    prevActionButton->setToolTip(tr("Previous action"));

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
            stateComboModel,SLOT(changeState(int)));
    connect(nextActionButton,SIGNAL(clicked()),
            controller,SLOT(nextAction()));
    connect(prevActionButton,SIGNAL(clicked()),
            controller,SLOT(prevAction()));
    connect(this,SIGNAL(nextActionSignal()),
            controller,SLOT(nextAction()));
    connect(this,SIGNAL(prevActionSignal()),
            controller,SLOT(prevAction()));

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

    // Add sliders to bottom panel
    QWidget * botSolutionPanel = new QWidget();
    QFormLayout * botLayout = new QFormLayout(botSolutionPanel);
    botLayout->addRow(new QLabel(tr("Iteration:")),
                      iterSlider);
    botLayout->addRow(new QLabel(tr("Direction:")),
                      stepSlider);
    botSolutionPanel->setFixedHeight(60);
    botLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    layout = new QVBoxLayout();
    layout->addLayout(controlLayout);
    layout->addWidget(topSolutionPanel);
    layout->addWidget(botSolutionPanel);
} // constructor

void SGSolutionHandler_V2::setSolution(const SGSolution_V2 & newSoln)
{
    soln = newSoln;
    connect(detailPlot,SIGNAL(inspectPoint(SGPoint,int,bool)),
            this,SLOT(inspectPoint(SGPoint,int,bool)) );
    // connect(detailPlot,SIGNAL(simulateEquilibrium(SGPoint,int,bool)),
    // 	  this,SLOT(simulateEquilibrium(SGPoint,int,bool)) );
    // Set up state plots
    QLayoutItem * item;
    while ((item = statePlotsLayout->takeAt(0))!=0)
        delete item->widget();

    statePlots.clear();
    statePlots = vector<SGCustomPlot *>(soln.getGame().getNumStates());

    SGPoint UB, LB;
    soln.getGame().getPayoffBounds(UB,LB);
    payoffBound = std::max(UB[0]-LB[0],
            UB[1]-LB[1]);

    for (int state = 0;
         state < soln.getGame().getNumStates();
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

    controller->setSolution(&soln);

    solnLoaded = true;
} // setSolution

void SGSolutionHandler_V2::plotSolution()
{
    int numStates = soln.getGame().getNumStates();

    detailPlot->clearPlottables();
    detailPlot->clearItems();
    QCPAbstractItem * item;
    while ((item = detailPlot->item())!=0)
        delete item;

    int state = controller->getState();
    int action = controller->getAction();
    int actionIndex = controller->getActionIndex();
    list<SGIteration_V2>::const_iterator currentIter = controller->getCurrentIter();
    const SGStep & currentStep = controller->getCurrentStep();
    const list<SGStep> & steps = currentIter->getSteps();

    if (state >= 0 && action >= 0)
    {
        detailPlot->setState(state);

        plotSolution(detailPlot,state,false);

        const SGBaseAction & actionObject = currentIter->getActions()[state][actionIndex];

        // Add expected set
        QVector<double> expSetX(steps.size()),
                expSetY(expSetX.size()),
                expSetT(expSetX.size());

        int tupleC = 0;

        for (auto step = steps.cbegin();
             step != steps.cend();
             ++step)
        {
            SGPoint expPoint
                    = step->getPivot().expectation(soln.getGame().getProbabilities()
                                                   [state][action]);
            expSetX[tupleC] = expPoint[0];
            expSetY[tupleC] = expPoint[1];
            expSetT[tupleC] = tupleC;

            tupleC++;
        }

        QCPCurve * expCurve = new QCPCurve(detailPlot->xAxis,
                                           detailPlot->yAxis);
        expCurve->setData(expSetT,expSetX,expSetY);
        expCurve->setPen(plotSettings.expPen);
        detailPlot->addPlottable(expCurve);
        expCurve->setName(tr("Expected continuation values"));

        // Add previous iteration expected set
        if (currentIter != soln.getIterations().cbegin())
        {
            tupleC = 0;

            auto prevIter = currentIter;
	    prevIter--;
            QVector<double> prevExpSetX(prevIter->getSteps().size()),
                    prevExpSetY(prevExpSetX.size()),
                    prevExpSetT(prevExpSetX.size());

            for (auto step = prevIter->getSteps().cbegin();
                 step != prevIter->getSteps().cend();
                 ++step)
            {
                SGPoint expPoint
                        = step->getPivot().expectation(soln.getGame().getProbabilities()
                                                       [state][action]);
                prevExpSetX[tupleC] = expPoint[0];
                prevExpSetY[tupleC] = expPoint[1];
                prevExpSetT[tupleC] = tupleC;

                tupleC++;
            }

            QCPCurve * prevExpCurve = new QCPCurve(detailPlot->xAxis,
                                               detailPlot->yAxis);
            prevExpCurve->setData(prevExpSetT,prevExpSetX,prevExpSetY);
            prevExpCurve->setPen(plotSettings.prevExpPen);
            detailPlot->addPlottable(prevExpCurve);
            prevExpCurve->setName(tr("Expected continuation values"));
        }
        QCPRange xrange = getBounds(expSetX),
                yrange = getBounds(expSetY);

        SGPoint stagePayoffs = soln.getGame().getPayoffs()[state][action];
        double delta = soln.getGame().getDelta();
        // if (controller->getPlotMode() == SGPlotController::Directions)
        // 	{
        // Non-binding direction
        SGPoint expPivot = currentStep.getPivot().expectation(soln.getGame().getProbabilities()
                                                              [state][action]);

        SGPoint nonBindingPayoff = (1-delta)*stagePayoffs + delta*expPivot;

        QCPItemLine * nonBindingGenLine
                = sgToQCPItemLine(detailPlot,stagePayoffs,
                                  expPivot-stagePayoffs);
        nonBindingGenLine->setPen(plotSettings.genLinePen);
        detailPlot->addItem(nonBindingGenLine);

        addPoint(nonBindingPayoff,detailPlot,plotSettings.payoffStyle);

        QCPItemLine * nonBindingDirection
                = sgToQCPItemLine(detailPlot,currentStep.getPivot()[state],
                                  nonBindingPayoff-currentStep.getPivot()[state]);
        nonBindingDirection->setHead(QCPLineEnding::esSpikeArrow);
        detailPlot->addItem(nonBindingDirection);

        // Binding directions
        for (vector<SGTuple>::const_iterator tuple
             = actionObject.getBindingContinuations().cbegin();
             tuple != actionObject.getBindingContinuations().cend();
             ++tuple)
        {
            if (tuple->size()==0)
                continue;
            for (int pointIndex = 0; pointIndex < tuple->size(); pointIndex++)
            {
                SGPoint continuationValue = (*tuple)[pointIndex];
                SGPoint bindingPayoff = (1-delta)*stagePayoffs
                        + delta*continuationValue;
                QCPItemLine * bindingGenCurve
                        = sgToQCPItemLine(detailPlot,stagePayoffs,
                                          continuationValue - stagePayoffs);
                bindingGenCurve->setPen(plotSettings.genLinePen);
                detailPlot->addItem(bindingGenCurve);

                QCPItemLine * bindingDirection
                        = sgToQCPItemLine(detailPlot,currentStep.getPivot()[state],
                                          bindingPayoff - currentStep.getPivot()[state]);
                bindingDirection->setHead(QCPLineEnding::esSpikeArrow);
                detailPlot->addItem(bindingDirection);

                addPoint(bindingPayoff,detailPlot,plotSettings.payoffStyle);
                addPoint(continuationValue,detailPlot,
                         plotSettings.bindingPayoffStyle);
            } // for
        } // for tuple

        // Add expected pivot
        addPoint(expPivot,detailPlot,plotSettings.expPivotStyle);
        // }
        //   else
        // 	{
        // 	  // Plot the generation only of the current pivot
        // 	  SGPoint continuationValue = (pivotIter.getPivot()[state]-(1-delta)*stagePayoffs)/delta;
        // 	  QCPItemLine * genLine
        // 	    = sgToQCPItemLine(detailPlot,stagePayoffs,continuationValue-stagePayoffs);
        // 	  detailPlot->addItem(genLine);

        // 	  // Add continuation value
        // 	  addPoint(continuationValue,detailPlot,plotSettings.payoffStyle);

        // }


        // Add action
        addPoint(stagePayoffs,detailPlot,plotSettings.stageStyle);

        xrange.expand(QCPRange(stagePayoffs[0],stagePayoffs[0]));
        yrange.expand(QCPRange(stagePayoffs[1],stagePayoffs[1]));
        xrange.expand(detailPlot->getNominalXRange());
        yrange.expand(detailPlot->getNominalYRange());

        // Add IC region
        const SGPoint & minIC = currentIter->getActions()[state][actionIndex].getMinICPayoffs();

        QCPCurve * ICCurveH = vectorToQCPCurve(detailPlot,minIC,
                                               SGPoint(0,yrange.upper-minIC[1]));
        QCPCurve * ICCurveV = vectorToQCPCurve(detailPlot,minIC,
                                               SGPoint(xrange.upper-minIC[0],0));
        ICCurveH->setPen(plotSettings.ICPen);
        ICCurveV->setPen(plotSettings.ICPen);
        detailPlot->addPlottable(ICCurveH);
        detailPlot->addPlottable(ICCurveV);

        detailPlot->setRanges(xrange,yrange);

        detailPlot->getTitle()->setText(generatePlotTitle(state,action,true));
    }
    // else if (state < 0)
    //   {
    //     plotSolution(detailPlot,pivotIter.getBestState(),false);
    //   }
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

void SGSolutionHandler_V2::plotSolution(SGCustomPlot * plot, int state,
                                        bool addSquares)
{
  // int start = controller->getStartSliderPosition();
  // if (start == -1)
  //   start = 0;
  // else
  //   start = controller->getStartIter().getNumExtremeTuples()-1;
  // int end = controller->getEndIter().getNumExtremeTuples()-1;

  list<SGIteration_V2>::const_iterator currentIter = controller->getCurrentIter();
  const list<SGStep> & steps = currentIter->getSteps();
  const SGStep & currentStep = controller->getCurrentStep();

  QVector<double> x(steps.size());
  QVector<double> y(x.size());
  QVector<double> t(x.size());

  // assert(end>=start);

  int stepC = 0;

  for (auto step = steps.cbegin();
       step != steps.cend();
       ++step)
    {
      x[stepC] = step->getPivot()[state][0];
      y[stepC] = step->getPivot()[state][1];
      t[stepC] = stepC;

      stepC++;
    }
  // const SGIteration & endIter = controller->getEndIter();
  // t[tupleC-start] = tupleC;
  // x[tupleC-start] = endIter.getPivot()[state][0];
  // y[tupleC-start] = endIter.getPivot()[state][1];

  QCPRange xrange = getBounds(x),
    yrange = getBounds(y);

  QCPCurve * newCurve = new QCPCurve(plot->xAxis,
				     plot->yAxis);
  newCurve->setData(t,x,y);
  plot->addPlottable(newCurve);

  // Add current pivot and current direction
  double norm = currentStep.getHyperplane().getNormal().norm();
  if (norm==0)
    norm=1;
  QCPCurve * directionCurve = vectorToQCPCurve(plot,
					       currentStep.getPivot()[state],
					       currentStep.getHyperplane().getNormal()*1.5*payoffBound/norm);

  directionCurve->setPen(plotSettings.directionPen);
  plot->addPlottable(directionCurve);

  addPoint(currentStep.getPivot()[state],plot,plotSettings.pivotStyle);

  if (addSquares)
    {
      plot->addGraph();
      plot->graph(1)->setData(x,y);
      plot->graph(1)->setLineStyle(QCPGraph::lsNone);
      plot->graph(1)->setScatterStyle(plotSettings.tupleStyle);
    }

  plot->setRanges(xrange,yrange);

  plot->getTitle()->setText(generatePlotTitle(state,
					      currentIter->getActions()[state][currentStep.getActionTuple()[state]].getAction(),
					      false));
} // plotSolution

QString SGSolutionHandler_V2::generatePlotTitle(int state, int action,
                                                bool addIterStep)
{
  list<SGIteration_V2>::const_iterator currentIter = controller->getCurrentIter();
  const SGStep & currentStep = controller->getCurrentStep();
  // Update the title
  QString titleString = QString("");
  if (addIterStep)
    {
        titleString += QString("Iteration: ");
        titleString += QString::number(controller->getCurrentIterIndex());
        titleString += QString(", ");
    }
    if (controller->getCurrentIterIndex() >=0
            && detailedTitlesAction->isChecked())
    {
        titleString += QString("S");
        titleString += QString::number(state);

        vector<int> actions;
        if (action>-1)
        {
            indexToVector(action,actions,
                          soln.getGame().getNumActions()[state]);
            titleString += QString(", (R");
            titleString += QString::number(actions[0]);
            titleString += QString(",C");
            titleString += QString::number(actions[1]);
            titleString += QString(")");
        }
        else
            titleString += QString(", Null action");

        switch (currentStep.getRegimeTuple()[state])
        {
        case SG::NonBinding:
            titleString += "\n(Non-binding)";
            break;

        case SG::Binding:
            titleString += "\n(Binding)";
            break;

        case SG::Binding0:
            titleString += "\n(Binding 0)";
            break;

        case SG::Binding1:
            titleString += "\n(Binding 1)";
            break;

        case SG::Binding01:
            titleString += "\n(Binding 0 and 1)";
            break;
        }
    }

    return titleString;
} // generatePlotTitle

void SGSolutionHandler_V2::addPoint(const SGPoint & point,QCustomPlot* plot,
                                    const QCPScatterStyle & style)
{
    QVector<double> X(1), Y(1);
    X[0] = point[0];
    Y[0] = point[1];

    plot->addGraph();
    plot->graph()->setData(X,Y);
    plot->graph()->setScatterStyle(style);
} // addPoint

QCPCurve * SGSolutionHandler_V2::vectorToQCPCurve(SGCustomPlot * plot,
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

QCPItemLine * SGSolutionHandler_V2::sgToQCPItemLine(SGCustomPlot * plot,
                                                    const SGPoint & point,
                                                    const SGPoint & dir)
{
    QCPItemLine * line = new QCPItemLine(plot);
    line->start->setCoords(point[0],point[1]);
    line->end->setCoords(point[0]+dir[0],point[1]+dir[1]);
    
    return line;
} // sgToQCPItemLine

QCPRange SGSolutionHandler_V2::getBounds(const QVector<double> & x) const
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

void SGSolutionHandler_V2::inspectPoint(SGPoint point,
                                        int state, bool isDetailPlot)
{
    // Find the point that is closest for the given state.
    controller->setPlotMode(SGPlotController_V2::Generation);
    controller->setCurrentDirection(point,state);

} // inspectPoint

void SGSolutionHandler_V2::simulateEquilibrium(SGPoint point,
                                               int state, bool isDetailPlot)
{
    // simHandler = new SGSimulationHandler(parent,soln,
    // 				       point,state);

    // simHandler->adjustSize();

    // simHandler->move(parent->pos()
    // 		   +parent->rect().center()
    // 		   -simHandler->pos()
    // 		   -simHandler->rect().center());

    // simHandler->show();
} // simulateEquilibrium

