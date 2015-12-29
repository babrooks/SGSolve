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
		     new SGDblParamEdit(this,env,SGEnv::ERRORTOL));
  editLayout->addRow(QString("Direction tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::DIRECTIONTOL));
  editLayout->addRow(QString("Past threat tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::UPDATEPIVOTTOL));
  editLayout->addRow(QString("Update pivot tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::UPDATEPIVOTTOL));
  editLayout->addRow(QString("IC tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::ICTOL));
  editLayout->addRow(QString("Norm tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::NORMTOL));
  editLayout->addRow(QString("Improvement tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::IMPROVETOL));
  editLayout->addRow(QString("Flat tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::FLATTOL));
  editLayout->addRow(QString("Level tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::LEVELTOL));
  editLayout->addRow(QString("Intersection tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::INTERSECTTOL));
  editLayout->addRow(QString("Rounding:"),
		     new SGDblParamEdit(this,env,SGEnv::ROUNDTOL));
  editLayout->addRow(QString("Back bending tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::BACKBENDINGTOL));
  editLayout->addRow(QString("Movement tolerance:"),
		     new SGDblParamEdit(this,env,SGEnv::ROUNDTOL));

  // Construct and add integer parameter edits.
  editLayout->addRow(QString("Max iterations:"),
		     new SGIntParamEdit(this,env,SGEnv::MAXITERATIONS));
  editLayout->addRow(QString("Max update pivot passes:"),
		     new SGIntParamEdit(this,env,SGEnv::MAXUPDATEPIVOTPASSES));
  editLayout->addRow(QString("Tuple reserve size:"),
		     new SGIntParamEdit(this,env,SGEnv::TUPLERESERVESIZE));
  editLayout->addRow(QString("Store iterations:"),
		     new SGIntParamEdit(this,env,SGEnv::STOREITERATIONS));

  // Construct and add boolean parameter edits.
  editLayout->addRow(QString("Merge tuples:"),
		     new SGBoolParamBox(this,env,SGEnv::MERGETUPLES));

  mainLayout->addLayout(editLayout);
  mainLayout->addWidget(defaultButton);
  mainLayout->addWidget(closeButton);

  setLayout(mainLayout);
  
} // constructor

