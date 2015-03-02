#ifndef SGPAYOFFTABLEMODEL_H
#define SGPAYOFFTABLEMODEL_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgtablemodel.hpp"

// One SGPayoffTableModel for each state. Control which one is
// displayed by changing the model that is plugged into the table
// view.
class SGPayoffTableModel : public SGTableModel
{
  Q_OBJECT

public:
  SGPayoffTableModel(SGGame * _game,
		     int _state):
    SGTableModel(_game,_state)
  {}
  
  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
  QVariant headerData(int section,
		      Qt::Orientation orientation,
		      int role) const Q_DECL_OVERRIDE;

  
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  
}; // SGPayoffTableModel

#endif
