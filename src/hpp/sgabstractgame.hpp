#ifndef SGABSTRACTGAME_HPP
#define SGABSTRACTGAME_HPP

class SGAbstractGame
{
protected:
  int numPlayers;
  double delta;
  int numStates;
  vector< vector<int> > numActions;
  
  vector<int> indexToActions(int index,int state) const
  {
    vector<int> actions(2,0);
    actions[0] = index%numActions[state][0];
    actions[1] = index/numActions[state][0];
    return actions;
  } // indexToActions
  
public:
  SGAbstractGame(double _delta,
		 int _numStates,
		 vector< vector<int> > _numActions):
    numPlayers(2),
    delta(_delta),
    numStates(_numStates),
    numActions(_numActions)
  {}

  virtual SGPoint payoffs(int state,const vector<int> & actions) const = 0;
  virtual double probability(int state,const vector<int> &actions,int statep) const = 0;

  SGPoint payoffs(int state,int action) const
  { return payoffs(state,indexToActions(action,state)); }
  double probability(int state,int action,int statep) const
  { return probability(state,indexToActions(action,state),statep); }

  virtual bool isEquilibriumAction(int state, int action) const {return true;}
  virtual bool constrained(int player) const { return true;}

  double getDelta() const { return delta; }
  double getNumStates() const { return numStates; }
  const vector< vector<int> > getNumActions() const { return numActions; }
};


#endif
