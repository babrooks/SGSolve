#ifndef SGSETTINGSHANDLER_HPP
#define SGSETTINGSHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include "sgenv.hpp"

//! Class for changing double parameters.
/*! A customized version of QLineEdit for modifying double parameters
    in an SGEnv object that is used by the SGSettingsHandler. It has a
    private members which are the particular SG::DBL_PARAM that is
    associated with this edit and a pointer to an associated SGEnv
    object. There are also mutator methods for setting the parameter
    value and resetting to the default value of the given parameter.

    \ingroup viewer
*/
class SGDblParamEdit : public QLineEdit
{
  Q_OBJECT;
private:
  //! The double parameter associated with this edit.
  SG::DBL_PARAM param;
  //! The associated SGEnv object.
  SGEnv * env;
public:
  //! Constructor
  SGDblParamEdit(QWidget * parent,
		 SGEnv * _env,SG::DBL_PARAM _param): 
    env(_env),
    param(_param), 
    QLineEdit()
  {
    setText(QString::number(env->getParam(param)));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeParam(const QString &)));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
  } // constructor

private slots:
  //! Slot called when the QLineEdit is edited.
  void changeParam(const QString & text)
  {
    double newValue = text.toDouble();
    env->setParam(param,newValue);
  }
  //! Slot called when resetting to default values.
  void resetParam()
  { setText(QString::number(env->getParam(param))); }

}; // SGDblParamEdit

//! Class for changing integer parameters.
/*! A customized version of QLineEdit for modifying int parameters
    in an SGEnv object that is used by the SGSettingsHandler. It has a
    private members which are the particular SG::INT_PARAM that is
    associated with this edit and a pointer to an associated SGEnv
    object. There are also mutator methods for setting the parameter
    value and resetting to the default value of the given parameter.

    \ingroup viewer
*/
class SGIntParamEdit : public QLineEdit
{
  Q_OBJECT;
private:
  //! The int parameter associated with this edit.
  SG::INT_PARAM param;
  //! The associated SGEnv object.
  SGEnv * env;
public:
  //! Constructor
  SGIntParamEdit(QWidget * parent,
		 SGEnv * _env,SG::INT_PARAM _param): 
    env(_env),
    param(_param), 
    QLineEdit()
  {
    setText(QString::number(env->getParam(param)));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeParam(const QString &)));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
  } // constructor

private slots:
  //! Slot called when the QLineEdit is edited.
  void changeParam(const QString & text)
  {
    double newValue = text.toInt();
    env->setParam(param,newValue);
  } 
  //! Slot called when resetting to default values.
  void resetParam()
  { setText(QString::number(env->getParam(param))); }

}; // SGIntParamEdit

//! Class for changing boolean parameters.
/*! A customized version of QCheckBox for modifying boolean parameters
    in an SGEnv object that is used by the SGSettingsHandler. It has a
    private members which are the particular SG::BOOL_PARAM that is
    associated with this edit and a pointer to an associated SGEnv
    object. There are also mutator methods for setting the parameter
    value and resetting to the default value of the given parameter.

    \ingroup viewer
*/
class SGBoolParamBox : public QCheckBox
{
  Q_OBJECT;
private:
  //! The boolean parameter associated with the box.
  SG::BOOL_PARAM param;
  //! The associated environment.
  SGEnv * env;
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
  SGBoolParamBox(QWidget * parent,
		 SGEnv * _env,
		 SG::BOOL_PARAM _param): 
    env(_env),
    param(_param), 
    QCheckBox()
  {
    setCheck(env->getParam(param));
    
    connect(this,SIGNAL(clicked(bool)),
	    this,SLOT(changeParam()));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
    
  } // constructor

private slots:
  //! Slot called when the check is modified.
  void changeParam()
  {
    env->setParam(param,isChecked());
    setCheck(env->getParam(param));
  }
  //! Slot called when resetting to default values.
  void resetParam()
  { setCheck(env->getParam(param)); }
}; // SGBoolParamBox

//! A widget for setting parameters of the algorithm.
/*! This widget is created from the Tools->Settings menu in the main
    SGViewer window, and it allows for the editing of algorithm
    parameters in the SGEnv object that is used for the
    computation. It uses edits that are derived from the QLineEdit and
    QCheckBox, one for each parameter, that directly synchronize the
    entered values with those in the associated SGEnv.

  \ingroup viewer
 */
class SGSettingsHandler : public QDialog
{
  Q_OBJECT;
  
private:
  //! Pointer to the associated SGEnv object.
  SGEnv * env;

public:
  //! Constructor
  SGSettingsHandler(QWidget * parent,
		    SGEnv * _env);
  //! Destructor
  virtual ~SGSettingsHandler() {}

signals:
  //! Signals to SGSolutionHandler to delete this widget.
  void closeSettingsHandler();
  //! Signals all of the edits and check boxes to reset to default values.
  void restoreDefaultSignal();
private slots:
  //! Closes the window by sending signal to SGSolutionHandler.
  void closeWindow() {emit closeSettingsHandler();}
  //! Restores default parameter values in the SGEnv object and
  //! signals edits/boxes to reset.
  void restoreDefaults() 
  { 
    env->restoreDefaults();
    env->setParam(SG::PRINTTOCOUT,false);
    emit restoreDefaultSignal();
  } // restoreDefaults
  
};

#endif
