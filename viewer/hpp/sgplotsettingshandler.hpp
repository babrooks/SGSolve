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

#ifndef SGPLOTSETTINGSHANDLER_HPP
#define SGPLOTSETTINGSHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include "sgplotsettings.hpp"

class SGPlotFeatureBox : public QCheckBox
{
  Q_OBJECT;
private:
  //! The boolean parameter associated with the box.
  SGPlotSettings::SGPlotFeature param;
  //! The associated environment.
  SGPlotSettings * plotSettings;
  //! The set method.
  void setCheck(bool tf)
  {
    if (tf)
      setCheckState(Qt::Checked);
    else
      setCheckState(Qt::Unchecked);
  }

public:
  //! Constructor
  SGPlotFeatureBox(QWidget * parent,
		 SGPlotSettings * _plotSettings,
		 SGPlotSettings::SGPlotFeature _param): 
    plotSettings(_plotSettings),
    param(_param), 
    QCheckBox()
  {
    setCheck(plotSettings->get(param));
    
    connect(this,SIGNAL(clicked(bool)),
	    this,SLOT(changeParam()));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
    
  } // constructor

private slots:
  //! Slot called when the check is modified.
  void changeParam()
  {
    plotSettings->set(param,isChecked());
  }
  //! Slot called when resetting to default values.
  void resetParam()
  {
    setCheck(Qt::Checked);
    plotSettings->set(param,true);
  }
}; // SGPlotFeatureBox

//! A widget for setting parameters of the algorithm.
/*! This widget is created from the Tools->Settings menu in the main
    SGViewer window, and it allows for the editing of algorithm
    parameters in the SGEnv object that is used for the
    computation. It uses edits that are derived from the QLineEdit and
    QCheckBox, one for each parameter, that directly synchronize the
    entered values with those in the associated SGEnv.

  \ingroup viewer
 */
class SGPlotSettingsHandler : public QDialog
{
  Q_OBJECT;
  
private:
  //! Pointer to the associated SGEnv object.
  SGPlotSettings * plotSettings;

public:
  //! Constructor
  SGPlotSettingsHandler(QWidget * parent,
		    SGPlotSettings * _plotSettings);
  //! Destructor
  virtual ~SGPlotSettingsHandler() {}

signals:
  //! Signals to SGSolutionHandler to delete this widget.
  void closePlotSettingsHandler();
  //! Signals all of the edits and check boxes to reset to default values.
  void restoreDefaultSignal();
private slots:
  //! Closes the window by sending signal to SGSolutionHandler.
  void closeWindow() {emit closePlotSettingsHandler();}
  //! Restores default parameter values in the SGEnv object and
  //! signals edits/boxes to reset.
  void restoreDefaults() 
  { 
    plotSettings->restoreDefaults();
    //! Signals all of the edits and check boxes to reset to default values.
    emit restoreDefaultSignal();
  } // restoreDefaults
  
};

#endif
