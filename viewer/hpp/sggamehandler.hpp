#ifndef SGGAMEHANDLER_H
#define SGGAMEHANDLER_H

#include <QtWidgets>
// #include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgpayofftablemodel.hpp"
#include "sgprobabilitytablemodel.hpp"
#include "sgtableview.hpp"

class SGGameHandler : public QObject
{
  Q_OBJECT;
  
protected:
  SGGame game;
  SGPayoffTableModel* payoffModel;
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
  vector<SGTableView *> probabilityTableViews;
  QVBoxLayout * probabilityTableLayout;

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
  void pushBackProbabilityTable(int newS);
  void popBackProbabilityTable();
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
