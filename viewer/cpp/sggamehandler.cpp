#include <QtWidgets>
#include "sggamehandler.hpp"

QVariant SGPayoffTableModel::data(const QModelIndex & index,
				  int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::SizeHintRole)
    return QSize(1,1);
  else if (role == Qt::DisplayRole)
    {

      int action = (index.column() * game->getNumActions()[state][0]
		    + index.row() );
      
      return QVariant(QString::number(game->getPayoffs()[state][action][0])
		      +QString(", ")
		      +QString::number(game->getPayoffs()[state][action][1]));
    }
  else
    return QVariant();
} // data
    
QVariant SGPayoffTableModel::headerData(int section,
					Qt::Orientation orientation,
					int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::DisplayRole)
    {
      switch (orientation)
	{
	case Qt::Horizontal:
	  return QVariant(QString("C")
			  +QString::number(section));

	case Qt::Vertical:
	  return QVariant(QString("R")
			  +QString::number(section));
	}
    }
  else
    return QVariant();
} // headerData

bool SGPayoffTableModel::setData(const QModelIndex & index,
				 const QVariant & value,
				 int role)
{
  if (role == Qt::EditRole)
    {
      int action = index.row()+index.column()*game->getNumActions()[state][0];

      QRegExp rx("[, ]");
      QStringList list = value.toString().split(rx,QString::SkipEmptyParts);

      for (int player = 0; player < std::min(list.size(),2); player++)
	game->setPayoff(state,action,player,list.at(player).toDouble());

      emit dataChanged(index,index);
      return true;
    }
  return false;
} // setData

QVariant SGProbabilityTableModel::data(const QModelIndex & index,
				  int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::SizeHintRole)
    return QSize(1,1);
  else if (role == Qt::DisplayRole)
    {
      return QVariant(QString::number(game->getProbabilities()
				      [state][index.row()][index.column()]));
    }
  else
    return QVariant();
} // data
    
QVariant SGProbabilityTableModel::headerData(int section,
					Qt::Orientation orientation,
					int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::DisplayRole)
    {
      switch (orientation)
	{
	case Qt::Horizontal:
	  return QVariant(QString("S")
			  +QString::number(section));

	case Qt::Vertical:
	  return QVariant(QString("(R")
			  +QString::number(section
					   %game->getNumActions()[state][0])
			  +QString(",C")
			  +QString::number(section
					   /game->getNumActions()[state][0])
			  +QString(")"));
	}
    }
  else
    return QVariant();
} // headerData

bool SGProbabilityTableModel::setData(const QModelIndex & index,
				      const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    {
      QRegExp rx("[, ]");
      QStringList list = value.toString().split(rx,QString::SkipEmptyParts);

      if (list.size())
	game->setProbability(state,index.row(),index.column(),
			     list.at(0).toDouble());
      
      emit dataChanged(index,index);
      return true;
    }
  return false;
} // setData

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

  payoffTableView = new QTableView();
  probabilityTableView = new QTableView();
  payoffTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  probabilityTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);

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
}

SGGameHandler::~SGGameHandler()
{
  for (int state = 0; state < game.getNumStates(); state++)
    {
      delete payoffModels[state];
      delete probabilityModels[state];
    }
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
}

void SGGameHandler::changeNumberOfStates(int newS)
{
  numStatesEdit->setText(QString::number(game.getNumStates()));
  
  int state = currentStateCombo->count();
  // Remove items larger than the new number of states
  while (state > newS)
    currentStateCombo->removeItem(--state);
  // Add new states if number of states increased
  while (state < newS)
    currentStateCombo->addItem(QString::number(state++));

  currentStateCombo->setCurrentIndex(0);
}

void SGGameHandler::initializeModels()
{
  for (int state = payoffModels.size()-1;
       state >= 0;
       state --)
    {
      delete payoffModels[state];
      delete probabilityModels[state];
    }

  payoffModels = vector < SGPayoffTableModel* > (game.getNumStates());
  probabilityModels = vector < SGProbabilityTableModel* > (game.getNumStates());
    
  for (int state = 0;
       state < game.getNumStates();
       state++)
    {
      payoffModels[state] = new SGPayoffTableModel(&game,state);
      probabilityModels[state] = new SGProbabilityTableModel(&game,state);
    }
  
  payoffTableView->setModel(payoffModels[0]);
  payoffTableView->show();
  probabilityTableView->setModel(probabilityModels[0]);
  probabilityTableView->show();

  payoffTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  probabilityTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
}

void SGGameHandler::setState(int state)
{
  for (int player = 0; player < 2; player ++)
    numActionsEdits[player]
      ->setText(QString::number(game.getNumActions()[state][0]));
  
  payoffTableView->setModel(payoffModels[state]);
  probabilityTableView->setModel(probabilityModels[state]);

  currentStateCombo->setCurrentIndex(state);
}

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

  payoffModels[state]->emitLayoutChanged();
  probabilityModels[state]->emitLayoutChanged();
}

void SGGameHandler::stateAdded()
{
  int state = currentStateCombo->currentIndex();
  game.addState(state+1);
  changeNumberOfStates(game.getNumStates());
  numStatesEdit->setText(QString::number(game.getNumStates()));

  payoffModels.insert(payoffModels.begin()+state+1,
		      new SGPayoffTableModel(&game,state+1));
  probabilityModels.insert(probabilityModels.begin()+state+1,
			   new SGProbabilityTableModel(&game,
						       state+1));
  for (int statep = state+2;
       statep < game.getNumStates();
       statep++)
    {
      payoffModels[statep]->setState(statep);
      probabilityModels[statep]->setState(statep);
    }

  setState(state+1);
  // payoffModels[state]->emitLayoutChanged();
  // probabilityModels[state]->emitLayoutChanged();
}

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

  payoffModels[state]->emitLayoutChanged();
  probabilityModels[state]->emitLayoutChanged();
}

void SGGameHandler::stateRemoved()
{
  if (game.getNumStates() == 1)
    return;

  int state = currentStateCombo->currentIndex();

  for (int statep = state+1; statep < game.getNumStates(); statep++)
    {
      payoffModels[statep]->setState(statep-1);
      probabilityModels[statep]->setState(statep-1);
    }
  
  int newState;
  if (state == 0)
    newState = 1;
  else
    newState = state-1;
  setState(newState);

  delete payoffModels[state];
  payoffModels.erase(payoffModels.begin()+state);
  delete probabilityModels[state];
  probabilityModels.erase(probabilityModels.begin()+state);

  currentStateCombo->setCurrentIndex(min(state,newState));
  currentStateCombo->removeItem(currentStateCombo->count()-1);

  game.removeState(state);

  numStatesEdit->setText(QString::number(game.getNumStates()));
  
} // stateRemoved

void SGGameHandler::setConstrained(int newState)
{
  if (feasibleCheckBox->isChecked())
    game.setConstrained(vector<bool>(2,true));
  else
    game.setConstrained(vector<bool>(2,false));
}
