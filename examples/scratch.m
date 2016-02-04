

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