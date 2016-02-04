%% Kocherlakota style risk sharing example.
G = [0 10; 4 9; 7 7; 9 4; 10 0];
G = {G G};

persistence = 0.5;
P = [persistence*ones(5,1) (1-persistence)*ones(5,1)];
P = {P,fliplr(P)};

numActions = [5 1;1 5];
delta = 0.6;

tic;
W = abstwist(G,P,numActions,delta);
toc

for state = 1:2
    subplot(1,2,state); 
%    hold on; 
    plot(W(:,2*(state-1)+1),W(:,2*(state-1)+2),'b-'); 
    set(gca,'xlim',[0 10],'ylim',[0 10]);
    hold off;
    axis square; 
end
display(size(W));
return;


%% Two state ABS twist example

G1 = [16 3 1; 21 10 -1; 9 5 -5];
G2 = [9 13 3; 1 4 0; 0 -4 -10];

G = [reshape(G1,[],1) reshape(G2,[],1)];

G = {bsxfun(@plus,G,5*[1,-1]),bsxfun(@plus,G,5*[-1,1])};

% P = {[0.7*ones(9,1) 0.3*ones(9,1)],[0.3*ones(9,1) 0.7*ones(9,1)]};
persistence = 0.6;
P = {[persistence*ones(9,1) (1-persistence)*ones(9,1)],[(1-persistence)*ones(9,1) persistence*ones(9,1)]};

% transitions = rand(9,2).^1.5; transitions = bsxfun(@times,transitions,1./sum(transitions,2));
% P = {transitions, fliplr(transitions)};

numActions = 3*ones(2,2);

delta=0.85;

tic;
W = abstwist(G,P,numActions,delta);
toc

for state = 1:2
    subplot(1,2,state); 
%     hold on; 
    plot(W(:,2*(state-1)+1),W(:,2*(state-1)+2),'b--'); 
    hold off;
    axis square; set(gca,'xlim',[-15 30],'ylim',[-20,20]);
end
display(size(W));
return;

%% Compute feasible set for previous example
% Run previous cell first!
% transitions = rand(9,2).^1.5; transitions = bsxfun(@times,transitions,1./sum(transitions,2));
% P = {transitions, fliplr(transitions)};

tic;
W = absfeasible(G,P,numActions,delta);
toc

numStates = numel(G);
for state=1:numel(G)
    subplot(1,2,state); plot(W(:,(state-1)*2+1),W(:,(state-1)*2+2),'m'); axis equal; 
end
return;

%% One state ABS twist example.

G1 = [16 3 1; 21 10 -1; 9 5 -5];
G2 = [9 13 3; 1 4 0; 0 -4 -10];

G = [reshape(G1,[],1) reshape(G2,[],1)];

G = {G};
P = {ones(9,1)};
numActions = 3*ones(1,2);

delta = 0.6;

tic;
W = abstwist(G,P,numActions,delta);
toc

subplot(1,2,1); plot(W(:,1),W(:,2)); axis equal

%% Plot up to a certain point in the sequence. For diagnostic purposes
numStates = 2;
plotFrom = 1;
plotUntil =252;

for state=1:numStates
    subplot(1,numStates,state);
%     xlim = get(gca,'xlim'); ylim = get(gca,'ylim');
    plot(W(plotFrom:plotUntil,2*(state-1)+1),W(plotFrom:plotUntil,2*(state-1)+2),'b-',...
        W(plotUntil,2*(state-1)+1),W(plotUntil,2*(state-1)+2),'r.'); axis equal
%     set(gca,'xlim',xlim); set(gca,'ylim',ylim);
end % for state

%% Resize main figure for printing
set(gcf,'units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
