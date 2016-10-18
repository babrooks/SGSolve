%%

H=[-1, 0, 0, 3.75;...
	0, -1, 0, 3.75;...
	0, 0, -1, 3.75;...
	0.70710678118654746,0.70710678118654746, 0, -28.284271247461898;...
	0.70710678118654746, -0,0.70710678118654746, -28.284271247461898;...
	0, 0.70710678118654746,0.70710678118654746, -28.284271247461898;...
	0.12309149097933272,0.12309149097933272, -0.98473192783466179, 1.2309149097933272;...
	0.40824829046386302,0.40824829046386302, -0.81649658092772603, -8.164965809277259;...
	-0.70710678118654757, -0,-0.70710678118654757, 7.0710678118654755;...
	-0, -0.70710678118654757,-0.70710678118654757, 7.0710678118654755;...
	0.12309149097933272,-0.98473192783466179, 0.12309149097933272, 1.2309149097933272;...
	-0.98473192783466179,0.12309149097933272, 0.12309149097933272, 1.2309149097933272;...
	0.40824829046386302,-0.81649658092772603, 0.40824829046386302, -8.164965809277259;...
	-0.70710678118654746,-0.70710678118654746, -0, 7.0710678118654746;...
	-0.81649658092772603,0.40824829046386302, 0.40824829046386302, -8.164965809277259];
H2=[H(:,[1 3]),-H(:,2)*3.75-H(:,4)];

[x,y] = meshgrid(linspace(2,14,100),linspace(2,14,100));
x= reshape(x,1,[]); y= reshape(y,1,[]);

tf = H2(:,1:2)*[x;y]<=H2(:,3*ones(numel(x),1));
tf = all(tf,1);

plot(x(tf)',y(tf)','r.');

%% Load data for AS program
% x=importdata('threeplayer2_0.5.dat');
% x=importdata('threeplayer2_largeextpnts.dat');
x = importdata('threeplayer2_fouraction.dat');
numActions = x(2,1:3);
delta=x(1,2);
numExtPnts = x(1,3);
nat = x(1,1);

row=3;
G=x(row:row+nat-1,1:3);
row=row+nat;
gains=x(row:row+nat-1,1:3);
row=row+nat;
extPnts = x(row:row+numExtPnts-1,1:3);
extPntIndex = x(row:row+numExtPnts-1,4);
row=row+numExtPnts;
numRaw = (size(x,1)-row+1);
rawPnts = x(row:row+numRaw-1,1:3);
rawActs = x(row:row+numRaw-1,4);
rawConstrs = x(row:row+numRaw-1,5);

payoffs = rawPnts(extPntIndex+1,:);

payoffs = unique(payoffs,'rows');
% payoffs2=payoffs;
size(payoffs,1);
faces=convhulln(payoffs);

threats=min(extPnts);

% [~,uniquePoints] = unique(optPayoffs,'rows');
% vFaces = convhulln(optPayoffs(uniquePoints,:));
% [~,hullPoints]= unique(vFaces);
% size(hullPoints)

%%
a=4;

[az,el] = view;

scatter3(payoffs(:,1),payoffs(:,2),payoffs(:,3),'b','filled');
hold on
for f=1:size(faces,1)
    p = patch(payoffs(faces(f,:),1),payoffs(faces(f,:),2),...
        payoffs(faces(f,:),3),0.75*ones(1,3));
    set(p,'facealpha',0.75);
end % for face

actionPnts = rawActs==(a-1);
minCV = threats+(1-delta)/delta*gains(a,:);
g=G(a,:);
scatter3(minCV(1),minCV(2),minCV(3),'k','filled');
scatter3(G(a,1),G(a,2),G(a,3),'g','filled');
cv = bsxfun(@plus,rawPnts(actionPnts,:),-(1-delta)*G(a,:))./delta;
scatter3(cv(:,1),cv(:,2),cv(:,3),'r','filled');
for k=1:size(cv,1)
    genline=[G(a,:);cv(k,:)];
    line(genline(:,1),genline(:,2),genline(:,3));
end
pnts = rawPnts(rawActs==(a-1),:);
scatter3(pnts(:,1),pnts(:,2),pnts(:,3),'y','filled');

maxPayoff = 2;
side1 = [minCV;...
    minCV+[maxPayoff - minCV(1),0,0];...
    minCV+[maxPayoff-minCV(1),maxPayoff-minCV(2),0];...
    minCV+[0,maxPayoff-minCV(2),0];...
    minCV];
side2 = [minCV;...
    minCV+[0,maxPayoff - minCV(2),0];...
    minCV+[0,maxPayoff-minCV(2),maxPayoff-minCV(3)];...
    minCV+[0,0,maxPayoff-minCV(3)];...
    minCV];
side3 = [minCV;...
    minCV+[maxPayoff - minCV(1),0,0];...
    minCV+[maxPayoff-minCV(1),0,maxPayoff-minCV(3)];...
    minCV+[0,0,maxPayoff-minCV(3)];...
    minCV];

p = patch(side1(:,1),side1(:,2),side1(:,3),0.75*ones(1,size(side1,1)));
set(p,'facealpha',0.75);
p = patch(side2(:,1),side2(:,2),side2(:,3),0.75*ones(1,size(side1,1)));
set(p,'facealpha',0.75);
p = patch(side3(:,1),side3(:,2),side3(:,3),0.75*ones(1,size(side1,1)));
set(p,'facealpha',0.75);

display(G(a,:))
display(cv)
display(minCV)

view(az,el);

hold off

%% Load data 
x = importdata('threeplayer_1920.dat');
numActions = x(2,:);
delta = x(1,end);
numGradients = x(1,2);
numActions_total = x(1,1);

row = 3;
G = x(row:row+numActions_total-1,:);
row = row + numActions_total;

gains = x(row:row + numActions_total-1,:);
row = row + numActions_total;

gradients = x(row:row+numGradients-1,:);
row = row + numGradients;

feasPayoffs = x(row:row+numGradients-1,:);
row = row + numGradients;

optPayoffs = x(row:row+numGradients-1,:);
row = row + numGradients;

optData = x(row:row+numGradients-1,:);

threats = min(feasPayoffs);


%% Plot gradients
scatter3(gradients(:,1),gradients(:,2),gradients(:,3));
faces=convhulln(gradients);
hold on
for f=1:size(faces,1)
    p = patch(gradients(faces(f,:),1),gradients(faces(f,:),2),...
        gradients(faces(f,:),3),0.75*ones(1,3));
    set(p,'facealpha',0.75);
end % for face
hold off

%% Plot payoffs
% k=31;

% scale = 1e10;
% payoffs2 = round(scale*vertices)/scale;
x=importdata('threeplayer2_0.8.dat');
payoffs=x(57:end,:);
payoffs = unique(payoffs,'rows');
% payoffs2=payoffs;
size(payoffs,1);
faces=convhulln(payoffs);

% [~,uniquePoints] = unique(optPayoffs,'rows');
% vFaces = convhulln(optPayoffs(uniquePoints,:));
% [~,hullPoints]= unique(vFaces);
% size(hullPoints)

[az,el] = view;

scatter3(payoffs(:,1),payoffs(:,2),payoffs(:,3));
hold on
for f=1:size(faces,1)
    p = patch(payoffs(faces(f,:),1),payoffs(faces(f,:),2),...
        payoffs(faces(f,:),3),0.75*ones(1,3));
    set(p,'facealpha',0.75);
end % for face

% 
% v=uniquePoints(vFaces(hullPoints(k)));
% a=optData(v,1)+1;
% minCV = threats+(1-delta)/delta*gains(a,:)
% scatter3(minCV(1),minCV(2),minCV(3));
% scatter3(G(a,1),G(a,2),G(a,3));
% scatter3(optPayoffs(v,1),optPayoffs(v,2),optPayoffs(v,3));
% cv = (optPayoffs(v,:)-(1-delta)*G(a,:))/delta
% scatter3(cv(1),cv(2),cv(3));
% genline=[G(a,:);cv];
% line(genline(:,1),genline(:,2),genline(:,3));
% 
% 
% side1 = [minCV;...
%     minCV+[30 - minCV(1),0,0];...
%     minCV+[30-minCV(1),30-minCV(2),0];...
%     minCV+[0,30-minCV(2),0];...
%     minCV];
% side2 = [minCV;...
%     minCV+[0,30 - minCV(2),0];...
%     minCV+[0,30-minCV(2),30-minCV(3)];...
%     minCV+[0,0,30-minCV(3)];...
%     minCV];
% side3 = [minCV;...
%     minCV+[30 - minCV(1),0,0];...
%     minCV+[30-minCV(1),0,30-minCV(3)];...
%     minCV+[0,0,30-minCV(3)];...
%     minCV];
% 
% grad = [optPayoffs(v,:);...
%     optPayoffs(v,:)+gradients(v,:)];
% line(grad(:,1),grad(:,2),grad(:,3));
% 
% p = patch(side1(:,1),side1(:,2),side1(:,3),0.75*ones(1,size(side1,1)));
% set(p,'facealpha',0.75);
% p = patch(side2(:,1),side2(:,2),side2(:,3),0.75*ones(1,size(side1,1)));
% set(p,'facealpha',0.75);
% p = patch(side3(:,1),side3(:,2),side3(:,3),0.75*ones(1,size(side1,1)));
% set(p,'facealpha',0.75);
% 
% display(G(a,:))
% display(cv)
% display(minCV)
% display((1-delta)*G(a,:)+delta*cv)
% 
% view(az,el);

hold off

%% Exact set
V=[20,20,20;...
    30,10,10;...
    10,30,10;...
    10,10,30;...
    

%% Plots the final JYC set


x = importdata('./logs/abs_jyc.log');

dirs = x(1:2,:)';
bounds = x(3:end,:)';

numStates = size(bounds,2);
rows = ceil(sqrt(numStates));
cols = ceil(numStates/rows);

for state=1:size(bounds,2)
	Y = zeros(size(dirs));
	for k = 1:size(dirs,1)
		indices = [k;mod(k,size(dirs,1))+1];

		d = dirs(indices,:);
		
		b = bounds(indices,state);
		
		% Want to solve the equation dx=b
		Y(k,:)=(d\b)';
	end
	
	subplot(rows,cols,state);
	plot(Y(:,1),Y(:,2),'b.-');
	title(sprintf('State %d',state-1));
	axis square
end