function varargout = bookedit_exp( book, channel, bwfactor )
%function BOOKEDIT_EXP Interface for plotting and editing a Matching Pursuit book
%
%    BOOKEDIT_EXP 
%    with no arguments asks the user for a MPTK binary book file and plot
%    You can set a '.mat' variable called 'MPTKdir.mat' containing 
%    the full path to your book directory:
%       bookdir = '/my/path/to/mptk/book/';
%       save 'MPTKdir.mat' bookdir
%
%    BOOKEDIT_EXP('mptkBook.bin')
%    with a string to a book load the book 
%
%    BOOKEDIT_EXP( book, chan ) Read bookplots the channel number chan
%    of a MPTK book structure in the current axes.
%    If book is a string, it is understood as a filename and
%    the book is read from the corresponding file. Books
%    can be read separately using the BOOKREAD utility.
%
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

% Check if a bookDir has already been provided bu user
bdf = 'MPTKdir.mat'; % book dir file
if (exist(bdf,'file')==2)
    bd = load(bdf);
else
    disp([ 'You can set a default directory for opening books in a variable called ' ...
        '''MPTKdir.mat'' containing' 10 ...
        'the full path to your book directory: ' 10 'bookdir = ''/my/path/to/mptk/book/'';' 10 ...
        'save ''MPTKdir.mat'' bookdir' ]);

    bd.bookdir = pwd;
end

if nargin<1
    loadBook(bd.bookdir);
    return;
end


%% Test input args
if ischar(book),
    disp('Loading the book...');
    [p,n] = fileparts(book);
    data.loadBookDir = p;
    book = bookread_exp( book );
    % Add Fields in structure for graphical / selection information to atoms
    book = addMatlabBookFields(book);
    % Record path of the book for open book
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

if (nargout == 1)
    varargout(1) = { figH };
end
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
menuItem(item) = uimenu(menuItem(1),'Label','&Save visible to book','Callback',@saveVisibleBook,'Separator','off','Accelerator','V');
item = item + 1;
menuItem(item) = uimenu(menuItem(1),'Label','&Close window','Callback','close(gcf)','Separator','on','Accelerator','W');
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
menuItem(item) = uimenu(menuItem(2),'Label','&Refresh figure','Callback',@refreshFigure,'Separator','on','Accelerator','R');
item = item + 1;
% 'Transform' subitems
menuItem(item) = uimenu(menuItem(3),'Label','&Pitch Shift ...','Callback',@pitchShift,'Separator','off','Accelerator','P');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Time Stretch ...','Callback',@timeStretch,'Separator','off','Accelerator','T');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','Apply &Gain on selection ...','Callback',@applyGain,'Separator','off','Accelerator','G');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','T&ime Reverse selection ...','Callback',@timeReverse,'Separator','off','Accelerator','I');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','&Freq reverse selection ...','Callback',@freqReverse,'Separator','off','Accelerator','F');
item = item + 1;
menuItem(item) = uimenu(menuItem(3),'Label','Te&mpo detection ...','Callback',@tempoDetect,'Separator','on','Accelerator','M');
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
        'Position',[0.2675 ( vspace*ac +axHeight*(ac-1) ) 0.7 axHeight ], ...
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
data.toolH = toolH;         % Vector of toolbar icons handles
data.book = book;           % Vector of toolbar icons handles
data.axH = axH;           % Vector of toolbar icons handles
if (~isfield(data,'loadBookDir'))
    data.loadBookDir = pwd;
end
data.saveBookDir = data.loadBookDir;
set(figH,'UserData',data)

%% ------------------
%  CALLBACK FUNCTIONS
%  ------------------

%% FIGURE MENUS CALLBACKS
% -------------------------

%% OPEN AND LOAD A BOOK
% This load function can be called at different times:
% - At Initialisation with a directory (possibly saved in MPTKbookdir.mat)
% - from uimenu 'Open Book ...'
% - From a subfunction (typically after an operation on atoms)

    function loadBook(varargin)
        curFig = gcf;
        data = get(gcf,'UserData');
        curdir = pwd; % Save current directory
        % Just check if interface is already created or start program
        if (~isfield(data,'book'))
            close(curFig);
            cd(varargin{1})
            [filename, pathname] = uigetfile({'*.bin';'*.txt';'*.xml'},'Open a book file');
        else
            if (nargin==2) % Call from uimenu 'open book'
                % Ask the user a book name
                if (exist(data.loadBookDir,'dir') == 7) % sometimes this variable is lost ...                    
                    cd(data.loadBookDir);
                end
                [filename, pathname] = uigetfile({'*.bin';'*.txt';'*.xml'},'Open a book file');
            else   % Call from a subfunction
                [pathname,fn,e] = fileparts(varargin{1});
                filename = [ fn e ];
            end
        end
        cd(curdir); % Back to current directory

        if (filename)
            if (~isempty(pathname))
                data.loadBookDir = pwd;
            else
                data.loadBookDir = pathname;
            end
            bookname = fullfile(pathname,filename);
            if (exist(bookname,'file')==2)

                disp('Loading the book...');
                newFig = bookedit_exp( bookname );
                data = get(newFig,'UserData');
                data.loadBookDir = pathname;
                set(newFig,'UserData',data);
                if (curFig ~= newFig)
                    close(curFig);
                end
            else % Bookname does not exists, save book directory path (the rest is unchanged)
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
        % Copy selected info into index
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
        % Set atom.selected field to one
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
        data = get(gcf,'UserData');
        % Copy selected info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).selected;
        end
        % Get selected atom entries in index (atoms to remove)
        selectIndex = find(data.book.index(4,:)==1);
        
        % Remove atoms given index
        data.book = removeBookAtom(data.book,selectIndex);
        set(gcf,'UserData',data);
        refreshFigure();
        
        % Clear selection
        selectNone();
        
    end

    function keepSelection(varargin)
        % disp('keepSelection() - Not implemented')
        data = get(gcf,'UserData');
        % Copy selected info into index
        for t = 1:length(data.book.atom)
            data.book.index(4,data.book.index(2,:)==t) = data.book.atom(t).selected;
        end
         % Get non selected atom entries in index
        selectIndex = find(data.book.index(4,:)~=1);
        
        % Remove atoms given index
        data.book = removeBookAtom(data.book,selectIndex);
        set(gcf,'UserData',data);
        refreshFigure();

        % Clear selection
        selectNone();
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
        prompt = {'Enter the gain to apply to selected atom in dB:'};
        name = 'Apply gain to selection';
        numlines = 1;
        defaultanswer = {'3'};
        val = inputdlg(prompt,name,numlines,defaultanswer);
        gaindB=str2double(val);
        if isnan(gaindB), 
            errordlg('You must enter a numeric value','Bad Input','modal');
        else
            data = get(gcf,'UserData');
            % Convert gain in dB to linear scale 
            gain = 10.^(gaindB/20);
            % Browse atoms type and apply gain on selected atoms
            for t = 1:length(data.book.atom)
               data.book.atom(t).params.amp(data.book.atom(t).selected==1,:) = data.book.atom(t).params.amp(data.book.atom(t).selected==1,:) * gain;
            end
            set(gcf,'UserData',data);
            refreshFigure();
        end
        
    end

    function pitchShift(varargin)
        % Ask the user for pitch shift parameters
        inputPitchShift(); 
        % When validating user parameters 'ok' button 
        % launches @applyPitchShift()
    end

    function timeStretch(varargin)
        figBookedit = gcbf;
        % Get input arguments
        d = inputTimeStretch();
        uiwait(d);
        if (ishandle(d)) % OK button pushed
            args = get(d,'UserData');
            close(d);

            % Core function for applying pitchShift
            data = get(figBookedit,'UserData');
            data.book = applyTimeStretch(data.book,args);
            set(figBookedit,'UserData',data);
            refreshFigure();
        end

    end

    function timeReverse(varargin)
        disp('timeReverse() - not implemented')
    end

    function freqReverse(varargin)
        disp( 'freqReverse() - not implemented')
    end
   function tempoDetect(varargin)
        disp( 'tempoDetect() - not implemented')
    end

%% SUB FUNCTIONS
%  -------------


%% Get index entries of visible atoms (in all atom type)
    function index = indexOfVisible(book)
        index = zeros(1,book.numAtoms);
        for t = 1:length(book.atom)
            index(1,book.index(2,:)==t) = book.atom(t).visible;
        end
    end

%% Get index entries of selected atoms (in all atom type)
    function index = indexOfSelected(book)
        index = zeros(1,book.numAtoms);
        for t = 1:length(book.atom)
            index(1,book.index(2,:)==t) = book.atom(t).visible;
        end
    end

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

            signal = mpReconstruct(book);
            if (~isempty(signal))
                soundsc(signal,book.sampleRate);
                PlotSoundJava(signal,book.sampleRate);
            end
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
            for nv = 1:length(ind),
                a = ind(nv);

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
                        f0  = fs*data.book.atom(k).params.freq(a);
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

%% Reconstruct Signal from book using external system command mpr (needs
% MPTK_CONFIG_FILENAME) to be set
% IMPORTANT NOTE: THIS FUNCTION SHOULD BE REPLACED BY MEX INTERFACE "BOOKMPR_EXP()"
%                 WHEN IT IS WORKING (CURRENTLY GIVES A CURIOUS SEG FAULT PROBABLY BECAUSE OF THREADS)
    function sig = mpReconstruct(book)

        % GET MPTK BINARY DIRECTORY
        [mptkPath,mptkConfig] = fileparts(getenv('MPTK_CONFIG_FILENAME'));
        if (isempty(mptkPath))
            f = warndlg([ 'Environment variable MPTK_CONFIG_FILENAME is not set' ...
                10 'Hope ''mpr'' binary will be found' ], 'MPTK BINARY WARNING', 'modal');
        end

        tmpbook = 'booktemp.bin';
        tmpwav = 'tempmpr.wav';
        % Save temp book
        wb = waitbar(0,'Exporting visible part of book','Name','Play visible atoms');
        bookwrite_exp(book,tmpbook);

        % Reconstruct book with mpr in tempmpr.wav
        waitbar(0.3,wb,'Reconstructing book');
        exec = 'mpr';
        c = computer;
        switch c
            case 'PCWIN',
                execname = ['"' fullfile(mptkPath,[exec '.exe']) '"'];
            case {'MAC','MACI','GLNX86'},
                execname = ['"' fullfile(mptkPath,exec) '"'];
            otherwise
                disp('Exotic platform ... it may work anyhow')
                execname = ['"' fullfile(mptkPath,exec) '"'];
        end

        command = [ execname ' ' tmpbook ' ' tmpwav ];
        [s,w] = system(command);

        % Loadsound into matlab variable
        waitbar(0.6,wb,'Load and play sound');
        if (s==0)
            [sig,Fs] = wavread(tmpwav);
        else
            sig = [];
            waitbar(0.9,wb,'Problem using mpr');
            disp('Problem using mpr')
        end

        % Delete tmpfiles
        waitbar(0.9,wb,'Deleting temp files');
        delete(tmpbook)
        delete(tmpwav)
        close(wb)
    end



%% PATCH THE BOOK STRUCTURE WITH TWO FIELDS FOR MATLAB graphical / selection information to atoms
% add 2 fields in atom structure :
%   - 'selected' for selection information
%   - 'visible' for plotting information
    function newbook = addMatlabBookFields(b)
        for t = 1:length(b.atom)
            [nA, nC] = size(b.atom(t).params.amp);
            b.atom(t).selected = zeros(nA,nC);
            b.atom(t).visible = ones(nA,nC);
        end
        newbook = b;
    end

%% INPUT DIALOG FIGURE FOR TIME STRETCH ARGUMENTS
% create a figure in which the user data is a structure with fields
% scaleVal: (double) Time stretch factor
% scaleLength: (bool) 0: scale position and length of atoms, 1: scale only pos
% applyTo: 'visible', 'selected' or 'all'
%
% called by @timeStretch()

    function dialogH = inputTimeStretch(varargin)
        % Get Arguments
        dialogH = dialog('Name','Time strecth arguments','Units','Normalized','Position',[0.3 0.4 0.3 0.2]);

        args.scaleVal = 2;
        args.scaleLength = 0;
        args.applyTo = 'all';
        
        % RADIO button group for specifying to which atoms apply the pich shift
        h1 = uibuttongroup('visible','off','Position',[0.05 0.65 .9 .3]);
        % Create 2 radio buttons in the button group.
        u1 = uicontrol('Style','Radio','String','Apply to all atoms',...
            'Units','normalized','Tag','all', ...
            'pos',[0.05 0.03 0.8 0.3],'parent',h1,'HandleVisibility','off');
        uicontrol('Style','Radio','String','Apply to selected atoms',...
            'Units','normalized','Tag','selected', ...
            'pos',[0.05 0.33 0.8 0.3],'parent',h1,'HandleVisibility','off');
        uicontrol('Style','Radio','String','Apply to visible atoms',...
            'Units','normalized','Tag','visible', ...
            'pos',[0.05 0.66 0.8 0.3],'parent',h1,'HandleVisibility','off');
        
        % Initialize some button group properties.
        set(h1,'SelectionChangeFcn','args = get(gcbf,''UserData''); args.applyTo = get(gcbo,''Tag''); set(gcbf,''UserData'',args);');
        set(h1,'SelectedObject',u1);  % 'all' selected
        set(h1,'Visible','on')
        
        % Text edit for Stretching factor (double)
        uicontrol('Style','Text','Units','normalized', ...
            'Position',[0.05 0.5 0.45 0.1],...
            'String','Enter time stretch factor ]0,+inf[');
        uicontrol('Style','Text','Units','normalized', ...
            'Position',[0.05 0.4 0.9 0.1],...
            'String','value<1 : compress time, value>1 : expand time');
        args.scaleH = uicontrol('Style','edit', ...
            'Units','normalized', ...
            'Enable','on',...
            'Visible','on',...
            'Position',[0.5 0.525 0.45 0.1], ...
            'String',num2str(args.scaleVal), ...
            'Callback', [ 'args = get(gcbf,''UserData''); val=str2double(get(args.scaleH,''String'')); ' ...
            'if isnan(val), errordlg(''You must enter a numeric value'',''Bad Input'',''modal'');return; end; ' ...
            'args.scaleVal = val; set(gcbf,''UserData'',args);']);

        % Checkbox for limiting atoms length (don't shift atoms below a given length)
        uicontrol( ...
            'Style','checkbox', ...
            'Units','normalized', ...
            'Position',[0.05 0.25 0.9 0.15], ...
            'String','Don''t scale atom length (only start)', ...
            'Value',0, ...
            'Enable','on',...
            'Visible','on',...
            'Callback',[ 'args = get(gcbf,''UserData'');  '...
            'args.scaleLength=get(gcbo,''Value''); ' ...
            'set(gcbf,''UserData'',args);' ]);

        % Ok and cancel buttons
        % OK Button, resume ui
        uicontrol('Style','pushbutton', ...
            'Units','normalized', ...
            'Position',[ 0.6 0.05 0.3 0.15 ], ...
            'String','OK', ...
            'Visible','on',...
            'Callback','uiresume(gcbf)');

        % Cancel action - just close argument gui
        uicontrol('Style','pushbutton', ...
            'Units','normalized', ...
            'Position',[ 0.1 0.05 0.3 0.15 ], ...
            'String','Cancel', ...
            'Enable','on',...
            'Visible','on',...
            'Callback','close(gcbf);');

        set(dialogH,'UserData',args);
    end

%% CREATE A LITTLE GUI TO ASK THE USER FOR PITCH SHIFT INPUT ARGUMENTS
% Called by @pitchShift()
    function inputPitchShift(varargin)
        data = get(gcf,'UserData');
        % Init Pitch Shift algo arguments
        data.args = [];
        data.args.compensatePhase = 0; % Bool for phase optimization
        data.args.useLowLimit = 0;     % Bool for lower limit on atom length for freq shifting
        data.args.lowLimitVal = 0;     % value (int) lower limit on atom length for freq shifting
        data.args.scaleVal = -12;        % value (int or float) for shifting atom length
        data.args.scaleType = 'semitone'; % String 'hertz' or 'semitone' for shifting atoms
        data.args.applyTo = 'all';     % String 'all' or 'selected' or 'visible', to what apply pitch shift
        
        psArgH = figure('Name','Bookedit - Pitch Shift arguments', ...
            'NumberTitle','off', ...
            'Backingstore','off', ...
            'Units','normalized', ...
            'Position',[ 0.3 .3 .3 .3 ], ...
            'MenuBar','none',...
            'Visible','on');
        
        % RADIO button group for Semitone or Hertz
        h = uibuttongroup('visible','off','Position',[0.1 0.7 .85 .25]);
        % Create 2 radio buttons in the button group.
        u0 = uicontrol('Style','Radio','String','Pitch shift in Semitones',...
            'Units','normalized','Tag','semitone', ...
            'pos',[0.05 0.075 0.5 0.35],'parent',h,'HandleVisibility','off');
        uicontrol('Style','Radio','String','Pitch Shift factor in Hertz',...
            'Units','normalized','Tag','hertz', ...
            'pos',[0.05 0.525 0.5 0.35],'parent',h,'HandleVisibility','off');

        % Initialize some button group properties.
        set(h,'SelectionChangeFcn','data = get(gcf,''UserData''); data.args.scaleType = get(gcbo,''Tag''); set(gcf,''UserData'',data);');
        set(h,'SelectedObject',u0);  % Default semitones
        set(h,'Visible','on')

        % RADIO button group for specifying to which atoms apply the pich shift
        h1 = uibuttongroup('visible','off','Position',[0.1 0.4 .85 .25]);
        % Create 2 radio buttons in the button group.
        u1 = uicontrol('Style','Radio','String','Apply to all atoms',...
            'Units','normalized','Tag','all', ...
            'pos',[0.05 0.03 0.8 0.3],'parent',h1,'HandleVisibility','off');
        uicontrol('Style','Radio','String','Apply to selected atoms',...
            'Units','normalized','Tag','selected', ...
            'pos',[0.05 0.33 0.8 0.3],'parent',h1,'HandleVisibility','off');
        uicontrol('Style','Radio','String','Apply to visible atoms',...
            'Units','normalized','Tag','visible', ...
            'pos',[0.05 0.66 0.8 0.3],'parent',h1,'HandleVisibility','off');
        
        % Initialize some button group properties.
        set(h1,'SelectionChangeFcn','data = get(gcf,''UserData''); data.args.applyTo = get(gcbo,''Tag''); set(gcf,''UserData'',data);');
        set(h1,'SelectedObject',u1);  % 'all' selected
        set(h1,'Visible','on')
   
        % Popup Menu with number of semitones for pitch
        for k=1:25
            semiStr{k} = k-13;
        end
        % Popup for shifting semitones (int)
        data.args.semiH = uicontrol( ...
            'Style','popupmenu', ...
            'Units','normalized', ...
            'Position',[0.55 0.0525 0.4 0.25], ...
            'String',semiStr,...
            'parent',h,'HandleVisibility','off', ...
            'Tag','semitone',...
            'Value',1,...
            'Callback', [ 'data = get(gcf,''UserData''); vals = get(data.args.semiH,''String''); val=str2double(vals{get(data.args.semiH,''Value'')}); ' ...
            'data.args.scaleVal = val; set(gcf,''UserData'',data);']);

        % Text edit for shifting by hertz (double)
        data.args.hertzH = uicontrol('Style','edit', ...
            'Units','normalized', ...
            'Position',[0.55 0.475 0.4 0.4], ...
            'String','0', ...
            'Value',1, ...
            'parent',h,'HandleVisibility','off',...
            'Callback', [ 'data = get(gcf,''UserData''); val=str2double(get(data.args.hertzH,''String'')); ' ...
            'if isnan(val), errordlg(''You must enter a numeric value'',''Bad Input'',''modal'');return; end; ' ...
            'data.args.scaleVal = val; set(gcf,''UserData'',data);']);


        % Checkbox for limiting atoms length (don't shift atoms below a given length)
        u4 = uicontrol( ...
            'Style','checkbox', ...
            'Units','normalized', ...
            'Position',[ 0.1 0.25 0.5 0.075], ...
            'String','Don''t shift atoms with length <=', ...
            'Value',0, ...
            'Enable','on',...
            'Visible','on',...
            'Callback',[ 'data = get(gcf,''UserData'');  '...
            'data.args.useLowLimit=get(gcbo,''Value''); ' ...
            'set(gcf,''UserData'',data);' ]);

        % Checkbox for compensating phase of atoms length
        for k=1:15
            lowStr{k} = 2^k;
        end
        data.args.lowLimitVal = lowStr{1};
        data.args.lowH = uicontrol( ...
            'Style','popupmenu', ...
            'Units','normalized', ...
            'Position',[ 0.65 0.25 0.25 0.075], ...
            'String',lowStr,...
            'Tag','semitone',...
            'Value',1,...
            'Callback', [ 'data = get(gcf,''UserData''); vals = get(data.args.lowH,''String''); val=str2double(vals{get(data.args.lowH,''Value'')}); ' ...
            'data.args.lowLimitVal = val; set(gcf,''UserData'',data);']);

        u5 = uicontrol( ...
            'Style','checkbox', ...
            'Units','normalized', ...
            'Position',[ 0.1 0.15 0.3 0.075], ...
            'String','Compensate phase', ...
            'Value',0, ...
            'Enable','on',...
            'Visible','on',...
            'Callback',[ 'data = get(gcf,''UserData''); '...
            'data.args.compensatePhase=get(gcbo,''Value''); ' ...
            'set(gcf,''UserData'',data);' ]);

        % OK Button, becomes valid when a popup is chosen
        % Launch function
        uicontrol('Style','pushbutton', ...
            'Units','normalized', ...
            'Position',[ 0.6 0.05 0.3 0.075 ], ...
            'String','OK', ...
            'Value',1, ...
            'Visible','on',...
            'Callback',@applyPitchShift);

        % Cancel action - just close argument gui
        uCancel = uicontrol('Style','pushbutton', ...
            'Units','normalized', ...
            'Position',[ 0.1 0.05 0.3 0.075 ], ...
            'String','Cancel', ...
            'Value',1, ...
            'Enable','on',...
            'Visible','on',...
            'Callback','close(gcf);');

        set(psArgH,'UserData',data);
    end

%% Apply pitch Shift algorithm to 
    function applyPitchShift(varargin)
        % Get arguments and close little gui
        data = get(gcf,'UserData');
        close(gcf);
        % data.args structure has field : 
        %    - compensatePhase: Recompute phase problems (0 or 1)
        %    - useLowLimit: Don't scale atoms with small length (0 or 1)
        %    - lowLimitVal: Lower scale value for scaling atoms (int)
        %    - applyTo: 'all' or 'seleted' or 'visible'
        %    scaleType: 'hertz' or 'semitone'
        %    scaleVal: (int of float) shift value according to shift type
        
        % data.args % debug display
        
        nTypes = length(data.book.atom);
        % Get the index of atoms to apply the pitch shift
        switch (data.args.applyTo) 
            case 'selected'
               indApply = find(data.book.index(:,2)==1);  %% Todo
            case 'visible' 
               indApply = find(data.book.index(:,2)==1);  %% Todo
            otherwise % case 'all'
               indApply = (1:size(data.book.index,2));
        end
        
        % Restrict according to atoms length is useLowLimit==1
        if (data.args.useLowLimit==1)
            indinf = [];
            for inda = 1:length(indApply)
                aType = data.book.index(2,indApply(inda));
                aNum  = data.book.index(3,indApply(inda));
                if (data.book.atom(aType).params.len(aNum) <= data.args.lowLimitVal)
                    indinf = [ indinf inda ];
                end
            end
            indApply(indinf) = []; % Remove these atoms from atoms to shift
        end
        
        % Calculate Pitch shift factor
        switch(data.args.scaleType)
            case 'semitone',
                pitchScale = exp(data.args.scaleVal/12 * log(2));
            case 'hertz',
                pitchScale = data.args.scaleVal;
            otherwise
                pitchScale = 1;
                disp(['Unknown scale type ' data.args.scaleType])
        end
        
        disp(['Pitch shift factor : ' num2str(pitchScale) ])

        % Apply Pitch shift factor to the atoms
        nApply = length(indApply);
        wb = waitbar(0,'Pitch shift atoms','Name','Pitch shift ...');
        removeAtomIndex = [];
        for inda = 1:nApply
            waitbar(inda/nApply,wb); % update waitbar
            % Get shortcuts for atom index
            aType = data.book.index(2,indApply(inda));
            aNum  = data.book.index(3,indApply(inda));
            % Different shift rule according to atom type
            switch(data.book.atom(aType).type)
                case {'gabor','mclt','mdct','mdst','harmonic'},
                    newfreq = data.book.atom(aType).params.freq(aNum) * pitchScale;
                    % Compensate phase
                    if (data.args.compensatePhase)
                        if (isfield(data.book.atom(aType).params,'phase'))
                            % Generate a random phase in [0,2*PI]
                        data.book.atom(aType).params.phase = 2*pi*randn(size(data.book.atom(aType).params.phase));
                        end
                    end
                    % Check that shifted atom is not over Fs
                    if (newfreq < data.book.sampleRate/2)
                        data.book.atom(aType).params.freq(aNum) = newfreq;
                    else % Mark atom to be removed
                        removeAtomIndex = [removeAtomIndex indApply(inda)];
                    end
                otherwise,
                    disp(['No rule for pitch shifting atom type:' data.book.atom(aType).type])
            end
        end
        % Remove aliased atoms 
        if (~isempty(removeAtomIndex))
            disp('Removing aliased atoms')
            data.book = removeBookAtom(data.book,removeAtomIndex);
        end
        close(wb);
        set(gcf,'UserData',data);
        % Redraw book
        refreshFigure();
    end


%% PROCESS TIME STRETCH ON BOOK ATOMS
% Input arguments structure has fields :
%  - scaleVal: (double) time stretch factor value<1= compress, value>1=expand
%  - scaleLength: (bool) Scale atom length or only atoms position
%  - applyTo: (string) 'all' 'selected' or 'visible'
    function newbook = applyTimeStretch(oldbook,args)
        % Copy book
        newbook = oldbook;
        % Get the index of atoms to apply the pitch shift
        switch (args.applyTo) 
            case 'selected'               
               indApply = find(indexOfSelected(newbook)==1);
            case 'visible' 
               indApply = find(indexOfVisible(newbook)==1);
            otherwise % case 'all'
               indApply = (1:newbook.numAtoms);
        end
        
        aType = newbook.index(2,indApply);
        aNum  = newbook.index(3,indApply);
        [uType,i,j] = unique(aType); % Get types without doublons
        wb = waitbar(0,'Time Stretch atoms','Name','Time Stretch ...');
        % Remove corresponding params in atom struct
        for t = 1:length(uType)            
            % Get index of atom to remove in each atom struct
            ind = find(aType==uType(t));

            % Scale atom position
            newbook.atom(uType(t)).params.pos(aNum(ind),:) = round(newbook.atom(uType(t)).params.pos(aNum(ind),:) * args.scaleVal);

            % Scale Length            
            if (args.scaleLength==0)
                newbook.atom(uType(t)).params.len(aNum(ind),:) = round(newbook.atom(uType(t)).params.len(aNum(ind),:) * args.scaleVal);
            end
            wb = waitbar(0.9*t/length(uType),wb);
        end
        newbook.numSamples = bookLength(newbook);
        close(wb);
        refreshFigure();
        
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

%% Refresh book plot - Used after a transformation on the book
    function refreshFigure(varargin)
        data = get(gcf,'UserData');
        
        % Clear Patches and Checkbox handles
        delete(data.typeHandles(ishandle(data.typeHandles)));
        delete(data.atomHandles(ishandle(data.atomHandles)));
        data.typeHandles = [];
        data.atomHandles = [];
        set(gcf,'UserData',data);
        
        % Redraw patches and checkboxes
        data.typeHandles = addCheckBoxTypes(data.book,gcf);        
        data.atomHandles = plotBook(data.book,axH);
        
        % Redraw selection
        % Todo if necessary (should be yet applied to book structure)
        
        % Store data structure
        set(gcf,'UserData',data)
        
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
                axis([0 (book.numSamples/book.sampleRate) 0 book.sampleRate/2]);
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

            % Init Patch Cell (1xnChans)
            pX = cell(1,nC);
            pY = cell(1,nC);
            pZ = cell(1,nC);
            pC = cell(1,nC);

            % Patch Coordinates
            for chan = 1:nC
                pX{chan} = [];
                pY{chan} = [];
                pZ{chan} = [];
                pC{chan} = [];
            end

            % DEBUG DISPLAY
            %  disp(['plotBook: [' type '] atoms']);

            switch type
                case {'gabor','mclt','mdct','mdst'},
                    % Read all the atoms corresponding to this type
                    for a = 1:length(params.amp)
                        for chan = 1:nC
                            pos = params.pos(a,chan)/fs;   % Position in seconds
                            len = params.len(a,chan)/fs;   % Length in seconds
                            freq = fs*params.freq(a); % Atom central frequency
                            bw2 = 0.5 / len;               % Atom bandwidth
                            amp = params.amp(a,chan);      % Amplitude
                            amp = max(-80,20*log10(abs(amp)));  % Set a minimum amp value : -80dB

                            if (strcmp(type,'gabor') ) % Chirped atoms
                                ch = fs*fs*params.chirp(a);
                            else                                               % No chirp
                                ch = 0;
                            end

                            pv = [pos; pos; pos+len; pos+len];    % Patch coordinates in X plane (time position)
                            fv = [freq-bw2; freq+bw2; freq+bw2+ch*len; freq-bw2+ch*len]; % Patch coordinates in Y plane (freq position)
                            av = [amp; amp; amp; amp];            % Patch coordinates in Z plane (Amplitude position)

                            pX{chan} = [pX{chan}, pv];
                            pY{chan} = [pY{chan}, fv];
                            pZ{chan} = [pZ{chan}, av];
                            pC{chan} = [pC{chan}, amp];
                        end
                    end

                case 'harmonic',
                    for a = 1:length(params.amp)
                        for chan = 1:nC
                            p = params.pos(a,chan)/fs;
                            l = params.len(a,chan)/fs;
                            bw2 = ( fs / (params.len(a,chan)/2 + 1) ) / bwfactor;
                            A = params.amp(a,chan);
                            f = params.freq(a)*fs;
                            pv = repmat([p;p;p+l;p+l],1,params.numPartials(a));

                            fv = f*params.harmonicity(a,:);
                            ch = fs*fs*params.chirp(a);
                            dfv = ch*l;
                            fvup = fv+bw2;
                            fvdown = fv-bw2;
                            fv = [fvup;fvdown;fvdown+dfv;fvup+dfv];

                            A = A*params.partialAmp(a,:,chan);
                            A = 20*log10(abs(A));

                            av = [A ; A; A; A];

                            pX{chan} = [pX{chan}, pv];
                            pY{chan} = [pY{chan}, fv];
                            pZ{chan} = [pZ{chan}, av];
                            pC{chan} = [pC{chan}, A];
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

                            pX{chan} = [pX{chan}, pv];
                            pY{chan} = [pY{chan}, fv];
                            pZ{chan} = [pZ{chan}, av];
                            pC{chan} = [pC{chan}, amp];

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
                % Scale patch Color between 0 and 1
                cmin = min(min(pC{chan}));
                cmax = max(max(pC{chan}));
                pC{chan} = (pC{chan} - cmin) / (cmax-cmin);
                % Plot patch in channel axeHandle
                axes(axH(chan));
                if (~isempty(pX{chan}))
                    if (strcmp(type,'dirac'))
                        atomH(h,chan) = patch(pX{chan},pY{chan},100+pZ{chan},pC{chan},'FaceAlpha',1,'EdgeAlpha',1);
                    else
                        atomH(h,chan) = patch(pX{chan},pY{chan},100+pZ{chan},pC{chan},'FaceAlpha',1,'EdgeAlpha',1);
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

%% Remove Atoms with given index vector from book 
    function newbook = removeBookAtom(oldbook,index)
        % Copy book
        newbook = oldbook;
        aType = newbook.index(2,index);
        aNum  = newbook.index(3,index);
        [uType,i,j] = unique(aType); % Get types without doublons
        % Remove corresponding params in atom struct
        for t = 1:length(uType)
            % Get index of atom to remove in each atom struct
            ind = find(aType==uType(t));

            % Clear visible and selected fields
            if isfield(newbook.atom(uType(t)),'visible')
                newbook.atom(uType(t)).visible(aNum(ind),:) = [];
            end
            if isfield(newbook.atom(uType(t)),'selected')
                newbook.atom(uType(t)).selected(aNum(ind),:) = [];
            end
            % Clear all params fields
            f = fieldnames(newbook.atom(uType(t)).params);
            for g = 1:length(f)
                newbook.atom(uType(t)).params.(f{g})(aNum(ind)) = []; %#ok<FNDSB>
            end
        end
        
        % Remove atoms entries in index
        newbook.index(:,index) = [];
        % Regenerate atoms number in index for the remaining atoms
        removeType = []; % Remove type if it is empty
        for t = 1:length(uType)
            ind = find(newbook.index(2,:)==uType(t)); % Seek atoms with correct index           
            if (isempty(ind))
                removeType = [removeType uType(t)]; % Collect empty atom indexes
            else
                newbook.index(3,ind) = 1:length(ind);     % Make their number 1 to new length                
            end
        end
        % Remove empty atom types
        if (~isempty(removeType))
            % Clear empty atom 
            newbook.atom(removeType) = [];
            % reassign atom type number in index
            [uType,i,j] = unique(newbook.index(2,:));
            newType = 1:length(uType);
            newbook.index(2,:) = newType(j);
        end
            
        % Update book length
        newbook.numAtoms = newbook.numAtoms - length(index);
        disp([ '[' num2str(length(index)) '] atoms removed from book'])
 
    end

%% bookLength : recompute book length for atoms
% usefull after Time Stretching
    function newLength = bookLength(book)
       newLength = 0;
       for t=1:length(book.atom);
           maxTypeLength = max(max(book.atom(t).params.pos + book.atom(t).params.len));
           newLength = max(newLength,maxTypeLength);           
       end
    end


end % End of bookedit.m (function declaration)

