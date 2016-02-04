%% More interesting version of risk sharing. 
% Higher consumption today increases likelihood of high consumption
% tomorrow.



%% Two state ABS twist example, with c++ and sgmex

delete(findobj('parent',0));

G1 = [16 3 1; 21 10 -1; 9 5 -5];
G2 = [9 13 3; 1 4 0; 0 -4 -10];

G1 = {G1+5,G1-5};
G2 = {G2-5,G2+5};

% P = {[0.7*ones(9,1) 0.3*ones(9,1)],[0.3*ones(9,1) 0.7*ones(9,1)]};
persistence = 0.299;
P = {[persistence*ones(9,1) (1-persistence)*ones(9,1)],[(1-persistence)*ones(9,1) persistence*ones(9,1)]};

% transitions = rand(9,2).^1.5; transitions = bsxfun(@times,transitions,1./sum(transitions,2));
% P = {transitions, fliplr(transitions)};

numActions = 3*ones(2,2);

delta=0.7;

sgmex('SaveGame',delta,G1,G2,P,'sgmextest.sgm');

% tic;
% sgmex('Solve',delta,G1,G2,P,...
% 	'errorTol',1e-7,...
% 	'unconstrained',false(1,2));
% toc
% 
% sgmex('Save',sprintf('twostate_delta=%1.2f.dat',delta));
% 
% % sgviewer();

%% 2 state Kocherlakota style risk sharing example, with sgmex
G1 = [10; 9; 7; 4; 0];
G2 = [0; 4; 7; 9; 10];

persistence = 0.5;
P = [persistence*ones(5,1) (1-persistence)*ones(5,1)];
P = {P,fliplr(P)};

delta=0.7;

sgmex('Solve',delta,{G1; G2'},{G2,G1'},P,'errorTol',1e-10,'unconstrained',true(1,2));

sgviewer();

%% n state kocherlakota style risk sharing example
delete(findobj('parent',0));

% This took 1730 seconds with 50 transfers, 25 endowments, and delta=0.8.
% About 60k iterations, 50 revolutions, error tolerance of 1e-8

delta = 0.8;

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
	
end % e1

tic;
sgmex('Solve',delta,G1,G2,P,'unconstrained',false(1,2));
toc

sgviewer();

%% Positively correlated endowments with risk sharing
% Independent distribution over size of the pie

% Each share is determined "normally" around the previous shares.
% Endowments are between 0 and 1. Just do binomial on a grid on [0,2] with
% expectation 1, shift it to be between [c-1,c+1], and truncate that
% distribution at [0,1].

delta = 0.6;

numEndowments = 9; % Should be odd
numTransfers = 50;

numStates = numEndowments;

G1 = cell(1,numStates);
G2 = cell(1,numStates);
P = cell(1,numStates);

E = linspace(0,1,numEndowments+1);
T = linspace(0,1,numTransfers); % possible transfers

rho = 1; % between 0 and 1. The lower is rho, the closer to a uniform.

X = geopdf(0:(numEndowments),rho/numEndowments);
Y = [fliplr(X) X(2:end)];

state = 1;
for e1 = E
	e2 = 1-e1;
	if e1==e2
		I = floor(numEndowments/2)+(0:numEndowments-1);
		P{state} = Y(I)/sum(Y(I));
		G1{state} = sqrt(e1);
		G2{state} = sqrt(e2);
	else
		if e1 > e2
			G1{state} = sqrt(e1 + (e2-e1)*T);
			G2{state} = sqrt(e2 + (e1-e2)*T);
		elseif e2>e1
			G1{state} = sqrt(e1 + (e2-e1)*T');
			G2{state} = sqrt(e2 + (e1-e2)*T');
		end

		P{state} = zeros(numel(T),numel(E));
		for t = 1:numel(T)
			[~,closest] = min(abs(e1+(e2-e1)*T(t)-E));
			I = (numEndowments-closest+2):(2*numEndowments-closest+2);
			P{state}(t,:) = Y(I)/sum(Y(I));
		end
	end
	state = state + 1;
	
end % e1

tic;
sgmex('Solve',delta,G1,G2,P,'unconstrained',false(1,2));
toc

sgviewer();


%% Duopoly with capacity constraints
delete(findobj('parent',0));

% Linear demand P = A - B * Q
A = 1.5;
B = 1;

delta = 0.35;

% Marginal cost of production c>0
c = 0.1;

% Capacity: Cannot produce more than K. K is at least 1. Can invest I
% between 0 and the maximum K. With positive investment, capital increases
% by I. Otherwise, decreases by 1 with probability pD. Price of investment
% is rI^2.
pD = 0.5;
r = 0.25;

nK = 2;
QtoK = 10;
nQ = QtoK*nK;
K = QtoK*(0:nK);
Q = 0:nQ;

numStates = (nK+1)^2;
G1 = cell(numStates,1);
G2 = cell(numStates,1);
P = cell(numStates,1);

state = 1;
for k2 = K
	for k1 = K
		nA1 = k1*((nQ-k1)/QtoK+1);
		nA2 = k2*((nQ-k2)/QtoK+1);
		
		G1{state} = zeros(nA1,nA2);
		G2{state} = zeros(nA1,nA2);
		P{state} = zeros(nA1*nA2,numStates);
		
		action = 1;
		a2 = 1;
		
		for q2 = 0:k2
			for i2 = K(K>=k2)-k2
				a1 = 1;
				for q1 = 0:k1
					for i1 = K(K>=k1)-k1
						p = max(0,A-B*(q1+q2)/nQ);
						
						G1{state}(a1,a2) = q1/nQ*(p-c) - r*(i1/nQ)^2;
						G2{state}(a1,a2) = q2/nQ*(p-c) - r*(i2/nQ)^2;
						
						%
% 						if i1 == 0 && k1 == 0
% 							K1distr = [1 zeros(1,nK)];
% 						elseif i1 == 0 && k1 > 0
% 							K1distr = [zeros(1,k1/QtoK-1) pD (1-pD) zeros(1,(nQ-k1)/QtoK)];
% 						else
% 							K1distr = [zeros(1,(k1+i1)/QtoK) 1 zeros(1,(nQ-k1-i1)/QtoK)];
% 						end
% 						
% 						if i2 == 0 && k2 == 0
% 							K2distr = [1 zeros(1,nK)];
% 						elseif i2 == 0 && k2 > 0
% 							K2distr = [zeros(1,k2/QtoK-1) pD (1-pD) zeros(1,(nQ-k2)/QtoK)];
% 						else
% 							K2distr = [zeros(1,(k2+i2)/QtoK) 1 zeros(1,(nQ-k2-i2)/QtoK)];
% 						end
% 						
% 						P{state}(action,:) = reshape(bsxfun(@times,K1distr,K2distr'),1,[]);

						statep = 1;
						for k2p = K
							for k1p = K
								prob = 1;

								if i1==0 && k1==0
									if k1p ~= 0
										prob = 0;
									end
								elseif i1 == 0 && k1 > 0
									if k1p == k1-QtoK
										prob = prob * pD;
									elseif k1p == k1
										prob = prob * (1-pD);
									else
										prob = 0;
									end
								else
									if k1p ~= k1 + i1
										prob = 0;
									end
								end
								
								if i2==0 && k2==0
									if k2p ~= 0
										prob = 0;
									end
								elseif i2 == 0 && k2 > 0
									if k2p == k2-QtoK
										prob = prob * pD;
									elseif k2p == k2
										prob = prob * (1-pD);
									else
										prob = 0;
									end
								else
									if k2p ~= k2 + i2
										prob = 0;
									end
								end
								
								P{state}(action,statep) = prob;
								
								statep = statep+1;
							end
						end
						
% 						P{state}(action,:) = ones(1,numStates)/numStates;
						a1 = a1 + 1;
						
						action = action + 1;
					end % i1
				end % q1
				a2 = a2 + 1;
				
			end % i2
		end % q2
		
		state = state + 1;
	end % k1
end % k2

tic;
% sgmex('Solve',delta,G1,G2,P,'unconstrained',false(1,2),'errorTol',1e-6);
toc

sgmex('Save',sprintf('cournot_delta=%1.2f.dat',delta));

sgviewer();









