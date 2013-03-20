/******************************************************************************/
/*                                                                            */
/*                              mpd_core.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
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
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

#include <mptk.h>
#include <iostream>

using namespace std;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************/
/* Factory function:*/
/********************/

/* - signal+book only: */
MP_Mpd_Core_c* MP_Mpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook ) {

  const char* func = "MP_Mpd_Core_c::init(2 args)";
  
  MP_Mpd_Core_c* newCore;

  /* Check for NULL entries */
  if ( setSignal == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_Mpd_Core_c from a NULL signal.\n" );
    return( NULL );
  }
  
  if ( setBook == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_Mpd_Core_c from a NULL dictionary vector.\n" );
    return( NULL );
  } 

  /* Instantiate and check */
  newCore = new MP_Mpd_Core_c();
  
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );
  }
  /* Plug the book */
  newCore->book = setBook;

  /* Plug the signal */
  newCore->residual = setSignal;
  newCore->residualEnergy = newCore->initialEnergy = setSignal->energy;
  newCore->decay.clear();
  newCore->decay.append( newCore->initialEnergy );
  
  return( newCore );
}

/* - signal+approximant+dict */
MP_Mpd_Core_c* MP_Mpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict ) 
{
	const char		*func = "MP_Mpd_Core_c::init(3 args)";
	MP_Mpd_Core_c	*newCore;
	
	newCore = MP_Mpd_Core_c::create( setSignal, setBook );
	if ( newCore == NULL ) 
	{
		mp_error_msg( func, "Failed to create a new mpd core.\n" );
		return NULL;
	}
    if ( setDict == NULL) 
	{
		mp_error_msg( func, "Could not use a NULL dictionary.\n" );
		return NULL;
	}

	if(!newCore->change_dict(setDict))
		return NULL;	
	
	return( newCore );
}

MP_Mpd_Core_c* MP_Mpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Signal_c* setApproximant )
{
  const char* func = "MP_Mpd_Core_c::init(3 args)";
  MP_Mpd_Core_c* newCore;
  newCore = MP_Mpd_Core_c::create( setSignal, setBook );
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );}
    
  if ( setApproximant ) newCore->plug_approximant(setApproximant);
  else {
    mp_error_msg( func, "Could not use a NULL approximant.\n" );
    return( NULL );}
    
  return( newCore );
}



/********************/
/* NULL constructor */
MP_Mpd_Core_c::MP_Mpd_Core_c() {
	
  /* File names */
  bookFileName  =  NULL;
  approxFileName   = NULL;
  
  /* Manipulated objects */
  dict = NULL;
  book = NULL;
  approximant = NULL;
}

/**************/
/* Destructor */
MP_Mpd_Core_c::~MP_Mpd_Core_c() {
  if (bookFileName) free(bookFileName);
  if (approxFileName) free(approxFileName);
}


/***************************/
/* SET OBJECTS             */
/***************************/

/************/
/* Set dict */
MP_Bool_t MP_Mpd_Core_c::change_dict( MP_Dict_c *newDict ) 
{
	const char	*func = "MP_Mpd_Core_c::change_dict( MP_Dict_c * )";
	MP_Dict_c	*oldDict = dict;
	
	if ( newDict->signal == NULL ) 
	{
		// If there was a non-NULL dictionary before, detach the residual to avoid its destruction:
		if ( oldDict ) 
			residual = oldDict->detach_signal();
			
		// Set the new dictionary:
		dict = newDict;
   
		// Plug dictionary to signal:
		if(!plug_dict_to_signal())
			return false;
  
		return true;
	}
	else
	{ 
		mp_error_msg( func, "Could not set a dictionary with a pluged signal.\n" );
		return false;
	}
}

MP_Bool_t MP_Mpd_Core_c::plug_dict_to_signal(void)
{
	const char* func = "MP_Mpd_Core_c::plug_dict_to_signal()";
	
	// If the new dictionary is not NULL, replug the residual:
	if (!dict) 
	{
		mp_error_msg( func, "Could not plug a null dictionary .\n" );
		return false;
	}
	if(!residual)
	{
		mp_error_msg( func, "Could not plug a dictionary with a null signal.\n" );
		return false;		
	}
	if(dict->plug_signal( residual ) == 1)
	{
		mp_error_msg( func, "Could not plug this dictionary into the signal.\n" );
		return false;		
	}
	
	/* Note:
     - if a NULL dictionary is given, the residual is kept alive in the residual variable;
     - at the first use of set_dict(dict), the oldDict is NULL and the residual is copy-constructed from the signal at the mpdCore->init(signal,book) pahse. */
	return true;
}


void MP_Mpd_Core_c::init_dict(){
  dict = MP_Dict_c::init();
}
int MP_Mpd_Core_c::add_default_block_to_dict( const char* blockName ){
  if (NULL!= dict) return dict->add_default_block(blockName);
  else return 0;
}
/********************/
/* Plug approximant */

void MP_Mpd_Core_c::plug_approximant( MP_Signal_c *setApproximant  ) {
  
  const char* func = "Toggle_approximant";
  
  if ( book ){
    approximant = setApproximant;
    if ( approximant == NULL ) {
      mp_error_msg( func, "Failed to create an approximant in the mpd core."
		    " Returning NULL.\n" );
    } else 
      // Rebuild from the book 
      book->substract_add( NULL, approximant, NULL );
  }
}
  
/***************************/
/* OTHER METHODS           */
/***************************/

/********************************/
/* Save the book/residual/decay */
void MP_Mpd_Core_c::save_result() 
{
	const char			*func = "Save info";
    unsigned long int	nWrite;

	// 1) Save the book
	if(bookFileName)
    {
		if ( (strcmp( bookFileName, "-" ) != 0) )
		{
			if ( (strstr( bookFileName, ".bin" ) != NULL) )
				book->print( bookFileName, MP_BINARY);
			else if ( (strstr( bookFileName, ".xml" ) != NULL) )
				book->print( bookFileName, MP_TEXT);
			else
				mp_error_msg( func, "The book [%s] has an incorrect extension (xml or bin).\n", bookFileName );

			if ( verbose ) 
			{ 
				if (numIter >0 ) 
					mp_info_msg( func, "At iteration [%lu] : saved the book to file [%s].\n", numIter, bookFileName );  
				else 
					mp_info_msg( func, "Saved the book to file [%s]...\n", bookFileName ); 
			}  
		}
		else
		{
				book->print( stdout, MP_TEXT );
				fflush( stdout );
				if ( verbose ) 
					mp_info_msg( func, "Sent the book to stdout in text mode.\n" );
		}
	}
  
	// 2) Save the approximant
	if ( approxFileName && approximant ) 
	{
		if (approximant->wavwrite( approxFileName ) == 0 ) 
			mp_error_msg( func, "Can't write approximant signal to file [%s].\n", approxFileName );
		else
		{
			if ( verbose )
			{ 
				if (numIter >0 ) 
					mp_info_msg( func, "At iteration [%lu] : saved the approximant to file [%s].\n", numIter , approxFileName );
				else 
				{
					mp_info_msg( func, "Saved the approximant signal to file [%s]...\n", approxFileName );
					mp_info_msg( func, "The resulting signal has [%lu] samples in [%d] channels, with sample rate [%d]Hz.\n", book->numSamples, book->numChans, book->sampleRate );
				}
			}
		}
	}

	// 3) Save the residual
	if ( resFileName ) 
	{
		if ( residual->wavwrite( resFileName ) == 0 ) 
			mp_error_msg( func, "Can't write residual signal to file [%s].\n", resFileName );
		else
		{
			if ( verbose ) 
			{
				if (numIter >0 ) 
					mp_info_msg( func, "At iteration [%lu] : saved the residual signal to file [%s].\n", numIter , resFileName );
  				else 
					mp_info_msg( func, "Saved the residual signal to file [%s]...\n", resFileName );
			}
		}
	}

	// 4) Save the decay
	if ( decayFileName ) 
	{
		nWrite = save_decay(decayFileName);
		if( nWrite != (numIter+1) ) 
		{
			mp_warning_msg( func, "Wrote less than the expected number of doubles to the energy decay file.\n" );
			mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter+1, nWrite );
		}
		if ( verbose ) 
			mp_info_msg( func, "At iteration [%lu] : saved the energy decay to file [%s].\n", numIter , decayFileName );	  
	}
}

/*************************/
/* Make one MP iteration */
unsigned short int MP_Mpd_Core_c::step() {

  const char* func = "MP_Mpd_Core_c::step()";

  /* Reset the state info */
  state = 0;

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
		10*log10(currentSnr), 10*log10(stopAfterSnr) );

  /* 1) Iterate: */
  dict->iterate_mp( book , approximant ); /* Note: if approximant is NULL, no computation will be performed on it. */
  residualEnergyBefore = residualEnergy;
  residualEnergy = (double)dict->signal->energy;
  
  if ( useDecay ) decay.append( residualEnergy );    
  numIter++;
  
  /* 2) Check for possible breakpoints: */
  if ( numIter == nextSnrHit ) {
    currentSnr = initialEnergy / residualEnergy;
    nextSnrHit += snrHit;
  }

  if ( numIter == nextReportHit ) {
    mp_progress_msg( "At iteration", "[%lu] : the residual energy is [%g] and the SNR is [%g].\n",
		     numIter, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
    nextReportHit += reportHit;
  }
    
  if ( numIter == nextSaveHit ) { save_result(); nextSaveHit += saveHit; }

  if ( numIter == ULONG_MAX ) state = ( state | MP_ITER_EXHAUSTED );
  
  /* 3) Check for possible stopping conditions: */
  if ( useStopAfterIter && (numIter >= stopAfterIter) )   state = ( state | MP_ITER_CONDITION_REACHED );
  if ( useStopAfterSnr  && (currentSnr >= stopAfterSnr) ) state = ( state | MP_SNR_CONDITION_REACHED );
  if ( residualEnergy < 0.0 ) state = ( state | MP_NEG_ENERGY_REACHED );
  if ( residualEnergy > residualEnergyBefore ) {
    mp_warning_msg( func, "Iteration [%lu] increases the energy of the residual ! Before: [%g] Now: [%g]\n",
		    numIter, residualEnergyBefore, residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    book->atom[book->numAtoms-1]->info( stderr );
    //state = ( state | MPD_INCREASING_ENERGY );
  }
  /*if ( (residualEnergyBefore - residualEnergy) < 5e-4 ) {
    mp_warning_msg( func, "At iteration [%lu]: the energy decreases very slowly !"
    " Before: [%g] Now: [%g] Diff: [%g]\n",
    numIter, residualEnergyBefore, residualEnergy, residualEnergyBefore - residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    book->atom[book->numAtoms-1]->info( stderr );*/
  /* BORK BORK BORK */
  /* Export the considered signal portion */
  /* RES */
  /* MP_Signal_c *exportSig = MP_Signal_c::init( dict->signal, book->atom[book->numAtoms-1]->support[0] );
     if ( exportSig != NULL ) exportSig->dump_to_double_file( "res.dbl" );
     fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [res.dbl].\n",
     exportSig->numChans,
     book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );*/
  /* ATOM */
  /*MP_Signal_c *atomSig = MP_Signal_c::init( book->atom[book->numAtoms-1], dict->signal->sampleRate );
    if ( atomSig != NULL ) atomSig->dump_to_double_file( "atom.dbl" );
    fprintf( stderr, "Exported [%hu] channels from atom of length [%lu] to file [atom.dbl].\n",
    atomSig->numChans, atomSig->numSamples );*/
  /* SUM */
  /*unsigned long int i;
    for ( i = 0; i < (exportSig->numSamples*exportSig->numChans); i++ ) {
    exportSig->storage[i] = exportSig->storage[i] + atomSig->storage[i];
    }
    exportSig->dump_to_double_file( "bad_signal.dbl" );
    exportSig->wavwrite( "bad_signal.wav" );
    fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [bad_signal.dbl].\n",
    exportSig->numChans,
    book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );
    fflush( stderr );
    exit( 0 );*/
  /* \BORK BORK BORK */
  /*}*/

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "EXITING iteration [%lu]/[%lu].\n", numIter, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
		10*log10(currentSnr), 10*log10(stopAfterSnr) );

  return( state );
}

/**********************************/
/* Check if some objects are null */
MP_Bool_t MP_Mpd_Core_c::can_step( void ) {
  const char* func = "can_step";
  /* Check that all of dict, book and signal are not NULL */
  if (dict  &&  book) {
    if (dict->signal) return true;
    else { mp_error_msg( func,"book or dict are not set .\n"); 
      return false;}
  }
  else { mp_error_msg( func,"dict has no signal plugged .\n"); 
    return false;}
}


/* Infos */
void MP_Mpd_Core_c::info_conditions( void )
{

  const char* func = "Conditions";

  if ( useStopAfterIter ) mp_info_msg( func, "This run will perform [%lu] iterations, using [%lu] atoms.\n",
				       stopAfterIter, dict->num_atoms() );
  if ( useStopAfterSnr ) mp_info_msg( func, "This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
				      10*log10( stopAfterSnr ), dict->num_atoms() );
  if ( bookFileName ) {  
    if ( strcmp( bookFileName, "-" ) == 0 ) mp_info_msg( func, "The resulting book will be written"
							 " to the standard output [%s].\n", bookFileName );
    else mp_info_msg( func, "The resulting book will be written to book file [%s].\n", bookFileName );
  }
  if ( resFileName ) mp_info_msg( func, "The residual will be written to file [%s].\n", resFileName );
  else mp_info_msg( func, "The residual will not be saved.\n" );
  if ( useDecay ) mp_info_msg( func, "The energy decay will be stored at each iteration.\n" );
  if ( decayFileName ) mp_info_msg( func, "The energy decay will be written to file [%s].\n", decayFileName );
  else mp_info_msg( func, "The energy decay will not be saved.\n" );

}

void MP_Mpd_Core_c::info_result( void )
{
  const char* func = "Result";
  mp_info_msg( func, "[%lu] iterations have been performed.\n", numIter );
  mp_info_msg( func, "([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
               numIter, dict->num_atoms() );
  mp_info_msg( func, "The initial signal energy was [%g].\n", initialEnergy );
  mp_info_msg( func, "The residual energy is now [%g].\n", residualEnergy );
  mp_info_msg( func, "The SNR is now [%g].\n", 10*log10( initialEnergy / residualEnergy ) );
}

void MP_Mpd_Core_c::set_save_hit( const unsigned long int setSaveHit, const char* setBookFileName, const char* setResFileName, const char* setDecayFileName )
{
	const char* func = "set_save_hit";
	char* newBookFileName = NULL;
	char* newResFileName = NULL;
	char* newDecayFileName =NULL;
  
	if (setSaveHit>0)
		saveHit = setSaveHit;
	if (setSaveHit>0)
		nextSaveHit = numIter + setSaveHit;
  
 	// Reallocate memory and copy name
	if (setBookFileName && strlen(setBookFileName) >= 1 ) 
	{
		newBookFileName = (char*) realloc((void *)bookFileName  , ((strlen(setBookFileName)+1 ) * sizeof(char)));
		if ( newBookFileName == NULL )
		{
			mp_error_msg( func,"Failed to re-allocate book file name to store book [%s] .\n", setBookFileName );                 
		}
		else 
			bookFileName = newBookFileName;
		strcpy(bookFileName, setBookFileName);
  }

if ( setResFileName && strlen(setResFileName) > 1 ){ 
    newResFileName = (char*) realloc((void *)resFileName  , ((strlen(setResFileName)+1 ) * sizeof(char)));
    if ( newResFileName == NULL )
      {
	mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
		      setResFileName);                 
      }
    else resFileName = newResFileName;
    strcpy(resFileName, setResFileName); }
  
  if ( setDecayFileName && strlen(setDecayFileName)> 1 ){
    newDecayFileName = (char*) realloc((void *)decayFileName  , ((strlen(setDecayFileName)+1 ) * sizeof(char)));
    if ( newDecayFileName == NULL )
      {
	mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
		      setDecayFileName);                 
      }
    else decayFileName = newDecayFileName;
    strcpy(decayFileName, setDecayFileName); 
    useDecay = MP_TRUE;
  }
 
}

void MP_Mpd_Core_c::addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap){
  dict->create_block(dict->signal , setPropertyMap);
}

bool MP_Mpd_Core_c::save_dict( const char* dictName ){
  if (dict) { dict->print(dictName);
    return true;
  } else return false;
}

void MP_Mpd_Core_c::get_filter_lengths(vector<unsigned long int> * filterLengthsVector){
  if (dict && filterLengthsVector->size() == dict->numBlocks) { 
	
    for ( unsigned int i= 0; i < dict->numBlocks; i++) {
      filterLengthsVector->at(i)= dict->block[i]->filterLen;
    }     
  } 
}
