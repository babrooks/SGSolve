#ifndef SGGAMEHANDLER_H
#define SGGAMEHANDLER_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"

// Class that contains features common to SGPayoffTableModel and
// SGProbabilityTableModel.
class SGTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  SGTableModel(SGGame * _game,
	       int _state):
    game(_game), state(_state)
  { }
  
  Qt::ItemFlags flags(const QModelIndex & index) const 
  { return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable; }

  void emitLayoutChanged()
  { emit layoutChanged(); }
  bool setState(int newState)
  {
    if (newState < 0 || newState > game->getNumStates())
      return false;
    state = newState;
    return true;
  }
  
protected:
  int state;
  bool isConnceted;
  SGGame * game;
  
};

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
  
  int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][0]; }
  int columnCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][1]; }

  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
  QVariant headerData(int section,
		      Qt::Orientation orientation,
		      int role) const Q_DECL_OVERRIDE;

  
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  
}; // SGPayoffTableModel

// One SGProbabilityTableModel for each state. Control which one is
// displayed by changing the model that is plugged into the table
// view.
class SGProbabilityTableModel : public SGTableModel
{
  Q_OBJECT

public:
  SGProbabilityTableModel(SGGame * _game,
		     int _state):
    SGTableModel(_game,_state)
  { }
  
  int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return (game->getNumActions()[state][0]
	    * game->getNumActions()[state][1]); }
  int columnCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return game->getNumStates(); }

  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
    
  QVariant headerData(int section,
		      Qt::Orientation orientation,
		      int role) const Q_DECL_OVERRIDE;

  bool setData(const QModelIndex & index, const QVariant & value, int role);


}; // SGProbabilityTableModel

class SGGameHandler : public QObject
{
  Q_OBJECT;
  
protected:
  SGGame game;
  vector<SGPayoffTableModel*> payoffModels;
  vector<SGProbabilityTableModel*> probabilityModels;

  // Edits
  QLineEdit * deltaEdit;
  vector<QLineEdit *> numActionsEdits;
  QLineEdit * numStatesEdit;

  QLineEdit * errorTolEdit;

  // Push buttons
  vector<QPushButton *> addActionButtons;
  vector<QPushButton *> removeActionButtons;
  QPushButton * addStateButton;
  QPushButton * removeStateButton;
  QPushButton * nextStateButton;
  QPushButton * prevStateButton;
  
  // Combo box
  QComboBox * currentStateCombo;
  
  // Tables
  QTableView * payoffTableView;
  QTableView * probabilityTableView;

  // Check box  
  QCheckBox * feasibleCheckBox;

public:
  SGGameHandler();
  ~SGGameHandler();

  void setGame(const SGGame & _game);
  const SGGame & getGame() const
  { return game; }
  
  void setState(int state);
  
  friend class SGMainWindow;

private:
  void initializeModels();
  // void addState();
  // void addAction(int player);
  void changeNumberOfStates(int newS);

private slots:
  void currentStateChanged(int newS);
  void stateAdded();
  void action1Added();
  void action2Added();
  void actionAdded(int player);

  void stateRemoved();
  void action1Removed();
  void action2Removed();
  void actionRemoved(int player);
  void discountFactorChanged(const QString & text);

  void nextState();
  void prevState();

public slots:
  void setConstrained(int newState);
};


#endif
