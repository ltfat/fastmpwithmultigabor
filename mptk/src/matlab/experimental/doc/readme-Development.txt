      Development of Matlab experimental features
---------------------------------------------------------


The features developped in this directory aim at gathering and replacing the 
various MPTK / Matlab interfaces efforts that have been made by different developpers
(bookread/bookwrite, dictread/dictwrite, bookedit, MAT2MPTK-LAM ...)

This readme gives information for developpers on the following points:
1. Mex interfaces: read/write books (and dictionnary-todo)
2. The matlab "mptk book" structure
3. Bookedit interface: an alternative to bookplot that allows to edit MPTK books

Please feed it while you develop further tools.

Document main history:
----------------------
2008-03-25 - creation of this document - Gilles Gonon


-----------------------
1. MEX INTERFACES
-----------------------
The core mex interface with MPTK is gathered in a few classes that interact with the MPTK library.

The mex functions bookread/bookwrite(_exp) are thus simplified:
 - check the sanity of input/output arguments and class constructor.
 - Use the mxBook class constructor and method to parse MPTK and MATLAB book

The different classes are defined in subdirectory "classes/":
 - Class atomGroup (defined in mxBook.h and mxBook.cpp) :
 --------------------------------------------------------
     class for parsing MP_Atom and filling mex structure of atoms per type
     (used by functions for exporting / importing MP_Book )

 The matlab 'book.atom' structure contains the following fields:
     - type : string
     - params: structure of atoms parameters arrays, whose field are type dependent

-----------------------------------------
2. THE MATLAB BOOK STRUCTURE DEFINITION
-----------------------------------------

The book structure has been fully rewritten to fit "bookedit" needs and to allow fast operations on atoms.
This structure is extensively used in bookedit_exp.m so you can find many examples of processing a book with
matlab.

The original idea for the organization of the book is that atoms with the same sets of parameters (atoms
of the same type: gabor, dirac, mdct, constant, ....) are gathered in atom sub structures for fast group processing.
In order to further ease common atom manipulations, this idea has evolved into also grouping atoms of the same lengths.
Now, in the book structure, there are as many "atom" as [atoms type*length].

This leads to "book.atom" structures rather close to the different "blocks" in the MP_dictionnary definition.

The matlab 'book' structure contains the following fields:
    - numAtoms : number of atoms in book
    - numChans : number of channel in book 
    - numSamples : number of samples covered by the reconstructed book
    - sampleRate : signal samplerate
    - index: [ (4+numChans) x numAtoms matrix] : Index for reading/querying/sorting atoms with their occurrence in book
         1: Atom number in book (1 to numAtoms)
         2: Atom type
         3: Atom number in atom(type) structure
         4: Atom selected or not (used for saving part of a book) [NB: it may be better to remove this from the index sin ce this is a mask, not an index]
         4+channel: Atom position for channel 'channel' (1 to numChans)
    - atom:  [1 x Ntype struct]
         +- type : string
         +- params: structure of atoms parameters arrays, whose field are type dependent

The book.index matrix is convenient for fast indexing of the different atom types, and also to allow the
selection of some atoms in the book. See bookedit_exp subfunction applyTimeStretch for an example of 
group processing a selection of atoms.

-----------------------
2. BOOKEDIT MATLAB GUI
-----------------------
See and update doc/userman-bookedit.{odt,pdf} for user manual instructions.

The development and maintenance of bookedit should be quite simple and straightforward.

This GUI makes intensive use of function_handles ( denoted with @functionName ).
All the differents callbacks of the GUI are function handles which refer to sub functions.
As a consequence, the code is divided in a large number of subfunctions. A tip to navigate inside 
the code is to select the function name, right-click and "open selection" or F4 to jump to the sub-function.

The data (book info, selection info, figure handles and other variables) are organised in structure 
and are stored inside the figure.
The data structure is loaded at the beginning of many functions and saved back at their end using the commands:
data = get(gcbf,'UserData'); % load data
set(gcbf,'UserData',data);   % save data

'bookPlot' and 'applyTimeStretch' are quite complete examples of sub function for book and figure manipulations.


Here is a list of sub-function (grep 'function' bookedit_exp.m ):
-------------------------------
- FILE MENU CALLBACKS
loadBook(varargin)
saveVisibleBook(varargin)
saveSelectedBook(varargin)

- EDIT MENU CALLBACKS
selectAll(varargin)
selectNone(varargin)
cutSelection(varargin)
moveSelection(varargin) -> ALSO LINK TO TOOLBAR ICONS FUNCTIONS
keepSelection(varargin)
exportAnywave(varargin)

- HELP MENU CALLBACKS
aboutBookedit(varargin)

- TOOLBAR ICONS CALLBACKS
playvisiblesound(varargin)
playselectedsound(varargin)
toggleOnSelectAtoms(varargin)
clearMouseFcn(varargin)
zoomHorizontal(varargin)
zoomVertical(varargin)
panPlot(varargin)
zoomIn(varargin)
zoomOutFull(varargin)

- TOGGLE VIEW ATOM TYPE / LENGTH CALLBACKS
toggleViewAllAtom(varargin)
toggleViewAtomType(varargin)
toggleViewAtomLength(varargin)

- TRANSFORM MENU CALLBACKS
applyGain(varargin)
pitchShift(varargin)
timeStretch(varargin)
timeReverse(varargin)                       TODO
freqReverse(varargin)                       TODO
tempoDetect(varargin)                       TODO

- OTHER SUB FUNCTIONS
index = indexOfVisible(book)                TOFIX
index = indexOfSelected(book)               TOFIX
newsavedir = writeBook(book,defaultDir)
playBook(book)
[x,y] = figToAxe(curpoint)
startSelectRect(varargin)
stopSelectRect(varargin)
dragSelectRect(varargin)
startDragRect(varargin)
moveDragRect(varargin)
stopDragRect(varargin)
toggleToolbar(varargin)
[index,atomBounds] = findAtomInRect(rpos)
idx = getTypeIndex(book,type,varargin)
updateAtomSelection(rpos)
sig = mpReconstruct(book)                   CHECK IF IT MAKES A SEGMENTATION FAULT (depends on libMPTK)
newbook = addMatlabBookFields(b)
dialogH = inputTimeStretch(varargin)
dialogH = inputPitchShift(varargin)
newbook = applyPitchShift(oldbook,args)
newbook = applyTimeStretch(oldbook,args)
typeH = addCheckBoxTypes(book,figHandle)
refreshFigure(varargin)
atomH = plotBook(book,varargin)
newbook = removeBookAtom(oldbook,index)
newLength = bookLength(book)

TODO:
------
- Fix the colorbar bounds ( now [0-1], 'Yticks' should be set to [ampMindB,ampMaxdB] )
- Check the behavior of the interface in the case of multichannel signal (selection and transforms are not done properly)

- Externalize the different processing functions (pitch shift, time stretch, ...) so they become available as matlab command line tools
for processing MPTK books. There is nothing more to do that cut and paste the functions to new m.files. It just needs a little
folder re-organizations.

- Implement the functions not enabled in the GUI.

Known Bugs:
-----------
- It seems impossible to select Dirac atoms - check the boundaries of rectangle selections
- Sometimes the keyboard shortcuts becomes unavailable (I don't know why)
