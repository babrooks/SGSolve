%% Code for analyzing solution of the three player contribution game

sgmex2('LoadSolution','../examples/solutions/contribution.sln2');
sgmex2('IterToBeginning');
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
    
    set(gca,'zlim',payoffBounds,'xlim',payoffBounds,'ylim',payoffBounds);
    
end % state


%% 
i = 4;
s = 2;
a = 8;
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
[V,nr]=con2vert(iter.directions,0.5*sum(iter.levels(:,:),2));

% V=0.5*(iter.pivots(:,1:3)+iter.pivots(:,4:6));

% payoffs0=sgmex2('GetPayoffs',0);
% payoffs1=sgmex2('GetPayoffs',1);
% V=0.5*payoffs0+0.5*payoffs1;

faces=convhulln(V);
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
    set(p2,'facealpha',0.5,'edgealpha',0.5);
    %     end
end

hold on;

for p=1:3
    X=nextIter.actions{s}{a}.points{p};
    h=scatter3(X(:,1),X(:,2),X(:,3),'r.');
    set(h,'sizedata',500);

    X=kron(nextIter.actions{s}{a}.minIC',ones(5,1));
    Y=X;
    Y(2:3,mod(p,3)+1)=payoffBounds(2);
    Y(3:4,mod(p+1,3)+1)=payoffBounds(2);
    p2=patch(Y(:,1),Y(:,2),Y(:,3),ones(size(Y,1),1))
    set(p2,'facealpha',0.3,'facecolor','r');
end
hold off
view(az,el);
bounds = [min(min(V)),max(max(V))];
Z=0.5*payoffs0+0.5*payoffs1;
bounds = [min(min(Z)),max(max(Z))];
set(gca,'zlim',bounds,'xlim',bounds,'ylim',bounds);
