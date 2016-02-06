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

#include "sgsettingshandler.hpp"

SGSettingsHandler::SGSettingsHandler(QWidget * parent,
				     SGEnv * _env)
  : QDialog(parent), env(_env)
{
  setWindowFlags(Qt::Window);
  setWindowTitle(tr("SGViewer: Settings"));
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
  // Construct and add the double parameter edits
  editLayout->addRow(QString("Error tolerance:"),
		     new SGDblParamEdit(this,env,SG::ERRORTOL));
  editLayout->addRow(QString("Direction tolerance:"),
		     new SGDblParamEdit(this,env,SG::DIRECTIONTOL));
  editLayout->addRow(QString("Past threat tolerance:"),
		     new SGDblParamEdit(this,env,SG::UPDATEPIVOTTOL));
  editLayout->addRow(QString("Update pivot tolerance:"),
		     new SGDblParamEdit(this,env,SG::UPDATEPIVOTTOL));
  editLayout->addRow(QString("IC tolerance:"),
		     new SGDblParamEdit(this,env,SG::ICTOL));
  editLayout->addRow(QString("Norm tolerance:"),
		     new SGDblParamEdit(this,env,SG::NORMTOL));
  editLayout->addRow(QString("Improvement tolerance:"),
		     new SGDblParamEdit(this,env,SG::IMPROVETOL));
  editLayout->addRow(QString("Flat tolerance:"),
		     new SGDblParamEdit(this,env,SG::FLATTOL));
  editLayout->addRow(QString("Level tolerance:"),
		     new SGDblParamEdit(this,env,SG::LEVELTOL));
  editLayout->addRow(QString("Intersection tolerance:"),
		     new SGDblParamEdit(this,env,SG::INTERSECTTOL));
  editLayout->addRow(QString("Rounding:"),
		     new SGDblParamEdit(this,env,SG::ROUNDTOL));
  editLayout->addRow(QString("Back bending tolerance:"),
		     new SGDblParamEdit(this,env,SG::BACKBENDINGTOL));
  editLayout->addRow(QString("Movement tolerance:"),
		     new SGDblParamEdit(this,env,SG::ROUNDTOL));

  // Construct and add integer parameter edits.
  editLayout->addRow(QString("Max iterations:"),
		     new SGIntParamEdit(this,env,SG::MAXITERATIONS));
  editLayout->addRow(QString("Max update pivot passes:"),
		     new SGIntParamEdit(this,env,SG::MAXUPDATEPIVOTPASSES));
  editLayout->addRow(QString("Tuple reserve size:"),
		     new SGIntParamEdit(this,env,SG::TUPLERESERVESIZE));
  editLayout->addRow(QString("Store iterations:"),
		     new SGIntParamEdit(this,env,SG::STOREITERATIONS));

  // Construct and add boolean parameter edits.
  editLayout->addRow(QString("Merge tuples:"),
		     new SGBoolParamBox(this,env,SG::MERGETUPLES));
  editLayout->addRow(QString("Store actions:"),
		     new SGBoolParamBox(this,env,SG::STOREACTIONS));
  editLayout->addRow(QString("Check sufficient conditions for containment:"),
		     new SGBoolParamBox(this,env,SG::CHECKSUFFICIENT));


  mainLayout->addLayout(editLayout);
  QHBoxLayout * buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(defaultButton);
  buttonLayout->addWidget(closeButton);
  defaultButton->setDefault(false);
  closeButton->setDefault(true);
  mainLayout->addLayout(buttonLayout);
    
  setLayout(mainLayout);
  
} // constructor

