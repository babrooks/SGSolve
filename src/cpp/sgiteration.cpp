// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
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

#include "sgapprox.hpp"

SGIteration::SGIteration(const SGApprox & approx,
			 bool storeActions):
  iteration(approx.getNumIterations()),
  revolution(approx.getNumRevolutions()),
  numExtremeTuples(approx.getExtremeTuples().size()),
  regime(approx.getBestRegime()),
  direction(approx.getBestDirection()),
  pivot(approx.getPivot()),
  threatTuple(approx.getThreatTuple()),
  regimeTuple(approx.getRegimeTuple()),
  actionTuple(approx.getActionTuple().size(),-1),
  actions(approx.getActions().size())
{
  if (approx.getBestAction() != approx.getActions()[0].end())
    {
      bestState = approx.getBestAction()->getState();
      bestAction = approx.getBestAction()->getAction();
    }
  else
    {
      bestState = -1;
      bestAction = -1;
    }

  for (int state = 0; state < actionTuple.size(); state++)
    {
      if (!(approx.getActionTuple()[state]->getIsNull()))
	actionTuple[state] = approx.getActionTuple()[state]->getAction();
      else
	actionTuple[state] = -1;
    }

  if (storeActions)
    {
      for (int state = 0; state < actions.size(); state++)
	{
	  for (list<SGAction>::const_iterator action
		 = approx.getActions()[state].begin();
	       action != approx.getActions()[state].end();
	       ++action)
	    {
	      actions[state].push_back(SGBaseAction(*action));
	    }
	}
    }
}

