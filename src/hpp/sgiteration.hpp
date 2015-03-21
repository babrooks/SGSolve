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

  //! The state that generated the best direction.
  int bestState; 
  //! True if the best direction was non-binding.
  bool nonBinding; 
  //! The current action tuple.
  vector<int> actionTuple; 
  //! The states in which IC constraints are not binding. 
  vector<bool> nonBindingStates; 
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
	      bool _nonBinding,
	      const SGTuple &_pivot,
	      const SGPoint & _direction,
	      const vector<const SGAction*> &_actionTuple,
	      const vector<bool> & _nonBindingStates,
	      const SGTuple & _threatTuple):
    iteration(_iteration),
    revolution(_revolution),
    numExtremeTuples(_numExtremeTuples),
    nonBinding(_nonBinding),
    pivot(_pivot),
    bestState(_bestState),
    direction(_direction),
    nonBindingStates(_nonBindingStates),
    threatTuple(_threatTuple)
  {
    actionTuple = vector<int>(_actionTuple.size(),-1);
    for (int state = 0; state < actionTuple.size(); state++)
      {
	if (!(_actionTuple[state]->getIsNull()))
	  actionTuple[state] = _actionTuple[state]->getAction();
      }
  }

  //! Serializes the SGIteration object using boost.
  friend class boost::serialization::access;
  template<class Archive>

  //! Serialize the iteration using Boost.
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & iteration & revolution & numExtremeTuples
      & pivot & direction
      & bestState & nonBinding & actionTuple
      & nonBindingStates & threatTuple;
  }
}; // SGIteration

#endif
