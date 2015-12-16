#include "sgsettingshandler.hpp"

SGSettingsHandler::SGSettingsHandler(QWidget * parent,
				     SGEnv * _env)
  : QWidget(parent), env(_env)
{
  setWindowFlags(Qt::Window);
  setWindowTitle(tr("SGViewer: Settings"));
  setWindowModality(Qt::WindowModal);
  
  setMinimumSize(parent->size()/2);

  closeButton = new QPushButton(QString("Close"));
  defaultButton = new QPushButton(QString("Restore defaults"));

  connect(closeButton,SIGNAL(clicked()),
	  this,SLOT(closeWindow()));
  connect(defaultButton,SIGNAL(clicked()),
	  this,SLOT(restoreDefaults()));

  QVBoxLayout * mainLayout = new QVBoxLayout();

  errorTolEdit = new SGDblParamEdit(this,env,SGEnv::ERRORTOL);
  directionTolEdit = new SGDblParamEdit(this,env,SGEnv::DIRECTIONTOL);
  pastThreatTolEdit = new SGDblParamEdit(this,env,SGEnv::PASTTHREATTOL);
  updatePivotTolEdit = new SGDblParamEdit(this,env,SGEnv::UPDATEPIVOTTOL);
  ICTolEdit = new SGDblParamEdit(this,env,SGEnv::ICTOL);
  normTolEdit = new SGDblParamEdit(this,env,SGEnv::NORMTOL);
  flatTolEdit = new SGDblParamEdit(this,env,SGEnv::FLATTOL);
  improveTolEdit = new SGDblParamEdit(this,env,SGEnv::IMPROVETOL);
  levelTolEdit = new SGDblParamEdit(this,env,SGEnv::LEVELTOL);
  roundTolEdit = new SGDblParamEdit(this,env,SGEnv::ROUNDTOL);
  backBendingTolEdit = new SGDblParamEdit(this,env,SGEnv::BACKBENDINGTOL);
  movementTolEdit = new SGDblParamEdit(this,env,SGEnv::ROUNDTOL);

  maxIterationsEdit = new SGIntParamEdit(this,env,SGEnv::MAXITERATIONS);
  maxUpdatePivotPassesEdit = new SGIntParamEdit(this,env,SGEnv::MAXUPDATEPIVOTPASSES);
  tupleReserveSizeEdit = new SGIntParamEdit(this,env,SGEnv::TUPLERESERVESIZE);
  
  mergeTuplesButton = new SGBoolParamBox(this,env,SGEnv::MERGETUPLES);
  storeIterationsButton = new SGBoolParamBox(this,env,SGEnv::STOREITERATIONS);
  
  QFormLayout * editLayout = new QFormLayout ();
  editLayout->addRow(QString("Error tolerance:"),errorTolEdit);
  editLayout->addRow(QString("Direction tolerance:"),directionTolEdit);
  editLayout->addRow(QString("Past threat tolerance:"),pastThreatTolEdit);
  editLayout->addRow(QString("Update pivot tolerance:"),updatePivotTolEdit);
  editLayout->addRow(QString("IC tolerance:"),ICTolEdit);
  editLayout->addRow(QString("Norm tolerance:"),normTolEdit);
  editLayout->addRow(QString("Improvement tolerance:"),improveTolEdit);
  editLayout->addRow(QString("Level tolerance:"),levelTolEdit);
  editLayout->addRow(QString("Rounding:"),roundTolEdit);
  editLayout->addRow(QString("Back bending tolerance:"),backBendingTolEdit);
  editLayout->addRow(QString("Movement tolerance:"),movementTolEdit);
  
  editLayout->addRow(QString("Max iterations:"),maxIterationsEdit);
  editLayout->addRow(QString("Max update pivot passes:"),maxUpdatePivotPassesEdit);
  editLayout->addRow(QString("Tuple reserve size:"),tupleReserveSizeEdit);

  editLayout->addRow(QString("Merge tuples:"),mergeTuplesButton);
  editLayout->addRow(QString("Store iterations:"),storeIterationsButton);

  mainLayout->addLayout(editLayout);
  mainLayout->addWidget(defaultButton);
  mainLayout->addWidget(closeButton);

  setLayout(mainLayout);
  
} // constructor

