
%% n state kocherlakota style risk sharing example
delete(findobj('parent',0));

delta = 0.6;

numEndowments = 25; numTransfers = 50;

numStates = numEndowments;

G1 = cell(1,numStates);
G2 = cell(1,numStates);
P = cell(1,numStates);

E = linspace(0,1,numEndowments);
T = linspace(0,1,numTransfers); % possible transfers

state = 1;
for e1 = E
	e2 = 1-e1;
	if e1==e2
		P{state} = ones(1,numStates)/numStates;
		G1{state} = sqrt(e1);
		G2{state} = sqrt(e2);
	else
		P{state} = ones(numTransfers,numStates)/numStates;
		if e1 > e2
			G1{state} = sqrt(e1 + (e2-e1)*T);
			G2{state} = sqrt(e2 + (e1-e2)*T);
		elseif e2>e1
			G1{state} = sqrt(e1 + (e2-e1)*T');
			G2{state} = sqrt(e2 + (e1-e2)*T');
		end
	end
	state = state + 1;
	
% 	for e2 = E
% 		if e1==e2
% 			P{state} = ones(1,numStates)/numStates;
% 			G1{state} = sqrt(e1);
% 			G2{state} = sqrt(e2);
% 		else
% 			P{state} = ones(numTransfers,numStates)/numStates;
% 			if e1 > e2
% 				G1{state} = sqrt(e1 + (e2-e1)*T);
% 				G2{state} = sqrt(e2 + (e1-e2)*T);
% 			elseif e2>e1
% 				G1{state} = sqrt(e1 + (e2-e1)*T');
% 				G2{state} = sqrt(e2 + (e1-e2)*T');
% 			end
% 		end
% 		state = state + 1;
% 	end % e2

end % e1

tic;
sgmex('Solve',delta,G1,G2,P,'unconstrained',false(1,2));
toc

% sgviewer();

%% 





