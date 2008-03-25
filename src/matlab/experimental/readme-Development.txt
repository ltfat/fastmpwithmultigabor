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
 - Class mxAtoms (defined in mxBook.h and mxBook.cpp) :
 --------------------------------------------------------
     class for parsing MP_Atom and filling mex structure of atoms per type
     (used by mxBook for importing MP_Book )

 The matlab 'atom' structure contains the following fields:
     - type : string
     - params: structure of atoms parameters arrays, whose field are type dependent

 - Class mxBook (defined in mxBook.h and mxBook.cpp) :
 -------------------------------------------------------
     class for interfacing MP_Book_c with matlab structure
     As the atoms in MPTK have parameters dependent to their type
     the matlab 'book' structure store the atom parameters per type of atom.

    Constructors allows to construct a matlab book from a (MP_Book_c *) use for loading a binary book file
    Or to use a matlab book structure

    The main interface methods are :
   - Export matlab book structure to MP_Book_c class:
     MP_Book_c * Book_MEX_2_MPTK();
   - Export matlab book structure to MP_Book_c class
     void MP_BookWrite(string fileName, const char mode);

   - Reconstruct Signal from book and return a pointer to a mxArray containing the MP_Signal samples.
     This is the same as calling the "mpr" shell command with no residual
     mxArray * Book_Reconstruct();
  NOTE: unfortunately calling this function gives a segmentation fault. 
        After some unsuccessful debugging attemps, the error is very localized in call to
        MP_Book_c->substract_add( NULL,mpSignal, NULL);
        This command crashes matlab even started with gdb in debug mode.

   - See mxBook.h and mxBook.cpp

-----------------------------------------
2. THE MATLAB BOOK STRUCTURE DEFINITION
-----------------------------------------

The book structure has been fully rewritten to fit "bookedit" needs and to allow fast operations on atoms.
This structure is extensively used in bookedit_exp.m so you can find many examples of processing a book with
matlab.

The original idea for the organization of the book is that atoms with the same sets of parameters (atoms
of the same type: gabor, dirac, mdct, constant, ....) are gathered in atom sub structures for fast group processing.
In order to further ease common atom manipulations, this idea has evolved into also grouping atoms of the same lengths.
Now, in the book structure, there are as many "atom" as atoms type*length.

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
         4: Atom selected or not (used for saving part of a book)
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
bookedit opens a GUI


