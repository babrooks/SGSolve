function kocherlakota_8_19_2015
%% Ben's file for kocherlakota style simulations
% 8-19-2015

numEndowments = 11;
consumptionToEndowments = 5;
P = 0:10;
LR = zeros(numel(P),2);
delta=0.9;

for k=1:numel(P)
    persistence=P(k);

    fn = fileName(numEndowments,consumptionToEndowments,delta,persistence);

    try
        if exist(['./solutions/' fn '.sln'],'file')
            sgmex('LoadSolution',['./solutions/' fn '.sln']);
        else
            [p1payoffs,p2payoffs,transitions]=generateGame(numEndowments,...
                consumptionToEndowments,persistence);
            
            
            sgmex('SaveGame',delta,p1payoffs,p2payoffs,transitions,['./games/' fn '.sgm']);
            
            tic;
            sgmex('Solve',delta,p1payoffs,p2payoffs,transitions,'errorTol',1e-5);
            toc
            
            sgmex('SaveSolution',['./solutions/' fn '.sln']);
        end
        
        sgmex('IterToEnd');
        lastIter=sgmex('GetCurrentIteration');
        startOfLastRev = lastIter;
        utilitarianIter = lastIter;
        maxSoFar = sum(sum(startOfLastRev.pivot));
        while startOfLastRev.revolution == lastIter.revolution
            sgmex('Iter--');
            startOfLastRev = sgmex('GetCurrentIteration');
            if sum(sum(startOfLastRev.pivot))>maxSoFar
                utilitarianIter = startOfLastRev;
                maxSoFar=sum(sum(startOfLastRev.pivot));
            end
        end
        sgmex('Iter++');
        startOfLastRev = sgmex('GetCurrentIteration');
        
        tic;
        [actionDistr,tupleDistr,lr]=sgmex('Simulate',1e6,0,utilitarianIter.iteration);
        toc
        LR(k,:)=lr;
    catch
        display('I caught an exception!');
    end % catch
end % for

keyboard

end % kocherlakota_8-10-2015

function fn = fileName(numEndowments,consumptionToEndowments,delta,persistence) 
fn = sprintf('koch_ne=%d_c2e=%d_d=%1.2f_p=%1.2f',...
    numEndowments,consumptionToEndowments,delta,persistence);
end % filename

function [p1payoffs,p2payoffs,transitions]...
    = generateGame(numEndowments,endowmentsToConsumption,persistence)
assert(mod(numEndowments,2)==1);

midpointE = (numEndowments-1)/2;

E = linspace(0,1,numEndowments);

p1payoffs = cell(numEndowments,1);
p2payoffs = p1payoffs;
transitions = p1payoffs;

for state=1:numel(E)
    % For each state, player with higher endowment can give an amount
    % between e and e/2 to the other player, i.e., can shift the
    % distribution of income towards more equality.
    e = E(state);
    
    % action space of player with higher endowment
    C = linspace(min(e,1/2),max(1/2,e),...
        abs(midpointE-state+1)*endowmentsToConsumption+1);
    
    % flip orientation if endowment>1/2
    if e > 1/2
        C=C';
    end
    
    p1payoffs{state} = sqrt(C); 
    p2payoffs{state} = sqrt(1-C);
    
    transitions{state} = zeros(numel(C),numEndowments);
    for action=1:numel(C)
        % Want transition to be proportional to an exponential
        % distribution.
        c=C(action);
        
        if persistence>0
            F = @(x) 1-exp(-persistence*max(0,x));
        else
            F = @(x) x;
        end
        
        increment = E(2)-E(1);
        probs = F(E-c+increment)-F(E-c-increment)...
            +F(-E+c+increment)-F(-E+c-increment);
        transitions{state}(action,:) = probs/sum(probs);
    end % for action
    

end % for state
    
end % generateGame