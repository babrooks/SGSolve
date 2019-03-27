%% Plot the test intersection
x=importdata('../examples/threeplayertest.log')

% x([2 3],:) = x([3 2],:);

[az,el]=view;

clf 

rows = [0;find(x(:,1)==99)];
X=x;
X(rows(2:end),:)=[];
A=X(:,4:6);
b=dot(X(:,1:3),X(:,4:6),2);
[V,nr]=con2vert(A,b);
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


rows = [0;find(x(:,1)==99)];
for p=1
    z=x(rows(p)+1:rows(p+1)-1,:);
    
    h=scatter3(z(:,1),z(:,2),z(:,3),'b.');
    set(h,'sizedata',300);
    
    z(:,4:6)=bsxfun(@times,z(:,4:6),1.0./sqrt(sum(z(:,4:6).^2,2)));
    q=quiver3(z(:,1),z(:,2),z(:,3),...
        z(:,4),z(:,5),z(:,6));
    set(q,'linewidth',1.5,'autoscale','on','color','red');
    
%     y=cross(z([end,1:end-1],4:6),z(:,4:6),2);
    y=-0.15*cross(z([2:end,1],4:6),z(:,4:6),2);
    y=bsxfun(@times,y,1.0./sqrt(sum(y.^2,2)));
    q=quiver3(z(:,1),z(:,2),z(:,3),...
        y(:,1),y(:,2),y(:,3));
    set(q,'linewidth',1.5,'autoscale','on','color','blue');
end

hold off;

lim=[-0.2,1.2];
set(gca,'xlim',lim,'ylim',lim,'zlim',lim);

view(az,el);

xlabel('x axis');
ylabel('y axis');
zlabel('z axis');
