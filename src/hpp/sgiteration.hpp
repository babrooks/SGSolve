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
public:
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
