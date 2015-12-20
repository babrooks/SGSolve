#ifndef _SGITERATION_HPP
#define _SGITERATION_HPP

#include "sggame.hpp"
#include "sgaction.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/utility.hpp>

//! Stores data on the behavior of SGApprox::generate()
/*! This class records information on each cut made by the twist
    algorithm. 

    \ingroup src
*/
class SGIteration
{
private:
  int iteration; /*!< The value of SGApprox::numIterations. */
  int revolution; /*!< The value of SGApprox::numRevolutions. */
  int numExtremeTuples; /*!< The size of SGApprox::extremeTuples. */
  SGTuple pivot; /*!< The current value of SGApprox::pivot. */
  SGPoint direction; /*!< The shallowest admissible direction at the
                        current revolution. */
  vector< vector<SGBaseAction> > actions; /*!< The actions that can be
                                           supported at the current
                                           iteration */

  //! The state that generated the best direction.
  int bestState; 
  //! The action that generated the best direction.
  int bestAction; 
  //! True if the best direction was non-binding.
  SG::Regime regime; 
  //! The current action tuple.
  vector<int> actionTuple; 
  //! The states in which IC constraints are not binding. 
  vector<SG::Regime> regimeTuple; 
  //! The current threat tuple.
  SGTuple threatTuple;

public:
  //! Default constructor
  SGIteration() {}
  //! Initializes a new SGIteration object with data on the current
  //! iteration
  SGIteration(int _iteration,
	      int _revolution,
	      int _numExtremeTuples,
	      int _bestState,
	      int _bestAction,
	      SG::Regime _regime,
	      const vector< list<SGAction> > & _actions,
	      const SGTuple &_pivot,
	      const SGPoint & _direction,
	      const vector<const SGAction*> &_actionTuple,
	      const vector<SG::Regime> & _regimeTuple,
	      const SGTuple & _threatTuple):
    iteration(_iteration),
    revolution(_revolution),
    numExtremeTuples(_numExtremeTuples),
    regime(_regime),
    actions(_actions.size()),
    pivot(_pivot),
    bestState(_bestState),
    bestAction(_bestAction),
    direction(_direction),
    regimeTuple(_regimeTuple),
    threatTuple(_threatTuple)
  {
    actionTuple = vector<int>(_actionTuple.size(),-1);
    for (int state = 0; state < actionTuple.size(); state++)
      {
	if (!(_actionTuple[state]->getIsNull()))
	  actionTuple[state] = _actionTuple[state]->getAction();
      }

    for (int state = 0; state < actions.size(); state++)
      {
	for (list<SGAction>::const_iterator action = _actions[state].begin();
	     action != _actions[state].end();
	     ++action)
	  {
	    actions[state].push_back(SGBaseAction(*action));
	  }
      }
  }

  //! Get method for the iteration.
  int getIteration() const { return iteration; } 
  //! Get method for the revolution.
  int getRevolution() const { return revolution; }
  //! Get method for the number of extreme tuples.
  int getNumExtremeTuples() const { return numExtremeTuples; }
  //! Get method for the current pivot.
  const SGTuple & getPivot() const { return pivot; }
  //! Get method for the current direction
  const SGPoint & getDirection() const { return direction; }
  //! Get method for the actions available at the current iteration.
  const vector< vector<SGBaseAction> > & getActions() const { return actions; }
  //! Get method for the best state.
  int getBestState() const { return bestState; }
  //! Get method for the best action.
  int getBestAction() const { return bestAction; }
  //! Get method for the regime corresponding to the best direction.
  SG::Regime getRegime() const { return regime; }
  //! Get method for the action tuple.
  const vector<int> & getActionTuple() const { return actionTuple; }
  //! Get method for the regime tuple.
  const vector<SG::Regime> & getRegimeTuple() const { return regimeTuple; }
  //! Get method for the current threat tuple.
  const SGTuple & getThreatTuple() const { return threatTuple; }
  
  //! Serializes the SGIteration object using boost.
  friend class boost::serialization::access;
  template<class Archive>

  //! Serialize the iteration using Boost.
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & iteration & revolution & numExtremeTuples
      & pivot & direction & actions
      & bestState & bestAction & regime & actionTuple
      & regimeTuple & threatTuple;
  }
}; // SGIteration

#endif
