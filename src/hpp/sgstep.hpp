// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2018 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

#ifndef _SGSTEP_HPP
#define _SGSTEP_HPP

#include "sgaction_v2.hpp"
#include "sghyperplane.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/utility.hpp>

class SGStep
{
private:
  SGTuple pivot; /*!< The pivot tuple. */
  SGHyperplane hyperplane; /*!< The hyperplanes that define the
			      approximation. */
  vector<int> actionTuple; /*!< The actions that are optimal
			      in each direction. Note that this
			      is given by an index in the
			      actions array, not the primitive
			      action in the game. */
  vector<SG::Regime> regimeTuple; /*!< The regimes that are
				     optimal in each
				     direction. */
public:
  SGStep ()
  {} // default constructor

  SGStep(const vector< list<SGAction_V2>::const_iterator > & _actionTuple,
	 const vector<SG::Regime> & _regimeTuple,
	 const SGTuple & _pivot,
	 const SGHyperplane & _hyperplane,
	 const vector< list<SGAction_V2> > & actions):
    actionTuple (_actionTuple.size(),0),
    regimeTuple{_regimeTuple},
    pivot{_pivot},
    hyperplane{_hyperplane}
  {
    for (int state = 0; state < _actionTuple.size(); state++)
      {
	// Find the element of the actions array that corresponds to
	// the optimal action
	auto action = actions[state].cbegin();
	while (action->getAction() != _actionTuple[state]->getAction()
	       && action != actions[state].cend() )
	  {
	    actionTuple[state]++;
	    ++action;
	  }
      }
  } // Constructor

  // Get method for the pivot
  const SGTuple & getPivot() const {return pivot;}
  // Get method for the action tuple
  const vector<int> & getActionTuple() const {return actionTuple;}
  // Get method for the regimeTuple
  const vector<SG::Regime> & getRegimeTuple() const {return regimeTuple;}
  // Get method for the hyperplane
  const SGHyperplane & getHyperplane() const {return hyperplane;}

    //! Serializes the SGStep object using boost.
  friend class boost::serialization::access;
  
  //! Serialize the iteration using Boost.
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & pivot;
    ar & hyperplane;
    ar & actionTuple;
    ar & regimeTuple;
  }

}; // SGStep

#endif
