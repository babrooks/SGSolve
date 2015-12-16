#ifndef SGSETTINGSHANDLER_HPP
#define SGSETTINGSHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include "sgenv.hpp"

class SGDblParamEdit : public QLineEdit
{
  Q_OBJECT;
private:
  SGEnv::DBL_PARAM param;
  SGEnv * env;
public:
  SGDblParamEdit(QWidget * parent,
		 SGEnv * _env,SGEnv::DBL_PARAM _param): 
    env(_env),
    param(_param), 
    QLineEdit()
  {
    setText(QString::number(env->getParam(param)));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeParam(const QString &)));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
  }

private slots:
  void changeParam(const QString & text)
  {
    double newValue = text.toDouble();
    env->setParam(param,newValue);
  }
  
  void resetParam()
  { setText(QString::number(env->getParam(param))); }

}; // SGDblParamEdit

class SGIntParamEdit : public QLineEdit
{
  Q_OBJECT;
private:
  SGEnv::INT_PARAM param;
  SGEnv * env;
public:
  SGIntParamEdit(QWidget * parent,
		 SGEnv * _env,SGEnv::INT_PARAM _param): 
    env(_env),
    param(_param), 
    QLineEdit()
  {
    setText(QString::number(env->getParam(param)));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeParam(const QString &)));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
  }

private slots:
  void changeParam(const QString & text)
  {
    double newValue = text.toInt();
    env->setParam(param,newValue);
  }

  void resetParam()
  { setText(QString::number(env->getParam(param))); }

}; // SGIntParamEdit

class SGBoolParamBox : public QCheckBox
{
  Q_OBJECT;
private:
  SGEnv::BOOL_PARAM param;
  SGEnv * env;
  
  void setCheck(bool tf)
  {
    if (tf)
      setCheckState(Qt::Checked);
    else
      setCheckState(Qt::Unchecked);
  }

public:
  SGBoolParamBox(QWidget * parent,
		 SGEnv * _env,
		 SGEnv::BOOL_PARAM _param): 
    env(_env),
    param(_param), 
    QCheckBox()
  {
    setCheck(env->getParam(param));
    
    connect(this,SIGNAL(clicked(bool)),
	    this,SLOT(changeParam()));
    connect(parent,SIGNAL(restoreDefaultSignal()),
	    this,SLOT(resetParam()));
    
  }

private slots:
  void changeParam()
  {
    env->setParam(param,isChecked());
    setCheck(env->getParam(param));
  }

  void resetParam()
  { setCheck(env->getParam(param)); }
}; // SGBoolParamBox

class SGSettingsHandler : public QWidget
{
  Q_OBJECT;
  
private:
  SGEnv * env;

  QPushButton * closeButton;
  QPushButton * defaultButton;

  SGDblParamEdit * errorTolEdit;
  SGDblParamEdit * directionTolEdit;
  SGDblParamEdit * pastThreatTolEdit;
  SGDblParamEdit * updatePivotTolEdit;
  SGDblParamEdit * ICTolEdit;
  SGDblParamEdit * normTolEdit;
  SGDblParamEdit * flatTolEdit;
  SGDblParamEdit * levelTolEdit;
  SGDblParamEdit * improveTolEdit;
  SGDblParamEdit * roundTolEdit;
  SGDblParamEdit * backBendingTolEdit;
  SGDblParamEdit * movementTolEdit;

  SGIntParamEdit * maxIterationsEdit;
  SGIntParamEdit * maxUpdatePivotPassesEdit;
  SGIntParamEdit * tupleReserveSizeEdit;
  
  SGBoolParamBox * mergeTuplesButton;
  SGBoolParamBox * storeIterationsButton;

public:
  SGSettingsHandler(QWidget * parent,
		    SGEnv * _env);

  virtual ~SGSettingsHandler() {}

signals: 
  void closeSettingsHandler();
  void restoreDefaultSignal();
private slots:
  void closeWindow() {emit closeSettingsHandler();}
  void restoreDefaults() 
  { 
    env->restoreDefaults();
    env->setParam(SGEnv::PRINTTOCOUT,false);
    emit restoreDefaultSignal();
  }
  
};

#endif
