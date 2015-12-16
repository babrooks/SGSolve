#ifndef SGSOLUTIONSTATECOMBOMODEL_HPP
#define SGSOLUTIONSTATECOMBOMODEL_HPP

#include "sgsolutionplotcontroller.hpp"
#include <QAbstractListModel>
#include <QComboBox>

class SGSolutionStateComboModel : public QAbstractListModel
{
  Q_OBJECT;
private:
  SGSolutionPlotController * controller;

public:
  //! Constructor
  SGSolutionStateComboModel(SGSolutionPlotController * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
	    this,SLOT(changeLayout()));
  }

  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution())
      return controller->getCurrentIter()->actions.size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      return QString("S")+QString::number(index.row()-1);
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  void stateChanged(int index)
  { controller->setState(index-1); }

  void changeLayout()
  { emit layoutChanged(); }

  void solutionChanged()
  { 
    emit layoutChanged(); 
  }

}; // SGSolutionStateComboModel

#endif
