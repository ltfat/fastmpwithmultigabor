/******************************************************************************/
/*                                                                            */
/*                  	    mxBook.cpp                                        */
/*                                                                            */
/*          		    matlab4mptk toolbox									  */
/*                                                                            */
/*          Class for interfacing MP_Book with matlab strcture                */
/*                                                                            */
/* Gilles Gonon                                               	  Feb 20 2008 */
/* Remi Gribonval                                              	  July 2008   */
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
/*  CLASS atomGroup METHODS  */
/*                         */
/***************************/

//
//! Constructor with no allocation of params mxArray
atomGroup::atomGroup(string t, unsigned int nC) : curIdx(0), typeLen(t), typeIdx(0), nAtom(0), nChannel(nC)  
{ 
  size_t i;
  if ((i = typeLen.rfind("_")) != string::npos) {  type = typeLen.substr(0,i); }
  else { type = typeLen; }
}


// DESCTRUCTOR
atomGroup::~atomGroup()
{
  /* todo */ 
}
  
/** OTHER MEMBER FUNCTIONS */

//! Allocate Atom matlab memory for each parameters
void atomGroup::allocParams(unsigned long int nA, MP_Atom_c* example) {
	int 		iIndexMono = 0;
	int 		iIndexMulti = 0;
	// Create param container according to atom type
	// Default params Fieds
	MP_Chan_t nC = example->numChans;

	// Multichannels loop
	for(iIndexMulti = MP_NUM_MULTI_BEGININDEX_PROPS; iIndexMulti < MP_NUM_MULTI_BEGININDEX_PROPS + MP_NUM_MULTI_PROPS; iIndexMulti++)
	{
		if(example->has_field(iIndexMulti))
			params[atomMultiField[iIndexMulti - MP_NUM_MULTI_BEGININDEX_PROPS]] = mxCreateDoubleMatrix(nA, nC, mxREAL);
	}
	// Monochannels loop
	for(iIndexMono = MP_NUM_MONO_BEGININDEX_PROPS; iIndexMono < MP_NUM_MONO_BEGININDEX_PROPS + MP_NUM_MONO_PROPS; iIndexMono++)
	{
		if(example->has_field(iIndexMono))
			params[atomMonoField[iIndexMono - MP_NUM_MONO_BEGININDEX_PROPS]] = mxCreateDoubleMatrix(nA, 1, mxREAL);
	}
}
            
//! Read an atom and store values in params mxArrays
void atomGroup::append(MP_Atom_c *atom) {
	unsigned int iIndexChannel = 0,iIndex = 0,iNumPartial = 0, iIndexMono = 0, iIndexMulti = 0, iIndexOthers = 0;
	const char * func = "atomGroup:append";
	mp_debug_msg(MP_DEBUG_FUNC_ENTER,func,"entering %s\n",type.c_str());

	// MonoChannels
	for(iIndexMono = MP_NUM_MONO_BEGININDEX_PROPS; iIndexMono < MP_NUM_MONO_BEGININDEX_PROPS + MP_NUM_MONO_PROPS; iIndexMono++)
	{
		if(atom->has_field(iIndexMono))
		{
			if(iIndexMono == MP_NUMPARTIALS_PROP)
			{
				iNumPartial = (unsigned int) atom->get_field(iIndexMono,0);
				*(mxGetPr( params[atomMonoField[iIndexMono - MP_NUM_MONO_BEGININDEX_PROPS]]) + curIdx ) = (double) iNumPartial;
			}
			else
				*(mxGetPr( params[atomMonoField[iIndexMono - MP_NUM_MONO_BEGININDEX_PROPS]]) + curIdx ) = (double) atom->get_field(iIndexMono,0);
		}
	}

	//Harmonicity which allocate params field with numPartials if necessary
	if (params.find(atomOthersField[MP_HARMONICITY_PROP - MP_NUM_OTHERS_BEGININDEX_PROPS]) == params.end())
	{
		if(atom->has_field(MP_HARMONICITY_PROP))
			params[atomOthersField[MP_HARMONICITY_PROP - MP_NUM_OTHERS_BEGININDEX_PROPS]] = mxCreateDoubleMatrix(nAtom, iNumPartial, mxREAL);
		mwSize dims[3] = {nAtom, iNumPartial, nChannel};
		if(atom->has_field(MP_PARTIAL_AMP_PROP))
			params[atomOthersField[MP_PARTIAL_AMP_PROP - MP_NUM_OTHERS_BEGININDEX_PROPS]] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);   //! channel dependent
		if(atom->has_field(MP_PARTIAL_PHASE_PROP))
			params[atomOthersField[MP_PARTIAL_PHASE_PROP - MP_NUM_OTHERS_BEGININDEX_PROPS]] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL); //! channel dependent
		// Read harmonicity values
		for (iIndex = 0 ; iIndex < iNumPartial ; iIndex++)
		{
			if(atom->has_field(MP_HARMONICITY_PROP))
				*( mxGetPr(params[atomOthersField[MP_HARMONICITY_PROP - MP_NUM_OTHERS_BEGININDEX_PROPS]]) + iIndex*nAtom + curIdx ) = (double) atom->get_field(MP_HARMONICITY_PROP,iIndex);
		}
	}

	// MultiChannels
	for (iIndexChannel = 0 ; iIndexChannel < nChannel ; iIndexChannel++)
	{
		for(iIndexMulti = MP_NUM_MULTI_BEGININDEX_PROPS; iIndexMulti < MP_NUM_MULTI_BEGININDEX_PROPS + MP_NUM_MULTI_PROPS; iIndexMulti++)
		{
			if(iIndexMulti == MP_AMP_PROP || iIndexMulti == MP_POS_PROP || iIndexMulti == MP_LEN_PROP)
			{
				if(atom->has_field(MP_AMP_PROP))
					*(mxGetPr( params[atomMultiField[MP_AMP_PROP - MP_NUM_MULTI_BEGININDEX_PROPS]]) + iIndexChannel*nAtom + curIdx ) = (double) atom->amp[iIndexChannel];
				if(atom->has_field(MP_POS_PROP))
					*(mxGetPr( params[atomMultiField[MP_POS_PROP - MP_NUM_MULTI_BEGININDEX_PROPS]]) + iIndexChannel*nAtom + curIdx ) = (double) atom->support[iIndexChannel].pos;
				if(atom->has_field(MP_LEN_PROP))
					*(mxGetPr( params[atomMultiField[MP_LEN_PROP - MP_NUM_MULTI_BEGININDEX_PROPS]]) + iIndexChannel*nAtom + curIdx ) = (double) atom->support[iIndexChannel].len;
			}
			else
			{
				if(atom->has_field(iIndexMulti))
					*(mxGetPr( params[atomMultiField[iIndexMulti - MP_NUM_MULTI_BEGININDEX_PROPS]]) + iIndexChannel*nAtom + curIdx ) = (double) atom->get_field(iIndexMulti,iIndexChannel);
			}
		}

		for(iIndexOthers = MP_NUM_OTHERS_BEGININDEX_PROPS; iIndexOthers < MP_NUM_OTHERS_BEGININDEX_PROPS + MP_NUM_OTHERS_PROPS; iIndexOthers++)
		{
			if(iIndexOthers == MP_PARTIAL_AMP_PROP || iIndexOthers == MP_PARTIAL_PHASE_PROP)
			{
				// Read Partial amp/phase info
				for (iIndex = 0 ; iIndex < iNumPartial ;iIndex++)
				{
					// !! CHECK THESE VALUES
					if(atom->has_field(iIndexOthers))
						*(mxGetPr( params[atomOthersField[iIndexOthers - MP_NUM_OTHERS_BEGININDEX_PROPS]]) + iIndexChannel*(nAtom*iNumPartial) + iIndex*nAtom + curIdx ) = (double) atom->get_field(iIndexOthers,iIndex*nChannel+iIndexChannel);
				}
			}
		}
	}
	curIdx++; //! Go to next index
}
            

//! Fill a given MEX 'atom' structure at index 'a' with parameters
mxArray * atomGroup::outputMxStruct(mxArray * atom, unsigned int a) {
  const char * func = "atomGroup::outputMxStruct";
  mp_debug_msg(MP_DEBUG_ABUNDANT,func," - Fill atom Structure for type [%s]\n",typeLen.c_str()); 
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
      mp_debug_msg(MP_DEBUG_ABUNDANT,func,"   - \"%s\" added\n",miter->first.c_str());
      p = mxAddField(par,miter->first.c_str());
      mxSetFieldByNumber(par,0, p, miter->second);
    } // End of atom parameters definition
                
  mxSetField(atom, a, "params", par);
  return atom;
}

/**********************************/
/*                                */
/*  END OF CLASS atomGroup METHODS  */
/*                                */
/**********************************/

// In preparation ...
/* atomCollection:append(MP_Atom_c* atom) {
	string aType;
	stringstream ss1;
	ss1 << atom->type_name() << "_" <<  atom->support[0].len ;
	ss1 >> aType;
	// Add a new type if needed.
	if( atomGroups.find(aType) == atomStats.end() ) {
		mp_debug_msg(MP_DEBUG_ABUNDANT,func,"Registering new atom type [%s]\n",aType.c_str());
		atomGroups[aType] = new atomGroup(aType,atom->numChans);
	}

}
 */
MP_Atom_c *GetMP_Atom(const mxArray *mxBook,MP_Chan_t numChans,unsigned long int atomIdx) {
  const char *func = "mxBook::getMP_Atom";
  
  // Add index info to book structure
  mxArray * mxIdx;
  unsigned int indexSize = 4+numChans;

  mxIdx = mxGetField(mxBook, 0, "index"); //! Matrix of size 4*nAtoms
  unsigned int t =  (unsigned int) *( mxGetPr(mxIdx) + atomIdx*indexSize + 1 ); //! typeMap index
  unsigned long int n =  (unsigned long int) *( mxGetPr(mxIdx) + atomIdx*indexSize + 2 ); //! atom number for given type
  MP_Chan_t c;
  
  // Get Atom type structure
  mxArray *mxAtom, *mxType, *mxParams;
  mxAtom = mxGetField(mxBook,0,"atom");      //! Structure with field 'type' 'params'
  mxParams = mxGetField(mxAtom,t-1,"params"); //! Structure with fields different for each type
  mxType = mxGetField(mxAtom,t-1,"type");     //! Get string type
  string aType(mxArrayToString(mxType));

  // Get Atom creator method 
  MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator(aType.c_str());
  if (NULL == emptyAtomCreator)  {
      mp_error_msg(func,"-- unknown  MP_Atom_Factory_c method for atomType:%s\n",aType.c_str());
      return( NULL );
    }
  
  // Create empty atom 
  MP_Atom_c *newAtom = (*emptyAtomCreator)();
  if ( NULL==newAtom ) {
	mp_error_msg(func,"-- could not create empty atom of type %s\n",aType.c_str());
	return( NULL );
  }
  // Allocate main fields
  if ( newAtom->alloc_atom_param(numChans) ) {
	  mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aType.c_str());
	  delete newAtom;
	  return( NULL );
	}
  mp_debug_msg(MP_DEBUG_SPARSE,func," -- atom index %ld [%s]\n",atomIdx,aType.c_str());
  
  // Retrieve pointer for common parameters to all atoms (pos, len, amp)
  mxArray *mxpos, *mxamp, *mxlen;
  mxpos = mxGetField(mxParams,0,"pos");
  mxlen = mxGetField(mxParams,0,"len");
  mxamp = mxGetField(mxParams,0,"amp");
   
  // number of atoms for this type (used for browsing mxArray)
  unsigned long int nA = (unsigned long int) mxGetM(mxamp); 

  // Fill the main fields
   for (c=0;c<numChans;c++) { // loop on channels
	newAtom->support[c].pos = (unsigned long int) *(mxGetPr(mxpos) + c*nA + (n-1));
	newAtom->support[c].len = (unsigned long int) *(mxGetPr(mxlen) + c*nA + (n-1));
	newAtom->totalChanLen += newAtom->support[c].len;
 	newAtom->amp[c] = (MP_Real_t)  *(mxGetPr(mxamp) + c*nA + (n-1));
	}
	
 
  // and fill it with the params field
  // CONSTANT / DIRAC / NYQUIST / 
  if (aType=="constant" || aType=="dirac" || aType=="nyquist") {
     return (newAtom);
    } 
  // GABOR / HARMONIC
  else if (aType=="gabor" || aType=="harmonic") {
	MP_Gabor_Atom_Plugin_c* gaborAtom =  (MP_Gabor_Atom_Plugin_c*)newAtom;
	if ( gaborAtom->alloc_gabor_atom_param(numChans) )	{
		mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aType.c_str());
		delete newAtom;
		return( NULL );
	}

	mxArray *mxchirp, *mxfreq, *mxphase, *mxwintype, *mxwinopt;
    mxchirp = mxGetField(mxParams,0,"chirp");
    mxfreq = mxGetField(mxParams,0,"freq");
	mxphase = mxGetField(mxParams,0,"phase");
	mxwintype = mxGetField(mxParams,0,"windowtype");
	mxwinopt = mxGetField(mxParams,0,"windowoption");
      
    gaborAtom->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
	gaborAtom->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
	gaborAtom->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
	gaborAtom->chirp = (MP_Real_t) *(mxGetPr(mxchirp) + (n-1));
	for (c=0;c<numChans;c++) { // loop on channels
		gaborAtom->phase[c] = (MP_Real_t)  *(mxGetPr(mxphase) + c*nA + (n-1));
	}
      
	if(aType=="gabor") {
		return (newAtom);
	}
	else if (aType=="harmonic")  {
		MP_Harmonic_Atom_Plugin_c* harmonicAtom =  (MP_Harmonic_Atom_Plugin_c*)newAtom;

        mxArray *mxnumPartial;
		mxnumPartial = mxGetField(mxParams,0,"numPartials");
		unsigned int numPartials = (unsigned int) *(mxGetPr(mxnumPartial) + (n-1));
		unsigned int p;
		if ( harmonicAtom->alloc_harmonic_atom_param( numChans, numPartials ) ) {
			mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aType.c_str());
			delete newAtom;
			return( NULL );
		}
		/** set atoms params */
		mxArray *mxharmo, *mxpartialamp, *mxpartialphase;      
		mxharmo = mxGetField(mxParams,0,"harmonicity");
		mxpartialamp = mxGetField(mxParams,0,"partialAmp");
		mxpartialphase = mxGetField(mxParams,0,"partialPhase");
      
		harmonicAtom->numPartials = numPartials;
		for (p=0;p<numPartials;p++) { // loop on partials
			harmonicAtom->harmonicity[p] =  (MP_Real_t)  *(mxGetPr(mxharmo) + p*nA +  (n-1));	  
		}
		for (c=0;c<numChans;c++) { // loop on channels
			for (p=0;p<numPartials;p++) { // loop on partials
				harmonicAtom->partialAmp[c][p] = (MP_Real_t)  *(mxGetPr(mxpartialamp) + c*(nA*numPartials) + p*nA + (n-1)); // When reading was c*(nAtom*nP) + h*nAtom + curIdx
				harmonicAtom->partialPhase[c][p] = (MP_Real_t)  *(mxGetPr(mxpartialphase) + c*(nA*numPartials) + p*nA + (n-1));
			}
		}
		return (newAtom);
	} else {
		mp_error_msg(func,"This code should never be reached!\n");
		delete newAtom;
		return(NULL);
	}
  }
  // ANYWAVE / ANYWAVE_HILBERT
  else if (aType=="anywave" || aType=="anywavehilbert") {  
	MP_Anywave_Atom_Plugin_c* anywaveAtom =  (MP_Anywave_Atom_Plugin_c*)newAtom;
    mxArray *mxtableidx, *mxanywaveidx;
	mxanywaveidx = mxGetField(mxParams,0,"anywaveIdx");
	anywaveAtom->anywaveIdx =  (unsigned long int) *(mxGetPr(mxanywaveidx) + (n-1));
	mxtableidx = mxGetField(mxParams,0,"tableIdx");
	anywaveAtom->tableIdx =  (unsigned long int) *(mxGetPr(mxtableidx) + (n-1));
	anywaveAtom->anywaveTable = MPTK_Server_c::get_anywave_server()->tables[anywaveAtom->tableIdx];
	if(NULL==anywaveAtom->anywaveTable) {
		mp_error_msg(func,"Failed to retrieve anywaveTable number %d from server\n",anywaveAtom->tableIdx);
		delete newAtom;
		return(NULL);
	}
	if(aType=="anywave") {
      return (newAtom);
    } else if (aType=="anywavehilbert") {
		MP_Anywave_Hilbert_Atom_Plugin_c* anywaveHilbertAtom =  (MP_Anywave_Hilbert_Atom_Plugin_c*)newAtom;
		if ( anywaveHilbertAtom->alloc_hilbert_atom_param(numChans) ) {
			mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aType.c_str());
			delete newAtom;
			return( NULL );
		}      
		// Channel independent fields
		mxArray *mxrealtableidx = mxGetField(mxParams,0,"realTableIdx");
		if(NULL== mxrealtableidx) {
			mp_error_msg(func,"Could not retrieve realTableIdx\n");
			delete newAtom;
			return(NULL);
		}
        anywaveHilbertAtom->realTableIdx =  (unsigned long int) *(mxGetPr(mxrealtableidx) + (n-1));
		anywaveHilbertAtom->anywaveRealTable = MPTK_Server_c::get_anywave_server()->tables[anywaveHilbertAtom->realTableIdx];
		if(NULL==anywaveHilbertAtom->anywaveRealTable) {
			mp_error_msg(func,"Failed to retrieve anywaveRealTable number %d from server\n",anywaveHilbertAtom->realTableIdx);
			delete newAtom;
			return(NULL);
		}
 
		mxArray *mxhilberttableidx = mxGetField(mxParams,0,"hilbertTableIdx");
	    if(NULL== mxhilberttableidx) {
			mp_error_msg(func,"Could not retrieve hilbertTableIdx\n");
			delete newAtom;
			return(NULL);
		}
  
		anywaveHilbertAtom->hilbertTableIdx =  (unsigned long int) *(mxGetPr(mxhilberttableidx) + (n-1));
		anywaveHilbertAtom->anywaveHilbertTable = MPTK_Server_c::get_anywave_server()->tables[anywaveHilbertAtom->hilbertTableIdx];
		if(NULL==anywaveHilbertAtom->anywaveHilbertTable) {
			mp_error_msg(func,"Failed to retrieve anywaveHilbertTable number %d from server\n",anywaveHilbertAtom->hilbertTableIdx);
			delete newAtom;
			return(NULL);
		}
 
		// Channel dependent fields
		mxArray *mxrealpart = mxGetField(mxParams,0,"realPart");
		if(NULL== mxrealpart) {
			mp_error_msg(func,"Could not retrieve realPart\n");
			delete newAtom;
			return(NULL);
		}
		mxArray *mxhilbertpart = mxGetField(mxParams,0,"hilbertPart");
		if(NULL== mxhilbertpart) {
			mp_error_msg(func,"Could not retrieve hilbertPart\n");
			delete newAtom;
			return(NULL);
		}
		for (c=0;c<numChans;c++) { // loop on channels
			// Real/Hilbert part
			anywaveHilbertAtom->realPart[c] = (MP_Real_t) *(mxGetPr(mxrealpart) + c*nA + (n-1));
			anywaveHilbertAtom->hilbertPart[c] = (MP_Real_t) *(mxGetPr(mxhilbertpart) + c*nA + (n-1));
		}
   
		return(newAtom);
	} else {
		mp_error_msg(func,"This code should never be reached!\n");
		delete newAtom;
		return(NULL);
	}
  }
   // MCLT ATOM
  else if (aType=="mclt")  {
	MP_Mclt_Atom_Plugin_c* mcltAtom =  (MP_Mclt_Atom_Plugin_c*)newAtom;
	if ( mcltAtom->alloc_mclt_atom_param(numChans) ) {
		mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n",aType.c_str());
		return( NULL );
	}

	mxArray *mxchirp, *mxfreq, *mxphase, *mxwintype, *mxwinopt;
	mxchirp = mxGetField(mxParams,0,"chirp");
	mxfreq = mxGetField(mxParams,0,"freq");
	mxphase = mxGetField(mxParams,0,"phase");
	mxwintype = mxGetField(mxParams,0,"windowtype");
	mxwinopt = mxGetField(mxParams,0,"windowoption");
      
	mcltAtom->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
	mcltAtom->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
	mcltAtom->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
	mcltAtom->chirp = (MP_Real_t) *(mxGetPr(mxchirp) + (n-1));
	for (c=0;c<numChans;c++) { // loop on channels
		mcltAtom->phase[c] = (MP_Real_t)  *(mxGetPr(mxphase) + c*nA + (n-1));
	}
      
      return (newAtom);
  }
  // MDCT ATOM
  else if (aType=="mdct") {
	MP_Mdct_Atom_Plugin_c* mdctAtom =  (MP_Mdct_Atom_Plugin_c*)newAtom;
	mxArray *mxfreq, *mxwintype, *mxwinopt;
	mxfreq = mxGetField(mxParams,0,"freq");
	mxwintype = mxGetField(mxParams,0,"windowtype");
	mxwinopt = mxGetField(mxParams,0,"windowoption");
      
	mdctAtom->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
	mdctAtom->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
	mdctAtom->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
	return(newAtom);
  }
  // MDST ATOM
  else if (aType=="mdst") {
	MP_Mdst_Atom_Plugin_c* mdstAtom =  (MP_Mdst_Atom_Plugin_c*)newAtom;
	mxArray *mxfreq, *mxwintype, *mxwinopt;
	mxfreq = mxGetField(mxParams,0,"freq");
	mxwintype = mxGetField(mxParams,0,"windowtype");
	mxwinopt = mxGetField(mxParams,0,"windowoption");
      
	mdstAtom->windowType = (unsigned char) *(mxGetPr(mxwintype) + (n-1));
	mdstAtom->windowOption = (double) *(mxGetPr(mxwinopt) + (n-1));
	mdstAtom->freq  = (MP_Real_t) *(mxGetPr(mxfreq) + (n-1));
	return(newAtom);
  }
  else {
	mp_error_msg(func,"Atom type [%s] unknown, consider adding its information in mxBook{h,cpp}\n",aType.c_str());
    return (NULL);
  } 

}


// Conversion from MEX book array to MPTK book
MP_Book_c *mp_create_book_from_mxBook(const mxArray *mxBook)  {
	const char * func = "mp_create_book_from_mxBook(mxArray)";
	mxArray *mxTmp,*mxIndex;
	MP_Chan_t numChans;
	int sampleRate;
	unsigned long int nAtom,numSamples;
    
	// Read matlab book structure header
	mxTmp = mxGetField(mxBook,0,"numAtoms");
	nAtom = (unsigned long int)mxGetScalar(mxTmp);
	mxTmp = mxGetField(mxBook,0,"numChans");
	numChans = (MP_Chan_t)mxGetScalar(mxTmp);
	mxTmp = mxGetField(mxBook,0,"numSamples");
	numSamples = (unsigned long int)mxGetScalar(mxTmp);
	mxTmp = mxGetField(mxBook,0,"sampleRate");
	sampleRate = (int)mxGetScalar(mxTmp);
	// Get book index
	unsigned int indexSize;
	mxIndex = mxGetField(mxBook,0,"index");
	indexSize = 4+numChans;
    
	// Create an empty book
	MP_Book_c *book = MP_Book_c::create(numChans,numSamples,sampleRate);

	// Add selected atoms to book
 	unsigned long int a;
	for (a=0;a<nAtom;a++) {
		if (*(mxGetPr(mxIndex) + a*indexSize + 3) != 0.0 ) { //Only keeps an atom if "selected".
			//! add_atom
			mp_debug_msg(MP_DEBUG_ABUNDANT,func," - Adding Atom [%ld] to book :",a);
			MP_Atom_c * atom = GetMP_Atom(mxBook,numChans,a);      
			if ( NULL==atom ) {
				delete book;
				mp_error_msg(func," GetMP_Atom returned NULL while adding Atom [%ld] to book :",a);
				return(NULL);
			} else {
				book->append( atom );
			}
		}
	}
    
	mp_info_msg(func," - [%ld] atoms have been added to book.\n",book->numAtoms);
	return book;
}

 
// Conversion from MPTK book array to MEX book array
mxArray *mp_create_mxBook_from_book(MP_Book_c *book) {

	const char *func = "mp_create_mxBook_from_book(MP_Book_c)";

	// Allocate Output book structure
	int numBookFieldNames = 7;
	const char *bookFieldNames[] = {"format","numAtoms","numChans","numSamples","sampleRate","index","atom"};
  	mxArray *mxBook = mxCreateStructMatrix((mwSize)1,(mwSize)1,numBookFieldNames,bookFieldNames); //! Output book structure

	// Fill header info 
	mxArray *mxTmp;
	mxTmp = mxCreateString("0.1");
	mxSetField(mxBook,0, "format", mxTmp);
	mxTmp = mxCreateDoubleScalar((double) book->numAtoms); //! numAtoms
	mxSetField(mxBook, 0, "numAtoms", mxTmp);
	mxTmp = mxCreateDoubleScalar((double) book->numChans); //! numChans
	mxSetField(mxBook, 0, "numChans", mxTmp);
	mxTmp = mxCreateDoubleScalar((double) book->numSamples); //! numSamples
	mxSetField(mxBook, 0, "numSamples", mxTmp);
	mxTmp = mxCreateDoubleScalar(book->sampleRate); //! sampleRate
	mxSetField(mxBook, 0, "sampleRate", mxTmp);
	
	// Count atom types and fill index
	mxArray * mxIndex;
	unsigned int indexSize = 4+book->numChans;
	mxIndex = mxCreateDoubleMatrix((mwSize)indexSize,(mwSize)book->numAtoms, mxREAL); //! mxIndex contains (1: Atom number, 2: type index, 3: atom index, 4: Atom selected, 4+chan: atom pos of channel chan)
    
	mp_debug_msg(MP_DEBUG_GENERAL,func,"Counting atom types in book : \n");

	// Some declarations
	map <string, atomGroup *> atomStats;       //! Map <atom type , nb of occurence>
	map <string, atomGroup *>::iterator miter; //! and a iterator on it
	map <string, MP_Atom_c *> firstOc;       //! Map <atom type , first occurence>
	map <string, MP_Atom_c *>::iterator fiter; //! and a iterator on it
	mxArray *mxAtom;
	unsigned long int n;

	for (n=0;n<book->numAtoms;n++) {
		// Get the aType string "<type_name>_<length>"
		string aType;
		stringstream ss1;
		ss1 << book->atom[n]->type_name() << "_" <<  book->atom[n]->support[0].len ;
		ss1 >> aType;
		// If aType is a new type, register it in maps
		if ( atomStats.find(aType) == atomStats.end() ) {
			mp_debug_msg(MP_DEBUG_ABUNDANT,func,"Registering new atom type [%s]\n",aType.c_str());
			atomStats[aType] = new atomGroup(aType,book->numChans);
			firstOc[aType] = book->atom[n];
		}
        atomStats[aType]->nAtom++; //! Increment atom count 
		//! Fill book index info for this atom
		*( mxGetPr(mxIndex) + n*indexSize ) = (double) (n+1);                         //! Atom number
		// As the map is not filled the typeIdx cannot be set at this stage
		// *( mxGetPr(mxIndex) + 4*n + 1 ) = (double) atomStats[aType]->typeIdx; //! Type index
		*( mxGetPr(mxIndex) + n*indexSize + 2 ) = (double) atomStats[aType]->nAtom;   //! Atom number
		// loop on channels to get their position
		*( mxGetPr(mxIndex) + n*indexSize + 3 ) = (double) 1.0;   //! Atom selected 
		// loop on channels to get their position
		unsigned short int c;
		for (c=0;c<book->numChans;c++) {
			*( mxGetPr(mxIndex) + n*indexSize + 4 + c) = (double) book->atom[n]->support[c].pos;     //! Atom pos
		}
	}

	/* Matlab console info */
	mp_debug_msg(MP_DEBUG_GENERAL,func, "found %d different atom types\n",atomStats.size());
    
	/* Init Atom structure
	 *  atom.type = string
	 *  atom.params = structure, size : 1*1 of atoms type found in book
	 *       For each type of atom, params is a structure
	 *       with the specified parameters (depend on atom type)
	 */
	
	/** Allocate structure params for the different type of atoms
	 *  and set the correct type index */
	unsigned int t = 0;  //! Index of type used for atomGroup constructors
	for ( miter = atomStats.begin(), fiter = firstOc.begin(); miter != atomStats.end(); ++miter, ++fiter )
    {
		mp_debug_msg(MP_DEBUG_SPARSE,func," - atom [%s] :  %ld occurences\n",miter->first.c_str(), miter->second->nAtom);
		miter->second->allocParams(miter->second->nAtom, fiter->second);
		miter->second->typeIdx = t;  //! Set type index according to the map iterator
		t++; //! increment type index
    }
	
    
	mp_debug_msg(MP_DEBUG_GENERAL,func,"Load each atom\n");
	
	/* Parse Atoms parameters */
	for ( n=0 ; n<book->numAtoms ; n++ ) {
		string aType;
		stringstream ss1;
		ss1 << book->atom[n]->type_name() << "_" <<  book->atom[n]->support[0].len ;
		ss1 >> aType;
		
		if ( atomStats.find(aType) != atomStats.end() ) {
			atomStats[aType]->append(book->atom[n]);
			*( mxGetPr(mxIndex) + n*indexSize + 1 ) = (double) ( atomStats[aType]->typeIdx + 1); //! Type index
		} else {
			mxFree(mxBook);
			mp_error_msg(func,"Atom [%ld] was not recognized\n",n);
			return(NULL);
		}
	}
	
	// Add atom structure to output Variables
	int numAtomFieldNames = 2;
	const char *atomFieldNames[] = {"type","params"};
	mxAtom = mxCreateStructMatrix((mwSize)1,(mwSize)(atomStats.size()),numAtomFieldNames,atomFieldNames);
    
	mp_debug_msg(MP_DEBUG_GENERAL,func,"Creating output structure\n");
	n = 0;
	for ( miter = atomStats.begin(); miter != atomStats.end(); ++miter )
    {
		mp_debug_msg(MP_DEBUG_SPARSE,func," - atom [%s] \n",miter->second->type.c_str());
		miter->second->outputMxStruct(mxAtom,n);
		n++;
    } // End of atom parameters definition
	
	/** Add index info to book structure */ 
	mxSetField(mxBook, 0, "index", mxIndex);
	
	// Attach the atom structure to mexbook.atom
	mxSetField(mxBook, 0, "atom", mxAtom);
	return(mxBook);
}


