function sgmex2
% outputs = sgmex2(mode,input)
% 
%   Interface to the SGSolve C++ library.
%   BAB
%   10-12-2018
%
%   Options are:
% 
%       sgmex('LoadSolution',path)
%            Loads the SG solution at path.
%
%       delta = sgmex('GetDelta')
%            Returns the discount factor.
%
%       numStates = sgmex('GetNumStates')
%            Returns the number of states.
%
%       numActions = sgmex('GetNumActions')
%            numActions(s,p) is the number of actions for
%            player p in state s. 
%
%       probabilities = sgmex('GetProbabilities',s)
%            Returns an array of transition probabilities from
%            state s. probabilities(a,s) is the probability of
%            transitioning to state s after action profile
%            a=a0+n0*a1.
%
%       [P0,P1] = sgmex('GetPayoffs',s)
%            Payoffs for each player in state s. Pi(a0,a1) is
%            player i's payoff after actions a0 and a1 are played.
%
%       sgmex('Iter++'), sgmex('Iter++',n)
%            Increments the current iteration by n. Default of 1 if
%            no n is provided.
%
%       sgmex('Iter--'), sgmex('Iter--',n)
%            Decrements the current iteration by n. Default of 1 if
%            no n is provided.
%
%       sgmex('IterToBeginning')
%            Returns current iteration to beginning. 
%
%       sgmex('IterToEnd')
%            Sets current iteration to end. 
%
%       s = sgmex('GetCurrentIteration')
%            Returns a structure that contains the list of pivots, hyperplanes, and optimal actions.
%
end % sgmex
