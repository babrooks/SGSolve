#ifndef SGRANDOMHNALDER_HPP
#define SGRANDOMHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QValidator>
#include "sgrisksharinghandler.hpp"

class SGRandomHandler : public QDialog 
{
  Q_OBJECT;

private: 
  int & numStates;
  int & numActions;

public: 
  //! Constructor
  SGRandomHandler(QWidget * parent, 
	   int & _numStates,
	   int & _numActions):
    QDialog(parent),
    numStates(_numStates), 
    numActions(_numActions)
   {
    setWindowTitle(tr("SGViewer: Random game parameters"));
    setWindowModality(Qt::WindowModal);

    QPushButton * okButton = new QPushButton(QString("OK"));
    QPushButton * cancelButton = new QPushButton(QString("Cancel"));
    connect(okButton,SIGNAL(clicked()),
	    this,SLOT(accept()));
    connect(cancelButton,SIGNAL(clicked()),
	    this,SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout();

    QFormLayout * editLayout = new QFormLayout ();
    // Construct and add the new parameters 
    SGIntAttrEdit * stateEdit = new SGIntAttrEdit(numStates);
    QIntValidator * stateValidator = new QIntValidator();
    stateValidator->setBottom(1);
    stateEdit->setValidator(stateValidator);

    SGIntAttrEdit * actionEdit = new SGIntAttrEdit(numActions);
    QIntValidator * actionValidator = new QIntValidator();
    actionValidator->setBottom(1);
    actionEdit->setValidator(actionValidator);

    editLayout->addRow(QString("Number of states:"),
		       stateEdit);
    editLayout->addRow(QString("Number of Actions:"),
		       actionEdit);

    mainLayout->addLayout(editLayout);
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);
    okButton->setDefault(true);
    cancelButton->setDefault(false);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
  }
  //! Destructor
  virtual ~SGRandomHandler() {}

};
    
#endif




















