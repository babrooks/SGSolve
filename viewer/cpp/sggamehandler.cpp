#include <QtWidgets>
#include "sggamehandler.hpp"

SGGameHandler::SGGameHandler()
{
  deltaEdit = new QLineEdit("0.9");
  deltaEdit->setSizePolicy(QSizePolicy::Preferred,
			   QSizePolicy::Preferred);

  numStatesEdit = new QLineEdit("1");
  numStatesEdit->setSizePolicy(QSizePolicy::Preferred,
			       QSizePolicy::Preferred);
  numStatesEdit->setReadOnly(true);
  addStateButton = new QPushButton("+");
  removeStateButton = new QPushButton(" -");
  nextStateButton = new QPushButton("->");
  prevStateButton = new QPushButton("<-");

  QSize buttonSize(50,addStateButton->height());
  addStateButton->resize(buttonSize);
  removeStateButton->resize(buttonSize);
  nextStateButton->resize(buttonSize);
  prevStateButton->resize(buttonSize);

  addStateButton->setMinimumWidth(buttonSize.width());
  removeStateButton->setMinimumWidth(buttonSize.width());
  nextStateButton->setMinimumWidth(buttonSize.width());
  prevStateButton->setMinimumWidth(buttonSize.width());

  addStateButton->setSizePolicy(QSizePolicy::Fixed,
				QSizePolicy::Preferred);
  removeStateButton->setSizePolicy(QSizePolicy::Fixed,
				   QSizePolicy::Preferred);
  nextStateButton->setSizePolicy(QSizePolicy::Fixed,
				 QSizePolicy::Preferred);
  prevStateButton->setSizePolicy(QSizePolicy::Fixed,
				 QSizePolicy::Preferred);

  numActionsEdits = vector<QLineEdit*>(2);
  addActionButtons = vector<QPushButton*>(2);
  removeActionButtons = vector<QPushButton*>(2);
  for (int player = 0; player < 2; player++)
    {
      numActionsEdits[player] = new QLineEdit("1");
      numActionsEdits[player]->setReadOnly(true);
      numActionsEdits[player]->setSizePolicy(QSizePolicy::Preferred,
					     QSizePolicy::Preferred);

      addActionButtons[player] = new QPushButton("+");
      removeActionButtons[player] = new QPushButton(" -");

      addActionButtons[player]->resize(buttonSize);
      removeActionButtons[player]->resize(buttonSize);

      addActionButtons[player]->setMinimumWidth(buttonSize.width());
      removeActionButtons[player]->setMinimumWidth(buttonSize.width());

      addActionButtons[player]->setSizePolicy(QSizePolicy::Fixed,
					      QSizePolicy::Preferred);
      removeActionButtons[player]->setSizePolicy(QSizePolicy::Fixed,
						 QSizePolicy::Preferred);
    }
  
  errorTolEdit = new QLineEdit("1e-8");
  errorTolEdit->setSizePolicy(QSizePolicy::Preferred,
			      QSizePolicy::Preferred);

  payoffTableView = new SGTableView();
  payoffTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  probabilityTableViews = vector<SGTableView*>(1);
  probabilityTableViews[0] = new SGTableView();

  payoffModel = NULL;

  probabilityTableLayout = new QVBoxLayout();
  qDebug() << "First there are " << probabilityTableLayout->count() << endl;
  probabilityTableLayout->addWidget(probabilityTableViews[0]);

  qDebug() << "Then there are " << probabilityTableLayout->count() << endl;

  currentStateCombo = new QComboBox();
  currentStateCombo->addItem("0");
  currentStateCombo->setSizePolicy(QSizePolicy::Preferred,
				   QSizePolicy::Preferred);

  feasibleCheckBox = new QCheckBox(QString("Only calculate feasible set"));

  // Connect slots
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  connect(deltaEdit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(discountFactorChanged(const QString &)));

  connect(addActionButtons[0],SIGNAL(clicked()),
	  this,SLOT(action1Added()));
  connect(addActionButtons[1],SIGNAL(clicked()),
	  this,SLOT(action2Added()));
  connect(addStateButton,SIGNAL(clicked()),
	  this,SLOT(stateAdded()));

  connect(removeActionButtons[0],SIGNAL(clicked()),
	  this,SLOT(action1Removed()));
  connect(removeActionButtons[1],SIGNAL(clicked()),
	  this,SLOT(action2Removed()));
  connect(removeStateButton,SIGNAL(clicked()),
	  this,SLOT(stateRemoved()));

  connect(nextStateButton,SIGNAL(clicked()),
	  this,SLOT(nextState()));
  connect(prevStateButton,SIGNAL(clicked()),
	  this,SLOT(prevState()));

  connect(feasibleCheckBox,SIGNAL(stateChanged(int)),
	  this,SLOT(setConstrained(int)));

  // qDebug() << "Finished sggamehandler constructor" << endl;

}

SGGameHandler::~SGGameHandler()
{
  if (payoffModel != NULL)
    delete payoffModel;
  for (int state = 0; state < game.getNumStates(); state++)
    delete probabilityModels[state];
}

void SGGameHandler::setGame(const SGGame & _game)
{
  game = _game;

  if (game.getConstrained()[0] && game.getConstrained()[1])
    feasibleCheckBox->setChecked(true);
  else
    feasibleCheckBox->setChecked(false);

  changeNumberOfStates(game.getNumStates());
  deltaEdit->setText(QString::number(game.getDelta()));
  for (int player = 0; player < 2; player++)
    numActionsEdits[player]
      ->setText(QString::number(game.getNumActions()[0][player]));
  
  initializeModels();

  // qDebug() << "Finished setting game" << endl;
}

void SGGameHandler::changeNumberOfStates(int newS)
{
  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));

  numStatesEdit->setText(QString::number(game.getNumStates()));
  
  int state = currentStateCombo->count();
  // Remove items larger than the new number of states
  while (state > newS)
    currentStateCombo->removeItem(--state);
  // Add new states if number of states increased
  while (state < newS)
    currentStateCombo->addItem(QString::number(state++));

  currentStateCombo->setCurrentIndex(0);

  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
} // changeNumberOfStates

void SGGameHandler::initializeModels()
{
  // Create new payoffModel
  delete payoffModel;

  payoffModel = new SGPayoffTableModel(&game,0);
  payoffTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  payoffTableView->setModel(payoffModel);
  payoffTableView->resizeColumnsToContents();

  // Now create new probabilityModels
  for (int state = probabilityModels.size()-1;
       state >= 0;
       state --)
    delete probabilityModels[state];

  // Clear out probabilityTableLayout
  QLayoutItem * item;
  while (item = probabilityTableLayout->takeAt(0))
    delete item->widget();
  
  probabilityModels.clear();
  probabilityTableViews.clear();

  probabilityModels.reserve(game.getNumStates());
  probabilityTableViews.reserve(game.getNumStates());

  // Add new models/views
  for (int state = 0;
       state < game.getNumStates();
       state++)
    pushBackProbabilityTable(state);  
} // initializeModels


void SGGameHandler::pushBackProbabilityTable(int newState)
{
  probabilityTableViews.push_back(new SGTableView());
  probabilityTableViews.back()
    ->verticalScrollBar()->setDisabled(true);
  probabilityTableViews.back()
    ->horizontalScrollBar()->setDisabled(true);

  probabilityTableLayout->addWidget(new QLabel(QString("State ")
					       +QString::number(newState)
					       +QString(":")) );
  probabilityTableLayout->addWidget(probabilityTableViews.back());

  probabilityModels.push_back(new SGProbabilityTableModel(&game,0,newState));

  probabilityTableViews.back()->setModel(probabilityModels.back());

  probabilityTableViews.back()->resizeColumnsToContents();
} // pushBackProbabilityTable

void SGGameHandler::popBackProbabilityTable()
{
  delete probabilityModels.back();
  probabilityModels.pop_back();

  delete probabilityTableLayout->takeAt(probabilityTableLayout->count()-1)->widget();
  delete probabilityTableLayout->takeAt(probabilityTableLayout->count()-1)->widget();
  probabilityTableViews.pop_back();
} // popBackProbabilityTable

void SGGameHandler::setState(int state)
{
  for (int player = 0; player < 2; player ++)
    numActionsEdits[player]
      ->setText(QString::number(game.getNumActions()[state][player]));
  
  payoffModel->setState(state);
  payoffModel->emitLayoutChanged();
  payoffTableView->resizeColumnsToContents();

  for (int nextState = 0; nextState < game.getNumStates(); nextState++)
    {
      probabilityModels[nextState]->setState(state);
      probabilityModels[nextState]->emitLayoutChanged();

      // sizeHint for the SGTableView objects has changed. Call
      // updateGeometry.
      probabilityTableViews[nextState]->resizeColumnsToContents();
      probabilityTableViews[nextState]->updateGeometry();
    }
  
  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));
  currentStateCombo->setCurrentIndex(state);
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
} // setState

void SGGameHandler::currentStateChanged(int newS)
{
  setState(newS);
}

void SGGameHandler::nextState()
{
  int state = currentStateCombo->currentIndex();

  if (state < currentStateCombo->count()-1)
    setState(state+1);
}

void SGGameHandler::prevState()
{
  int state = currentStateCombo->currentIndex();

  if (state > 0)
    setState(state-1);
}

void SGGameHandler::discountFactorChanged(const QString & text)
{
  double newDelta = text.toDouble();
  if (newDelta > 0 && newDelta < 1)
    game.setDiscountFactor(newDelta);
}

void SGGameHandler::action1Added()
{
  actionAdded(0);
}

void SGGameHandler::action2Added()
{
  actionAdded(1);
}

void SGGameHandler::actionAdded(int player)
{
  if (player < 0 || player > 1)
    return;
  
  int state = currentStateCombo->currentIndex();
  int newAction = game.getNumActions()[state][player];
  
  if (payoffTableView->selectionModel()->hasSelection())
    {
      if (player==1)
	newAction = (payoffTableView->selectionModel()
		     ->selectedIndexes().front().column()+1);
      else
	newAction = (payoffTableView->selectionModel()
		     ->selectedIndexes().front().row()+1);
    }
    
  game.addAction(state,player,newAction);
  numActionsEdits[player]
    ->setText(QString::number(game.getNumActions()[state][player]));

  payoffModel->emitLayoutChanged();
  payoffTableView->resizeColumnToContents(newAction);
  // payoffTableView->updateGeometry();
  for (int nextState = 0; nextState < game.getNumStates(); nextState++)
    {
      probabilityModels[nextState]->emitLayoutChanged();
      probabilityTableViews[nextState]->resizeColumnToContents(newAction);
      probabilityTableViews[nextState]->updateGeometry();
    }
}  // actionAdded

void SGGameHandler::stateAdded()
{
  int newState = currentStateCombo->currentIndex()+1;
  game.addState(newState);
  changeNumberOfStates(game.getNumStates());
  numStatesEdit->setText(QString::number(game.getNumStates()));
  pushBackProbabilityTable(newState);
  
  setState(newState);
} // stateAdded

void SGGameHandler::action1Removed()
{
  actionRemoved(0);
}

void SGGameHandler::action2Removed()
{
  actionRemoved(1);
}

void SGGameHandler::actionRemoved(int player)
{
  int state = currentStateCombo->currentIndex();
  if (game.getNumActions()[state][player] == 1)
    return;
  
  int action = game.getNumActions()[state][player]-1;
  if (payoffTableView->selectionModel()->hasSelection())
    {
      if (player == 1)
	action = (payoffTableView->selectionModel()
		  ->selectedIndexes().front().column());
      else
	action = (payoffTableView->selectionModel()
		  ->selectedIndexes().front().row());
    }

  game.removeAction(state,player,action);
  numActionsEdits[player]->setText(QString::number(game.getNumActions()
						   [state][player]));

  payoffModel->emitLayoutChanged();
  for (int statep = 0; statep < game.getNumStates(); statep++)
    {
      probabilityModels[statep]->emitLayoutChanged();
      probabilityTableViews[statep]->updateGeometry();
    }
}

void SGGameHandler::stateRemoved()
{
  if (game.getNumStates() == 1)
    return;

  int state = currentStateCombo->currentIndex();

  popBackProbabilityTable();

  int newState;
  if (state > 0)
    newState = state-1;
  else
    newState = 0;

  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));
  currentStateCombo->setCurrentIndex(min(state,newState));
  currentStateCombo->removeItem(currentStateCombo->count()-1);
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
  numStatesEdit->setText(QString::number(game.getNumStates()));
  
  game.removeState(state);

  setState(newState);
  
} // stateRemoved

void SGGameHandler::setConstrained(int newState)
{
  if (feasibleCheckBox->isChecked())
    game.setConstrained(vector<bool>(2,true));
  else
    game.setConstrained(vector<bool>(2,false));
}
