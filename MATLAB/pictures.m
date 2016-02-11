% Code for generating pictures for draft of abreu brooks sannikov

%% resize picture
set(gcf,'position',[ 7.13333333333333          8.58888888888889          5.18888888888889          4.86666666666667]);
% set(gcf,'units','inches','position',[11.7667    6.8111    6.3778    6.1000]);
subplot(1,1,1)

%% Nash payoffs

cmode='E';
cmode='C';

x = importdata(sprintf('../../examples/rsg_nash_ne=9_c2e=25_d=0.85_cmode=%s.log',cmode));
y = importdata(sprintf('../../examples/rsg_lrp_ne=9_c2e=25_d=0.85_cmode=%s.log',cmode));

rows = 1:ceil(size(x,1)/6):size(x,1);

persistences = y(rows,1);
p = plot(linspace(0,1,9),x(rows,1:2:end)');
xlabel('Player 1''s endowment (e)');
ylabel('Player 1''s Nash payoff');
legendtitles = arrayfun(@(x) {['\rho=' num2str(x)]},persistences);
legend(p,legendtitles{:},'location','southeast');
box off


set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_nash_cmode=%s.pdf',cmode));

%% Long run payoffs chart
cmode='E';
cmode='C';
x = importdata(sprintf('../../examples/rsg_lrp_ne=9_c2e=25_d=0.85_cmode=%s.log',cmode));
subplot(1,1,1);
plot(x(:,1),sum(x(:,2:3)/2,2),'b-',...
	x(:,1),sum(x(:,4:5)/2,2),'r-')
xlabel('Persistence (\rho)');
ylabel('Average long run payoff');
legend('Efficient equilibrium','Autarky','location','northeast')
box off

set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_longrun_cmode=%s.pdf',cmode));

%% Correspondences
cmode='C';
P = [0 4 9 14];

% cmode='E';
% P = [0 5 10 20];

ne = 9; c2e = 25;
colors={'b','r','m',[0 0.8 0]};
handles = zeros(ne*numel(P),1);
for k=1:numel(P);
	p = P(k);
	sgmex('LoadSolution',...
		sprintf('../../examples/solutions/rsg_ne=%d_c2e=%d_d=0.85_p=%s_cmode=%s.sln',ne,c2e,num2str(p,3),cmode))
	
	sgmex('IterToBeginning');
	firstIter = sgmex('GetCurrentIteration');
	
	numStates = sgmex('GetNumStates');
	T=sgmex('GetTuples');
	allPoints = zeros(0,2);
	
	for s = 1:numStates
		statePoints=T(firstIter.numExtremeTuples:end,2*s+[-1,0]);
		handles((k-1)*ne+s) = ...
			plot(statePoints(:,1),statePoints(:,2),'-',...
			'linewidth',0.5,'color',colors{k});

		allPoints=[allPoints;statePoints];
		
		hold on;
	end
% 
% 	K=convhull(allPoints);
% 	K=[K(2:end);K(1:2)];
% 	[~,kmax]=max(allPoints(:,1));
% 	K=K(kmax:end);
% 	plot(allPoints(K,1),allPoints(K,2),'-',...
% 		'linewidth',1.5,'color',colors(k));
	
end % p

xlabel('Player 1''s payoff');
ylabel('Player 2''s payoff');
hold off;
box off
axis square

handlepositions = 1+(0:ne:((numel(P)-1)*ne));
legendtitles = arrayfun(@(x) {['\rho=' num2str(x)]},P);
legend(handles(handlepositions),legendtitles{:},'location','southwest')

set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_correspondences_cmode=%s.pdf',cmode));

%% Single correspondence

cmode='C';
p = 0;
delta=0.7;

ne = 5; c2e = 50;
colors={'b','r','m',[0 0.8 0]};
handles = zeros(ne,1);

fname=sprintf('../examples/solutions/rsg_ne=%d_c2e=%d_d=%s_p=%s_cmode=%s.sln',ne,c2e,num2str(delta),num2str(p,3),cmode);
sgmex('LoadSolution',fname)

sgmex('IterToBeginning');
firstIter = sgmex('GetCurrentIteration');

numStates = sgmex('GetNumStates');
T=sgmex('GetTuples');
allPoints = zeros(0,2);
autarkyPoints = zeros(0,2);
efficientPoints = zeros(0,2);
for s = 1:numStates
	statePoints = T(firstIter.numExtremeTuples:end,2*s+(-1:0));
	handles(s) = ...
		plot(statePoints(:,1),statePoints(:,2),'b-',...
		'linewidth',0.5);
	allPoints = [allPoints;statePoints];
	
	levels=statePoints*[1;1];
	[~,k]=max(levels);
	efficientPoints=[efficientPoints;statePoints(k,:)];
	[~,k]=min(levels);
	autarkyPoints=[autarkyPoints;statePoints(k,:)];
	
	textPoint = autarkyPoints(s,:)+[0.02,0.02];
	text(textPoint(1),textPoint(2),['$${\bf V}(' num2str((s-1)/(ne-1)) ')$$'],...
		'interpreter','latex',...
		'verticalalignment','bottom')

	hold on;
end

K=convhull(allPoints);
K=[K(2:end-1); K(1:2)];
[~,k]=max(allPoints(K,1));
X = allPoints(K(k:end),:);

plot(X(:,1),X(:,2),'b-','linewidth',1.5);
handles(end+1)=plot(autarkyPoints(:,1),autarkyPoints(:,2),'b.',...
	'markersize',13);
handles(end+1)=plot(efficientPoints(:,1),efficientPoints(:,2),'r*',...
	'markersize',8);
l = legend(handles(end-2:end),'$${\bf V}$$','$$\underline{\bf v}$$','$${\bf v}^*$$');
set(l,'interpreter','latex')

xlabel('Player 1''s payoff');
ylabel('Player 2''s payoff');
hold off;
box off
axis square

% handlepositions = 1+(0:ne:((numel(P)-1)*ne));
% legendtitles = arrayfun(@(x) {['\rho=' num2str(x)]},P);
% legend(handles(handlepositions),legendtitles{:},'location','southwest')

set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_singlecorrespondence_cmode=%s.pdf',cmode));

%% Trajectory

cmode='C';
p = 0;
delta=0.7;
set(gcf,'units','inches','position',[7.12222222222222          10.3888888888889                       9.8          3.07777777777778]);

ne = 5; c2e = 50;
colors={'b','r','m',[0 0.8 0]};
handles = zeros(ne,1);

fname=sprintf('../examples/solutions/rsg_ne=%d_c2e=%d_d=%s_p=%s_cmode=%s.sln',ne,c2e,num2str(delta),num2str(p,3),cmode);
sgmex('LoadSolution',fname)

sgmex('IterToBeginning');
firstIter = sgmex('GetCurrentIteration');

numStates = sgmex('GetNumStates');
T=sgmex('GetTuples');
T=T(:,1:2);

lastTuple = 3e4;

xlim1 = [-0.05,1.05]; ylim1=xlim1;
d2=0.15; p2 = [0.35,0.75]; xlim2=p2(1)+[0 d2]; ylim2=p2(2)+[0 d2];
d3=0.02; p3 = [0.42,0.8215]; xlim3=p3(1)+[0 d3]; ylim3=p3(2)+[0 d3];

J=@(x,y) [x(1) y(1);...
	x(2) y(1);...
	x(2) y(2);...
	x(1) y(2);...
	x(1) y(1)];

box2=J(xlim2,ylim2);
box3=J(xlim3,ylim3);

subplot(1,3,1);
plot(T(1:lastTuple,1),T(1:lastTuple,2),'b-',...
	0,1,'b.');
set(gca,'xlim',xlim1,'ylim',ylim1);
xlabel('Player 1''s payoff');
ylabel('Player 2''s payoff');
box off
axis square

subplot(1,3,2);
plot(T(1:lastTuple,1),T(1:lastTuple,2),'b-',...
	0,1,'b.');
set(gca,'xlim',xlim2,'ylim',ylim2);
xlabel('Player 1''s payoff');
box off
axis square

subplot(1,3,3);
plot(T(1:lastTuple,1),T(1:lastTuple,2),'b-',...
	0,1,'b.');
set(gca,'xlim',xlim3,'ylim',ylim3);
xlabel('Player 1''s payoff');
box off
axis square


% handlepositions = 1+(0:ne:((numel(P)-1)*ne));
% legendtitles = arrayfun(@(x) {['\rho=' num2str(x)]},P);
% legend(handles(handlepositions),legendtitles{:},'location','southwest')

set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_trajectory_cmode=%s.pdf',cmode));

%% Densities

f=@(e,c,rho) rho*exp(-rho*abs(e-c))./(2-exp(-rho*c)-exp(-rho*(1-c)));
E=linspace(0,1,200)';
K=5;
C=linspace(0,1,K);
R=linspace(8,0.0001,K);
kmax=ceil(2*nk/3);
D=zeros(numel(E),K);
legendStrings=cell(k,1);
for k=1:K
	D(:,k)=f(E,C(k),R(k));
	legendStrings{k}=['$$(c,\rho)=(' num2str(C(k)) ',' num2str(R(k),1) ')$$'];
end
h=plot(E,D);
l=legend(h,legendStrings{:});
set(l,'interpreter','latex');
hold off;
box off;

set(gcf,'paperunits','inches','units','inches');
fpos = get(gcf,'position');
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf',sprintf('~/Dropbox/abreubrookssannikov/figures/rsg_densities.pdf',cmode));

%%
