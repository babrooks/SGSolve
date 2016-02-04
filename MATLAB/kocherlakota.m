function [P,LR,ATK] = kocherlakota
%% Ben's file for kocherlakota style simulations
% 8-19-2015

numEndowments = 5; % Should be odd?
consumptionToEndowments = 25;
% P = linspace(0,10,numEndowments);
% P=0:20;
 P=0:0.25:12.75;
% P=[13 13.25 13.5 13.75 14];
LR = zeros(numel(P),2);
ATK=LR;
delta=0.9;
numSimulations = 1e8;

tic;
for k=1:numel(P)
    persistence=P(k);

    fn = fileName(numEndowments,consumptionToEndowments,delta,persistence);
	
	try
		[p1payoffs,p2payoffs,transitions]=generateGame(numEndowments,...
			consumptionToEndowments,persistence);
		
		fprintf('Iteration %d of %d. Persistence parameter is %1.2f. Loading solution...\n',...
			k,numel(P),P(k));

		tic;
		if exist(['./solutions/' fn '.sln'],'file')
			sgmex('LoadSolution',['./solutions/' fn '.sln']);
        else

            sgmex('SaveGame',delta,p1payoffs,p2payoffs,transitions,['./games/' fn '.sgm']);
            
            tic;
            sgmex('Solve',delta,p1payoffs,p2payoffs,transitions,'errorTol',1e-7);
            toc
            
            sgmex('SaveSolution',['./solutions/' fn '.sln']);
		end
		toc
		
		fprintf('Solution loaded. Calculating autarkic equilibrium...\n');
		% First compute the autarkic equilibrium.
		Pi = zeros(numEndowments);
		G = zeros(numEndowments,2);
		for s=1:ceil(numel(transitions)/2)
			Pi(s,:) = transitions{s}(1,:);
			G(s,:) = [p1payoffs{s}(1) p2payoffs{s}(1)];
		end
		
		for s=ceil(size(transitions)/2)+1:numel(transitions)
			Pi(s,:) = transitions{s}(end,:);
			G(s,:) = [p1payoffs{s}(end) p2payoffs{s}(end)];
		end
		
		V = mldivide(eye(numEndowments)-delta*Pi,(1-delta)*G);
		Y = (Pi^1000);
		mu = Y(1,:);
		ATK(k,:)=mu*V;
		
		fprintf('Autarky computation done. Autarky payoffs are: (%1.3f,%1.3f). Finding the iteration that maximizes utilitarian welfare...\n',...
			ATK(k,1),ATK(k,2));
		sgmex('IterToEnd');
		lastIter=sgmex('GetCurrentIteration');
        startOfLastRev = lastIter;
        utilitarianIter = lastIter;
        maxSoFar = sum(sum(startOfLastRev.pivot));
		utilitarianPayoffs=[];
        while startOfLastRev.revolution == lastIter.revolution
            sgmex('Iter--');
            startOfLastRev = sgmex('GetCurrentIteration');
            if sum(sum(startOfLastRev.pivot))>maxSoFar
                utilitarianIter = startOfLastRev;
				utilitarianPayoffs = sum(startOfLastRev.pivot);
                maxSoFar=sum(sum(startOfLastRev.pivot));
            end
        end
        sgmex('Iter++');
        startOfLastRev = sgmex('GetCurrentIteration');
        
		
		fprintf('Utilitarian welfare is maximized at iteration %d. Simulating for %d periods...\n',...
			utilitarianIter.iteration,numSimulations);
        tic;
		numTrials = 1;
		for t=1:numTrials
			[actionDistr,tupleDistr,lr]=sgmex('Simulate',numSimulations,...
				ceil(numEndowments/2),utilitarianIter.iteration);
			LR(k,:)=LR(k,:)+lr/numTrials;
		end
		
		fprintf('Simulation completed. Long run payoffs: (%1.3f,%1.3f)\n',...
			LR(k,1),LR(k,2));
        toc
		fprintf('\n');
	catch me
        display('I caught an exception!');
		display(me);
		
		LR(k,:)=ATK(k,:);
	end % catch
	
end % for
toc

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