function W = abstwist(G,P,numActions,delta)
% W = ABSTWIST(G,P,NUMACTIONS,DELTA) implements Abreu Brooks Sannikov twist
% algorithm for solving two player stochastic repeated games with perfect
% monitoring. It returns a K X 2S matrix W, which lists the pivots that
% were generated over the course of the algorithm.
% 
% G is a cell arrays containing n(s) X 2 matrices. G{s}(a,i) is player
% i's payoff when action profile a is played in state s. a = (a2-1)*m1+a1.
%
% numActions is a k X 2 matrix. numActions(s,i) is the number of
% actions of player i in state s.
%
% P is a cell array containing the probability transition
% matrices. The probability of transitioning to state s1 from state s0
% after action profile a is P{s0}(a,s1).
%
% delta is the players' common discount factor.


maxIter = 10000; % Maximum iterations

%% Some parameters
numIterations = 0;
numRevolutions = 0;

Wincrement = 15000; % number of space to add to pivot list when
% we fill it up.

errorTol = 1e-7; % Main algorithm's error tolerance
global improveTol;
improveTol = 1e-13;%1e-14; % Tolerance for the improvement function.
probTol = 1e-12; % Tolerance for probabilities summing to one.
flatTol = 1e-10; % Tolerance for checking flats in coordinate directions for binding IC computation.
pastThreatTol = 1e-12; % Tolerance for passing threat tuple on binding IC computation.
normTol = 1e-12; % Minimum norm for direction to be considered non-zero.
levelTol = 1e-10; % Tolerance for checking level in normal directions.
directionTol = 1e-12; % Tolerance for direction changes.
mergeRankTol = 1e-9; % Tolerance for merging flats.

normal = @(x) [-x(2) x(1)]; % returns the clockwise normal of the 2d vector x.
findLim = @(x) [min(x) max(x)]+[-1 1]*(max(x)-min(x))*0.1;

% Min rotation. Best direction is initialized to opposite of current
% direction, rotated ccw by minRotation radians.
minRotation = 0.01;
rotationMat = @(theta) [cos(theta) -sin(theta); sin(theta) cos(theta)];

% Debug flags
debugMode = false;
movieMode = false;

% Merge colinear tuples on the frontier
mergeTuples = false;

% use min max flags
useMinMax = false;

%% Check inputs for consistency.
assert(numel(G)==numel(P));
assert(numel(G)==size(numActions,1));
assert(size(numActions,2)==2);

numStates = numel(G);
numActionProfiles = numActions(:,1).*numActions(:,2);

X = zeros(numStates);

for state = 1:numStates
    % Check that matrices are all the correct size.
    assert(size(G{state},1)==size(P{state},1));
    assert(size(P{state},2)==numStates);
    assert(size(P{state},1)==numActionProfiles(state));
    
    % Check that state transitions sum to 1.
    assert(all(abs(sum(P{state},2)-ones(numActionProfiles(state),1))<probTol));
end  % for state

%% Initialization
% First find the convex hull of the union of all of the payoffs.
feasiblePayoffs = cat(1,G{:});

% Take feasible set to be the box containing all payoffs.
maxes = max(feasiblePayoffs);
mins = min(feasiblePayoffs);

% we store deviation payoffs here, used
% in the calculation of ICs
bestDeviationPayoffs = cell(2, numStates);
for state = 1:numStates
    for player = 1:2
        bestDeviationPayoffs{player, state} = zeros(numActions(state,3-player),1);
    end
end

% Cell array containing lists of actions which can be supported in
% each state.
viableActions = cat(2,arrayfun(@(n) true(1,n),numActionProfiles,...
    'uniformoutput',false));

if useMinMax
    threatTuple = repmat(min(feasiblePayoffs),1,numStates);
        
    errorLevel = 1;
    while errorLevel > 1e-15
        tempTuple = inf(size(threatTuple));
        for state = 1:numStates
            for player = 1:2
                actionIndices = ones(2,numActions(state,player));
                actionIndices(player,:) = 1:numActions(state,player);
                for a = 1:numActions(state,3-player)
                    % Find maximum payoff player can achieve when 3-player
                    % uses a.
                    actionIndices(3-player,:) = a;
                    rows = sub2ind(numActions(state,:),actionIndices(1,:),actionIndices(2,:));
                    expCont = P{state}(rows,:)*threatTuple(player:2:end)';
                    
                    tempTuple(2*(state-1)+player) = min(tempTuple(2*(state-1)+player),...
                        max((1-delta)*G{state}(rows,player) + delta * expCont));
                end % for other player's action
            end % for player
        end % for state
        
        errorLevel = norm(threatTuple-tempTuple);
        threatTuple = tempTuple;
    end % while
    
else
    threatTuple = repmat(min(feasiblePayoffs),1,numStates);
end

feasiblePayoffs = [mins(1) maxes(2); maxes; maxes(1) mins(2); mins; mins(1) maxes(2)];


% Initialize the payoffs in every state to the feasible payoffs.
numPivots = 5;
W = zeros(Wincrement,2*numStates);
for state = 1:numStates
    W(1:5,2*(state-1)+[1 2]) = ...
        [threatTuple(2*(state-1)+1) maxes(2);...
        maxes;...
        maxes(1) threatTuple(2*(state-1)+2);...
        threatTuple(2*(state-1)+[1 2]);...
        threatTuple(2*(state-1)+1) maxes(2)];
end % for state

% For the initial pivot, take the first element of feasible payoffs.
pivot = W(1,:);

startOfW = 1;

% Current direction takes us back to the beginning of feasible payoffs.
currentDirection = feasiblePayoffs(2,:)-feasiblePayoffs(1,:);
oldDirection = currentDirection;

facingNorth = true;
facingEast = true;
facingSouth = false;
facingWest = false;

% This is the "J" set. Starts off with none of the states being
% non-binding.
nonBindingStates = false(1,numStates);
% When states start to be non-binding, we will record the action
% profile in the following vector.
actionTuple = zeros(1,numStates);

[~,westPoint] = max(feasiblePayoffs*[-1;-directionTol]); oldWest = -1;

errorLevel = 1; % distance between pivot and startOfW-1.

% Initialize binding continuation payoffs. For each state and action,
% we will have a structure that contains (1) the minimum IC
% continuation value and (2) the list of binding continuation
% values.
bindingInfo = arrayfun(@(x) struct('action',num2cell(1:x), ...
    'minIC',-inf(1,2), ...
    'payoffs',repmat({cell(1,2)},1,x)), ...
    numActionProfiles,'uniformOutput',false);

updateBindingInfoFlag = true(1,2);
% flag for the loop to update the binding IC points at the
% beginning of the iteration

xlim = findLim(feasiblePayoffs(:,1));
ylim = findLim(feasiblePayoffs(:,2));

plotDims = zeros(0,2);
smallestDistance = inf;
for numCols=1:numStates
    minRows = ceil(numStates/numCols);
    
    if abs(numCols-minRows)<smallestDistance || (abs(numCols-minRows)==smallestDistance && minRows<plotDims(1))
        plotDims(1) = minRows; plotDims(2) = numCols;
        smallestDistance = abs(numCols-minRows);
    end
end

                    
totalRebuildingTime = 0;

%% Main loop
while errorLevel > errorTol && numIterations < maxIter
    
    if numIterations == 175
		% keyboard
        % debugMode = true; %numIterations >= 10;
    end
    
    %% Update the threat tuple/binding continuation values
    if any(updateBindingInfoFlag)
        tstart = tic;
            
        for state=1:numStates
            actions = cell(1,2);

            %% First update minmium IC continuation values
            for action = 1:numActionProfiles(state)
                if ~viableActions{state}(action), continue; end;

                % Decompose action into players' individual actions
                [actions{:}] = ind2sub(numActions(state,:),action);
                
                for player=1:2
                    if ~updateBindingInfoFlag(player), continue; end;
                    
                    %Start with minIC initialized to -inf.
                    minIC = bindingInfo{state}(action).minIC(player);
                    deviations = actions;
                    
                    for dev = 1:numActions(state,player)
                        deviations{player} = dev;
                        deviation = sub2ind(numActions(state,:),deviations{:});
                        
                        minIC = max(minIC,...
                            (1-delta)/delta*(G{state}(deviation,player)...
                            -G{state}(action,player))...
                            +P{state}(deviation,:)*threatTuple(player:2:end)');
                    end % for deviation
                    bindingInfo{state}(action).minIC(player) = minIC;

                end % player
                
                %% Find binding continuations
                for player=1:2
                    % Only update if the flag has been thrown
                    if ~updateBindingInfoFlag(player) % && ~isempty(bindingInfo{state}(action).payoffs{player})
                        continue;
                    end % if updateBindingInfo for player
                    
                    minIC = bindingInfo{state}(action).minIC(player);
                    
                    % Reset the payoffs for the player
                    newPayoffs = zeros(4,2);
                    filledupto = 0;
                    
                    % Find the binding IC continuation values. We search
                    % through the list of edges in W.
                    point = numPivots;
                    while point >= 2
                        % Calculate expectation of point and point-1.
                        
                        x = [W((point-1):point,1:2:end)*(P{state}(action,:)')...
                            W((point-1):point,2:2:end)*(P{state}(action,:)')];
                        
                        % Check if we are on a flat. If so, we will pick this point up on the
                        % previous or next iteration. Skip to the
                        % next player.
                        if abs(x(1,player) - x(2,player)) < flatTol...
                                && abs(x(1,player)-minIC) < flatTol
                            newPayoffs(filledupto+1:filledupto+size(x,1),:) = x;
                            filledupto = filledupto + size(x,1);
                        elseif (x(1,player)<=minIC && minIC <= x(2,player))...
                                || (x(1,player)>=minIC && minIC >= x(2,player))
                            % These elements of the tuple flank the minimum IC payoff.
                            
                            % Find the weight on x(1,player)
                            alpha = (minIC-x(2,player))/(x(1,player)-x(2,player));
                            
                            newPayoffs(filledupto+1,:) = x(2,:)+alpha*(x(1,:)-x(2,:));
                            filledupto = filledupto + 1;
                        end % if
                        
                        newPayoffs(filledupto+1:end,:)=[];
                        
                        if (any(W(point,player:2:end)<threatTuple(player:2:end)-pastThreatTol)...
                                && all(W(point,player:2:end) <= threatTuple(player:2:end)))
                            break
                        end
                        
                        point = point-1;
                    end % for point
                    
                    bindingInfo{state}(action).payoffs{player} = newPayoffs;
                end % player
                
                for player=1:2
                    % Remove points that are not IC.
                    if ~isempty(bindingInfo{state}(action).payoffs{player})
                        
                        [maxOtherPayoff,iMax] = max(bindingInfo{state}(action).payoffs{player}(:,3-player));
                        [minOtherPayoff,iMin] = min(bindingInfo{state}(action).payoffs{player}(:,3-player));
                        
                        if maxOtherPayoff >= bindingInfo{state}(action).minIC(3-player)
                            if minOtherPayoff < bindingInfo{state}(action).minIC(3-player)
                                bindingInfo{state}(action).payoffs{player} ...
                                    = [bindingInfo{state}(action).payoffs{player}(iMax,:); ...
                                    bindingInfo{state}(action).minIC];
                            else
                                bindingInfo{state}(action).payoffs{player} ...
                                    = bindingInfo{state}(action).payoffs{player}([iMax,iMin],:);
                            end % if
                        else
                            bindingInfo{state}(action).payoffs{player}=zeros(0,2);
                        end % if
                    end % if
                end % player
                
                % If we couldn't find any payoffs, then the action profile
                % is not viable.
                viableActions{state}(action) = ~all(arrayfun(@(x) isempty(x{1}),bindingInfo{state}(action).payoffs));
                
                if debugMode
                    set(gcf,'name',sprintf('BindingInfo: Iteration %d, state %d, action %d',numIterations,state,action));
                    
                    subplot(1,1,1);
                    plot(P{state}(action,:)*W(1:numPivots,1:2:end)',P{state}(action,:)*W(1:numPivots,2:2:end)','r-',...
                        P{state}(action,:)*W(startOfW:numPivots,1:2:end)',P{state}(action,:)*W(startOfW:numPivots,2:2:end)','b-',...
                        dot(P{state}(action,:),W(numPivots,1:2:end))+[0,currentDirection(1)],...
                        dot(P{state}(action,:),W(numPivots,2:2:end))+[0,currentDirection(2)],'m--',...
                         [xlim(2) bindingInfo{state}(action).minIC([1 1])],[bindingInfo{state}(action).minIC([2 2]) ylim(2)],'k--');
                    
                    
                    hold on;
                    plot(bindingInfo{state}(action).payoffs{1}(:,1),bindingInfo{state}(action).payoffs{1}(:,2),'g.',...
                        bindingInfo{state}(action).payoffs{2}(:,1),bindingInfo{state}(action).payoffs{2}(:,2),'g.');
                    hold off;
                    
                    set(gca,'xlim',xlim,'ylim',ylim);
                    axis square
                    title(sprintf('State %d, action %d',state,action));
                    
                    % Set break point here
                end % debugmode
                
            end % for action
            
            % Check if expected pivot is no longer IC for nonbinding
            % states.
            if nonBindingStates(state)....
                    && any( P{state}(actionTuple(state),:)*reshape(pivot,2,[])'...
                    <=bindingInfo{state}(actionTuple(state)).minIC-pastThreatTol)
                nonBindingStates(state)=false;
            end % if no longer IC
        end % for state
        totalRebuildingTime = totalRebuildingTime + toc(tstart);
    end % if updateBindingInfoFlag

    %% Trim the binding continuations
    if norm(currentDirection)>normTol ...
            && rank([currentDirection;oldDirection],mergeRankTol)>1
        % Trim the binding continuation segments to reflect the most recent
        % cut, only if the most recent cut was non-trivial.
        
        for player=1:2
            for state=1:numStates
                for action=1:numActionProfiles(state)
                    if ~viableActions{state}(action) ...
                            || isempty(bindingInfo{state}(action).payoffs{player})
                        continue;
                    end % if not viable or segment is empty
                    
                    currentNormal = normal(currentDirection);
                    pivotLevel = P{state}(action,:)*(currentNormal*reshape(pivot,2,[]))';
                    levels = bindingInfo{state}(action).payoffs{player}*currentNormal';
                    
                    [maxLevel,iMax] = max(levels);
                    [minLevel,iMin] = min(levels);
                    
                    if maxLevel <= pivotLevel-levelTol
                        continue; % Both are within halfspace
                    elseif pivotLevel < minLevel-levelTol
                        % Both are oustide half space and must be cut.

                        % Delete this player's binding payoffs.
                        bindingInfo{state}(action).payoffs{player}=zeros(0,2);
                        
                        % If other set is non-empty, action is now unviable
                        if isempty(bindingInfo{state}(action).payoffs{3-player})
                            % viableActions{state}(action) = false;
                            break;
                        end
                    elseif maxLevel > pivotLevel + levelTol
                        % Replace the payoff that is outside the cut.
                        weightOnMax = (pivotLevel-minLevel)/(maxLevel-minLevel);
                        bindingInfo{state}(action).payoffs{player}(iMax,:)...
                            = bindingInfo{state}(action).payoffs{player}(iMax,:)*weightOnMax ...
                            + bindingInfo{state}(action).payoffs{player}(iMin,:)*(1-weightOnMax);
                    end
                    
                    if debugMode
                        set(gcf,'name',sprintf('Update bindingInfo: Iteration %d, state %d, action %d',numIterations,state,action));
                        
                        subplot(1,1,1);
                        plot(P{state}(action,:)*W(1:numPivots,1:2:end)',P{state}(action,:)*W(1:numPivots,2:2:end)','r-',...
                            P{state}(action,:)*W(startOfW:numPivots,1:2:end)',P{state}(action,:)*W(startOfW:numPivots,2:2:end)','b-',...
                            dot(P{state}(action,:),W(numPivots,1:2:end))+[0,100*currentDirection(1)],...
                            dot(P{state}(action,:),W(numPivots,2:2:end))+[0,100*currentDirection(2)],'m--',...
                            [xlim(2) bindingInfo{state}(action).minIC([1 1])],...
                            [bindingInfo{state}(action).minIC([2 2]) ylim(2)],'k--');
                        
                        if ~isempty(bindingInfo{state}(action).payoffs{player})
                            hold on;
                            plot(bindingInfo{state}(action).payoffs{player}(:,1),...
                                bindingInfo{state}(action).payoffs{player}(:,2),'g.');
                            hold off;
                        end % if
                        
                        set(gca,'xlim',xlim,'ylim',ylim);
                        axis square
                        title(sprintf('State %d, action %d',state,action));
                        
                        if movieMode
                            F = getframe(gcf);
                            [X,map] = rgb2ind(F.cdata,256);
                            imwrite(X,map,fname,'gif','Writemode',writemode,...
                                'Delaytime',delaytime,'loopcount',inf);
                        end % if movieMode
                        
                        % Set breakpoint here
                    end % if debugMode
                    
                end % for action
            end % for state
        end % for player
    end % Trim
    
    %% Search for the next best direction

    % Structure to store information about the best direction
    best = struct('direction',-currentDirection*rotationMat(-minRotation),...
        'action',0,...
        'state',0,...
        'notBinding',false,...
        'payoff',zeros(1,2));

    % Loop over all states and action profiles.
    for state=1:numStates 
        statePivot = pivot(2*(state-1)+[1 2]);
        
        for action = 1:numActionProfiles(state)
            if ~viableActions{state}(action), continue, end
            
            expPivot = P{state}(action,:)*reshape(pivot,2,[])';
            nonBindingPayoff = (1-delta)*G{state}(action,:)+delta* ...
                expPivot;
            nonBindingDirection = nonBindingPayoff-statePivot;
            nonBindingNormal = normal(nonBindingDirection);
            
%             if dot(normal(best.direction),nonBindingDirection)<-1e-12*norm(nonBindingDirection)*norm(best.direction)
%                 keyboard
%                 continue;
%             end
            if improves(best.direction,-currentDirection,nonBindingDirection)
                continue;
            end

            if debugMode
                set(gcf,'name',sprintf('Non-binding direction: Iteration %d, state %d, action %d',numIterations,state,action));
                
                for innerState=1:numStates
                    subplot(plotDims(1),plotDims(2),innerState);
                    
                    p=plot(W(1:numPivots,2*(innerState-1)+1),W(1:numPivots,2*(innerState-1)+2),'r-',...
                        W(startOfW:numPivots,2*(innerState-1)+1),W(startOfW:numPivots,2*(innerState-1)+2),'b-',...
                        pivot(2*(innerState-1)+1),pivot(2*(innerState-1)+2),'k.',...
                        pivot(2*(innerState-1)+1)+[0 currentDirection(1)], ...
                        pivot(2*(innerState-1)+2)+[0,currentDirection(2)],'k-',...
                        pivot(2*(innerState-1)+1)+[0 nonBindingDirection(1)], ...
                        pivot(2*(innerState-1)+2)+[0,nonBindingDirection(2)],'m--',...
                        pivot(2*(innerState-1)+1)+[0 best.direction(1)], ...
                        pivot(2*(innerState-1)+2)+[0,best.direction(2)],'g:');
                    set(p,'markersize',13);
                    if state==innerState
                        hold on;
                        p = plot(nonBindingPayoff(1),nonBindingPayoff(2),'m',...
                            [xlim(2) bindingInfo{state}(action).minIC([1 1])],[bindingInfo{state}(action).minIC([2 2]) ylim(2)],'k--');
                        set(p,'markersize',13);
                        hold off;
                        
                        hold on;
                        p = plot(nonBindingPayoff(1),nonBindingPayoff(2),'m.',...
                            [xlim(2) bindingInfo{state}(action).minIC([1 1])],[bindingInfo{state}(action).minIC([2 2]) ylim(2)],'k--',...
                            G{state}(action,1),G{state}(action,2),'kx');
                        set(p,'markersize',13);
                        
                        for player=1:2
                            if ~isempty(bindingInfo{state}(action).payoffs{player})
                                continuations = bindingInfo{state}(action).payoffs{player};
                                stagePayoffs = repmat(G{state}(action,:),size(continuations,1),1);
                                stagePivots = repmat(statePivot,size(continuations,1),1);
                                genPayoffs = (1-delta)*stagePayoffs+delta*continuations;
                                l = line([stagePayoffs(:,1),continuations(:,1)]',[stagePayoffs(:,2),continuations(:,2)]');
                                set(l,'linestyle',':','color','k');
                                l = line([stagePivots(:,1),genPayoffs(:,1)]',[stagePivots(:,2),genPayoffs(:,2)]');
                                set(l,'linestyle',':','color','c');
                                plot(continuations(:,1),continuations(:,2),'g.',...
                                    genPayoffs(:,1),genPayoffs(:,2),'c.');
                            end
                        end % player
                        hold off;                    
                    end
                    
                    set(gca,'xlim',xlim,'ylim',ylim);
                    axis square
                    title(sprintf('State %d',innerState));
                    
                    if movieMode
                        F = getframe(gcf);
                        [X,map] = rgb2ind(F.cdata,256);
                        imwrite(X,map,fname,'gif','Writemode',writemode,...
                            'Delaytime',delaytime,'loopcount',inf);
                    end % if movieMode
                    
                end % for innerstate
                
                % Set break point here
            end % debugmode

            % Non-binding direction is available if it the expected pivot
            % is IC.
            if all(expPivot >= bindingInfo{state}(action).minIC) ... rounding here?
                    && norm(nonBindingDirection)>normTol
                if improves(currentDirection,best.direction,nonBindingDirection)...
                        && ~(action==actionTuple(state) && nonBindingStates(state))
                    % New best direction
                    
                    assert(abs(dot(nonBindingNormal,nonBindingDirection))<levelTol)
                    
                    best.direction = nonBindingDirection;
                    best.state = state;
                    best.action = action;
                    best.notBinding = true;
                    best.payoff = nonBindingPayoff;

                end
                
                continue; % Move on to the next action/state.
            end
            
            % These two vectors are, respectively, the steepest ccw
            % direction and shallowest cw direction from
            % nonBindingDirection.
            aboveDirection = [];
            belowDirection = [];
            belowPayoff = [];
%          
%             bestBindingDirection = best.direction;
%             bestBindingPayoff = [];
            
            available = false;
                    
            for player=1:2
                for point=1:size(bindingInfo{state}(action).payoffs{player},1)
                    bindingPayoff = (1-delta) * G{state}(action,:) ...
                        + delta * bindingInfo{state}(action).payoffs{player}(point,:);
                    bindingDirection = bindingPayoff - statePivot;
                    
                    bindingLevel = dot(nonBindingNormal,bindingDirection)/(norm(nonBindingNormal)*norm(bindingDirection));
                    if norm(nonBindingDirection)>normTol...
                            && norm(bindingDirection)>normTol
                        if bindingLevel < levelTol...
                                && (isempty(belowDirection) ...
                                || improves(nonBindingDirection,belowDirection,bindingDirection))
                            belowDirection = bindingDirection;
                            belowPayoff = bindingPayoff;
                        end
                        if bindingLevel > -levelTol...
                                && (isempty(aboveDirection) ...
                                || improves(aboveDirection,nonBindingDirection,bindingDirection))
                            aboveDirection = bindingDirection;
                        end
                    end
                    
                                        
                    if ~isempty(aboveDirection) && ~isempty(belowDirection)...
                            && (dot(aboveDirection,normal(belowDirection)) >= levelTol...
                            || (dot(belowDirection,aboveDirection)>=levelTol ...
                            && dot(belowDirection,nonBindingDirection)>=levelTol))% Maybe should have tolerance here?
                        % Found both an above direction and a below
                        % direction whose cone contains the non-binding
                        % direction.
          
                        available = true;
                        
%                         % Make sure that this direction is not pointing out
%                         % of the IC set, relative to the old continuation
%                         % value.
%                         if action==actionTuple(state)
%                             oldContVal = (pivot(2*(state-1)+[1 2])-(1-delta)*G{state}(action,:))/delta;
%                             if any(oldContVal <= bindingInfo{state}(action).minIC ... 
%                                     & nonBindingDirection < -directionTol)
%                                 continue;
%                             end % if
%                         end % if
                        
                        % Non-binding direction is available. Now check if
                        % it improves, and is not currently being used as
                        % the non-binding action in the current state.
                        if norm(nonBindingDirection)>normTol...
								&& improves(currentDirection,best.direction,nonBindingDirection)...
								&& ~(action==actionTuple(state) && nonBindingStates(state))
                            % And is the new best direction
                            best.direction = nonBindingDirection;
                            best.state = state;
                            best.action = action;
                            best.notBinding = true;
                            best.payoff = nonBindingPayoff;
                        end
                    end


%                     if improves(currentDirection,bestBindingDirection,bindingDirection)...
%                             && (action~=actionTuple(state) || nonBindingStates(state) ...
%                             || norm(bindingPayoff - statePivot)>normTol)
%                         bestBindingDirection = bindingDirection;
%                         bestBindingPayoff = bindingPayoff;
%                     end % if

                    % If available, skip further search of binding
                    % directions.
                    if available, break; end;
                end % for point
                
                if available, break; end;
            end % for player
            
            % If ~available and we found a below direction that improves on
            % best.direction, use it.
%             if ~(isempty(bestBindingPayoff) || available)
%                 best.direction = bestBindingDirection;
%                 best.payoff = bestBindingPayoff;
            if ~(available || isempty(belowDirection)) ...
                    && improves(currentDirection,best.direction,belowDirection)...
                    && norm(belowPayoff-statePivot)>normTol
                % Found a binding direction that improves and is below
                % non-binding direction.
                best.direction = belowDirection;
                best.payoff = belowPayoff;
                best.state = state;
                best.action = action;
                best.notBinding = false;
            end
            
        end % for action
    end % for state
    
	assert(best.action ~= 0 && best.state ~= 0)
	
    %% Update the pivot
    oldPivot = pivot;
    nonBindingStates(best.state) = best.notBinding;
    newPivot = pivot;
    newPivot(2*(best.state-1) + [1,2]) = best.payoff;
    actionTuple(best.state)=best.action;
    
    % Advance the pivot for non-binding states.
    if any(nonBindingStates)
        impulses = zeros(numStates,1);
        impulses(best.state)=1;
        
        % The maximum 11amount the pivot can move in each state, in best.direction
        maxMovements = inf(numStates,1);
        
        violationMovements = impulses;
        oldViolationMovements = zeros(size(impulses));
        tempViolationMovements = zeros(size(impulses));
        
        % Matrix that stores state-transition probabilities for each state.
        X(best.state,:) = delta*P{best.state}(actionTuple(best.state),:);
        
        % First calculate the maximum movements.
        if any(best.direction<0)
            % Find max movements
            relevantIC = best.direction<0;
            for state=1:numStates
                if ~nonBindingStates(state), continue; end;
                
                oldContVal = (pivot(2*(state-1)+[1,2])-(1-delta)*G{state}(actionTuple(state),:));
                % note - we didn't divide old cont val by delta!
                
                alpha = (delta*bindingInfo{state}(actionTuple(state)).minIC(relevantIC)...
                    - oldContVal(relevantIC))./best.direction(relevantIC);
                alpha = alpha(~isnan(alpha) & alpha>0);
                if ~isempty(alpha)
                    maxMovements(state)=min(alpha);
                end
                
            end % for
            
            assert(all(maxMovements>=0));
        end
        
        % Now update the pivot. We try to invert the system of equations.
        % If the movements are all less then maxMovements, we are done.
        % Otherwise, we iterate a Bellman operator until one of the
        % movement restrictions is violated, fix the violating state at
        % maxMovement and make it a binding state, and try to reinvert the
        % system. Repeat until we invert without violating maxMovements.
        movement = zeros(numStates,1);
        while any(nonBindingStates)
            
            movement(nonBindingStates) = (eye(sum(nonBindingStates))-X(nonBindingStates,nonBindingStates))...
                \(impulses(nonBindingStates)+X(nonBindingStates,~nonBindingStates)*impulses(~nonBindingStates,:));
            
            if all(movement(nonBindingStates) <= maxMovements(nonBindingStates)+1e-13)
                break;
            end
            
            % Something was violated. Iterate until we find the violation.
            while true
                tempViolationMovements(nonBindingStates) ...
                    = violationMovements(nonBindingStates)+X(nonBindingStates,:)*(violationMovements-oldViolationMovements);
                oldViolationMovements = violationMovements;
                violationMovements(nonBindingStates) = tempViolationMovements(nonBindingStates);
                
                violations = violationMovements > maxMovements;
                if any(violations)
                    impulses(violations) = maxMovements(violations);
                    violationMovements(violations) = maxMovements(violations);
                    nonBindingStates(violations)=false;
                    break;
                end % if any violations
            end % while bellman procedure
        end % while
        
        assert(all(movement>=0));
        movement(~nonBindingStates)=impulses(~nonBindingStates);
        newPivot = pivot + kron(movement',best.direction);
    end

    assert(all(~isnan(newPivot)));
    
    %% Some final updates before the next iteration
    % If we pass due north, update player 1's threat points. If we
    % pass due west, update player 2's threat points. If we
    % pass doe east, update binding info.
    updateBindingInfoFlag = false(1,2);
    
    if mergeTuples
        % Sum is not such a good idea. Should weight by size of payoffs in
        % each state.
        if dot(currentDirection,normal(best.direction))...
                <mergeRankTol*norm(best.direction)*norm(currentDirection)
            numPivots = numPivots - 1;
        end
    end
    
    passNorth = false;
    if facingWest && best.direction(1) > directionTol
        %% pass north
        facingWest = false; facingEast = true;
        passNorth = true;
        numRevolutions = numRevolutions + 1;

        westPoint = oldWest;
        oldWest = numPivots+1;

        if any(oldPivot(1:2:end)>threatTuple(1:2:end)+pastThreatTol)
            threatTuple(1:2:end) = max(oldPivot(1:2:end),threatTuple(1:2:end));
            updateBindingInfoFlag(1) = true;
        end
    elseif facingEast && best.direction(1) < -directionTol
        % Pass south
        facingEast = false; facingWest = true;
    end % East/West
    
    if facingSouth && best.direction(2) > directionTol
        % Pass west
        facingSouth = false; facingNorth = true;

        if any(oldPivot(2:2:end) > threatTuple(2:2:end)+pastThreatTol)
            threatTuple(2:2:end) = max(oldPivot(2:2:end),threatTuple(2:2:end));
            updateBindingInfoFlag(2) = true;
        end
    elseif facingNorth && best.direction(2) < -directionTol
        % Pass east
        facingNorth = false; facingSouth = true;
    end % North/South
    
    numPivots = numPivots + 1;
    W(numPivots,:) = newPivot;
    pivot = newPivot;
    oldDirection = currentDirection;
    currentDirection = best.direction;
    
    if debugMode
        set(gcf,'name',sprintf('End of iteration %d',numIterations));
        best.direction = best.direction /norm(best.direction );
        for state=1:numStates
            subplot(plotDims(1),plotDims(2),state);
            plot(W(1:numPivots,2*(state-1)+1),W(1:numPivots,2*(state-1)+2),'r-',...
                W(startOfW:numPivots,2*(state-1)+1),W(startOfW:numPivots,2*(state-1)+2),'b-',...
                oldPivot(2*(state-1)+1),oldPivot(2*(state-1)+2),'k.',...
                oldPivot(2*(state-1)+1)+[0 best.direction(1)], ...
                oldPivot(2*(state-1)+2)+[0,best.direction(2)],'k-',...
                newPivot(2*(state-1)+1),newPivot(2*(state-1)+2),'m.');
            
            set(gca,'xlim',xlim,'ylim',ylim);
            axis square
            title(sprintf('State %d',state));
        end % for state
        
        if movieMode
            F = getframe(gcf);
            delaytime = 0.6;
            [X,map] = rgb2ind(F.cdata,256);
            imwrite(X,map,fname,'gif','Writemode',writemode,...
                'Delaytime',delaytime,'loopcount',inf);
        end % if movieMode
        
        % Set break point here
    end % debugMode

    if oldWest >= 0 && passNorth
        numExtremePoints = oldWest-westPoint;
        if numExtremePoints<2 || numExtremePoints > westPoint
            errorLevel = 1;
        else
            errorLevel = max(max(abs(W(numPivots-numExtremePoints:numPivots-1,:)...
                - W(numPivots-2*numExtremePoints:numPivots-numExtremePoints-1,:)),[],2));
        end
    end
    
    
    if debugMode
        set(gcf,'name',sprintf('End of iteration %d',numIterations));
        best.direction = best.direction /norm(best.direction );
        for state=1:numStates
            subplot(plotDims(1),plotDims(2),state);
            plot(W(1:numPivots,2*(state-1)+1),W(1:numPivots,2*(state-1)+2),'r-',...
                W(startOfW:numPivots,2*(state-1)+1),W(startOfW:numPivots,2*(state-1)+2),'b-',...
                oldPivot(2*(state-1)+1),oldPivot(2*(state-1)+2),'k.',...
                oldPivot(2*(state-1)+1)+[0 best.direction(1)], ...
                oldPivot(2*(state-1)+2)+[0,best.direction(2)],'k-',...
                newPivot(2*(state-1)+1),newPivot(2*(state-1)+2),'m.');
            
            set(gca,'xlim',xlim,'ylim',ylim);
            axis square
            title(sprintf('State %d',state));
        end % for state
    end  % set break point here
    
    if passNorth % ~mod(numIterations,50)
        display(['Error level is ' num2str(errorLevel) ...
            ' and numExtremePoints is ' num2str(numExtremePoints)...
            ' and number of viable actions is (' sprintf('%d, ',cellfun(@(x) sum(x),viableActions(1:end-1))) sprintf('%d',sum(viableActions{end})) ...
            ') at iteration ' num2str(numIterations) ...
            ' and revolution ' num2str(numRevolutions) '.']);
    end % show error
    
    
    numIterations = numIterations + 1;
    
end % while

% Remove unused space in W
W(numPivots+1:end,:)=[];

fprintf(1,'Final error level is %e\n\n',errorLevel);

fprintf(1,'Total time rebuilding binding continuations = %1.2f\n',totalRebuildingTime);

end % abstwist

function tf = improves(currentDirection,bestDirection,newDirection)
% this function determines if newDirection "improves" on bestDirection,
% given that currentDirection is the current direction.  We are looking for
% newDirection to be clockwise to currentDirection and counter-clockwise to
% best direction.  Recall that if we have vectors x and y, then x is a
% clockwise turn to y if x lies in the negative halfspace of the ccw-normal
% to y.
    
    newNormal = [-newDirection(2) newDirection(1)];
    curNorm = 1.0;
    newNorm = 1.0;
    bestNorm = 1.0;

    global improveTol;

    tf = (dot(newNormal,currentDirection) > improveTol * newNorm * curNorm...
        || (dot(newNormal,currentDirection) > -improveTol * newNorm * curNorm...
        && dot(newDirection,currentDirection) > 0)) ...
        && (dot(newNormal,bestDirection) < -improveTol * newNorm * bestNorm);
end

function tf = improvesStrict(currentDirection,bestDirection,newDirection)
% this function determines if newDirection "improves" on bestDirection,
% given that currentDirection is the current direction.  We are looking for
% newDirection to be clockwise to currentDirection and counter-clockwise to
% best direction.  Recall that if we have vectors x and y, then x is a
% clockwise turn to y if x lies in the negative halfspace of the ccw-normal
% to y.
    
    newNormal = [-newDirection(2) newDirection(1)];
    curNorm = 1.0;
    newNorm = 1.0;
    bestNorm = 1.0;

    global improveTol;

    tf = (dot(newNormal,currentDirection) > improveTol * newNorm * curNorm ) ...
        && (dot(newNormal,bestDirection) < -improveTol * newNorm * bestNorm);
end
