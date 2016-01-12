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

