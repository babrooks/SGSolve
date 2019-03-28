%% Code for analyzing solution of the three player contribution game

% sgmex2('LoadSolution','../examples/solutions/contribution_fixed.sln2');
sgmex2('LoadSolution','../examples/solutions/contribution_endog.sln2');
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
    [V,nr]=con2vert(iter.directions,iter.levels(:,s));
    faces=convhulln(V);
    subplot(1,2,s)
    for row=1:size(faces,1)
        F = V(faces(row,:),:);
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
    h=scatter3(V(:,1),V(:,2),V(:,3),'k.');
    set(h,'sizedata',500);

    set(gca,'zlim',payoffBounds,'xlim',payoffBounds,'ylim',payoffBounds);
    hold off
    
end % state

%% 
% sgmex2('LoadSolution','../examples/solutions/contribution_fixed.sln2');
sgmex2('LoadSolution','../examples/solutions/contribution_endog.sln2');

%% 

i =3;
s = 1;
a = 8;
dir=7;

sgmex2('IterToBeginning');

for k=1:i-1
    sgmex2('Iter++');
end
[az,el]=view;

iter = sgmex2('GetCurrentIteration');
sgmex2('Iter++');
nextIter=sgmex2('GetCurrentIteration');

subplot(1,1,1);
clf
[V,nr]=con2vert(iter.directions,mean(iter.levels,2));

% V=0.5*(iter.pivots(:,1:3)+iter.pivots(:,4:6));

payoffs0=sgmex2('GetPayoffs',0);
payoffs1=sgmex2('GetPayoffs',1);
% V=0.5*payoffs0+0.5*payoffs1;

faces=convhulln(V);
for row=1:size(faces,1)
    F = V(faces(row,:),:);
    p2 = patch(F(:,1),...
        F(:,2),...
        F(:,3),...
        ones(size(faces,2),1));
    colormap(gray);
    set(p2,'facealpha',0.5,'edgealpha',0.5);
    %     end
end

hold on;

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
set(q,'linewidth',1.5,'autoscale','off','color','red');
q=scatter3(p(1),p(2),p(3),'r.');
set(q,'sizedata',500);

d1=[d(2)+d(3) -d(1) -d(1)];
d2=cross(d1,d);
optPlane = [p+100*(d1+d2);p+100*(d1-d2);p+100*(-d1-d2);p+100*(-d1+d2)];
p3=patch(optPlane(:,1),optPlane(:,2),optPlane(:,3),ones(size(optPlane,1),1));
set(p3,'facealpha',0.1,'facecolor','b');

Z1=0.5*payoffs0+0.5*payoffs1;
Z=[payoffs0;payoffs1];
% Z=Z1

h=scatter3(Z1(:,1),Z1(:,2),Z1(:,3),'k.');
set(h,'sizedata',500);

hold off
view(az,el);
bounds = [min(min(V)),max(max(V))];
bounds = [min(min(Z)),max(max(Z))];
% bounds(1)=1;
set(gca,'zlim',bounds,'xlim',bounds,'ylim',bounds);

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
set(gcf,'paperunits','inches','units','inches');
fpos = [-14.9028    1.1944    7.7778    5.8333];
set(gcf,'position',fpos);
set(gcf,'papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
print(gcf,'-dpdf','contribution_fixed2.pdf');



