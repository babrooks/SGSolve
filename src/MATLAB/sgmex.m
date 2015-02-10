function sgmex
% outputs = sgmex(mode,input)
% 
%   Interface to the SGSolve C++ library.
%   BAB
%   10-25-2012
%
%   Options are:
% 
%       sgmex('SaveGame',delta,p1payoffs,p2payoffs,transitions,path)
%            Creates a new game and saves it: delta is the discount
%            factor, p1payoffs and p2payoffs are cell arrays of
%            two-dimensional payoff matrices, transitions is a cell
%            array of transition matrices, and path is the location of
%            the output file. The arrays should be consistent in size,
%            e.g., p1payoffs, p2payoffs, transitions should all have
%            the same number of elements (equal to the number of
%            states), p1payoffs{1} and p1payoffs{2} should have the
%            same dimensions, and the number of rows of transitions{k}
%            should be the number of elements of p1payoffs{k} and a
%            number of rows equal to the number of states.
%       
%       sgmex('Solve',delta,p1payoffs,p2payoffs,transitions,...
%                parameter1,value1,parameter2,value2...)
%            See above for arguments. Solves the game described by
%            delta, p1payoffs, p2payoffs, transitions. Game
%            arguments can be followed by a sequence of
%            parameter/value pairs. Possible parameter values are:
%                  
%                  errorTol
%                  directionTol
%                  pastThreatTol
%                  updatePivotTol
%                  ICTol
%                  normTol
%                  flatTol
%                  levelTol
%                  improveTol
%                  backBendingTol
%                  movementTol
%                  unconstrained
% 
%            Consult the SGSolve documentation for further details
%            on these parameters.
%
%       sgmex('LoadSolution',path)
%            Loads the SG solution at path.
%
%       sgmex('SaveSolution',path)
%            Saves the current SG solution at path.
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
%            Returns a structure that contains the index, best action,
%            state, direction, payoffs, startOfW and sizeOfW.
%
%       [T0,T1] = sgmex('GetTuples')
%            Returns the list of extremeTuples. Ti(k,s) is player
%            i's coordinate in state s at pivot k.
%
end % sgmex
