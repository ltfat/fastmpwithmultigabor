/******************************************************************************/
/*                                                                            */
/*                  	    mxBook.cpp                                        */
/*          		    mptkMEX toolbox  	      	                      */
/*          Class for interfacing MP_Book with matlab strcture                */
/*                                                                            */
/* Gilles Gonon                                               	  Feb 20 2008 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/
/*
 * $Version 0.5.4$
 * $Date 05/22/2007$
 */

#include "mxBook.h"  


/***************************/
/*                         */
/*  CLASS mxAtoms METHODS  */
/*                         */
/***************************/

/** CONSTRUCTORS */
//! Default empty constructor
mxAtoms::mxAtoms() {} 

//! Constructor with no allocation of params mxArray
mxAtoms::mxAtoms(string t, unsigned int nC) : typeLen(t), typeIdx(0), nAtom(0), nChannel(nC), curIdx(0) 
{ 
  size_t i;
  if ((i = typeLen.rfind("_")) != string::npos) {  type = typeLen.substr(0,i); }
  else { type = typeLen; }
}

//! Constructor with correct allocation of params mxArray
mxAtoms::mxAtoms(string t,unsigned int tI,unsigned long int nA,unsigned int nC) : typeLen(t), typeIdx(tI), nAtom(nA), nChannel(nC), curIdx(0) 
{
  size_t i;
  if ((i = typeLen.rfind("_")) != string::npos) {  type = typeLen.substr(0,i); }
  else { type = typeLen; }

  
  // Init map for each parameters
  mxAtoms::allocParams(nAtom,nChannel);
}  /* end of constructor */

/** DESCTRUCTOR */
mxAtoms::~mxAtoms()
{
  /* todo */ 
}
  
/** OTHER MEMBER FUNCTIONS */

//! Allocate Atom matlab memory for each parameters
void mxAtoms::allocParams(unsigned long int nA,unsigned int nC) {
  // Create param container according to atom type
  // Default params Fieds
  //( OK for sType=="nyquist" || sType=="constant" || sType=="dirac" )
  params["amp"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
  params["pos"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
  params["len"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
                
  // Add atom specific field to structure
  // Anywave atoms
  if (type=="anywave") {
    params["tableIdx"] = mxCreateDoubleMatrix(nA, 1, mxREAL);     //! BAD VALUE TO FIX
    params["anywaveTable"] = mxCreateDoubleMatrix(nA, 1, mxREAL); //! BAD VALUE TO FIX
    params["anywaveIdx"] = mxCreateDoubleMatrix(nA,1, mxREAL);   //! BAD VALUE TO FIX
  }
  // Anywave hilbert atoms
  else if (type=="anywave_hilbert") {
    params["realTableIdx"] = mxCreateDoubleMatrix(nA, 1, mxREAL);       //! BAD VALUE TO FIX
    params["hilbertTableIdx"] = mxCreateDoubleMatrix(nA, 1, mxREAL);    //! BAD VALUE TO FIX
    params["anywaveRealTable"] = mxCreateDoubleMatrix(nA, 1, mxREAL);   //! BAD VALUE TO FIX
    params["anywaveHilbertTable"] = mxCreateDoubleMatrix(nA,1, mxREAL); //! BAD VALUE TO FIX
    params["realPart"] = mxCreateDoubleMatrix(nA, nC, mxREAL);           //! BAD VALUE TO FIX
    params["hilbertPart"] = mxCreateDoubleMatrix(nA, nC, mxREAL);        //! BAD VALUE TO FIX
  }
  // Atoms that use a analysis window
  else if (type=="mdct" || type=="mdst" || type=="gabor" || type=="mclt" || type=="harmonic") {
    params["freq"] = mxCreateDoubleMatrix(nA, 1, mxREAL);
    params["windowtype"] = mxCreateDoubleMatrix(nA, 1, mxREAL);
    params["windowoption"] = mxCreateDoubleMatrix(nA, 1, mxREAL);
    // Chirped atoms
    if (type=="gabor" || type=="mclt" || type=="harmonic") {
      params["chirp"] = mxCreateDoubleMatrix(nA, 1, mxREAL);
      params["phase"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
      // Harmonic atoms
      if (type=="harmonic") {
	params["numPartials"] = mxCreateDoubleMatrix(nA, 1, mxREAL);
	// The following parameters can only be allocated when reading for the first time numPartials ... 
	//params["harmonicity"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
	//params["partialAmp"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
	//params["partialPhase"] = mxCreateDoubleMatrix(nA, nC, mxREAL);
      }
    }
  }
}
            
//! Read an atom and store values in params mxArrays
void mxAtoms::parseAtom(MP_Atom_c *atom) {
  unsigned int c,h,nP;
                
  /* CHANNEL INDEPENDENT PARAMETERS */
  /* ATOM Specific parameters */
  // Anywave atoms
  if (type=="anywave") {
    //MP_Anywave_Atom_Plugin_c * catom = (MP_Anywave_Atom_Plugin_c *) atom;
    *(mxGetPr( params["tableIdx"]) + curIdx ) = (double) atom->get_field(MP_TABLE_IDX_PROP,0);
    *(mxGetPr( params["anywaveTable"]) + curIdx ) = (double) 0.0; //! FAKE VALUE - NOT DONE
    *(mxGetPr( params["anywaveIdx"]) + curIdx ) = (double) atom->get_field(MP_ANYWAVE_IDX_PROP,0);
  }
  // Anywave hilbert atoms
  else if (type=="anywave_hilbert") {
    *(mxGetPr( params["realTableIdx"]) + curIdx ) = (double) atom->get_field(MP_REAL_TABLE_IDX_PROP,0);
    *(mxGetPr( params["hilbertTableIdx"]) + curIdx ) = (double) atom->get_field(MP_HILBERT_TABLE_IDX_PROP,0);
    *(mxGetPr( params["anywaveRealTable"]) + curIdx ) = (double) 0.0;    //! FAKE VALUE - NOT DONE
    *(mxGetPr( params["anywaveHilbertTable"]) + curIdx ) = (double) 0.0; //! FAKE VALUE - NOT DONE
  }
  // Atoms that use a analysis window
  else if (type=="mdct" || type=="mdst" || type=="gabor" || type=="mclt" || type=="harmonic") {
    *(mxGetPr( params["freq"]) + curIdx ) = (double) atom->get_field(MP_FREQ_PROP,0);
    *(mxGetPr( params["windowtype"]) + curIdx ) = (double) atom->get_field(MP_WINDOW_TYPE_PROP,0);
    *(mxGetPr( params["windowoption"]) + curIdx ) = (double) atom->get_field(MP_WINDOW_OPTION_PROP,0);
    // Chirped atoms
    if (type=="gabor" || type=="mclt" || type=="harmonic") {
      *(mxGetPr( params["chirp"]) + curIdx ) = (double) atom->get_field(MP_CHIRP_PROP,0);
      // Harmonic atoms
      if (type=="harmonic") {
	nP = (unsigned int) atom->get_field(MP_NUMPARTIALS_PROP,0);
	*(mxGetPr( params["numPartials"]) + curIdx ) = (double) nP;
	/* Allocate params field with numPartials if necessary */
	if (params.find("harmonicity") == params.end()) {
	  params["harmonicity"] = mxCreateDoubleMatrix(nAtom, nP, mxREAL);
	  mwSize dims[3] = {nAtom,nP,nChannel};
	  params["partialAmp"] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);   //! channel dependent
	  params["partialPhase"] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL); //! channel dependent
	}
	// Read harmonicity values
	for (h=0;h<nP;h++) {
	  *( mxGetPr(params["harmonicity"]) + h*nAtom + curIdx ) = (double) atom->get_field(MP_HARMONICITY_PROP,h);
	}
      }
    }
  }
                                        
  /* CHANNEL DEPENDENT PARAMETERS */
  for (c=0;c<nChannel;c++) {
    /* ATOM Common parameters */
    *(mxGetPr( params["amp"]) + c*nAtom + curIdx ) = (double) atom->amp[c];
    *(mxGetPr( params["pos"]) + c*nAtom + curIdx ) = (double) atom->support[c].pos;
    *(mxGetPr( params["len"]) + c*nAtom + curIdx ) = (double) atom->support[c].len;
    /* ATOM Specific parameters */
    if (type=="anywave_hilbert") {
      *(mxGetPr( params["realPart"]) + c*nAtom + curIdx ) = (double) atom->get_field(MP_REAL_PART_PROP,c);
      *(mxGetPr( params["hilbertPart"]) + c*nAtom + curIdx ) = (double) atom->get_field(MP_HILBERT_PART_PROP,c);
    }
    // Atoms that have phase information
    // WAS // if (type=="mdct" || type=="mdst" || type=="gabor" || type=="mclt" || type=="harmonic") {
    else if (type=="gabor" || type=="mclt" || type=="harmonic") {
      *(mxGetPr( params["phase"]) + c*nAtom + curIdx ) = (double) atom->get_field(MP_PHASE_PROP,c);
      // Harmonic atoms
      if (type=="harmonic") {
	// Read Partial amp/phase info
	for (h=0;h<nP;h++) {
	  /* !! CHECK THESE VALUES */    *(mxGetPr( params["partialAmp"]) + c*(nAtom*nP) + h*nAtom + curIdx ) = (double) atom->get_field(MP_PARTIAL_AMP_PROP,h*nChannel+c);
	  /* !! CHECK THESE VALUES */    *(mxGetPr( params["partialPhase"])+ c*(nAtom*nP) + h*nAtom + curIdx ) = (double) atom->get_field(MP_PARTIAL_PHASE_PROP,h*nChannel+c);
	}
      }
    }
  }          
  curIdx++; //! Go to next index
}
            

//! Fill a given 'atom' structure at index 'a' with parameters
mxArray * mxAtoms::outputMxStruct(mxArray * atom, unsigned int a) {

  mexPrintf(" - Fill atom Structure for type [%s]\n",typeLen.c_str()); 
  // Create atom Structure              
  mwSize dims[2] = {1, 2};
  mxArray *par;
  unsigned int p;
                
  mxSetField(atom, a, "type", mxCreateString(type.c_str()) );
                
  // Create parameters Structure (added to atom)
  dims[1] = 1; // change dimension [1x1] struct
  const char *emptyFieldNames[] = {""};
  par = mxCreateStructArray(2,dims,0,emptyFieldNames);  //! Parameter structure
                
  // Loop on map parameters: add field name and corresponding values to struct
  map <string, mxArray *>::iterator miter;
  for ( miter = params.begin(); miter != params.end(); miter++ )
    {
      mexPrintf("   - \"%s\" added\n",miter->first.c_str());
      p = mxAddField(par,miter->first.c_str());
      mxSetFieldByNumber(par,0, p, miter->second);
    } // End of atom parameters definition
                
  mxSetField(atom, a, "params", par);
  return atom;
}


/**********************************/
/*                                */
/*  END OF CLASS mxAtoms METHODS  */
/*                                */
/**********************************/

/***************************/
/*                         */
/*  CLASS mxBook  METHODS  */
/*                         */
/***************************/

/** CONSTRUCTORS */
/** Constructor 1: copy mxArray pointer to book structure */
/* used by bookwrite.mex */
mxBook::mxBook(const mxArray * mxbook) {
  // Copy mex structure 
  mexbook = mxDuplicateArray(mxbook);
  //! Init other parameters 
  setMapTypeName();
  
  mxArray *tmp;
  tmp = mxGetField(mexbook,0,"numAtoms");
  numAtoms = (unsigned long int)mxGetScalar(tmp);
  tmp = mxGetField(mexbook,0,"numChans");
  numChans = (MP_Chan_t)mxGetScalar(tmp);
  
}

/** Constructor 2: Construct mxArray book structure from MP_Book_c pointer */ 
/* used by bookread.mex */
mxBook::mxBook(MP_Book_c *mpbook) {
  /* Declarations */
  int tmpcharlen,m;
  
  const char *bookFieldNames[] = {"numAtoms","numChans","numSamples","sampleRate","index","atom"};
  mwSize bookDims[2] = {1, 1};
  mwSize atomDims[2] = {1, 1};
  
  mxArray *tmp, *type, *atom, *params;
  unsigned long int n;
  unsigned short int c;
  
  /* class parameters initialisation */
  setMapTypeName();
  numChans = mpbook->numChans;
  numAtoms = mpbook->numAtoms;
  
  /** Allocate Output book structure */
  mexbook = mxCreateStructArray(2,bookDims,6,bookFieldNames); //! Output book structure
  
  /** Fill header info */
  tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) numAtoms; //! numAtoms
  mxSetField(mexbook, 0, "numAtoms", tmp);
  tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) numChans; //! numChans
  mxSetField(mexbook, 0, "numChans", tmp);
  tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) mpbook->numSamples; //! numSamples
  mxSetField(mexbook, 0, "numSamples", tmp);
  tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) mpbook->sampleRate; //! sampleRate
  mxSetField(mexbook, 0, "sampleRate", tmp);
    

    
  /** Count atom types and fill index */
  mxArray * index;
  unsigned int indexSize = 4+numChans;
  index = mxCreateDoubleMatrix(indexSize,numAtoms, mxREAL); //! index contains (1: Atom number, 2: type index, 3: atom index, 4: Atom selected, 4+chan: atom pos of channel chan)
    
  mexPrintf("Counting atom types in book : ");
    
  map <string, mxAtoms *> atomStats;       //! Map <atom type , nb of occurence>
  map <string, mxAtoms *>::iterator miter; //! and a iterator on it
    
  for (n=0;n<numAtoms;n++) {
    string aType;
    stringstream ss1;
    ss1 << mpbook->atom[n]->type_name() << "_" <<  mpbook->atom[n]->support[0].len ;
    ss1 >> aType;

    // aType is a new type, register it in maps
    if ( atomStats.find(aType) == atomStats.end() ) {
      atomStats[aType] = new mxAtoms(aType,numChans);
    }
        
    atomStats[aType]->nAtom++; //! Increment atom count 
                
    //! Fill book index info for this atom
    *( mxGetPr(index) + n*indexSize ) = (double) (n+1);                         //! Atom number
    // As the map is not filled the typeIdx cannot be set at this stage
    // *( mxGetPr(index) + 4*n + 1 ) = (double) atomStats[aType]->typeIdx; //! Type index
    *( mxGetPr(index) + n*indexSize + 2 ) = (double) atomStats[aType]->nAtom;   //! Atom number
    // loop on channels to get their position
    *( mxGetPr(index) + n*indexSize + 3 ) = (double) 1.0;   //! Atom selected 
    // loop on channels to get their position
    for (c=0;c<numChans;c++) {
      *( mxGetPr(index) + n*indexSize + 4 + c) = (double) mpbook->atom[n]->support[c].pos;     //! Atom pos
    }
  }
    
 
  /* Matlab console info */
  mexPrintf("found %d different atom types\n",atomStats.size());
    
  /* Init Atom structure
   *  atom.type = string
   *  atom.params = structure, size : 1*1 of atoms type found in book
   *       For each type of atom, params is a structure
   *       with the specified parameters (depend on atom type)
   */
    
    
  /** Allocate structure params for the different type of atoms
   *  and set the correct type index */
  unsigned int t = 0;  //! Index of type used for mxAtoms constructors
  for ( miter = atomStats.begin(); miter != atomStats.end(); ++miter )
    {
      mexPrintf(" - atom [%s] :  %ld occurences\n",miter->second->type.c_str(), miter->second->nAtom);
      miter->second->allocParams(miter->second->nAtom, numChans);
      miter->second->typeIdx = t;  //! Set type index according to the map iterator
      t++; //! increment type index
    }

    
  mexPrintf("Load each atom\n");

  /* Parse Atoms parameters */
  for ( n=0 ; n<numAtoms ; n++ ) {
    string aType;
    stringstream ss1;
    ss1 << mpbook->atom[n]->type_name() << "_" <<  mpbook->atom[n]->support[0].len ;
    ss1 >> aType;

    if ( atomStats.find(aType) != atomStats.end() ) {
      atomStats[aType]->parseAtom(mpbook->atom[n]);
      *( mxGetPr(index) + n*indexSize + 1 ) = (double) ( atomStats[aType]->typeIdx + 1); //! Type index
    } else {
      mexPrintf("Atom [%ld] was not recognized\n",n);
    }
  }
    
  // Add atom structure to output Variables
  atomDims[1] = atomStats.size();
  const char *atomFieldNames[] = {"type","params"};
  atom = mxCreateStructArray(2,atomDims,2,atomFieldNames);
    
  mexPrintf("Creating output structure\n");
  n = 0;
  for ( miter = atomStats.begin(); miter != atomStats.end(); ++miter )
    {
      mexPrintf(" - atom [%s] \n",miter->second->type.c_str());
      miter->second->outputMxStruct(atom,n);
      n++;
    } // End of atom parameters definition
    

  /** Add index info to book structure */ 
  mxSetField(mexbook, 0, "index", index);

  // Attach the atom structure to mexbook.atom
  mxSetField(mexbook, 0, "atom", atom);
}

/** DESTRUCTOR */
mxBook::~mxBook() {
  // todo
}


/** OTHER METHODS */
/** Set Map of correpondance between atom type (returned by MP_Book_c::type_name()) and atomName (required by MP_Atom_Factory) */
void mxBook::setMapTypeName() {
  typeName["gabor"] = "GaborAtom";
  typeName["constant"] = "ConstantAtom";
  typeName["dirac"] = "DiracAtom";
  typeName["nyquist"] = "NyquistAtom";
  typeName["harmonic"] = "HarmonicAtom";
  typeName["mdst"] = "MdstAtom";
  typeName["mdct"] = "MdctAtom";
  typeName["mclt"] = "McltAtom";
  typeName["anywave"] = "AnywaveAtom";
  typeName["anywaveHilbert"] = "AnywaveHilbertAtom";
}


/** Get MP_Atom from mx book structure */
/** This maybe far from fast method (get atom by type) */
MP_Atom_c * mxBook::getMP_Atom(unsigned long int atomIdx) {
  
  /** declare NULL atom pointers for each type */
  MP_Constant_Atom_Plugin_c* newConstant = NULL;
  MP_Dirac_Atom_Plugin_c* newDirac = NULL;
  MP_Nyquist_Atom_Plugin_c* newNyquist = NULL;
  MP_Gabor_Atom_Plugin_c* newGabor = NULL;
  MP_Harmonic_Atom_Plugin_c* newHarmonic = NULL;
  MP_Anywave_Atom_Plugin_c* newAnywave = NULL;
  MP_Anywave_Hilbert_Atom_Plugin_c* newAnywaveHilbert = NULL;
  MP_Mclt_Atom_Plugin_c* newMclt = NULL;
  MP_Mdct_Atom_Plugin_c* newMdct = NULL;
  MP_Mdst_Atom_Plugin_c* newMdst = NULL;
  MP_Atom_c *newAtom = NULL;
        
  /** Add index info to book structure */
  mxArray * mxIdx;
  unsigned int indexSize = 4+numChans;

  mxIdx = mxGetField(mexbook, 0, "index"); //! Matrix of size 4*nAtoms
  unsigned int t =  (unsigned int) *( mxGetPr(mxIdx) + atomIdx*indexSize + 1 ); //! typeMap index
  unsigned long int n =  (unsigned long int) *( mxGetPr(mxIdx) + atomIdx*indexSize + 2 ); //! atom number for given type
  MP_Chan_t c;
  
  /** Get Atom type structure */
  mxArray *mxAtom, *mxType, *mxParams;
  mxAtom = mxGetField(mexbook,0,"atom");      //! Structure with field 'type' 'params'
  mxParams = mxGetField(mxAtom,t-1,"params"); //! Structure with fields different for each type
  mxType = mxGetField(mxAtom,t-1,"type");     //! Get string type
  string aType(mxArrayToString(mxType));
  string aName;

  if (typeName.find(aType) != typeName.end() ) 
    {
      aName = typeName[aType];
    }
  else
    {
      mexPrintf("mxBook::getMP_Atom warning -- unknown atomName correspondance for type %s\n",aType.c_str());
      aName = "UnknowAtom";      
    }
   
  /** Get Atom creator method */
 
  MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator(aName.c_str());
  if (NULL == emptyAtomCreator)
    {
      mexPrintf("mxBook::getMP_Atom error -- unknown  MP_Atom_Factory_c method for atomName:%s\n",aName.c_str());
      return( newAtom );
    }
  
  // mexPrintf("mxBook::getMP_Atom -- atom index %ld [%s]\n",atomIdx,aType.c_str());
  
  /** Retrieve pointer for common parameters to all atoms (pos, len, amp) */
  mxArray *mxpos, *mxamp, *mxlen;
  mxpos = mxGetField(mxParams,0,"pos");
  mxlen = mxGetField(mxParams,0,"len");
  mxamp = mxGetField(mxParams,0,"amp");
   
  unsigned long int nA = (unsigned long int) mxGetM(mxamp); // number of atoms for this type (used for browsing mxArray)

  /** Create empty atom and fill it with the params field */
  /** CONSTANT ATOM */
  if (aType=="constant") 
    {
      /** Create and allocate atom */
      if ( (newConstant =  (MP_Constant_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newConstant->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      
      /** set atoms params */
      for (c=0;c<numChans;c++) { // loop on channels
	newConstant->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newConstant->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newConstant->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
          
      return (dynamic_cast<MP_Atom_c*>(newConstant));
    } 
  /** DIRAC ATOM */
  else if (aType=="dirac") 
    {
      if ( (newDirac =  (MP_Dirac_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newDirac->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      /** set atoms params */
      for (c=0;c<numChans;c++) { // loop on channels
	newDirac->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newDirac->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newDirac->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
      
      return (dynamic_cast<MP_Atom_c*>(newDirac));
    }
  /** NYQUIST ATOM */
  else if (aType=="nyquist") 
    {
      if ( (newNyquist =  (MP_Nyquist_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newNyquist->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      /** set atoms params */
      for (c=0;c<numChans;c++) { // loop on channels
	newNyquist->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newNyquist->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newNyquist->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
      
      return (dynamic_cast<MP_Atom_c*>(newNyquist));
    }
  /** GABOR ATOM */
  else if (aType=="gabor") 
    {
      if ( (newGabor =  (MP_Gabor_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }

      if ( newGabor->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      if ( newGabor->alloc_gabor_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      /** set atoms params */
      mxArray *mxchirp, *mxfreq, *mxphase, *mxwintype, *mxwinopt;
      mxchirp = mxGetField(mxParams,0,"chirp");
      mxfreq = mxGetField(mxParams,0,"freq");
      mxphase = mxGetField(mxParams,0,"phase");
      mxwintype = mxGetField(mxParams,0,"windowtype");
      mxwinopt = mxGetField(mxParams,0,"windowoption");
      
      newGabor->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
      newGabor->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
      newGabor->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
      newGabor->chirp = (MP_Real_t) *(mxGetPr(mxchirp) + (n-1));
      for (c=0;c<numChans;c++) { // loop on channels
	newGabor->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newGabor->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newGabor->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
	newGabor->phase[c] = (MP_Real_t)  *(mxGetPr(mxphase) + c*nA + (n-1));
      }
      
      return (dynamic_cast<MP_Atom_c*>(newGabor));
    } 
  /** HARMONIC ATOM */
  else if (aType=="harmonic") 
    {
      if ( (newHarmonic =  (MP_Harmonic_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      
      if ( newHarmonic->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      if ( newHarmonic->alloc_gabor_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      mxArray *mxnumPartial;
      mxnumPartial = mxGetField(mxParams,0,"numPartials");
      unsigned int numPartials = (unsigned int) *(mxGetPr(mxnumPartial) + (n-1));
      unsigned int p;

      if ( newHarmonic->alloc_harmonic_atom_param( numChans, numPartials ) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      /** set atoms params */
      mxArray *mxchirp, *mxfreq, *mxphase, *mxwintype, *mxwinopt, *mxharmo, *mxpartialamp, *mxpartialphase;
      mxchirp = mxGetField(mxParams,0,"chirp");
      mxfreq = mxGetField(mxParams,0,"freq");
      mxphase = mxGetField(mxParams,0,"phase");
      mxwintype = mxGetField(mxParams,0,"windowtype");
      mxwinopt = mxGetField(mxParams,0,"windowoption");
      mxharmo = mxGetField(mxParams,0,"harmonicity");
      mxpartialamp = mxGetField(mxParams,0,"partialAmp");
      mxpartialphase = mxGetField(mxParams,0,"partialPhase");
      
      newHarmonic->numPartials = numPartials;
      newHarmonic->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
      newHarmonic->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
      newHarmonic->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
      newHarmonic->chirp = (MP_Real_t) *(mxGetPr(mxchirp) + (n-1));
      for (p=0;p<numPartials;p++) { // loop on partials
	newHarmonic->harmonicity[p] =  (MP_Real_t)  *(mxGetPr(mxharmo) + p*nA +  (n-1));	  
      }
      
      for (c=0;c<numChans;c++) { // loop on channels
	newHarmonic->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newHarmonic->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newHarmonic->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
	newHarmonic->phase[c] = (MP_Real_t)  *(mxGetPr(mxphase) + c*nA + (n-1));
	for (p=0;p<numPartials;p++) { // loop on partials
	  newHarmonic->partialAmp[c][p] = (MP_Real_t)  *(mxGetPr(mxpartialamp) + c*(nA*numPartials) + p*nA + (n-1)); // When reading was c*(nAtom*nP) + h*nAtom + curIdx
	  newHarmonic->partialPhase[c][p] = (MP_Real_t)  *(mxGetPr(mxpartialphase) + c*(nA*numPartials) + p*nA + (n-1));
	}
      }

      return (dynamic_cast<MP_Atom_c*>(newHarmonic));
    }
  /** ANYWAVE ATOM */
  else if (aType=="anywave") 
    {
      if ( (newAnywave =  (MP_Anywave_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newAnywave->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      
      /** set atoms params */
      mxArray *mxtableidx, *mxanywaveidx, *mxanywavetable;
      mxtableidx = mxGetField(mxParams,0,"tableIdx");
      mxanywaveidx = mxGetField(mxParams,0,"anywaveIdx");
      mxanywavetable = mxGetField(mxParams,0,"anywaveTable");
      
      for (c=0;c<numChans;c++) { // loop on channels
	newAnywave->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newAnywave->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newAnywave->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
      newAnywave->tableIdx =  (unsigned long int) *(mxGetPr(mxtableidx) + (n-1));
      newAnywave->anywaveIdx =  (unsigned long int) *(mxGetPr(mxanywaveidx) + (n-1));
      // TODO : parse anywaveTable info
      mexPrintf("mxBook::getMP_Atom  warning -- Atom %s -- anywaveTable info not added yet\n",aType.c_str());

      return (dynamic_cast<MP_Atom_c*>(newAnywave));
    } 
  /** ANYWAVE HILBERT ATOM */
  else if (aType=="anywavehilbert") 
    {
      if ( (newAnywaveHilbert =  (MP_Anywave_Hilbert_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newAnywaveHilbert->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      /** set atoms params */
      mxArray *mxhilberttableidx, *mxrealtableidx, *mxrealpart, *mxhilbertpart, *mxanywavehilberttable, *mxanywaverealtable;
      mxhilberttableidx = mxGetField(mxParams,0,"hilbertTableIdx");
      mxrealtableidx = mxGetField(mxParams,0,"realTableIdx");
      /** TODO */
      //      mxhilbertpart = mxGetField(mxParams,0,"hilbertPart");
      //      mxrealpart = mxGetField(mxParams,0,"realPart");
      //      mxanywavehilberttable = mxGetField(mxParams,0,"anywaveHilbertTable");
      //      mxanywaverealtable = mxGetField(mxParams,0,"anywaveRealTable");
      
      for (c=0;c<numChans;c++) { // loop on channels
	newAnywaveHilbert->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newAnywaveHilbert->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newAnywaveHilbert->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
      newAnywaveHilbert->hilbertTableIdx =  (unsigned long int) *(mxGetPr(mxhilberttableidx) + (n-1));
      newAnywaveHilbert->realTableIdx =  (unsigned long int) *(mxGetPr(mxrealtableidx) + (n-1));
      // TODO : parse anywave(real,hilbert)Table and (real,hilbert)Part  info
      mexPrintf("mxBook::getMP_Atom  warning -- Atom %s -- anywave(Hilbert,Real)Table info not added yet\n",aType.c_str());
    
      return (dynamic_cast<MP_Atom_c*>(newAnywaveHilbert));
    }
  /** MCLT ATOM */
  else if (aType=="mclt") 
    {
      if ( (newMclt =  (MP_Mclt_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newMclt->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      if ( newMclt->alloc_mclt_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      /** set atoms params */
      mxArray *mxchirp, *mxfreq, *mxphase, *mxwintype, *mxwinopt;
      mxchirp = mxGetField(mxParams,0,"chirp");
      mxfreq = mxGetField(mxParams,0,"freq");
      mxphase = mxGetField(mxParams,0,"phase");
      mxwintype = mxGetField(mxParams,0,"windowtype");
      mxwinopt = mxGetField(mxParams,0,"windowoption");
      
      newMclt->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
      newMclt->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
      newMclt->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
      newMclt->chirp = (MP_Real_t) *(mxGetPr(mxchirp) + (n-1));
      for (c=0;c<numChans;c++) { // loop on channels
	newMclt->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newMclt->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newMclt->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
	newMclt->phase[c] = (MP_Real_t)  *(mxGetPr(mxphase) + c*nA + (n-1));
      }
      
      return (dynamic_cast<MP_Atom_c*>(newMclt));
    }
  /** MDCT ATOM */
  else if (aType=="mdct") 
    {
      if ( (newMdct =  (MP_Mdct_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newMdct->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}

      /** set atoms params */
      mxArray *mxfreq, *mxwintype, *mxwinopt;
      mxfreq = mxGetField(mxParams,0,"freq");
      mxwintype = mxGetField(mxParams,0,"windowtype");
      mxwinopt = mxGetField(mxParams,0,"windowoption");
      
      newMdct->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
      newMdct->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
      newMdct->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
      for (c=0;c<numChans;c++) { // loop on channels
	newMdct->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newMdct->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newMdct->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
      
      return (dynamic_cast<MP_Atom_c*>(newMdct));
    }
  /** MDST ATOM */
  else if (aType=="mdst") 
    {
      if ( (newMdst =  (MP_Mdst_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL ) {
	mexPrintf("mxBook::getMP_Atom error -- could not create empty atom of type %s\n",aType.c_str());
	return( newAtom );
      }
      if ( newMdst->alloc_atom_param(numChans) )
	{
	  mexPrintf("Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aName.c_str());
	  return( newAtom );
	}
      /** set atoms params */
      mxArray *mxfreq, *mxwintype, *mxwinopt;
      mxfreq = mxGetField(mxParams,0,"freq");
      mxwintype = mxGetField(mxParams,0,"windowtype");
      mxwinopt = mxGetField(mxParams,0,"windowoption");
      
      newMdst->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
      newMdst->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
      newMdst->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
      for (c=0;c<numChans;c++) { // loop on channels
	newMdst->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newMdst->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newMdst->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
      }
       
      return (dynamic_cast<MP_Atom_c*>(newMdst));
    }
  else 
    {
      mexPrintf("Atom type [%s] unknown, consider adding its information in mxBook{h,cpp}\n",aType.c_str());
      return (newAtom);
    } 
}


/** Export matlab book structure to MP_Book_c class */
MP_Book_c * mxBook::Book_MEX_2_MPTK() {
    
  MP_Book_c * mpBook;
  mxArray *tmp,*atoms,*mxIndex;
  MP_Chan_t numChans;
  int sampleRate;
  unsigned long int nAtom,numSamples,a,nAtomAdded;
  unsigned int indexSize;
    
  /* Read matlab book structure header */
  tmp = mxGetField(mexbook,0,"numAtoms");
  nAtom = (unsigned long int)mxGetScalar(tmp);
  tmp = mxGetField(mexbook,0,"numChans");
  numChans = (MP_Chan_t)mxGetScalar(tmp);
  tmp = mxGetField(mexbook,0,"numSamples");
  numSamples = (unsigned long int)mxGetScalar(tmp);
  tmp = mxGetField(mexbook,0,"sampleRate");
  sampleRate = (int)mxGetScalar(tmp);

  /** Get book index */
  mxIndex = mxGetField(mexbook,0,"index");
  indexSize = 4+numChans;

  /** Create a empty book */
  mpBook = MP_Book_c::create(numChans,numSamples,sampleRate);
    
  // Add selected atoms to book
  nAtomAdded = 0;
  for (a=0;a<nAtom;a++) {
    if (*(mxGetPr(mxIndex) + a*indexSize + 3) != 0.0 ) {
      //! add_atom
      // mexPrintf(" - Adding Atom [%ld] to book :",a);
      MP_Atom_c * mpAtom;      
      if ( (mpAtom = this->getMP_Atom(a)) == NULL ) {
	mexPrintf("!!! mxBook::Book_MEX_2_MPTK -- ERROR while adding Atom [%ld] to book :",a);
      } else {
	mpBook->append( mpAtom );
	nAtomAdded++;
      }
    }
  }
    
  mexPrintf("mxBook::Book_MEX_2_MPTK info - [%ld] atoms have been added to book.\n",nAtomAdded);

  return mpBook;
}

/** Export matlab book structure to MP_Book_c class */
void mxBook::MP_BookWrite(string fileName, const char mode) {
  MP_Book_c * mpBook;
    
  /** Export mexbook to a MP_Book_c */
  mpBook = this->Book_MEX_2_MPTK();
    
  /** Save it to file with mode (MP_TEXT or MP_BINARY) */
  mpBook->print(fileName.c_str(),mode);
  return;
}

/** Reconstruct Signal from book and return a pointer to a mxArray containing the MP_Signal samples 
 *  (A simplified version of mpr)
 */

mxArray * mxBook::Book_Reconstruct() {
  MP_Book_c   * mpBook;
  MP_Signal_c * mpSignal;
  mxArray     * mxSignal = NULL;
    
  /* Load the MPTK environment if not loaded */
  if (!MPTK_Env_c::get_env()->get_environment_loaded()) {
    MPTK_Env_c::get_env()->load_environment("");
  }
  
  /** Export mexbook to a MP_Book_c */
  mpBook = this->Book_MEX_2_MPTK();

  if ( mpBook == NULL ) {
    mexPrintf( "mxBook::Book_Reconstruct() info -- MP_Book_c is ill formed.\n" );
    mxSignal = mxCreateDoubleMatrix(0,0, mxREAL);
    return mxSignal;
  }

  
  // Reconstruct book to signal
  mpBook->info();
  //if (MP_FALSE == mpBook->recheck_num_channels()) {  mexPrintf( "mxBook::Book_Reconstruct() WARNING -- BOOK NUMCHANS NOT UP TO DATE\n" ); }
  
  //if (MP_FALSE == mpBook->recheck_num_samples()) {  mexPrintf( "mxBook::Book_Reconstruct() WARNING -- BOOK NUMSAMPLES NOT UP TO DATE\n" ); }
  
  // Init MP_Signal with book params
  mpSignal = MP_Signal_c::init( mpBook->numChans, mpBook->numSamples, mpBook->sampleRate );
  if ( mpSignal == NULL ) {
    mexPrintf( "mxBook::Book_Reconstruct() error -- Can't make a new signal.\n" );
    return mxSignal;
  }
  

  /** THIS LINE MAKES A SEG FAULT -- GDB GIVES NO TRACE -- MAYBE IT IS A PROBLEM OF MATLAB !!! */
  mpBook->substract_add( NULL,mpSignal, NULL);
  mexPrintf( "mxBook::Book_Reconstruct() info -- MP_Signal_c reconstructed from MP_Book_c\n" );
  
  
  // Convert MP_Signal to mxArray
  mxSignal = mxCreateDoubleMatrix(mpSignal->numSamples, mpSignal->numChans, mxREAL);
  unsigned long int nS;
  unsigned int nC;
  
  
  mexPrintf( "mxBook::Book_Reconstruct() info -- filling signal vector\n" );
  for (nS=0; nS<mpSignal->numSamples * mpSignal->numChans; nS++) {
    *( mxGetPr(mxSignal) + nS ) = (double) ( mpSignal->storage[nS]);
  }
  
  //  delete(mpBook);
  // delete(mpSignal);
    
  return mxSignal;
}
