#ifndef SGABSTRACTGAME_HPP
#define SGABSTRACTGAME_HPP

//! A virtual class for constructing games
/*! This class can be derived from to create a template from which an
    SGGame is constructed. For an example of how this is done, see
    risksharing.hpp. The user provides definitions of a payoffs method
    and a probability method that return the flow payoffs of the
    players and the transition probabilities between states,
    respectively. The value in this is that the user can provide \e
    rules for how these quantities are generated, rather than having
    to generate \e arrays of these values for passing to the SGGame
    constructor. See also SGGame::SGGame.

   \ingroup src
 */
class SGAbstractGame
{
protected:
  //! The number of players, always 2
  int numPlayers;
  //! The discount factor
  double delta;
  //! The number of states
  int numStates;
  //! The numbers of actions in each state
  /*! numActions[s][s] is the number of actions that player i can take
      in state s. */
  vector< vector<int> > numActions;
  
  //! Converts a linear index to multiindex
  /*! This function takes the linear index of an action pair in state
      s and returns a multiindex that gives each player's action. */
  vector<int> indexToActions(int index,int state) const
  {
    vector<int> actions(2,0);
    actions[0] = index%numActions[state][0];
    actions[1] = index/numActions[state][0];
    return actions;
  } // indexToActions
  
public:
  //! Constructor for the pure virtual class
  SGAbstractGame(double _delta,
		 int _numStates,
		 vector< vector<int> > _numActions):
    numPlayers(2),
    delta(_delta),
    numStates(_numStates),
    numActions(_numActions)
  {}
  
  //! The payoff function
  /*! A class that is derived from SGAbstractGame must define the
      payoffs method, which returns, for a given state and action
      pair, the flow payoffs that the players receive as an
      SGPoint. */
  virtual SGPoint payoffs(int state,const vector<int> & actions) const = 0;
  //! Transition probabilities
  /*! A class that derives from SGAbstractGame must define the
      probability method, which gives, for each state and action pair
      and new state, the probability of reaching the new state
      tomorrow when starting from the given state and when the given
      action pair is played. */
  virtual double probability(int state,const vector<int> &actions,int statep) const = 0;
  //! Returns true if the given action pair can be played in
  //! equilibrium
  /*! The default definition of this method always returns true, so that all
      action pairs can be played in equilibrium. By redefining this
      method, the user can create models in which only a subset of
      action pairs are played on the equilibrium path. */
  virtual bool isEquilibriumAction(int state, const vector<int> & actions) const
  {return true;}
  //! Returns true if the given player is incentive constrained
  /*! The default definition of this method always returns true, so
      that both players' behavior is subject to incentive
      constraints. By making this method return false for one or both
      players, the user can implement models in which players can
      commit to their actions. When both players are not constrained,
      the algorithm will compute the feasible payoff
      correspondence. */
  virtual bool constrained(int player) const { return true;}

  //! An overloaded version of payoffs that uses a linear action index
  /*! This method converts the linear action index into an action pair
      and then returns the result of the user defined payoffs
      method. */
  SGPoint payoffs(int state,int action) const
  { return payoffs(state,indexToActions(action,state)); }
  //! An overloaded version of probability that uses a linear action
  //! index
  /*! This method converts the linear action index into an action pair
      and then returns the result of the user defined probability
      method. */
  double probability(int state,int action,int statep) const
  { return probability(state,indexToActions(action,state),statep); }
  //! An overloaded version of isEquilibriumAction that uses a linear
  //! action index
  /*! This method converts the linear action index into an action pair
      and then returns the result of the user defined
      isEquilibriumAction method. */
  bool isEquilibriumAction(int state,int action) const
  { return isEquilibriumAction(state,indexToActions(action,state)); }

  //! Returns the discount factor
  double getDelta() const { return delta; }
  //! Returns the number of states
  double getNumStates() const { return numStates; }
  //! Returns the number of actions array
  const vector< vector<int> > getNumActions() const { return numActions; }
};


#endif
