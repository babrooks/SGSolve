%% Load data and plot gradients 
x = importdata('threeplayer.dat');
n = size(x,1)/2
gradients = x(1:n,:);
payoffs = x(n+1:end,:);

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
payoffs2 = unique(payoffs,'rows');
% payoffs2=payoffs;
scale = 1e2;
payoffs2 = round(scale*payoffs2)/scale;
size(payoffs2,1)
scatter3(payoffs2(:,1),payoffs2(:,2),payoffs2(:,3));
faces=convhulln(payoffs2);
hold on
for f=1:size(faces,1)
    p = patch(payoffs2(faces(f,:),1),payoffs2(faces(f,:),2),...
        payoffs2(faces(f,:),3),0.75*ones(1,3));
    set(p,'facealpha',0.75);
end % for face
hold off
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