%% Make pictures of risk sharing games
sgmex2('LoadSolution','../examples/solutions/risksharing_nume=2_c2e=200_delta=0.7.sln2');
sgmex2('IterToEnd');

iter = sgmex2('GetCurrentIteration');

%% Pictures with high delta
C=linspace(0,1,200*2+1);
C=[C 0];
plot(sqrt(C),sqrt(1-C),'k-');

hold on

pivots=unique(iter.pivots,'rows','stable');
for s=1:2
    plot(pivots(:,2*(s-1)+1),pivots(:,2*(s-1)+2),'b-');
end

efficient1 = abs(pivots(:,1).^2+pivots(:,2).^2-1)<1e-6;
%plot(iter.pivots(efficient1,1),iter.pivots(efficient1,2),'r.-');
maxEffIndex = find(efficient1,1);
minEffIndex = find(efficient1,1,'last');
maxEffPayoff = pivots(minEffIndex,1:2);
[~,maxIneffIndex] = min(pivots(:,1)+pivots(:,2));
maxIneffPayoff = pivots(maxIneffIndex,1:2);

expSet = flipud(bsxfun(@plus,0.5*maxEffPayoff,0.5*pivots([minEffIndex:end-4],1:2)));
expSet = [expSet;...
    flipud(pivots(maxEffIndex:minEffIndex,1:2));...
    flipud(fliplr(expSet));...
    0.5*maxIneffPayoff+0.5*maxIneffPayoff([2 1])];
expSet=[expSet;expSet(1,:)];
plot(expSet(:,1),expSet(:,2),'r-');

title('$\delta=0.7$','interpreter','latex');
xlabel('$v_1$','interpreter','latex');
ylabel('$v_2$','interpreter','latex');
set(gca,'ticklabelinterpreter','latex');

hold off
axis square

set(gcf,'paperunits','inches','units','inches');
fpos = [-10.5417    2.4306    3.7778    3.5278];
set(gcf,'position',fpos);
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf','~/Dropbox/work/collaborations/abreubrookssannikov/Ben/risksharing_twostate_eff.pdf');



%% Redux with low delta
sgmex2('LoadSolution','../examples/solutions/risksharing_nume=2_c2e=200_delta=0.4.sln2');
sgmex2('IterToEnd');

iter = sgmex2('GetCurrentIteration');

%% Ineff picture
C=linspace(0,1,200*2+1);
C=[C 0];
plot(sqrt(C),sqrt(1-C),'k-');

hold on

% pivots=unique(iter.pivots,'rows','stable');
pivots=iter.pivots;
for s=1:2
    plot(pivots(:,2*(s-1)+1),pivots(:,2*(s-1)+2),'b-');
end

[~,cornerPayoffIndex] = max(pivots(:,1));
cornerPayoff = pivots(cornerPayoffIndex,1:2);
[~,maxIneffIndex] = min(pivots(:,1)+pivots(:,2));
maxIneffPayoff = pivots(maxIneffIndex,1:2);

expSet = flipud(bsxfun(@plus,0.5*cornerPayoff([2 1]),0.5*pivots(cornerPayoffIndex+2:end-1,1:2)));
expSet = [expSet;flipud(fliplr(expSet));...
    0.5*maxIneffPayoff+0.5*maxIneffPayoff([2 1])];
expSet=[expSet;expSet(1,:)];
plot(expSet(:,1),expSet(:,2),'r-');

expCorner = 0.5*cornerPayoff+0.5*cornerPayoff([2 1]);
flowPayoff = (cornerPayoff - 0.4*expCorner)/(1-0.4);
genLine = [flowPayoff;expCorner;flowPayoff([2 1])];
p=plot(genLine(:,1),genLine(:,2),'k--',...
    flowPayoff([1 2]),flowPayoff([2 1]),'k.',...
    cornerPayoff([1 2]),cornerPayoff([2 1]),'b.',...
    expCorner(1),expCorner(2),'r.');
set(p,'markersize',13);

title('$\delta=0.7$','interpreter','latex');
xlabel('$v_1$','interpreter','latex');
ylabel('$v_2$','interpreter','latex');
set(gca,'ticklabelinterpreter','latex');

hold off
axis square

set(gcf,'paperunits','inches','units','inches');
fpos = [-10.5417    2.4306    3.7778    3.5278];
set(gcf,'position',fpos);
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf','~/Dropbox/work/collaborations/abreubrookssannikov/Ben/risksharing_twostate_ineff.pdf');

