#ifndef SGSOLUTIONACTIONCOMBOMODEL_HPP
#define SGSOLUTIONACTIONCOMBOMODEL_HPP

#include "sgsolutionplotcontroller.hpp"
#include <QAbstractListModel>
#include <QComboBox>

class SGSolutionActionComboModel : public QAbstractListModel
{
  Q_OBJECT;
private:
  SGSolutionPlotController * controller;

public:
  //! Constructor
  SGSolutionActionComboModel(SGSolutionPlotController * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
	    this,SLOT(solutionChanged()));
    connect(controller,SIGNAL(stateChanged()),
	    this,SLOT(changeLayout()));
  }
  
  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution()
	&& controller->getState()>-1)
      return controller->getCurrentIter()->actions[controller->getState()].size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      return QString::number(controller->getCurrentIter()->actions[controller->getState()][index.row()-1].getAction());
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  void actionChanged(int index)
  {
    controller->setAction(index-1);
  }
  void solutionChanged()
  {
    emit layoutChanged();
  }
  void changeLayout()
  { emit layoutChanged(); }

}; // SGSolutionActionComboModel

#endif
