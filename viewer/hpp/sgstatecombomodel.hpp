#ifndef SGSTATECOMBOMODEL_HPP
#define SGSTATECOMBOMODEL_HPP

#include "sgplotcontroller.hpp"
#include <QAbstractListModel>
#include <QComboBox>

//! Model for stateCombo
/*! Model that interfaces between the state QComboBox and the
    underlying SGSolution object. This model automatically
    synchronizes the number and labels of the states with the
    SGSolution, and it indicates with an asterisk which state
    generated the best test direction for the current iteration. When
    a new solution is loaded, or when the current iteration is changed
    in the SGPlotController, the changeLayout slot is triggered so
    that the stateCombo is updated to reflect the true number of
    states and the identity of the best state.

    \ingroup viewer
*/
class SGStateComboModel : public QAbstractListModel
{
  Q_OBJECT;
private:
  //! The associated SGPlotController object
  SGPlotController * controller;

public:
  //! Constructor
  SGStateComboModel(SGPlotController * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
    	    this,SLOT(changeLayout()));
  }

  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution())
      return controller->getCurrentIter().getActions().size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      {
	QString dataString = QString("S")+QString::number(index.row()-1);
	if ( (index.row()-1) == controller->getCurrentIter().getBestState())
	  dataString += QString("*");
	return dataString;
      }
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  //! Tells the controller to change the state when the index of the
  //! combo box is updated.
  void changeState(int index)
  { controller->setState(index-1); }

  //! Signals to the GUI to change the layout.
  void changeLayout()
  { emit layoutChanged(); }

}; // SGStateComboModel

#endif
