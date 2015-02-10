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
%% The two state example computed by Moshe to 2e-52
W2 = [23.5000000000000000, -1.1250000000000000;...
19.1250000000000000, 5.8750000000000000;...
8.6250000000000000, 9.1057692307692300;...
4.1250000000000000, 0.6383136094674557;...
4.1250000000000000, -3.1250000000000000;...
15.1875000000000000, -3.1250000000000000];

W2 = [W2,[17.2500000000000000, 5.1250000000000000;...
12.8749999999999980, 12.1250000000000000;...
2.3750000000000000, 15.3557692307692300;...
-2.1250000000000000, 6.8883136094674540;...
-2.1250000000000000, 3.1250000000000000;...
8.9375000000000000, 3.1250000000000000]];

W2=W2([1:end 1],:);
subplot(1,2,1); hold on; p1 = plot(W2(:,1),W2(:,2),'r'); hold off; 
subplot(1,2,2); hold on; p2=plot(W2(:,3),W2(:,4),'r'); hold off
% set([p1 p2],'linewidth',1);

%% Moshe's computation for kocherlakota example
% delta = 0.75
W2 = [8.55614973262032        3.529411764705882;...
    8.101604278074866       5.347593582887701;...
    7.0     7.0;...
    6.470588235294118       7.352941176470589;...
    6.470588235294118       3.529411764705882];
W2 = [W2 [7.352941176470589       6.470588235294118;...
    7.0     7.0;...
    5.347593582887701       8.101604278074866;...
    3.529411764705882       8.55614973262032;...
    3.529411764705883       6.470588235294118]];

W2=W2([1:end 1],:);
subplot(1,2,1); hold on; p1 = plot(W2(:,1),W2(:,2),'r'); hold off; 
subplot(1,2,2); hold on; p2=plot(W2(:,3),W2(:,4),'r'); hold off

%% Moshe's computation for kocherlakota example
% delta = 0.6
W2 = [8.295454545454547       2.7272727272727257;...
7.636363636363637       5.363636363636363;...
7.27272727272727        5.454545454545455;...
7.272727272727273       2.727272727272727];

W2 = [W2 [5.454545454545455       7.27272727272727;...
5.363636363636363       7.636363636363637;...
2.7272727272727257      8.295454545454547;...
2.727272727272727       7.272727272727273]];

W2=W2([1:end 1],:);
subplot(1,2,1); hold on; p1 = plot(W2(:,1),W2(:,2),'r'); hold off; 
subplot(1,2,2); hold on; p2=plot(W2(:,3),W2(:,4),'r'); hold off

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
