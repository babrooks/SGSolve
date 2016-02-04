function sgviewer(varargin)
    panelColor = get(0,'DefaultUipanelBackgroundColor');
    set(0,'units','characters');
    charsPerInch = get(0,'ScreenSize');
    set(0,'Units','inches');
    charsPerInch = charsPerInch./get(0,'ScreenSize');

    f=figure('name','SGViewer',...
             'units','normalized',...
             'outerposition',[0 0 1 1],...
             'numbertitle','off',...
             'toolbar','figure',...
             'keypressfcn',@keyPressCallback,...
             'units','characters',...
             'resizefcn',@figureResizeCallback);

    topLeftPanel = createAxesPanel(f);
    
    topRightPanel = uipanel('Parent',f,...
                            'resizefcn',@multiAxesResizeCallback,...
                            'bordertype','none',...
                            'backgroundcolor',panelColor,...
                            'UserData',[]);
    createAxesPanels(topRightPanel,1);


    botPanel = uipanel('Parent',f,...
                       'resizefcn',@botPanelResizeCallback,...
                       'bordertype','none',...
                       'backgroundcolor',panelColor);

    iterSlider = uicontrol('Parent',botPanel,...
                           'units','characters',...
                           'style','slider',...
                           'callback',@iterSliderCallback);

    startSlider = uicontrol('Parent',botPanel,...
                            'units','characters',...
                            'style','slider',...
                            'callback',@startSliderCallback);

    stateSlider = uicontrol('Parent',botPanel,...
                            'units','characters',...
                            'style','slider',...
                            'callback',@stateSliderCallback);

    actSlider = uicontrol('Parent',botPanel,...
                          'units','characters',...
                          'style','slider',...
                          'callback',@actSliderCallback);

    fileMenu = uimenu('Parent',f,...
                      'Label','SGViewer');
    loadMenu = uimenu('Parent',fileMenu,...
                      'Label','Load',...
                      'Callback',@loadMenuCallback);

    % Data
    data = struct('delta',[],...
                  'tuples',[],...
                  'probabilities',[],...
                  'payoffs',[],...
                  'numStates',[],...
                  'numActions',[],...
                  'plotIter',0,...
                  'plotAction',0,...
                  'plotState',0,...
                  'maxIterations',0,...
                  'startIter',0,...
                  'startTuple',0,...
                  'filename',[],...
                  'pathname','./',...
                  'lastRevStart',0);
    plotOpts = struct('feasSet','b-',...
                      'expSet','r-',...
                      'pivot','b.',...
                      'expPivot','r.',...
                      'bestExpDirection','r--',...
                      'bestDirection','m--',...
                      'action','k.',...
                      'expContVal','k.');


    %% Startup
    if nargin
        sgmex('Load',varargin{1});
        loadHelper();
    elseif sgmex('DataIsLoaded')
        loadHelper();
    end

    %% Plot tool
    function plotData
        handles = get(topLeftPanel,'UserData');
        s=sgmex('GetCurrentIteration');
        
        if data.plotState
            % On the leftPanel, plot a particular action relative
            % to the best direction
            currentState = get(handles.stateSelector,'value')-1;
            currentAction = data.plotAction;
        else
            % Plot the best action
            currentState = s.state;
            currentAction = s.action;
        end % data.plotAct
        
        stateTuples = data.tuples(data.startTuple:s.numExtremeTuples,...
                                  2*(currentState+1)-[1 0]);
        weights = data.probabilities{currentState+1}(currentAction+1,:);
        expTuples = [data.tuples(data.startTuple:s.numExtremeTuples,1:2:end)*(weights'),...
                     data.tuples(data.startTuple:s.numExtremeTuples,2:2:end)*(weights')];
        expPivot = weights * s.pivot;
        
        stagePayoff = [data.payoffs{currentState+1,1}(currentAction+1),...
                       data.payoffs{currentState+1,2}(currentAction+1)];
        
        expContVal = (s.pivot(currentState+1,:)-(1-data.delta)*stagePayoff)./data.delta;
        
        [xlim,ylim] = findlim([expTuples;stateTuples;stagePayoff]);
        directionWeight = abs(min((xlim(2)-xlim(1))/s.direction(1),...
                                  (ylim(2)-ylim(1))/s.direction(2)));

        plot(handles.ax,stateTuples(:,1),stateTuples(:,2),plotOpts.feasSet,...
             expTuples(:,1),expTuples(:,2),plotOpts.expSet,...
             s.pivot(currentState+1,1),s.pivot(currentState+1,2),plotOpts.pivot,...
             expPivot(1)+[0 s.direction(1)*directionWeight],...
             expPivot(2)+[0 s.direction(2)*directionWeight],plotOpts.bestExpDirection,...
             expPivot(1),expPivot(2),plotOpts.expPivot,...
             s.pivot(currentState+1,1)+[0 s.direction(1)*directionWeight],...
             s.pivot(currentState+1,2)+[0 s.direction(2)*directionWeight],...
             plotOpts.bestDirection,...
             stagePayoff(1),stagePayoff(2),plotOpts.action,...
             expContVal(1),expContVal(2),plotOpts.expContVal,...
             [stagePayoff(1) expContVal(1)],[stagePayoff(2) expContVal(2)],'k--');
        
        set(handles.ax,'xlim',xlim,'ylim',ylim,...
                       'buttonDownFcn',@axesButtonDownFcn);
        axis(handles.ax,'square');
        title(handles.ax,...
              sprintf('State: %d, action: %d, nonBinding: %d',...
                      currentState,currentAction,s.nonBinding));
        
        multiAxes = get(topRightPanel,'UserData');
        for axesCounter = 1:numel(multiAxes.axesPanels)
            handles = get(multiAxes.axesPanels(axesCounter), ...
                          'UserData');
            currentState = get(handles.stateSelector,'Value');
            plot(handles.ax,...
                 data.tuples(data.startTuple:s.numExtremeTuples,2*currentState - 1),...
                 data.tuples(data.startTuple:s.numExtremeTuples,2*currentState), plotOpts.feasSet,...
                 s.pivot(currentState,1),...
                 s.pivot(currentState,2), plotOpts.pivot,...
                 s.pivot(currentState,1)+[0 s.direction(1)*directionWeight],...
                 s.pivot(currentState,2)+[0 s.direction(2)*directionWeight],...
                 plotOpts.bestDirection);
            [xlim,ylim] = findlim(data.tuples(data.startTuple:s.numExtremeTuples,2*currentState-[1 0]));
            set(handles.ax,'xlim',xlim,'ylim',ylim,...
                           'buttonDownFcn',@axesButtonDownFcn);
            title(handles.ax,sprintf('State %d',currentState-1));
            axis(handles.ax,'square');
            
        end % axesCounter
        
        set(f,'name',sprintf('SGViewer: Iteration %d, Revolution %d',...
                             data.plotIter, s.revolution));
    
        get(handles.ax)
    
    end % plotData

    %% Callbacks
    function keyPressCallback(src,event)
        switch event.Key
          case {'leftarrow','b'}
            set(iterSlider,'value',max(get(iterSlider,'min'),get(iterSlider,'value')-1));
            iterSliderCallback;
          case {'rightarrow','f'}
            set(iterSlider,'value',min(get(iterSlider,'max'),get(iterSlider,'value')+1));
            iterSliderCallback;
          case {'home'}
            set(iterSlider,'value',get(iterSlider,'min'));
            iterSliderCallback;
          case {'end'}
            set(iterSlider,'value',get(iterSlider,'max'));
            iterSliderCallback;
          case {'w'}
            if strcmp(event.Modifier{1},'control') && numel(event.Modifier)==1
                delete(f);
            end
          otherwise
        end
    end % keyPressCallback

    function loadMenuCallback(src,event)
        [filename, pathname] = uigetfile({[data.pathname,'*.dat']});
        if filename
            data.filename = filename;
            data.pathname = pathname;
            
            if ~isempty(data.filename)
                sgmex('Load',[data.pathname data.filename]);
                loadHelper();
            end
        end % if
    end % loadMenuCallback

    function loadHelper()
        data.numStates = sgmex('GetNumStates');
        data.numActions = sgmex('GetNumActions');
        data.delta = sgmex('GetDelta');
        data.payoffs = cell(data.numStates,2);
        data.probabilities = cell(data.numStates,1);
        for state=1:data.numStates
	    [data.payoffs{state,:}] ...
                = sgmex('GetPayoffs',state-1);
            data.probabilities{state} ...
                = sgmex('GetProbabilities',state-1);
        end % state
        data.tuples = sgmex('GetTuples');
        
        createAxesPanels(topRightPanel,min(data.numStates));
        multiAxesResizeCallback(topRightPanel);
        
        handles = get(topLeftPanel,'UserData');
        set(handles.stateSelector,'callback',@stateSelectorCallback,...
                          'string',[{'Best state'}...
                            arrayfun(@(x) {num2str(x)},0:data.numStates-1)]);
        
        sgmex('IterToEnd');
        s = sgmex('GetCurrentIteration');
        data.plotIter = s.iteration;
        data.maxIterations = s.iteration;
        data.startIter = 1;
        data.startTuple = 1;
        
        
        set(iterSlider,'min',1,'max',data.maxIterations,...
                       'value',1,...
                       'sliderstep',[1 1]/(data.maxIterations-1));
        set(startSlider,'min',0,'max',data.maxIterations,...
                        'value',0,...
                        'sliderstep',[1 1]/(data.maxIterations));
        set(stateSlider,'min',0,'max',data.numStates,...
                        'value',0,...
                        'sliderstep',[1 1]/data.numStates);
        set(actSlider,'value',0,...
                      'min',0,'max',1,...
                      'sliderstep',[1 1]);
        data.plotState = 0;
        data.plotAction = 0;
        
        % Find start of last revolution.
        sp = sgmex('GetCurrentIteration');
        while sp.revolution==s.revolution
            sgmex('Iter--');
            sp = sgmex('GetCurrentIteration');
        end
        sgmex('Iter++');
        sp = sgmex('GetCurrentIteration');
        data.lastRevStart = sp.numExtremeTuples-1;
        
        plotData;
    end % loadHelper

    function advanceTuple(v)
        while data.plotIter+1 < v
            data.plotIter = data.plotIter+1;
            sgmex('Iter++');
        end % while
        while data.plotIter+1 > v
            data.plotIter = data.plotIter-1;
            sgmex('Iter--');
        end % while
    end % advanceTuple

    function stateSelectorCallback(src,event)
        data.plotState = get(src,'value')-1;
        set(stateSlider,'value',data.plotState);
    end % stateSelectorCallback

    function iterSliderCallback(src,event)
        v = round(get(iterSlider,'value'));
        advanceTuple(v);
        % handles = get(topLeftPanel,'UserData');
        % set(handles.stateSelector,'value',1);
        
        plotData;
    end % iterSliderCallback

    function startSliderCallback(src,event)
        currentIter = get(iterSlider,'value');
        newStart = round(get(startSlider,'value'));
        if newStart>0
            set(iterSlider,'value',max(currentIter,newStart),...
                           'min',newStart,...
                           'sliderstep',[1 1]/(data.maxIterations-newStart));
            data.startIter = newStart;
            advanceTuple(newStart);
            s = sgmex('GetCurrentIteration');
            data.startTuple = s.numExtremeTuples;
            advanceTuple(get(iterSlider,'value'));
            iterSliderCallback(src,event);
        else
            data.startIter = 0;
            data.startTuple = 1;
            set(iterSlider,'min',1,...
                           'sliderstep',[1 1]/(data.maxIterations-1));
            iterSliderCallback(src,event);
        end
    end % startSliderCallback

    function stateSliderCallback(src,event)
    end % stateSliderCallback

    function actSliderCallback(src,event)
    end % actSliderCallback

    function createAxesPanels(panel,numStates)
    % Creates numStates axes panels within panel.
        
        stateStrings = arrayfun(@(x) {num2str(x)},0:numStates-1)';
        
        handles = get(panel,'UserData');
        if isstruct(handles)
            for axesCounter = 1:numel(handles.axesPanels)
                delete(handles.axesPanels(axesCounter));
            end % axesCounter
        end % isstruct
        
        axesPanels = zeros(numStates,1);
        
        for state=1:numStates
            axesPanels(state) = createAxesPanel(panel);
            handles = get(axesPanels(state),'UserData');
            set(handles.stateSelector,'String',stateStrings',...
                              'Value',state);
        end % state
        set(panel,'UserData',struct('axesPanels',axesPanels));
        
        multiAxesResizeCallback(panel);
    end % createAxesPanels

    function panel = createAxesPanel(parent)
        panel = uipanel('Parent',parent,...
                        'resizefcn',@axesPanelResizeCallback,...
                        'bordertype','none',...
                        'backgroundcolor',panelColor);
        axPanel = uipanel('Parent',panel,...
                          'bordertype','none',...
                          'backgroundcolor',panelColor);
        ax = axes('Parent',axPanel,...
                  'ButtonDownFcn',@axesButtonDownFcn);
        stateSelector = uicontrol('Parent',panel,...
                                  'style','popupmenu',...
                                  'String','0');
        set(panel,'UserData',struct('stateSelector',stateSelector,...
                                    'axPanel',axPanel,...
                                    'ax',ax));
    end % createAxesPanel
    
    function axesButtonDownFcn(src,event)
    % This function will cause the currently clicked state to be
    % replotted on the left panel

        if get(get(src,'parent'),'parent')==topLeftPanel
            clickPoint = get(src,'currentPoint');
            [~,newIter] = min(sum(abs(bsxfun(@minus,...
                                             data.tuples(data.lastRevStart,...
                                                         2*data.plotState+[-1 0]),...
                                             point(1,1:2))),1));
           
        else
            % abs(bsxfun(@minus,data.tuples(data.lastRevStart:end,...
            %2*state+[-1 0]),...
                                          %         event;
        end
        
    end % axesButtonDownFcn

    %% Resize functions
    function multiAxesResizeCallback(src,event)
        handles = get(src,'UserData');
        numStates = numel(handles.axesPanels);
        
        if numStates
            % First find correct # of rows/cols. Maximizes x^2 such that
            % rows*x<=panel height, cols*x<=panel width and rows*cols >=
            % numStates. Clear that either x=height/rows or x=width/cols and
            % cols = ceil(numStates/rows) and
            % rows=ceil(numStates/cols). Hence, we can set
            % x=min(height/rows,width/ceil(numStates/rows)), and optimize
            % over rows.
            set(src,'units','inches');
            pos = get(src,'Position');
            rows = (1:numStates)';
            cols = ceil(numStates./rows);
            x = min([pos(3)./cols pos(4)./rows],[],2);
            [increment,I] = max(x);
            rows = rows(I); cols = cols(I);
            
            set(src,'units','inches');
            pos = get(src,'position');
            
            heightIncrement = increment; widthIncrement = increment;
            heightIncrement = pos(4)/rows; widthIncrement = pos(3)/cols;
            
            panelPos = [0,pos(4)-heightIncrement,...
                        widthIncrement,heightIncrement];
            for state=1:numStates
                set(handles.axesPanels(state),...
                    'Units','inches',...
                    'Position',panelPos);
                if mod(state,rows)
                    % Middle of a column
                    panelPos(2) = panelPos(2)-heightIncrement;
                else
                    % New column
                    panelPos(1) = panelPos(1)+widthIncrement;
                    panelPos(2) = pos(4)-heightIncrement;
                end % if
                state = state + 1;
            end % for
        end % if handles non-empty
        
    end % multiAxesResizeCallback

    function axesPanelResizeCallback(src,event)
        comboBoxDim=[12,1.5]./charsPerInch(3:4);
        
        try
            handles = get(src,'UserData');
            set(src,'units','inches');
            pos = get(src,'Position');
            set(handles.axPanel,...
                'Units','inches',...
                'Position',[0 ...
                            comboBoxDim(2)+1./charsPerInch(4)...
                            pos(3) ...
                            pos(4)-1./charsPerInch(4)-comboBoxDim(2)]);
            set(handles.stateSelector,...
                'Units','inches',...
                'Position',[pos(3)-comboBoxDim(1)-.5/charsPerInch(4)...
                            0.25/charsPerInch(4) ...
                            comboBoxDim]);
        catch me
            pos
            [0 comboBoxDim(2)+1 pos(3) pos(4)-1-comboBoxDim(2)]
        end % try
    end % axesPanelResizeCallback

    function figureResizeCallback(src,event)
        botPanelHeight = 10.5;
        set(f,'units','characters');
        fpos = get(f,'Position');
        
        set(topLeftPanel,'Units','characters',...
                         'position',[0 botPanelHeight fpos(3)/2 fpos(4)-botPanelHeight]);
        set(topRightPanel,'Units','characters',...
                          'position',[fpos(3)/2 botPanelHeight fpos(3)/2 fpos(4)-botPanelHeight]);
        set(botPanel,'Units','characters',...
                     'position',[0 0 fpos(3) botPanelHeight]);
        
        set(f,'units','inches');
        fpos = get(f,'position');
        set(gcf,'paperunits','inches','papersize',fpos(3:4),'paperposition',[0 0 fpos(3:4)]);
    end % figureResizeCallback

    function botPanelResizeCallback(src,event)
        set(botPanel,'units','characters');
        pos = get(botPanel,'position');
        startHeights = [8 5.5 3 0.5];
        
        handles = [iterSlider startSlider stateSlider actSlider];
        for handleCounter=1:numel(handles)
            set(handles(handleCounter),...
                'Position',[1 ...
                            startHeights(handleCounter)...
                            pos(3)-2 ...
                            1.5]);
        end
    end % botPanelResizeCallback

end % sgviewer

%% Utilities
function [xlim,ylim] = findlim(X)
% Finds good limits for the display for coordinates listed in Y,
% compares with old limits.

    if isempty(X)
	xlim=[];
	ylim=[];
	return;
    end

    minmax = [min(X,[],1)',max(X,[],1)'];
    length = max(minmax(:,2)-minmax(:,1))*1.1;
    if length==0
	length = 1;
    end
    % length = max(length,1);
    midpoint = sum(minmax,2)/2;

    xlim = midpoint(1)+length/2*[-1,1]; 
    ylim=midpoint(2)+length/2*[-1,1];
end
