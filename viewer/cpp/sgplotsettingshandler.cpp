// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2019 Benjamin A. Brooks
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

#include "sgplotsettingshandler.hpp"

SGPlotSettingsHandler::SGPlotSettingsHandler(QWidget * parent,
					     SGPlotSettings * _plotSettings)
  : QDialog(parent), plotSettings(_plotSettings)
{
  setWindowFlags(Qt::Window);
  setWindowTitle(tr("SGViewer: Plot Settings"));
  setWindowModality(Qt::WindowModal);
  
  setMinimumSize(parent->size()/2);

  QPushButton * closeButton = new QPushButton(QString("Close"));
  QPushButton * defaultButton = new QPushButton(QString("Restore defaults"));

  connect(closeButton,SIGNAL(clicked()),
  	  this,SLOT(closeWindow()));
  connect(defaultButton,SIGNAL(clicked()),
  	  this,SLOT(restoreDefaults()));

  QVBoxLayout * mainLayout = new QVBoxLayout();
  
  QFormLayout * editLayout = new QFormLayout ();
  // Construct and add the check boxes
  editLayout->addRow(QString("Pivot:"),
  		     new SGPlotFeatureBox(this,
					   plotSettings,
					   SGPlotSettings::Pivot));
  editLayout->addRow(QString("Stage payoffs:"),
  		     new SGPlotFeatureBox(this,
					   plotSettings,
					   SGPlotSettings::StagePayoffs));
  editLayout->addRow(QString("Expected pivot:"),
  		     new SGPlotFeatureBox(this,
					   plotSettings,
					   SGPlotSettings::ExpPivot));
  editLayout->addRow(QString("Expected continuation values:"),
  		     new SGPlotFeatureBox(this,
					  plotSettings,
					  SGPlotSettings::ExpPayoffSet));
  editLayout->addRow(QString("Previous expected continuation values:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::PrevExpPayoffSet));
  editLayout->addRow(QString("IC boundary:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::ICBoundary));
  editLayout->addRow(QString("Binding payoffs:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::BindingPayoffs));
  editLayout->addRow(QString("Current direction:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::Direction));
  editLayout->addRow(QString("Generation lines:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::GenLines));
  editLayout->addRow(QString("Grid lines:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::GridLines));
  editLayout->addRow(QString("Zero lines:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::ZeroLines));
  editLayout->addRow(QString("Uniform ranges for state plots:"),
  		     new SGPlotFeatureBox(this,
					plotSettings,
					SGPlotSettings::UniformRanges));

  mainLayout->addLayout(editLayout);
  QHBoxLayout * buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(defaultButton);
  buttonLayout->addWidget(closeButton);
  defaultButton->setDefault(false);
  closeButton->setDefault(true);
  mainLayout->addLayout(buttonLayout);
    
  setLayout(mainLayout);
  
} // constructor

