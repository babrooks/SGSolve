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

#include "sglegend.hpp"

SGLegend::SGLegend(QWidget * parent)
{
  QPushButton * okButton = new QPushButton(QString("OK"));
    
  QCustomPlot * legendPlot = new QCustomPlot();
  QVBoxLayout * mainLayout = new QVBoxLayout();

  SGPlotSettings settings;

  setWindowTitle(tr("SGViewer: Legend"));
    
  mainLayout->addWidget(legendPlot);
  mainLayout->addWidget(okButton);
    
  setLayout(mainLayout);

  // Set up legend
  legendPlot->legend->setVisible(true);
  legendPlot->legend->setBorderPen(QPen(Qt::NoPen));

  QVector <double> x(1), y(1);
  
  int currentGraph = 0;

  legendPlot->addGraph();
  legendPlot->graph()->setName("Pivot trajectory/value set");
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::ValuePen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Extreme tuples");
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::ValuePen));
  legendPlot->graph()->setScatterStyle(settings.get(SGPlotSettings::TupleStyle));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Current pivot");
  legendPlot->graph()->setScatterStyle(settings.get(SGPlotSettings::PivotStyle));
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::PivotPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Expected continuation value set");
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::ExpPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Expected pivot");
  legendPlot->graph()->setScatterStyle(settings.get(SGPlotSettings::ExpPivotStyle));
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::ExpPivotPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Stage payoffs");
  legendPlot->graph()->setScatterStyle(settings.get(SGPlotSettings::StageStyle));
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::StagePen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("IC constraint");
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::ICPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Binding continuation value");
  legendPlot->graph()->setScatterStyle(settings.get(SGPlotSettings::BindingPayoffStyle));
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::BindingPayoffPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->addGraph();
  legendPlot->graph()->setName("Current direction");
  legendPlot->graph()->setPen(settings.get(SGPlotSettings::DirectionPen));
  legendPlot->graph()->setData(x,y);

  legendPlot->plotLayout()->insertRow(0);
  legendPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(legendPlot,"Legend"));

  legendPlot->plotLayout()->insertRow(1);
  legendPlot->plotLayout()->addElement(1,0,legendPlot->legend);

  legendPlot->xAxis->setVisible(false);
  legendPlot->yAxis->setVisible(false);

  legendPlot->plotLayout()->elementAt(2)->setVisible(false);
  legendPlot->plotLayout()->elementAt(2)->setMinimumSize(QSize(0,0));

  connect(okButton,SIGNAL(clicked()),
	  this,SLOT(accept()));
} // constructor
