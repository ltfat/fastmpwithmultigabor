function bookedit_exp( book, channel, bwfactor )
%function BOOKEDIT_EXP Plot and edit a Matching Pursuit book in the current axes
%
%    BOOKEDIT_EXP( book, chan ) plots the channel number chan
%    of a MPTK book structure in the current axes.
%    If book is a string, it is understood as a filename and
%    the book is read from the corresponding file. Books
%    can be read separately using the BOOKREAD utility.
%
%    BOOKEDIT_EXP( book ) defaults to the first channel.
%
%    The patches delimit the support of the atoms. Their
%    color is proportional to the atom's amplitudes,
%    mapped to the current colormap and the current caxis.
%
%    See also BOOKREAD_EXP, BOOKWRITE_EXP and
%    the patch handle graphics properties.

%% Authors:
% Gilles Gonon 
% Copyright (C) 2008 IRISA
%
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
%% SVN log:
%   $Author: broy $
%   $Date: 2008-02-14 18:00:30 +0100 (Wed, 14 Feb 2008) $
%   $Revision: 783 $
%

if nargin<1
    disp([ upper(mfilename) ' needs at least 1 input argument, see help'])
    return;
end


%% Test input args
if ischar(book),
    disp('Loading the book...');
    book = bookreadGil( book );
    disp('Done.');
end;

if nargin < 2,
    channel = 1;
end;

if channel > book.numChans,
    error('Book has %d channels. Can''t display channel number %d.', ...
        channel, book.numChans );
end;

if nargin < 3,
    bwfactor = 2;
end;

nS = book.numSamples;
fs = book.sampleRate;

%% Add graphical / selection information to atoms
% add 2 fields in atom structure :
%   - 'selected' for selection information 
%   - 'visible' for plotting information
for ty = 1:length(book.atom)
    [nA, nC] = size(book.atom(ty).params.amp);
    book.atom(ty).selected = zeros(nA,nC);
    book.atom(ty).visible = ones(nA,nC);
end

%----------------------------
% General UI variables
bHeight = 0.05;
vSpace = 0.01;
bgColor = [0.7 0.8 0.9];
fgColor = [0.8 0.9 1];

%% ------------------------------
%  Main window
%  -----------
figH = figure( ...
    'Name','MPTK - Visualisation and edition of books', ...
    'NumberTitle','off', ...
    'Backingstore','off', ...
    'Units','normalized', ...
    'Position',[ .1 .15 .8 .7 ], ...
    'Color',bgColor, ...
    'MenuBar','none',...
    'Visible','on');

%% ---------------------
%  Figure menus
%  ------------
item = 1;
% menuItem(1) : 'File' menu
menuItem(item) = uimenu(figH,'Label','&File','Callback','');
item = item + 1;
% menuItem(2) : 'Edit' menu
menuItem(item) = uimenu(figH,'Label','&Edit','Callback','');
item = item + 1;
% menuItem(3) : 'Transform' menu
menuItem(item) = uimenu(figH,'Label','&Tranform','Callback','');
item = item + 1;
% menuItem(4) : 'Help' menu
menuItem(item) = uimenu(figH,'Label','&Help','Callback','');
item = item + 1;
% 'File' sub items
menuItem(item) = uimenu(menuItem(1),'Label','&Open book','Callback',@loadBook,'Separator','off','Accelerator','O');
item = item + 1;
menuItem(item) = uimenu(menuItem(1),'Label','&Save selection to book','Callback',@saveSelectedBook,'Separator','off','Accelerator','S');
item = item + 1;
menuItem(item) = uimenu(menuItem(1),'Label','&Save visible to book','Callback',@saveVisibleBook,'Separator','off','Accelerator','S');
item = item + 1;
menuItem(item) = uimenu(menuItem(1),'Label','&Close window','Callback','close(gcf)','Separator','on','Accelerator','Q');
item = item + 1;
% 'Edit' subitems
menuItem(item) = uimenu(menuItem(2),'Label','Select &All','Callback',@selectAll,'Separator','off','Accelerator','A');
item = item + 1;
menuItem(item) = uimenu(menuItem(2),'Label','Select &None (clear selection)','Callback',@selectNone,'Separator','off','Accelerator','N');
item = item + 1;
menuItem(item) = uimenu(menuItem(2),'Label','Cut selected atoms','Callback',@cutSelection,'Separator','on','Accelerator','X');
item = item + 1;
menuItem(item) = uimenu(menuItem(2),'Label','&Keep only selected atoms','Callback',@keepSelection,'Separator','off','Accelerator','K');
item = item + 1;
menuItem(item) = uimenu(menuItem(2),'Label','&Export selection to Anywave','Callback',@exportAnywave,'Separator','on','Accelerator','E');
item = item + 1;
% 'Transform' subitems
menuItem(item) = uimenu(menuItem(3),'Label','&Pitch Shift ...','Callback',@pitchShift,'Separator','off','Accelerator','P');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Time Stretch ...','Callback',@timeStretch,'Separator','off','Accelerator','T');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Gain ...','Callback',@applyGain,'Separator','off','Accelerator','G');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Time Reverse selection ...','Callback',@timeReverse,'Separator','off','Accelerator','G');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Freq reverse selection ...','Callback',@freqReverse,'Separator','off','Accelerator','G');
item = item + 1;
% 'Help subitems'
menuItem(item) = uimenu(menuItem(4),'Label','How to use this editor','Callback','doc bookedit','Separator','off');
item = item + 1;
menuItem(item) = uimenu(menuItem(4),'Label','About BOOKEDIT','Callback',@aboutBookedit,'Separator','on');
item = item + 1;


%% ------------------------------
%  Add toolbar buttons (icons)
%  --------
if exist('MPtoolbaricons.mat','file')==2
    icons = load('MPtoolbaricons.mat');
else
    icons.playsound = rand(16,16,3);
    icons.zoomx = rand(16,16,3);
    icons.zoomy = rand(16,16,3);
    icons.zoomplus = rand(16,16,3);
    icons.fullview = rand(16,16,3);
    icons.playselected = rand(16,16,3);
    icons.selectadd = rand(16,16,3);
    icons.selectremove = rand(16,16,3);
    icons.open_hand = rand(16,16,3);
end

toolH(1) = uitoolbar(figH);
% Play visible atoms
toolH(end+1) = uipushtool(toolH(1),'CData',icons.playsound,'TooltipString','Play visible atom sound',...
    'ClickedCallback',@playvisiblesound);
% Play selected atoms
toolH(end+1) = uipushtool(toolH(1),'CData',icons.playselected,'TooltipString','Play slected atoms sound',...
    'ClickedCallback',@playselectedsound);
% Select atoms
toolH(end+1) = uitoggletool(toolH(1),'CData',icons.selectadd,'TooltipString','Select Atoms',...
    'OnCallback',@toggleOnSelectAtoms,...
    'OffCallback',@clearMouseFcn);
% Unselect atoms
%toolH(end+1) = uitoggletool(toolH(1),'CData',icons.selectremove,'TooltipString','UnSelect Atoms',...
%    'Enable','off',...
%    'OnCallback',@toggleOnUnSelectAtoms,...
%    'OffCallback',@clearMouseFcn);

% Zoom out full
toolH(end+1) = uipushtool(toolH(1),'CData',icons.fullview,'TooltipString','Zoom out full',...
    'ClickedCallback',@zoomOutFull);
% Zoom horizontal
toolH(end+1) = uitoggletool(toolH(1),'CData',icons.zoomx,'TooltipString','Zoom horizontal',...
    'OnCallback',@zoomHorizontal,...
    'OffCallback','zoom off','Separator','on');
% Zoom vertical
toolH(end+1) = uitoggletool(toolH(1),'CData',icons.zoomy,'TooltipString','Zoom vertical',...
    'OnCallback',@zoomVertical,...
    'OffCallback','zoom off');
% Zoom in
toolH(end+1) = uitoggletool(toolH(1),'CData',icons.zoomplus,'TooltipString','Zoom',...
    'OnCallback',@zoomIn,...
    'OffCallback','zoom off');
% Drag plot
toolH(end+1) = uitoggletool(toolH(1),'CData',icons.open_hand,'TooltipString','Grab and pan',...
    'OnCallback',@panPlot,...
    'OffCallback','pan off');

%% -------------------------------
%  Main Axes
%  ---------
axH = zeros(1,book.numChans);
vspace = 0.05; % vertical space between 2 axes
axHeight = (1-(book.numChans+1)*vspace ) / book.numChans; % axes Height
for ac = 1:book.numChans
    axH(ac) = axes( ...
        'Units','normalized', ...
        'Position',[0.2675 vspace+axHeight*(ac-1) 0.7 axHeight ], ...
        'Drawmode','fast', ...
        'Layer','top', ...
        'Visible','on');
end


%% -----------------------------------
%  UI FRAME for BOOK INFO - QUERIES
%  ----------------------
leftPanelH = uipanel('Title','Book Info/Selection','FontSize',12,...
    'BackgroundColor',bgColor,...
    'Position',[0.015 0.05 0.2 0.9]);

%---------------------------------
% Check boxes with type of atoms
%---------------------------------
data.typeHandles = addCheckBoxTypes(book,figH);

%-----------------------------------------------------
% Plot the atoms and get the handles to all the atoms
%  -----------------
data.atomHandles = plotBook(book,axH);
for ac = 1:book.numChans
    axH(ac);
    axis([0 nS/fs 0 fs/2]);
end

%% ---------------------------------------------------------
%  Application data : saving all needed variables in figure
%  -----------------
data.selectAtoms = 0;       % Bool for start drag selection of atoms
data.begSelectAtoms = [];   % Begining of rectangle selection
data.endSelectAtoms = [];   % End of rectangle selection
data.atomSelection = [];    % Stack of rectangle selections (for undo)
data.atomUnSelection = [];    % Stack of rectangle selections (for undo)
data.indAtomsSelected = []; % Indexes of selected atoms in book
data.curRectH = -1;         % Current rectangle handle
data.rectSelH = [];         % Vector of Selection handles
data.rectUnSelH = [];         % Vector of Selection handles
data.toolH = toolH;         % Vector of toolbar icons handles
data.book = book;           % Vector of toolbar icons handles
data.axH = axH;           % Vector of toolbar icons handles
data.loadBookDir = pwd;
data.saveBookDir = pwd;
set(figH,'UserData',data)

%% ------------------
%  CALLBACK FUNCTIONS
%  ------------------

    %% FIGURE MENUS CALLBACKS
    % -------------------------
    
    %% OPEN AND LOAD A BOOK
    function loadBook(varargin)
        data = get(gcf,'UserData');
        cd(data.loadBookDir);
        [filename, pathname] = uigetfile({'*.bin';'*.txt';'*.xml'},'Open a book file');
        if (filename)
            data.loadBookDir = pathname;
            bookname = fullfile(pathname,filename);          
            if (exist(bookname,'file')==2)
                disp('Loading the book...');
                book = bookreadGil( bookname );
                disp('Done.');
                data.book = book;
                %Redraw book
                delete(data.atomHandles(ishandle( data.atomHandles ))); % Delete atoms
                data.atomHandles = plotBook(book,data.axH);
                set(gcf,'UserData',data);
                % Redraw show/hide Atom type checkboxes
                delete(data.typeHandles(ishandle( data.typeHandles ))); % Delete checkbox atom type
                data.typeHandles = addCheckBoxTypes(book,gcf);
                % Clear selection
                selectNone();

            else % Bookname does not exists, save book directory path
                set(gcf,'UserData',data);
            end
        end
    end

    %% SAVE VISIBLE ATOMS AS A NEW BOOK
    function saveVisibleBook(varargin)
        data = get(gcf,'UserData');       
        data.book.index(4,:) = 0;
        % Copy visible info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).visible;
        end
        % Write book
        newSaveDir = writeBook(data.book,data.saveBookDir);
        if (~isempty(newSaveDir))
           data.saveBookDir = newSaveDir;
           set(gcf,'UserData',data)
        end
    end

    %% SAVE SELECTED ATOMS AS A NEW BOOK
    function saveSelectedBook(varargin)
        data = get(gcf,'UserData');       
        data.book.index(4,:) = 0;
        % Copy visible info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).selected;
        end
        % Write book
        newSaveDir = writeBook(data.book,data.saveBookDir);
        if (~isempty(newSaveDir))
           data.saveBookDir = newSaveDir;
           set(gcf,'UserData',data)
        end
    end

    %% 
    function selectAll(varargin)
        data = get(gcf,'UserData');       
        % Copy visible info into index
        for t = 1:length(data.book.atom)
            [n,m] = size(data.book.atom(t).selected);
            data.book.atom(t).selected = ones(n,m);
        end
        set(gcf,'UserData',data)
    end

    function selectNone(varargin)
       data = get(gcf,'UserData');
       % Clear selection rectangles
       delete(data.rectSelH(ishandle(data.rectSelH)));
       % Erase data info      
       data.selectAtoms = 0;       % Bool for start drag selection of atoms
       data.begSelectAtoms = [];   % Begining of rectangle selection
       data.endSelectAtoms = [];   % End of rectangle selection
       data.atomSelection = [];    % Stack of rectangle selections (for undo)
       data.atomUnSelection = [];    % Stack of rectangle selections (for undo)
       data.indAtomsSelected = []; % Indexes of selected atoms in book
       data.curRectH = -1;         % Current rectangle handle
       data.rectSelH = [];         % Vector of Selection handles
       data.rectUnSelH = [];         % Vector of Selection handles
        
       % Set book selected to 0
        % Copy visible info into index
        for t = 1:length(data.book.atom)
            [n,m] = size(data.book.atom(t).selected);
            data.book.atom(t).selected = zeros(n,m);
        end
        
       set(gcf,'UserData',data)
    end

    function cutSelection(varargin)
         disp('cutSelection() - Not implemented')
    end

    function keepSelection(varargin)
         disp('keepSelection() - Not implemented')
    end

    function exportAnywave(varargin)
         disp('exportAnywave() - Not implemented')
    end

    function aboutBookedit(varargin)
         disp('aboutBookedit() - Not implemented')
    end

    %% TOOLBAR ICONS CALLBACKS
    % -------------------------
    function playvisiblesound(varargin)
        data = get(gcf,'UserData');       
        data.book.index(4,:) = 0;
        % Copy visible info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).visible;
        end

        playBook(data.book);    
    end

    function playselectedsound(varargin)
        data = get(gcf,'UserData');       
        data.book.index(4,:) = 0;
        % Copy selected info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).selected;
        end

        playBook(data.book);
    end

    function toggleOnSelectAtoms(varargin)
        toggleToolbar();
        zoom off;
        set(gcf,'WindowButtonDownFcn',@startSelectRect);
        set(gcf,'WindowButtonUpFcn',@stopSelectRect);
        set(gcf,'WindowButtonMotionFcn',@dragSelectRect);
    end

    %% Remove action affected to mouse events
    function clearMouseFcn(varargin)
        set(gcf,'WindowButtonDownFcn',[]);
        set(gcf,'WindowButtonUpFcn',[]);
        set(gcf,'WindowButtonMotionFcn',[]);
    end

%    function toggleOnUnSelectAtoms(varargin)
%        toggleToolbar();
%        zoom off;
%        set(gcf,'WindowButtonDownFcn',@startUnSelectRect);
%        set(gcf,'WindowButtonUpFcn',@stopUnSelectRect);
%        set(gcf,'WindowButtonMotionFcn',@dragSelectRect);
%    end

    %% Toolbar callback horizontal zoom In
    function zoomHorizontal(varargin)
        toggleToolbar();
        zoom xon;
    end

    %% Toolbar callback vertical zoom In
    function zoomVertical(varargin)
        toggleToolbar();
        zoom yon;
    end

    %% Toolbar callback grab plot
    function panPlot(varargin)
        toggleToolbar();
        pan on;
    end

    %% Toolbar callback zoom in
    function zoomIn(varargin)
        toggleToolbar();
        zoom on;
    end
    %% Toolbar callback zoom out full
    function zoomOutFull(varargin)
        toggleToolbar();
        zoom out;
    end

    %% Checkbox callback for show/hide all atom types
    function toggleViewAllAtom(varargin)
       data = get(gcf,'UserData');
       val = get(data.typeHandles(1),'Value'); % Get checkbox 'hide/show all' value
       
       for th=2:length(data.typeHandles)
           set(data.typeHandles(th),'Value',val);
           toggleViewAtomLength(data.typeHandles(th));
       end
    end

    %% Checkbox callback for show/hide an atom type
    function toggleViewAtomType(varargin)
        cbH = gcbo; % Chekbx handles       
        data = get(gcf,'UserData');
        type = get(varargin{1},'Tag');
        val  = get(varargin{1},'Value');
        idx = getTypeIndex(data.book,type);
        
        indCbH = find(data.typeHandles==cbH); % Index of current handle in vector of handles
        if (length(idx)==1)
           set(data.typeHandles(indCbH),'Value',val);                       
           toggleViewAtomLength(data.typeHandles(indCbH)); 
        else
            for i=1:length(idx) % type found at idx
                set(data.typeHandles(indCbH+i),'Value',val);                       
                toggleViewAtomLength(data.typeHandles(indCbH+i));
            end
        end
    end
    
    %% Checkbox callback for show/hide an atom type
    function toggleViewAtomLength(varargin)
        data = get(gcf,'UserData');
        type = get(varargin{1},'Tag');
        len  = get(varargin{1},'String');
        val  = get(varargin{1},'Value');
        
        % In case there is only one scale       
        if (isempty(str2num(len)))
           idx = getTypeIndex(data.book,type);
        else
           idx = getTypeIndex(data.book,type,str2num(len));
        end
        
        for i = 1:length(idx) % type found at idx
            [nA nC] = size(data.book.atom(idx(i)).params.amp);
            if (val) % Show atom with type
                state = 'on';
                data.book.atom(idx(i)).visible = ones(nA,nC);
            else     % Hide atom with type
                state = 'off';
                data.book.atom(idx(i)).visible = zeros(nA,nC);
            end
            
            % If the atom was rendered, set is visible status
            if (ishandle(data.atomHandles(idx(i))) )
                set(data.atomHandles(idx(i)),'Visible',state);
            end
            
            set(gcf,'UserData',data);
        end
    end

    %% Transform Menu callbacks (TODO)
    % -----------------------------
    %% Apply a Gain to selection
    function applyGain(varargin)
        disp('applyGain() - not implemented')
    end

    function pitchShift(varargin)
        disp('pitchShift() - not implemented')
    end

    function timeStretch(varargin)
        disp('timeStretch() - not implemented')
    end

    function timeReverse(varargin)
        disp('timeReverse() - not implemented')
    end

    function freqReverse(varargin)
        disp( 'freqReverse() - not implemented')
    end

%% SUB FUNCTIONS
%  -------------

%% Write a MPTK binary book file - user is asked for the book name
    function newsavedir = writeBook(book,defaultDir)
        newsavedir = [];
        nAtom = sum(book.index(4,:));
        if (nAtom) % Check that there is non zero atom in book
            curDir = cd; % save current directory
            cd(defaultDir);
            [filename, pathname] = uiputfile( {'*.bin;*.txt','MPTK book-files (*.bin)'}, ...
                'Save Atoms in book', [ 'book_' num2str(nAtom) 'atoms.bin']);
            if (filename)
                newsavedir = pathname;
                bookwrite_exp(book,fullfile(pathname,filename));
            end
            cd(curDir); % return in current directory
        else
           warndlg('No Atom is selected, nothing to save in book', 'Book save info', 'modal');
        end  
    end

%% Reconstruct and Play Book as a sound
    function playBook(book)
         if (book.index(4,:)== 0)
            disp('There are no selected atoms for reconstruction')
        else
            tmpbook = 'booktemp.bin';
            tmpwav = 'tempmpr.wav';
            % Save book
            wb = waitbar(0,'Exporting visible part of book','Name','Play visible atoms');
            bookwrite_exp(data.book,tmpbook);

            % Reconstruct book with mpr
            waitbar(0.3,wb,'Reconstructing book');
            execname = 'mpr';
            c = computer;
            if (strcmp(computer,'PCWIN'))
                execname = ['"' execname '.exe' '"'];
            end
            command = [ execname ' ' tmpbook ' ' tmpwav ];
            [s,w] = system(command);

            % Playsound
            waitbar(0.6,wb,'Load and play sound');
            if (s==0)
                [Y,Fs] = wavread(tmpwav);
                %figure;
                %plot(Y)
                soundsc(Y,Fs);
                waitbar(0.9,wb,'Erase temp files');
                PlotSoundJava(Y,Fs);
            else
                waitbar(0.9,wb,'Problem using mpr');
                disp('Problem using mpr')
            end

            % Delete tmpfiles
            delete(tmpbook)
            delete(tmpwav)
            close(wb)
            %uiresume(fig);
        end
       
    end 

%% Convert Current point coordinates in Figure to axis cartesian position
    function [x,y] = figToAxe(curpoint)
        pa = get(gca,'Position'); % Must be normalized position
        lim = axis(gca);

        x = (curpoint(1)-pa(1)) / pa(3);
        y = (curpoint(2)-pa(2)) / pa(4);

        x = x * (lim(2)-lim(1)) + lim(1);
        y = y * (lim(4)-lim(3)) + lim(3);
    end

%% START TO SELECT ATOMS RECTANGLE (MOUSE CLICK)
    function startSelectRect(varargin)
        data = get(gcf,'UserData');
        if (gco==gca) % Click on axis or one of its children
            data.selectAtoms = 1;
            % Get and check current mouse coordinates in axis
            xy = get(varargin{1},'CurrentPoint');
            [x,y] = figToAxe(xy);
            data.begSelectAtoms = [x y];
            axl = axis(gca);
            rpos = [ x y 1e-6 1e-6 ];
            data.curRectH = rectangle('Position', rpos,'faceColor',[0.9 0.6 0.9]);
            % Swap Depth of plot (set rectangle==axChild(1) at the end of the
            % gcf children handles)
            axChild = get(gca,'Children');
            set(gca,'Children',[axChild(axChild~=data.curRectH); axChild(axChild==data.curRectH)] );
        end
        set(gcf,'UserData',data)
    end

%% STOP TO SELECT ATOMS RECTANGLE (MOUSE RELEASE)
    function stopSelectRect(varargin)
        axChild = get(gca,'Children');
        if ( (gco==gca) || sum(gco==axChild) )
            data = get(gcf,'UserData');
            % Update userdata
            data.selectAtoms = 0; % realeased
            % Copy rectangle handle to vector of selection handles
            if (ishandle(data.curRectH))
                data.rectSelH(end+1) = data.curRectH;
                data.curRectH = -1;
            end

            % Get and check current mouse coordinates in axis
            xy = get(varargin{1},'CurrentPoint');
            [x,y] = figToAxe(xy);
            % Update data information
            rpos = [ min(data.begSelectAtoms(1),x) max(data.begSelectAtoms(1),x) ...
                min(data.begSelectAtoms(2),y) max(data.begSelectAtoms(2),y)];
            data.atomSelection(end+1,:) = rpos;
            set(gcf,'UserData',data);
            updateAtomSelection(rpos);
        end
    end

%% DRAG SELECTION ATOMS RECTANGLE - (MOUSE DRAG)
    function dragSelectRect(varargin)

        if (~isempty(gco))
            data = get(gcf,'UserData');
            axChild = get(gca,'Children');
            if ( (gco==gca) || sum(gco==axChild) )
                if (data.selectAtoms==1)
                    rpos = get(data.curRectH,'Position');
                    xy = get(varargin{1},'CurrentPoint');
                    [x,y] = figToAxe(xy);
                    axlim = axis();
                    if (x~=data.begSelectAtoms(1) && y~=data.begSelectAtoms(2) && ...
                            x>axlim(1) && x<axlim(2) && y>axlim(3) && y<axlim(4) )
                        newrpos = [ min(data.begSelectAtoms(1),x) min(data.begSelectAtoms(2),y) ...
                            abs(data.begSelectAtoms(1)-x) abs(data.begSelectAtoms(2)-y) ];
                        set(data.curRectH,'Position',newrpos);
                    end

                end
            end % if gco==gca
        end
    end

%% START TO UNSELECT ATOMS RECTANGLE (MOUSE CLICK)
%     function startUnSelectRect(varargin)
%         data = get(gcf,'UserData');
%         if (gco==gca) % Click on axis or one of its children
%             data.selectAtoms = 1;
%             % Get and check current mouse coordinates in axis
%             xy = get(varargin{1},'CurrentPoint');
%             [x,y] = figToAxe(xy);
%             data.begSelectAtoms = [x y];
%             axl = axis(gca);
%             rpos = [ x y 1e-6 1e-6 ];
%             data.curRectH = rectangle('Position', rpos,'faceColor',[1 1 1]);
%             % Swap Depth of plot (set rectangle==axChild(1) at the end of the
%             % gcf children handles)
%             axChild = get(gca,'Children');
%             set(gca,'Children',[axChild(axChild~=data.curRectH); axChild(axChild==data.curRectH)] );
%         end
%         set(gcf,'UserData',data)
%     end

%% STOP TO UNSELECT ATOMS RECTANGLE (MOUSE RELEASE)
%     function stopUnSelectRect(varargin)
%         axChild = get(gca,'Children');
%         if ( (gco==gca) || sum(gco==axChild) )
%             data = get(gcf,'UserData');
%             % Update userdata
%             data.selectAtoms = 0; % realeased
%             % Copy rectangle handle to vector of selection handles
%             if (ishandle(data.curRectH))
%                 data.rectUnSelH(end+1) = data.curRectH;
%                 data.curRectH = -1;
%             end
% 
%             % Get and check current mouse coordinates in axis
%             xy = get(varargin{1},'CurrentPoint');
%             [x,y] = figToAxe(xy);
%             % Update data information
%             data.atomUnSelection(end+1,:) = [ min(data.begSelectAtoms(1),x) max(data.begSelectAtoms(1),x) ...
%                 min(data.begSelectAtoms(2),y) max(data.begSelectAtoms(2),y)];
%             set(gcf,'UserData',data)
%             % 
%         end
%     end

%% Toggle toolbar icons to 'off' state when a icon is pressed
    function toggleToolbar(varargin)
       co = gcbo; % Remember current object (should be an icons)
       data = get(gcf,'UserData');
       % Untoggle all icons excpet current object
       for k=2:length(data.toolH)
           if ( strcmp(get(data.toolH(k),'Type'),'uitoggletool') && data.toolH(k)~=co )
               set(data.toolH(k),'State','off')
           end
       end
    end


%% RETURN THE INDEXES TO THE CORREPONDING ATOM TYPE and optional LENGTH in the book, 
%  return empty matrix[] if type is not found

    function idx = getTypeIndex(book,type,varargin)
      
      idx = [];
       
      nt = length(book.atom);
      for t=1:nt,
          if( strcmp(book.atom(t).type,type) )
              if (nargin==3)
                  if( book.atom(t).params.len(1,1) == varargin{1})
                      idx = [idx t];
                  end
              else
                  idx = [idx t];
              end
          end
      end
      
    end

%% Update Atom selection when a new rectangle has been added
    function updateAtomSelection(rpos)
        data = get(gcf,'UserData');
        % Init atom Handles vector
        nC = data.book.numChans;
        nA = data.book.numAtoms;
        fs = data.book.sampleRate;
        nT = length(data.book.atom);
        % Channel from current axis;
        chan = 1; %find(data.axH == gca);
        % Get Last Rectangle of selection coordinates
        %rpos = data.atomSelection(end,:); % xmin xmax ymin ymax
        
        nAS = 0; % Counter for the number of atoms selected
        for k = 1:nT,

             data.book.atom(k).params;
             % Process any visible atom
             ind = find( data.book.atom(k).visible(:,chan) == 1 );
             for n = 1:length(ind), %#ok<FNDSB>
                 a = ind(n);
                 
                 % Get atom bounds 
                 % Temporal position
                 t0  = data.book.atom(k).params.pos(a,chan)/fs;   % Position in seconds
                 dt0 = data.book.atom(k).params.len(a,chan)/fs;   % Length in seconds
                            
                 xmin = max(0,t0); % limit xmin to 0
                 xmax = min(data.book.numSamples, xmin + dt0); % limit xmax to numSaples
                 
                 % Frequential position (type dependent)
                 % Get Atom central freq and bandwidth for each type 
                 switch (data.book.atom(k).type)
                     case 'dirac'
                         f0 = 0;
                         df0 = 0.5*fs;
                     case 'constant'
                         f0 = 0;
                         df0 = 0.5 / dt0; 
                     case {'harmonic','gabor','mdct','mdst','mclt'}
                         f0  = fs*data.book.atom(k).params.freq(a,chan);
                         df0 = 0.5 / dt0;
                     otherwise 
                         f0 = 0.25 *fs;
                         df0 = 0.25*fs;
                 end

                 ymin = max(0, f0 - df0);   % limit ymin to 0
                 ymax = min(fs/2,f0 + df0); % limit ymax to fs/2

                 % An atom is selected if its support is fully included in
                 % rectangle selection
                 
                 if ( (xmin>=rpos(1)) && (xmax<=rpos(2)) && (ymin>=rpos(3)) && ymax<=(rpos(4)) )
                       data.book.atom(k).selected(a,chan) = 1;
                       nAS = nAS + 1;
                       % disp(['[' data.book.atom(k).type '] - atom ' num2str(a) ' selected'])
                 end
             end
             
        end
        disp([ '[' num2str(nAS) '] - atoms in new selection'])
        set(gcf,'UserData',data)
    end

%% RETURNS A VECTOR OF UIHANDLES TO CHECKBOX FOR SHOWING/HIDING ATOM PER TYPE
    function typeH = addCheckBoxTypes(book,figHandle)
        nTypes = length(book.atom);
        % Order type and length
        if (nTypes)
            types = struct(book.atom(1).type,book.atom(1).params.len(1,1));
            
            for t=2:nTypes
                typestr = book.atom(t).type;
                if (isfield(types,typestr))
                    types.(typestr) = [ types.(typestr) book.atom(t).params.len(1,1)];
                else
                    types.(typestr) = book.atom(t).params.len(1,1);
                end
            end

            %% ADD CHECKBOXES HANDLES
            fields = fieldnames(types);
            % count number of chekboxes to set their vertical size
            l = length(fields);
            for t=1:l,
                if (length(types.(fields{t})) > 1) % Add sub-checkbox items for each scales
                    l = l + length(types.(fields{t}));
                end
            end

            bgColor = [0.7 0.8 0.9];
            fgColor = [0.8 0.9 1];
            vSpace = 0.01;
            bHeight = min(0.05,(0.83/l-vSpace));
            figure(figHandle);
            tl=0; % Counter of type and len (number of checkboxes)
            
            % First CHECKBOX IS FOR VIEW/HIDE ALL ATOMS
            tl = tl + 1;
            typeH(tl) = uicontrol( ...
                'Style','checkbox', ...
                'Units','normalized', ...
                'BackgroundColor',fgColor,...
                'Position',[ 0.02 0.92-tl*(bHeight+vSpace) 0.17 bHeight ], ...
                'String','Show/Hide All', ...
                'Value',1, ...
                'Enable','on',...
                'Visible','on',...
                'Callback',@toggleViewAllAtom);
            
            % NEXT CHECKBOXES ARE FOR DIFFERENT ATOM TYPES AND LENGTHS
            for t=1:length(fields),
                % Atom length sub-checkbox - ATOM TYPE IS STORED IN 'Tag' property
                len = types.(fields{t});
                if (length(len) == 1)
                    % If there is only one scale for atom, don't display
                    % sub-checkboxes
                    tl = tl + 1;
                    typeH(tl) = uicontrol( ...
                            'Style','checkbox', ...
                            'Units','normalized', ...
                            'BackgroundColor',fgColor,...
                            'Position',[ 0.02 0.92-tl*(bHeight+vSpace) 0.17 bHeight ], ...
                            'String',[ fields{t} ' length: ' num2str(len)], ...
                            'Tag',fields{t}, ...
                            'Value',1, ...
                            'Enable','on',...
                            'Visible','on',...
                            'Callback',@toggleViewAtomLength);
                % Display Atom type and sub-items for each scale
                else
                    % Atom type checkbox
                    tl = tl + 1;
                    typeH(tl) = uicontrol( ...
                        'Style','checkbox', ...
                        'Units','normalized', ...
                        'BackgroundColor',fgColor,...
                        'Position',[ 0.02 0.92-tl*(bHeight+vSpace) 0.17 bHeight ], ...
                        'String',[ fields{t} ' length: ' ], ...
                        'Tag',fields{t}, ...
                        'Value',1, ...
                        'Enable','on',...
                        'Visible','on',...
                        'Callback',@toggleViewAtomType);
                    for l = 1:length(len)
                        tl = tl + 1;
                        typeH(tl) = uicontrol( ...
                            'Style','checkbox', ...
                            'Units','normalized', ...
                            'BackgroundColor',fgColor,...
                            'Position',[ 0.03 0.92-tl*(bHeight+vSpace) 0.15 bHeight ], ...
                            'String',num2str(len(l)), ...
                            'Tag',fields{t}, ...
                            'Value',1, ...
                            'Enable','on',...
                            'Visible','on',...
                            'Callback',@toggleViewAtomLength);
                    end
                end
            end
        end
    end

%% RETURNS A VECTOR OF HANDLES TO ATOMS - ONE HANDLE PER TYPE
    function atomH = plotBook(book,varargin)

        % Init atom Handles vector
        nC = book.numChans;
        nA = book.numAtoms;
        fs = book.sampleRate;
        nT = length(book.atom);

        % Choose axis handle vector (1 per channel)
        if nargin == 1
            fig = figure;
            axH = zeros(1,nC);
            for c = 1:nC
                axH(c) = subplot(nC,1,c);
                hold on
            end
        else
            axH = varargin{1};
            for c = 1:nC
             axes(axH(c));
             hold on;
            end
        end
        
        % Init the vector of handle, one per type of atoms
        atomH = zeros(length(book.atom),book.numChans); % 
        
        h = 1; % handle index in vector
        %wb = waitbar(0,'Plotting atom per types','Name','plotBook info');
        for k = 1:nT
            type = book.atom(k).type;
            params = book.atom(k).params;
 
            % Patch Coordinates 
            pX = [];
            pY = [];
            pZ = [];
            pC = [];
 
            % DEBUG DISPLAY
            %  disp(['plotBook: [' type '] atoms']);
            
            switch type
                case {'gabor','mclt','mdct','mdst'},
                    % Read all the atoms corresponding to this type
                    for a = 1:length(params.amp)
                        for chan = 1:nC
                            pos = params.pos(a,chan)/fs;   % Position in seconds
                            len = params.len(a,chan)/fs;   % Length in seconds
                            freq = fs*params.freq(a,chan); % Atom central frequency
                            bw2 = 0.5 / len;               % Atom bandwidth
                            amp = params.amp(a,chan);      % Amplitude
                            amp = max(-80,20*log10(abs(amp)));  % Set a minimum amp value : -80dB

                            if (strcmp(type,'gabor') ) % Chirped atoms
                                ch = fs*fs*params.chirp(a,chan);
                            else                                               % No chirp
                                ch = 0;
                            end
                            
                            pv = [pos; pos; pos+len; pos+len];    % Patch coordinates in X plane (time position)
                            fv = [freq-bw2; freq+bw2; freq+bw2+ch*len; freq-bw2+ch*len]; % Patch coordinates in Y plane (freq position)
                            av = [amp; amp; amp; amp];            % Patch coordinates in Z plane (Amplitude position)
     
                            pX = [pX, pv];
                            pY = [pY, fv];
                            pZ = [pZ, av];
                            pC = [pC, amp];
                       end
                    end
     
                case 'harmonic',
                    for a = 1:length(params.amp)
                        for chan = 1:nC
                            p = params.pos(a,chan)/fs;
                            l = params.len(a,chan)/fs;
                            bw2 = ( fs / (params.len(a,chan)/2 + 1) ) / bwfactor;
                            A = params.amp(a,chan);
                            f = params.freq(a,chan)*fs;
                            pv = repmat([p;p;p+l;p+l],1,params.numPartials(a,chan));

                            fv = f*params.harmonicity(a,:,chan);
                            ch = fs*fs*params.chirp(a,chan);
                            dfv = ch*l;
                            fvup = fv+bw2;
                            fvdown = fv-bw2;
                            fv = [fvup;fvdown;fvdown+dfv;fvup+dfv];

                            A = A*params.partialAmp(a,:,chan);
                            A = 20*log10(abs(A));

                            av = [A ; A; A; A];

                            pX = [pX, pv];
                            pY = [pY, fv];
                            pZ = [pZ, av];
                            pC = [pC, A];
                        end
                    end
                    
                case 'dirac',
                    for a = 1:length(params.amp)
                        for chan = 1:nC
                            pos = params.pos(a,chan)/fs;
                            len = 1/fs;
                            amp = params.amp(a,chan);
                            amp = max(-80,20*log10(abs(amp))); % Set a mininum amplitude value to -80dB

                            pv = [pos; pos+len; pos+len; pos];
                            fv = [0; 0; fs/2; fs/2];
                            av = [amp; amp; amp; amp];
                           
                            pX = [pX, pv];
                            pY = [pY, fv];
                            pZ = [pZ, av];
                            pC = [pC, amp];

                        end
                    end

                    % Atom types not processed
                otherwise,
                    disp( [ '[' type '] cannot be displayed yet.'] );
             end
            % Add patch to handle vector
            % DEBUG DISPLAY 
            %   disp( sprintf('min pX=%f, min pY=%f, min pZ=%f, min pC=%f',min(min(min(pX))),min(min(min(pY))),min(min(min(pZ))),min(min(min(pC))) ) )
            %   disp( sprintf('max pX=%f, max pY=%f, max pZ=%f, max pC=%f',max(max(max(pX))),max(max(max(pY))),max(max(max(pZ))),max(max(max(pC))) ) )
            % END OF DEBUG DISPLAY 
        
            for chan = 1:nC
                axes(axH(chan));
                if (~isempty(pX))
                    if (strcmp(type,'dirac'))
                        atomH(h,chan) = patch(pX,pY,100+pZ,pC,'FaceAlpha',1,'EdgeAlpha',1);
                    else
                        atomH(h,chan) = patch(pX,pY,100+pZ,pC,'FaceAlpha',1,'EdgeAlpha',1);
                    end
                else
                    atomH(h,chan) = -1;
                end
            end
           % waitbar(h/nT,wb);
            h = h+1;
        end
        %close(wb);
    end

end % End of bookedit.m (function declaration)

