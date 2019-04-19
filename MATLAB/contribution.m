%% Code for analyzing solution of the three player contribution game

sgmex2('LoadSolution','../examples/solutions/contribution_fixed.sln2');
% sgmex2('LoadSolution','../examples/solutions/contribution_endog.sln2');
% sgmex2('IterToBeginning');
sgmex2('IterToEnd');

iter = sgmex2('GetCurrentIteration');

payoffBounds = [1e10,-1e10];
for s=0:1
    payoffs = sgmex2('GetPayoffs',s);
    payoffBounds(2) = max(payoffBounds(2),max(max(payoffs)));
    payoffBounds(1) = min(payoffBounds(1),min(min(payoffs)));
end


%%

scatter3(iter.directions(:,1),iter.directions(:,2),iter.directions(:,3));

%% Plot the planes

clf

for s=1:2
    [EV,nrEV]=con2vert(iter.directions,iter.levels(:,s));
    faces=convhulln(EV);
    subplot(1,2,s)
    for row=1:size(faces,1)
        F = EV(faces(row,:),:);
        %     if ~all(abs(sum(F,2)-2*c)<1e-6) ...
        %             && ~all(abs(F(:,1)-minU)<1e-6)...
        %             && ~all(abs(F(:,2)-minU)<1e-6)...
        %             && ~all(abs(F(:,3)-minU)<1e-6)
        p2 = patch(F(:,1),...
            F(:,2),...
            F(:,3),...
            ones(size(faces,2),1));
        colormap(gray);
        set(p2,'facealpha',0.8,'edgealpha',0.5);
        %     end
        
    end
    hold on
    h=scatter3(EV(:,1),EV(:,2),EV(:,3),'k.');
    set(h,'sizedata',500);

    set(gca,'zlim',payoffBounds,'xlim',payoffBounds,'ylim',payoffBounds);
    hold off
    
end % state

%% 
sgmex2('LoadSolution','../examples/solutions/contribution_fixed.sln2');
% sgmex2('LoadSolution','../examples/solutions/contribution_endog.sln2');

%% 

i = 46;
s = 1;
a = 8;
dir=2;

sgmex2('IterToBeginning');

probabilities=sgmex2('GetProbabilities',s-1);

for k=1:i-1
    sgmex2('Iter++');
end
[az,el]=view;

iter = sgmex2('GetCurrentIteration');
sgmex2('Iter++');
nextIter=sgmex2('GetCurrentIteration');

subplot(1,1,1);
clf
[EV,nrEV]=con2vert(iter.directions,iter.levels*probabilities(a,:)');
[V,nrV]=con2vert(iter.directions,iter.levels(:,s));

% V=0.5*(iter.pivots(:,1:3)+iter.pivots(:,4:6));

payoffs0=sgmex2('GetPayoffs',0);
payoffs1=sgmex2('GetPayoffs',1);
% V=0.5*payoffs0+0.5*payoffs1;

faces=convhulln(EV);
for row=1:size(faces,1)
    F = EV(faces(row,:),:);
    p2 = patch(F(:,1),...
        F(:,2),...
        F(:,3),...
        ones(size(faces,2),1));
    colormap(gray);
    set(p2,'facealpha',0.5,'edgealpha',0.5);
    %     end
end

hold on;

faces=convhulln(V);
for row=1:size(faces,1)
    F = V(faces(row,:),:);
    p2 = patch(F(:,1),...
        F(:,2),...
        F(:,3),...
        ones(size(faces,2),1));
    colormap(gray);
    set(p2,'facealpha',0.2,'edgealpha',0.5,'facecolor','r');
    %     end
end


Z1=0.5*payoffs0+0.5*payoffs1;
Z1=sgmex2('GetPayoffs',s-1);
Z=[payoffs0;payoffs1];
% Z=Z1

h=scatter3(Z1(a,1),Z1(a,2),Z1(a,3),'k.');
set(h,'sizedata',500);


for p=[1:3]
    X=nextIter.actions{s}{a}.points{p};
    h=scatter3(X(:,1),X(:,2),X(:,3),'r.');
    set(h,'sizedata',500);

    X=kron(nextIter.actions{s}{a}.minIC',ones(5,1));
    Y=X;
    Y(2:3,mod(p,3)+1)=payoffBounds(2);
    Y(3:4,mod(p+1,3)+1)=payoffBounds(2);
    p2=patch(Y(:,1),Y(:,2),Y(:,3),ones(size(Y,1),1));
    set(p2,'facealpha',0.1,'facecolor','r');

    if ~isempty(nextIter.actions{s}{a}.points{p})
        X1=nextIter.actions{s}{a}.points{p};
        X2a=nextIter.actions{s}{a}.bndryDirs{p};
        % X2=cross(X2a([end 1:end-1],:),X2a,2);
        X2=-1*cross(X2a([2:end 1],:),X2a,2);
        q=quiver3(X1(:,1),X1(:,2),X1(:,3),...
            X2(:,1),X2(:,2),X2(:,3));
        set(q,'linewidth',1.5,'autoscale','off','color','blue');
    end
end

d=iter.directions(dir,:);
p = 0.5*(iter.pivots(dir,1:3)+iter.pivots(dir,4:6));
q=quiver3(p(1),p(2),p(3),...
    d(1),d(2),d(3));
set(q,'linewidth',1.5,'autoscale','off','color','cyan');
q=scatter3(p(1),p(2),p(3),'c.');
set(q,'sizedata',500);

d1=[d(2)+d(3) -d(1) -d(1)];
d2=cross(d1,d);
optPlane = [p+100*(d1+d2);p+100*(d1-d2);p+100*(-d1-d2);p+100*(-d1+d2)];
p3=patch(optPlane(:,1),optPlane(:,2),optPlane(:,3),ones(size(optPlane,1),1));
set(p3,'facealpha',0.1,'facecolor','b');


hold off
view(az,el);
bounds = [min(min(EV)),max(max(EV))];
bounds = [min(min(Z)),max(max(Z))];
% bounds(1)=1;
set(gca,'zlim',bounds,'xlim',bounds,'ylim',bounds);

%% Plot the equilibrium payoff set
faceAlpha=0.6;
edgeAlpha=0.5;

% i = 46;
s = 1;
a = 8;
dir=2;

sgmex2('IterToEnd');

probabilities=sgmex2('GetProbabilities',s-1);
% 
% for k=1:i-1
%     sgmex2('Iter++');
% end
[az,el]=view;

iter = sgmex2('GetCurrentIteration');
sgmex2('Iter++');
nextIter=sgmex2('GetCurrentIteration');

subplot(1,1,1);
clf
[EV,nrEV]=con2vert(iter.directions,iter.levels*probabilities(a,:)');

% V=0.5*(iter.pivots(:,1:3)+iter.pivots(:,4:6));

payoffs0=sgmex2('GetPayoffs',0);
payoffs1=sgmex2('GetPayoffs',1);
% V=0.5*payoffs0+0.5*payoffs1;

faces=convhull(EV,'simplify',true);

% for row=1:size(faces,1)
%     F = EV(faces(row,:),:);
%     p2 = patch(F(:,1),...
%         F(:,2),...
%         F(:,3),...
%         ones(size(faces,2),1));
%     colormap(gray);
%     set(p2,'facealpha',0.5,'edgealpha',0.0);
%     %     end
% end

h=scatter3(EV(:,1),EV(:,2),EV(:,3),'k.');
set(h,'sizedata',100);

for d=1:size(iter.directions,1)
    tf = EV*(iter.directions(d,:)')>iter.levels(d,:)*(probabilities(a,:)')-1e-2;
    
    if sum(tf)<3
        continue;
    end
    
    facePnts = EV(tf,:);
    d1 = (facePnts(2,:)-facePnts(1,:))';
    d2 = (facePnts(3,:)-facePnts(1,:))';
    
    dirLvls = [facePnts*d1-facePnts(1,:)*d1,...
        facePnts*d2-facePnts(1,:)*d2];
    K=convhull(dirLvls);
    F=facePnts(K,:);
    
    p2 = patch(F(:,1),...
        F(:,2),...
        F(:,3),...
        ones(size(F,1),1));
    colormap(gray);
    set(p2,'facealpha',faceAlpha,'edgealpha',edgeAlpha);
    %     end
end
hold on

for s=1:2
    [V,nrV]=con2vert(iter.directions,iter.levels(:,s));
    h=scatter3(V(:,1),V(:,2),V(:,3),'k.');
    set(h,'sizedata',100);
    
    for d=1:size(iter.directions,1)
        tf = V*(iter.directions(d,:)')>iter.levels(d,s)-1e-2;
        
        if sum(tf)<3
            continue;
        end
        
        facePnts = V(tf,:);
        d1 = (facePnts(2,:)-facePnts(1,:))';
        d2 = (facePnts(3,:)-facePnts(1,:))';
        
        dirLvls = [facePnts*d1-facePnts(1,:)*d1,...
            facePnts*d2-facePnts(1,:)*d2];
        K=convhull(dirLvls);
        F=facePnts(K,:);
        
        p2 = patch(F(:,1),...
            F(:,2),...
            F(:,3),...
            ones(size(F,1),1));
        colormap(gray);
        set(p2,'facealpha',faceAlpha,'edgealpha',edgeAlpha,'facecolor',[0.75 .25 0.25]);
        %     end
    end
end


hold off
view(az,el);
bounds = [min(min(EV)),max(max(EV))];
bounds = [min(min(Z)),max(max(Z))];
% bounds(1)=1;
% set(gca,'zlim',bounds,'xlim',bounds,'ylim',bounds);

xlabel('$v_1$','interpreter','latex');
ylabel('$v_3$','interpreter','latex');
zlabel('$v_2$','interpreter','latex');
set(gca,'ticklabelinterpreter','latex');

set(gcf,'paperunits','inches','units','inches');
% fpos = [-7.7778       3.8472       7.7778       5.8333];
fpos = get(gcf,'position');
set(gcf,'position',fpos,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf','contribution_payoffsets1.pdf');


%% Count the number of true faces
numTrueFaces = 0;
trueFaces = false(size(iter.directions,1),1);
for k=1:size(iter.directions,1)
    lvls = V*iter.directions(k,:)';
    if (sum(lvls>=max(lvls)-1e-8)>=3)
        numTrueFaces = numTrueFaces+1;
    trueFaces(k)=true;
    end
end
numTrueFaces


%%
sgmex2('IterToBeginning');
iter0=sgmex2('GetCurrentIteration');
sgmex2('Iter++');
iter1=sgmex2('GetCurrentIteration');
sgmex2('Iter++');
iter2=sgmex2('GetCurrentIteration');

% [iter0.actionTuples iter0.regimeTuples,...
%     iter1.actionTuples iter1.regimeTuples,...
%     iter2.actionTuples iter2.regimeTuples]

x=max(max(iter2.levels-iter1.levels))

k=find(iter2.levels>iter1.levels+x/2,1,'first')

%%
X=iter1.actions{1}{3}.points{1}
X=iter1.actions{1}{3}.bndryDirs{1}

%% 
hold on
q=quiver3(5.5,5.5,5.5,12,3,3);
set(q,'linewidth',1.5,'autoscale','off','color','magenta');
q=quiver3(5.5,5.5,5.5,6,6,6);
set(q,'linewidth',1.5,'autoscale','off','color','green');
q=quiver3(5.5,5.5,5.5,6,-3,6);
set(q,'linewidth',1.5,'autoscale','off','color','cyan');
hold off

%% 
z1=(4+2/3)*[1 1 1]; z2=[5,4,4]; z3=[4,5,4];
cross(z2-z1,z3-z1)


%% 
z2=[4.5 4.5 1.5]; z1=[2.5 2.5 2.5]; z3=[4.5 1.5 4.5];
cross(z2-z1,z3-z1)

%%
fd = [-2/3 1/3 -2/3]; rd = [-2/3 -2-2/3 -2/3]; lvl0 = 0.05555555555; lvl1=0.25;

nd0 = fd*1/(1+lvl0)-rd*lvl0/(1+lvl0)
nd1 = fd*1/(1+lvl1)+rd*lvl1/(1+lvl1)

%%
set(gcf,'paperunits','inches','units','inches');
fpos = [-14.9028    1.1944    7.7778    5.8333];
set(gcf,'position',fpos);
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf','contribution_fixed2.pdf');

%% 
% x=rand(1,3)
x=[1 1 -1]
y=[x(2)+x(3),-x(1)-x(3),-x(1)+x(2)]
dot(x,y)
z=cross(x,y);
X=[x];
Y=zeros(size(X));
q=quiver3(Y(:,1),Y(:,2),Y(:,3),X(:,1),X(:,2),X(:,3))
set(q(1),'color','r');
hold on
X=[y];
Y=zeros(size(X));
q=quiver3(Y(:,1),Y(:,2),Y(:,3),X(:,1),X(:,2),X(:,3))
set(q(1),'color','b');
X=[z];
Y=zeros(size(X));
q=quiver3(Y(:,1),Y(:,2),Y(:,3),X(:,1),X(:,2),X(:,3))
set(q(1),'color','b');
hold off


