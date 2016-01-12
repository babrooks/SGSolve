#include "sgsettingshandler.hpp"

SGSettingsHandler::SGSettingsHandler(QWidget * parent,
				     SGEnv * _env)
  : QWidget(parent), env(_env)
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

  mainLayout->addLayout(editLayout);
  mainLayout->addWidget(defaultButton);
  mainLayout->addWidget(closeButton);

  setLayout(mainLayout);
  
} // constructor

