#ifndef SGGAMEHANDLER_H
#define SGGAMEHANDLER_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgpayofftablemodel.hpp"
#include "sgprobabilitytablemodel.hpp"
#include "sgtableview.hpp"

//! This class handles the widgets for editing/displaying the game.
/*! All of the widgets in the game tab and their slots are members of
  this class. It also contains the pointer to the SGGame, and handles
  the interfaces to the game objects.

  The class contains table models for editing the game, which
  interface with the respective data using the SGGame class's
  interface methods. 

  \ingroup viewer
 */
class SGGameHandler : public QObject
{
  Q_OBJECT;
  
private:
  //! The game object.
  /*! This is the game that is represented in the game tab. Note that
      this can be a different game than the one that is associated
      with the solution in the solution tab. */
  SGGame game;

  //! Error tolerance for measuring convergence
  double errorTol;

  //! The model for interfacing with payoffs
  SGPayoffTableModel* payoffModel;
  //! Vector of models for interfacing with transition probabilities
  vector<SGProbabilityTableModel*> probabilityModels;

  //! Layout for the game tab.
  QVBoxLayout * layout;

  //! Button that triggers solve routine.
  QPushButton * solveButton;
  //! Button that cancels solve.
  QPushButton * cancelButton;

  // Edits
  //! For editing the discount factor
  QLineEdit * deltaEdit;
  //! Vector of edits for players' numbers of actions
  vector<QLineEdit *> numActionsEdits;
  //! Edit for number of states.
  QLineEdit * numStatesEdit;
  //! Edit for controlling error tolerance
  QLineEdit * errorTolEdit;

  // Combo box
  //! Drop down menu for selecting a state.
  QComboBox * currentStateCombo;
  
  // Tables
  //! Table for displaying stage payoffs
  QTableView * payoffTableView;
  //! Vector of tables for displaying transition probabilities.
  vector<SGTableView *> probabilityTableViews;
  //! Layout for holding transition probability tables.
  QVBoxLayout * probabilityTableLayout;

  // Check box  
  //! Check box for whether or not to calculate feasible set.
  QCheckBox * feasibleCheckBox;

public:
  //! Constructor
  /*! Constructs edits and buttons, connects signals/slots, calls
      SGGameHandler::initializeModels. */
  SGGameHandler();
  //! Destructor.
  /*! Destroys probability table views, models, etc. */
  ~SGGameHandler();

  //! Replaces the current game with _game.
  /*! Calls initializeModels to reinitialize references for table
      models. */
  void setGame(const SGGame & _game);

  //! Returns constant reference to the current game.
  const SGGame & getGame() const
  { return game; }

  //! Returns the error tolerance
  double getErrorTol() const
  { return errorTol; }

  //! Returns the layout
  QVBoxLayout * getLayout() const
  { return layout; }
  
  //! Returns the solveButton
  QPushButton * getSolveButton() const
  { return solveButton; }
  
  //! Returns the cancelButton
  QPushButton * getCancelButton() const
  { return cancelButton; }
  
  //! Changes the current state
  /*! Switches all of the models over to new state and updates table
      views. */
  void setState(int state);
  
private:
  //! Delete old data models and create new ones.
  /*! Called in constructor and whenever game changes. */
  void initializeModels();
  //! Adds a new probability table model
  void pushBackProbabilityTable(int newS);
  //! Removes last probability table model
  void popBackProbabilityTable();
  //! Adds/removes models to achieve correct number of states.
  void changeNumberOfStates(int newS);
				     
private slots:
  //! Slot for changing the state. Calls setState.
  void currentStateChanged(int newS);
  //! Adds a new state. Calls changeNumberOfStates.
  void stateAdded();
  //! Action added for player 1. Calls actionAdded.
  void action1Added();
  //! Action added for player 2. Calls actionAdded.
  void action2Added();
  //! Adds a new action for the indicated player.
  void actionAdded(int player);

  //! Removes the current state.
  void stateRemoved();
  //! Action removed for player 1. Calls actionRemoved.
  void action1Removed();
  //! Action removed for player 2. Calls actionRemoved.
  void action2Removed();
  //! Removes action for the given player. 
  /*! Will remove the action that is currently selected in the payoff
      table, if one is selected. */
  void actionRemoved(int player);
  //! Changes the discount factor.
  void discountFactorChanged(const QString & text);

  //! Changes the error tolerance.
  void errorTolChanged(const QString & text);

  //! Advances currentState to the next state.
  void nextState();
  //! Decreases currentState to the previous state.
  void prevState();

public slots:
  //! Changes whether or not feasible set will be calculated.
  void setConstrained(int newState);
};


#endif
