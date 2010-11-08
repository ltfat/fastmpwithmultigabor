/******************************************************************************/
/*                                                                            */
/*                         mdct_block.cpp      		                      */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
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

/****************************************************************/
/*                                               		*/
/* mdct_block.cpp: methods for mclt blocks			*/
/*                                               		*/
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mdct_atom_plugin.h"
#include "mdct_block_plugin.h"
#include "mclt_abstract_block_plugin.h"
#include <sstream>


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
MP_Block_c *
MP_Mdct_Block_Plugin_c::create (MP_Signal_c * setSignal, map < string, string,
				mp_ltstring > *paramMap)
{

  const char *func =
    "MP_Mdct_Block_Plugin_c::create( MP_Signal_c *setSignal, map<const char*, const char*, MP_Dict_c::ltstr> *paramMap )";
  MP_Mdct_Block_Plugin_c *newBlock = NULL;
  char *convertEnd;
  unsigned long int filterLen = 0;
  unsigned long int filterShift = 0;
  unsigned char windowType;
  double windowOption = 0.0;
  double windowRate = 0.0;
  unsigned long int blockOffset = 0;

  /* Instantiate and check */
  newBlock = new MP_Mdct_Block_Plugin_c ();
  if (newBlock == NULL) {
    mp_error_msg (func, "Failed to create a new Gabor block.\n");
    return (NULL);
  }
  /* Analyse the parameter map */
  if (strcmp ((*paramMap)["type"].c_str (), "mdct")) {
    mp_error_msg (func, "Parameter map does not define a MDCT block.\n");
    return (NULL);
  }

  if ((*paramMap)["windowLen"].size () > 0) {
    /*Convert window length */
    filterLen = strtol ((*paramMap)["windowLen"].c_str (), &convertEnd, 10);
    if (*convertEnd != '\0') {
      mp_error_msg (func,
		    "cannot convert parameter windowLen in unsigned long int.\n");
      return (NULL);
    }
  }
  else {
    mp_error_msg (func, "No parameter windowLen in the parameter map.\n");
    return (NULL);
  }

  if ((*paramMap)["windowtype"].size () > 0)
    windowType = window_type ((*paramMap)["windowtype"].c_str ());
  else {
    mp_error_msg (func, "No parameter windowtype in the parameter map.\n");
    return (NULL);
  }

  if (window_needs_option (windowType)
      && ((*paramMap)["windowopt"].size () == 0)) {
    mp_error_msg (func, "MDCT block "
		  " requires a window option (the opt=\"\" attribute is probably missing"
		  " in the relevant <window> tag). Returning a NULL block.\n");
    return (NULL);
  }
  else {
    /*Convert windowopt */
    windowOption = strtod ((*paramMap)["windowopt"].c_str (), &convertEnd);
    if (*convertEnd != '\0') {
      mp_error_msg (func,
		    "cannot convert parameter window option in double.\n");
      return (NULL);
    }
  }

  if ((*paramMap)["windowShift"].size () > 0) {
    /*Convert windowShift */
    filterShift =
      strtol ((*paramMap)["windowShift"].c_str (), &convertEnd, 10);
    if (*convertEnd != '\0') {
      mp_error_msg (func,
		    "cannot convert parameter windowShift in unsigned long int.\n");
      return (NULL);
    }
  }
  else {
    if ((*paramMap)["windowRate"].size () > 0) {
      windowRate = strtod ((*paramMap)["windowRate"].c_str (), &convertEnd);
      if (*convertEnd != '\0') {
	mp_error_msg (func,
		      "cannot convert parameter windowRate in unsigned long int.\n");
	return (NULL);
      }
      filterShift =
	(unsigned long int) ((double) (filterLen) * windowRate + 0.5);
      filterShift = (filterShift > 1 ? filterShift : 1);	/* windowShift has to be 1 or more */

    }
    else {
      if ((*paramMap)["windowLen"].size () > 0) {
	if ((!strcmp (((*paramMap)["windowtype"]).c_str (), "rectangle"))
	    || (!strcmp (((*paramMap)["windowtype"]).c_str (), "cosine"))
	    || (!strcmp (((*paramMap)["windowtype"]).c_str (), "kbd")))
	  filterShift = filterLen / 2;
	else {
	  mp_error_msg (func,
			"Wrong window type. It has to be: rectangle, cosine or kbd.\n");
	}
      }
      else {
	mp_error_msg (func,
		      "No parameter windowShift or windowRate in the parameter map.\n");
	return (NULL);
      }
    }
  }

  if ((*paramMap)["blockOffset"].size () > 0) {
    /*Convert blockOffset */
    blockOffset =
      strtol ((*paramMap)["blockOffset"].c_str (), &convertEnd, 10);
    if (*convertEnd != '\0') {
      mp_error_msg (func,
		    "cannot convert parameter windowShift in unsigned long int.\n");
      return (NULL);
    }
  }
  /* Set the block parameters (that are independent from the signal) */
  if (newBlock->init_parameters (filterLen, filterShift, windowType, windowOption, blockOffset)) {
    mp_error_msg (func,
		  "Failed to initialize some block parameters in the new MDCT block.\n");
    delete (newBlock);
    return (NULL);
  }

  /* Set the block parameter map (that are independent from the signal) */
  if (newBlock->init_parameter_map (filterLen, filterShift, windowType, windowOption, blockOffset)) {
    mp_error_msg (func,
		  "Failed to initialize parameters map in the new MDCT block.\n");
    delete (newBlock);
    return (NULL);
  }
  /* Set the signal-related parameters */
  if (newBlock->plug_signal (setSignal)) {
    mp_error_msg (func, "Failed to plug a signal in the new MDCT block.\n");
    delete (newBlock);
    return (NULL);
  }

  return ((MP_Block_c *) newBlock);
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int
MP_Mdct_Block_Plugin_c::init_parameters (const unsigned long int setFilterLen,
					 const unsigned long int setFilterShift,
					 const unsigned char setWindowType,
					 const double setWindowOption,
					 const unsigned long int setBlockOffset)
{

  const char *func = "MP_Mdct_Block_Plugin_c::init_parameters()";

  /* Check the validity of setFilterLen */
  if ( is_odd(setFilterLen) || is_odd(setFilterLen/2) ) { /* If windowLen is odd: windowLen has to be even! */
    mp_error_msg( func, "windowLen [%lu] must be a multiple of 4.\n" ,
          setFilterLen );
    return( 1 );
  }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( setFilterLen, setFilterShift, setFilterLen/2, setBlockOffset ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Gabor block.\n" );
    return( 1 );
  }

  /* Create the DCT object */
  dct = (MP_DCT_Interface_c*)MP_DCT_Interface_c::init(filterLen);
  if ( dct == NULL ) {
    mp_error_msg( func, "Failed to init the DCT in the new MDCT block.\n" );
    return( 1 );
  }
  
  /* Hook the window */
  window = new MP_Real_t[filterLen];
  windowType = setWindowType;
  windowOption = setWindowOption;
  if (!window){
    mp_error_msg( func, "Failed to allocate the window in the new MDCT block.\n" );
    return( 1 );
  }
  make_window(window, filterLen, setWindowType, setWindowOption);

  return (0);
}

int
MP_Mdct_Block_Plugin_c::init_parameter_map (const unsigned long int setFilterLen,
		    const unsigned long int setFilterShift,
		    const unsigned char setWindowType,
		    const double setWindowOption,
		    const unsigned long int setBlockOffset)
{
  const char *func = "MP_Gabor_Block_c::init_parameter_map(...)";

  parameterMap = new map < string, string, mp_ltstring > ();

  /*Create a stream for convert number into string */
  std::ostringstream oss;

  (*parameterMap)["type"] = type_name ();

  /* put value in the stream */
  if (!(oss << setFilterLen)) {
    mp_error_msg (func,
		  "Cannot convert windowLen in string for parameterMap.\n");
    return (1);
  }
  /* put stream in string */
  (*parameterMap)["windowLen"] = oss.str ();
  /* clear stream */
  oss.str ("");
  if (!(oss << setFilterShift)) {
    mp_error_msg (func,
		  "Cannot convert windowShift in string for parameterMap.\n");
    return (1);
  }
  (*parameterMap)["windowShift"] = oss.str ();
  oss.str ("");

  oss.str ("");
  (*parameterMap)["windowtype"] = window_name (setWindowType);

  if (!(oss << setWindowOption)) {
    mp_error_msg (func,
		  "Cannot convert windowopt in string for parameterMap.\n");
    return (1);
  }
  (*parameterMap)["windowopt"] = oss.str ();
  oss.str ("");
  if (!(oss << setBlockOffset)) {
    mp_error_msg (func,
		  "Cannot convert blockOffset in string for parameterMap.\n");
    return (1);
  }
  (*parameterMap)["blockOffset"] = oss.str ();
  oss.str ("");

  return (0);
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int
MP_Mdct_Block_Plugin_c::plug_signal (MP_Signal_c * setSignal)
{

  const char* func = "MP_mdct_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    /* Go up the inheritance graph */
    if ( MP_Block_c::plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }
    
    /* Allocate the mag array */
    if ( (mag = (MP_Real_t*) calloc( numFilters*s->numChans , sizeof(MP_Real_t) )) == NULL ) {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
            " for the mag array. Nullifying all the signal-related parameters.\n",
            numFilters*s->numChans );
      nullify_signal();
      return( 1 );
    }
  }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void
MP_Mdct_Block_Plugin_c::nullify_signal (void)
{
  if ( mag ) { 
    free(mag);
    mag = NULL;
  }
  MP_Block_c::nullify_signal();
}

/********************/
/* NULL constructor */
MP_Mdct_Block_Plugin_c::MP_Mdct_Block_Plugin_c (void)
{

  frameBuffer = NULL;
  window = NULL;

}


/**************/
/* Destructor */
MP_Mdct_Block_Plugin_c::~MP_Mdct_Block_Plugin_c (){
    nullify_signal();
    if (window)
        delete[] window;
    if (frameBuffer)
        delete[] frameBuffer;
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
const char *
MP_Mdct_Block_Plugin_c::type_name ()
{
  return ("mdct");
}

/**********************/
/* Readable text dump */
int
MP_Mdct_Block_Plugin_c::info (FILE * fid)
{

  int nChar = 0;

  nChar += mp_info_msg (fid, "MDCT BLOCK", "%s window (window opt=%g)"
			" of length [%lu], shifted by [%lu] samples,\n",
			window_name (windowType), windowOption,
			filterLen, filterShift);
  nChar +=
    mp_info_msg (fid, "         |-", "projected on [%lu] frequencies;\n",
		 numFilters);
  nChar +=
    mp_info_msg (fid, "         O-",
		 "The number of frames for this block is [%lu], "
		 "the search tree has [%lu] levels.\n", numFrames, numLevels);

  return (nChar);
}

/********************************************/
/* Frame-based update of the inner products */
void
MP_Mdct_Block_Plugin_c::update_frame (unsigned long int frameIdx,
				      MP_Real_t * maxCorr,
				      unsigned long int *maxFilterIdx)
{
  unsigned long int inShift;
  unsigned long int i;

  MP_Real_t *in;
  MP_Real_t *magPtr;

  double sum;
  double max;
  unsigned long int maxIdx;

  int chanIdx;
  int numChans;

  assert (s != NULL);
  numChans = s->numChans;
  assert (mag != NULL);

  inShift = frameIdx * filterShift + blockOffset;

  mag[frameIdx] = 0;

  /*----*/
  /* Fill the mag array: */
  for (chanIdx = 0, magPtr = mag;	/* <- for each channel */
       chanIdx < numChans; chanIdx++, magPtr += numFilters) {

    assert (s->channel[chanIdx] != NULL);

    /* Hook the signal and the inner products to the fft */
    in = s->channel[chanIdx] + inShift;

    for (i = 0; i < lapSize; i++) {
      frameBuffer[i + numFilters] =
	in[i] * window[i] - in[numFilters - 1 - i] * window[numFilters - 1 - i];
      frameBuffer[i] =
	-in[filterLen - lapSize - 1 - i] * window[filterLen - lapSize -
						  1 - i] - in[filterLen -
							      lapSize +
							      i] *
	window[filterLen - lapSize + i];
    }

    dct->exec_mag (frameBuffer, mag + numFilters * chanIdx);
  }				/* end foreach channel */
  /*----*/

  /*----*/
  /* Make the sum and find the maxcorr: */
  /* --Element 0: */
  /* - make the sum */
  sum = (double) (*(mag));	/* <- channel 0      */
  for (chanIdx = 1, magPtr = mag + numFilters;	/* <- other channels */
       chanIdx < numChans; chanIdx++, magPtr += numFilters)
    sum += (double) (*(magPtr));
  /* - init the max */
  max = sum;
  maxIdx = 0;
  /* -- Following elements: */
  for (i = 1; i < numFilters; i++) {
    /* - make the sum */
    sum = (double) (*(mag + i));	/* <- channel 0      */
    for (chanIdx = 1, magPtr = mag + numFilters + i;	/* <- other channels */
	 chanIdx < numChans; chanIdx++, magPtr += numFilters)
      sum += (double) (*(magPtr));
    /* - update the max */
    if (sum > max) {
      max = sum;
      maxIdx = i;
    }
  }
  *maxCorr = (MP_Real_t) max;
  *maxFilterIdx = maxIdx;
}

/********************************************/
/* Frame-based update of the inner products */
void
MP_Mdct_Block_Plugin_c::update_frame (unsigned long int frameIdx,
				      MP_Real_t * maxCorr,
				      unsigned long int *maxFilterIdx,
				      GP_Param_Book_c * touchBook)
{
  unsigned long int inShift;
  unsigned long int i;

  MP_Real_t *in;
  MP_Real_t *magPtr;

  double sum;
  double max;
  unsigned long int maxIdx;
  MP_Real_t freq;

  int chanIdx;
  int numChans;

  bool found, atEnd;

  GP_Param_Book_Iterator_c iter;
  if (!touchBook)
    return update_frame (frameIdx, maxCorr, maxFilterIdx);
  //cerr << "touchBook.begin() = " << endl;
  //iter->info(stderr);

  assert (s != NULL);
  numChans = s->numChans;
  assert (mag != NULL);

  inShift = frameIdx * filterShift + blockOffset;

  mag[frameIdx] = 0;

  /*----*/
  /* Fill the mag array: */
  for (chanIdx = 0, magPtr = mag;	/* <- for each channel */
       chanIdx < numChans; chanIdx++, magPtr += numFilters) {

    assert (s->channel[chanIdx] != NULL);

    /* Hook the signal and the inner products to the fft */
    in = s->channel[chanIdx] + inShift;

    for (i = 0; i < lapSize; i++) {
      frameBuffer[i + numFilters] =
	in[i] * window[i] - in[numFilters - 1 - i] * window[numFilters - 1 - i];
      frameBuffer[i] =
	-in[filterLen - lapSize - 1 - i] * window[filterLen - lapSize -
						  1 - i] - in[filterLen -
							      lapSize +
							      i] *
	window[filterLen - lapSize + i];
    }

    dct->exec_dct (frameBuffer, mag + numFilters * chanIdx);
  }				/* end foreach channel */
  /*----*/

  /*----*/
  /* Make the sum, update touchBook and find the maxcorr: */
  /* --Element 0: */
  /* - make the sum and update touchBook */

  iter = touchBook->begin ();
  if (iter == touchBook->end ())
    atEnd = true;
  else
    atEnd = false;
  if (!atEnd) {
    freq = 1 / filterLen;
    found = (iter->get_field(MP_FREQ_PROP, 0) == freq);
  }
  else
    found = false;
  if (found)
    iter->corr[0] = *mag;
#ifdef MP_MAGNITUDE_IS_SQUARED
  sum += (double) (*mag * (*mag));
#else
  sum += fabs (*mag);
#endif
  /* <- channel 0      */
  for (chanIdx = 1, magPtr = mag + numFilters;	/* <- other channels */
       chanIdx < numChans; chanIdx++, magPtr += numFilters) {
    if (found)
      iter->corr[chanIdx] = *magPtr;
#ifdef MP_MAGNITUDE_IS_SQUARED
    sum += (double) (*magPtr * (*magPtr));
#else
    sum += fabs (*magPtr);
#endif
  }
  if (found) {
    ++iter;
    if (iter == touchBook->end ()) {
      atEnd = true;
      found = false;
    }
  }

  /* - init the max */
  max = sum;
  maxIdx = 0;

  /* -- Following elements: */
  for (i = 1; i < numFilters; i++) {
    if (!atEnd) {
      freq = (i + 0.5) / filterLen;
      found = (iter->get_field(MP_FREQ_PROP, 0) == freq);
    }
    if (found)
      iter->corr[0] = *(mag + i);
    /* - make the sum */
    sum = (double) (*(mag + i));	/* <- channel 0      */
    for (chanIdx = 1, magPtr = mag + numFilters + i;	/* <- other channels */
	 chanIdx < numChans; chanIdx++, magPtr += numFilters) {
      if (found)
	   iter->corr[chanIdx] = *magPtr;
#ifdef MP_MAGNITUDE_IS_SQUARED
      sum += (double) (*magPtr * (*magPtr));
#else
      sum += fabs (*magPtr);
#endif
    }
    /* - update the max */
    if (sum > max) {
      max = sum;
      maxIdx = i;
    }

    /* advance in touchBook */
    if (found) {
      ++iter;
      if (iter == touchBook->end ()) {
	   atEnd = true;
	   found = false;
      }
    }
  }
  *maxCorr = (MP_Real_t) max;
  *maxFilterIdx = maxIdx;
}

/***************************************/
/* Output of the ith atom of the block */
unsigned int
MP_Mdct_Block_Plugin_c::create_atom (MP_Atom_c ** atom,
				     const unsigned long int frameIdx,
				     const unsigned long int freqIdx)
{

  const char *func = "MP_Mdct_Block_c::create_atom(...)";
  MP_Mdct_Atom_Plugin_c *matom = NULL;
  /* Time-frequency location: */
  unsigned long int pos = frameIdx * filterShift + blockOffset;
  unsigned long int i;
  /* Parameters for a new FFT run: */
  MP_Real_t *in;
  /* Misc: */
  int chanIdx;

  /* Check the position */
  if ((pos + filterLen) > s->numSamples) {
    mp_error_msg (func,
		  "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n");
    *atom = NULL;
    return (0);
  }
  /* Allocate the atom */
  *atom = NULL;
  MP_Atom_c *(*emptyAtomCreator) (void) =
    MP_Atom_Factory_c::get_atom_factory ()->get_empty_atom_creator ("mdct");
  if (NULL == emptyAtomCreator) {
    mp_error_msg (func, "Mdct atom is not registred in the atom factory");
    return (0);
  }

  if ((matom = (MP_Mdct_Atom_Plugin_c *) (*emptyAtomCreator) ()) == NULL) {
    mp_error_msg (func, "Can't create a new Mclt atom in create_atom()."
		  " Returning NULL as the atom reference.\n");
    return (0);
  }
  if (matom->alloc_atom_param (s->numChans)) {
    mp_error_msg (func,
		  "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n");
    return (0);

  }
  if (window_type_is_ok (windowType)) {
    matom->windowType = windowType;
    matom->windowOption = windowOption;
  }
  else {
    mp_error_msg (func,
		  "The window type is unknown. Returning a NULL atom.\n");
    return (0);
  }

  /* Set the default freq */
  matom->freq = 0.0;

  /* 1) set the frequency of the atom */
  matom->freq = 1 / filterLen * (freqIdx + 0.5);
  matom->numSamples = pos + filterLen;

  /* For each channel: */
  for (chanIdx = 0; chanIdx < s->numChans; chanIdx++) {

    /* 2) set the support of the atom */
    matom->support[chanIdx].pos = pos;
    matom->support[chanIdx].len = filterLen;
    matom->totalChanLen += filterLen;

    /* 3) seek the right location in the signal */
    in = s->channel[chanIdx] + pos;

    /* 4) Preprocessing: windowing and folding */
    for (i = 0; i < lapSize; i++) {
      frameBuffer[i + numFilters] = in[i] * window[i] - 
        in[numFilters-1-i] * window[numFilters-1-i];
      frameBuffer[i] = -in[filterLen-lapSize-1-i] * window[filterLen-lapSize-1-i]
        -in[filterLen-lapSize +i] * window[filterLen-lapSize+i];
    }

    /* 5) Compute transform */
    dct->exec_dct (frameBuffer, mag);

    /* 6) fill in the atom parameters */
    matom->amp[chanIdx] = (MP_Real_t) (mag[freqIdx]);

  }

  *atom = matom;

  return (1);

}

unsigned long int MP_Mdct_Block_Plugin_c::build_frame_waveform_corr (
                            GP_Param_Book_c * frame,
						    MP_Real_t * outBuffer)
{
  GP_Param_Book_Iterator_c iter;
  unsigned long int freqIdx, t;
  MP_Chan_t c;
  MP_Real_t* magPtr;
  MP_Real_t* outPtr;

  cout << "MDCT" << endl;
  // clean the buffers
  memset (mag, 0, sizeof (MP_Real_t) * numFilters * s->numChans);
  
  // get the frequency index
  freqIdx = (unsigned long) (iter->get_field (MP_FREQ_PROP, 0) * filterLen - 0.5);

  for (c = 0, magPtr = mag, outPtr = outBuffer; 
       c < s->numChans; 
       c++, magPtr += numFilters, outPtr += filterLen) {
    // fill the buffers
    for (iter = frame->begin (); iter != frame->end (); ++iter) {
        // get the frequency index
        freqIdx = (unsigned long) (iter->get_field (MP_FREQ_PROP, 0) * filterLen - 0.5);

        *(magPtr + freqIdx) = (*((iter->corr) + c));
    }

    dct->exec_dct(magPtr, frameBuffer);
    
    // unfold and window
    t = 0;
    while (t < lapSize){
        outPtr[t] = -0.5*window[t]*frameBuffer[lapSize-1-t];
        outPtr[lapSize+t] = 0.5*window[lapSize+t]*frameBuffer[t];
    }
    while (t < numFilters){
        outPtr[lapSize+t] = 0.5*window[lapSize+t]*frameBuffer[t];
        outPtr[numFilters+t] = 0.5*window[numFilters+t]*frameBuffer[numFilters-1-t];
    }
  }
  return filterLen;
}

unsigned long int MP_Mdct_Block_Plugin_c::build_frame_waveform_amp (
                            GP_Param_Book_c * frame,
                            MP_Real_t * outBuffer)
{
  GP_Param_Book_Iterator_c iter;
  unsigned long int freqIdx, t;
  MP_Chan_t c;
  MP_Real_t* magPtr;
  MP_Real_t* outPtr;

  cout << "MDCT" << endl;
  // clean the buffers
  memset (mag, 0, sizeof (MP_Real_t) * numFilters * s->numChans);
  
  // get the frequency index
  freqIdx = (unsigned long) (iter->get_field (MP_FREQ_PROP, 0) * filterLen - 0.5);

  for (c = 0, magPtr = mag, outPtr = outBuffer; 
       c < s->numChans; 
       c++, magPtr += numFilters, outPtr += filterLen) {
    // fill the buffers
    for (iter = frame->begin (); iter != frame->end (); ++iter) {
        // get the frequency index
        freqIdx = (unsigned long) (iter->get_field (MP_FREQ_PROP, 0) * filterLen - 0.5);

        *(magPtr + freqIdx) = (*((iter->amp) + c));
    }

    dct->exec_dct(magPtr, frameBuffer);
    
    // unfold and window
    t = 0;
    while (t < lapSize){
        outPtr[t] = -0.5*window[t]*frameBuffer[lapSize-1-t];
        outPtr[lapSize+t] = 0.5*window[lapSize+t]*frameBuffer[t];
    }
    while (t < numFilters){
        outPtr[lapSize+t] = 0.5*window[lapSize+t]*frameBuffer[t];
        outPtr[numFilters+t] = 0.5*window[numFilters+t]*frameBuffer[numFilters-1-t];
    }
  }
  return filterLen;
}

/*********************************************/
/* get Paramater type map defining the block */
void
MP_Mdct_Block_Plugin_c::get_parameters_type_map (map < string, string,
						 mp_ltstring >
						 *parameterMapType)
{

  const char *func = "void MP_Mdct_Block_Plugin_c::get_parameters_type_map()";

  if ((*parameterMapType).empty ()) {
    (*parameterMapType)["type"] = "string";
    (*parameterMapType)["windowLen"] = "ulong";
    (*parameterMapType)["windowShift"] = "ulong";
    (*parameterMapType)["windowtype"] = "string";
    (*parameterMapType)["windowopt"] = "real";
    (*parameterMapType)["blockOffset"] = "ulong";
    (*parameterMapType)["windowRate"] = "real";

  }
  else
    mp_error_msg (func, "Map for parameters type wasn't empty.\n");



}

/***********************************/
/* get Info map defining the block */
void
MP_Mdct_Block_Plugin_c::get_parameters_info_map (map < string, string,
						 mp_ltstring >
						 *parameterMapInfo)
{

  const char *func = "void MP_Mdct_Block_Plugin_c::get_parameters_info_map()";

  if ((*parameterMapInfo).empty ()) {
    (*parameterMapInfo)["type"] = "the type of blocks";
    (*parameterMapInfo)["windowLen"] =
      "The common length of the atoms (which is the length of the signal window), in number of samples.";
    (*parameterMapInfo)["windowShift"] =
      "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
    (*parameterMapInfo)["windowtype"] =
      "The window type, which determines its shape. Examples include 'gauss', 'rect', 'hamming' (see the developper documentation of libdsp_windows.h). A related parameter is <windowopt>.";
    (*parameterMapInfo)["windowopt"] =
      "The optional window shape parameter (see the developper documentation oflibdsp_windows.h).";
    (*parameterMapInfo)["blockOffset"] =
      "Offset between beginning of signal and beginning of first atom, in number of samples.";
    (*parameterMapInfo)["windowRate"] =
      "The shift between atoms on adjacent time frames, in proportion of the <windowLen>. For example, windowRate = 0.5 corresponds to half-overlapping signal windows.";
  }
  else
    mp_error_msg (func, "Map for parameters info wasn't empty.\n");

}

/***********************************/
/* get default map defining the block */
void
MP_Mdct_Block_Plugin_c::get_parameters_default_map (map < string, string,
						    mp_ltstring >
						    *parameterMapDefault)
{

  const char *func =
    "void MP_Mdct_Block_Plugin_c::get_parameters_default_map( )";

  if ((*parameterMapDefault).empty ()) {
    (*parameterMapDefault)["type"] = "mdct";
    (*parameterMapDefault)["windowLen"] = "1024";
    (*parameterMapDefault)["windowShift"] = "512";
    (*parameterMapDefault)["windowtype"] = "rectangle";
    (*parameterMapDefault)["windowopt"] = "0";
    (*parameterMapDefault)["blockOffset"] = "0";
    (*parameterMapDefault)["windowRate"] = "0.5";
  }

  else
    mp_error_msg (func, "Map for parameter default wasn't empty.\n");

}

/******************************************************/
/* Registration of new block (s) in the block factory */

DLL_EXPORT void
registry (void)
{
  MP_Block_Factory_c::get_block_factory ()->register_new_block ("mdct",
								&MP_Mdct_Block_Plugin_c::
								create,
								&MP_Mdct_Block_Plugin_c::
								get_parameters_type_map,
								&MP_Mdct_Block_Plugin_c::
								get_parameters_info_map,
								&MP_Mdct_Block_Plugin_c::
								get_parameters_default_map);
}
