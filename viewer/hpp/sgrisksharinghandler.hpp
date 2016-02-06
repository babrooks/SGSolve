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

#ifndef SGRISKSHARINGHANDLER_HPP
#define SGRISKSHARINGHANDLER_HPP

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QValidator>

//! A widget for editing integer attributes
/*!
  \ingroup viewer
 */
class SGIntAttrEdit : public QLineEdit
{
  Q_OBJECT;
private:
  //! Reference to the attribute
  int & attr;

public:
  //! Constructor
  SGIntAttrEdit(int & _attr)
    : QLineEdit(), attr(_attr)
  {
    setText(QString::number(attr));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeAttr(const QString &)));
  } // constructor

private slots:
  //! Slot called when QLineEdit is edited
  void changeAttr(const QString & text)
  {
    attr = text.toInt();
  }
}; // SGIntAttrEdit

//! A widget for editing double attributes
/*!
  \ingroup viewer
 */
class SGDblAttrEdit : public QLineEdit
{
  Q_OBJECT;
private:
  //! Reference to the attribute
  double & attr;

public:
  //! Constructor
  SGDblAttrEdit(double & _attr)
    : QLineEdit(), attr(_attr)
  {
    setText(QString::number(attr));
    connect(this,SIGNAL(textChanged(const QString &)),
	    this,SLOT(changeAttr(const QString &)));
  } // constructor

private slots:
  //! Slot called when QLineEdit is edited
  void changeAttr(const QString & text)
  {
    attr = text.toDouble();
  }
}; // SGDblAttrEdit

//! A widget for setting parameters of the risk sharing game.
/*! 
  \ingroup viewer
 */
class SGRiskSharingHandler : public QDialog
{
  Q_OBJECT;
  
private:
  int & numEndowments;
  int & c2e;
  double & persistence;

public:
  //! Constructor
  SGRiskSharingHandler(QWidget * parent,
		int & _numEndowments,
		int & _c2e,
		double & _persistence):
    QDialog(parent),
    numEndowments(_numEndowments),
    c2e(_c2e),
    persistence(_persistence)
  {
    setWindowTitle(tr("SGViewer: Risk sharing game parameters"));
    setWindowModality(Qt::WindowModal);
  
    setMinimumSize(parent->size()/2);

    QPushButton * okButton = new QPushButton(QString("OK"));
    QPushButton * cancelButton = new QPushButton(QString("Cancel"));
    connect(okButton,SIGNAL(clicked()),
	    this,SLOT(accept()));
    connect(cancelButton,SIGNAL(clicked()),
	    this,SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout();
  
    QFormLayout * editLayout = new QFormLayout ();
    // Construct and add the double parameter edits
    SGIntAttrEdit * endowmentEdit = new SGIntAttrEdit(numEndowments);
    QIntValidator * endowmentValidator = new QIntValidator();
    endowmentValidator->setBottom(1);
    
    SGIntAttrEdit * c2eEdit = new SGIntAttrEdit(c2e);
    QIntValidator * c2eValidator = new QIntValidator();
    c2eValidator->setBottom(1);
    c2eEdit->setValidator(c2eValidator);
    
    SGDblAttrEdit * persistenceEdit = new SGDblAttrEdit(persistence);
    QDoubleValidator * persistenceValidator = new QDoubleValidator();
    persistenceValidator->setBottom(0);
    persistenceEdit->setValidator(persistenceValidator);

    editLayout->addRow(QString("Number of endowments:"),
		       endowmentEdit);
    editLayout->addRow(QString("Number of consumption levels per endowment:"),
		       c2eEdit);
    editLayout->addRow(QString("Persistence:"),
		       persistenceEdit);

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
  virtual ~SGRiskSharingHandler() {}
  
};

#endif
