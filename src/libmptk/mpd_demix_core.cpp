/******************************************************************************/
/*                                                                            */
/*                              mpd_demix_core.cpp                            */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* Rémi Gribonval                                                             */
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

#include <mptk.h>


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************/
/* Factory function:*/
/********************/
/* - signal+mixer+book only: */
MP_Mpd_demix_Core_c* MP_Mpd_demix_Core_c::create( MP_Signal_c *signal, MP_Mixer_c* setMixer, std::vector<MP_Book_c*> *setBookArray )
{

  const char* func = "MP_Mpd_demix_Core_c::init(3 args)";

  MP_Mpd_demix_Core_c* newCore;

  if ( signal == NULL )
    {
      mp_error_msg( func, "Can't initialize a MP_Mpd_demix_Core_c from a NULL signal.\n" );
      return( NULL );
    }

  if ( setMixer == NULL )
    {
      mp_error_msg( func, "Can't initialize a MP_Mpd_demix_Core_c from a NULL mixer.\n" );
      return( NULL );
    }

  if ( setBookArray == NULL )
    {
      mp_error_msg( func, "Can't initialize a MP_Mpd_demix_Core_c from a NULL book array.\n" );
      return( NULL );
    }

  if (setBookArray->size() != setMixer->numSources )
    {
      mp_error_msg( func, "Can't initialize a MP_Mpd_demix_Core_c from a book array of [%u] books and [%u] number sources in the mixer.\n", setBookArray->size() , setMixer->numSources );
      return( NULL );
    }

  /* Instantiate and check */
  newCore = new MP_Mpd_demix_Core_c();

  if ( newCore == NULL )
    {
      mp_error_msg( func, "Failed to create a new mpd demix core.\n" );
      return( NULL );
    }

  newCore->mixer = setMixer;



  /* Plug the signal */
  newCore->residual = signal;
  newCore->residualEnergy = newCore->initialEnergy = signal->energy;
  newCore->decay.clear();
  newCore->decay.append( newCore->initialEnergy );
  /* Create the source signal array*/
  newCore->sigArray = new  std::vector<MP_Signal_c*>(newCore->mixer->numSources);
  for (unsigned short int j = 0; j < newCore->mixer->numSources; j++ )
    {
      newCore->sigArray->at(j) = MP_Signal_c::init(1, signal->numSamples,signal->sampleRate);
      if ( newCore->sigArray->at(j) == NULL  )
        {
          mp_error_msg( func,"Could not initialize the [%u]-th signal in the signal array.\n",
                        j );


          return( NULL );
        }
    }
  /* Demix the signal in the source signal array*/
  newCore->mixer->applyAdjoint(newCore->sigArray,signal);

  newCore->bookArray = setBookArray;

  if ( (newCore->amp = (MP_Real_t*) calloc( signal->numChans, sizeof(MP_Real_t) )) == NULL )
    {
      mp_error_msg( func, "Failed to allocate an array of [%hu] real values"
                    " to store the atom's amplitudes.\n", signal->numChans );
      return( NULL );
    }


  return( newCore );
}

/********************/
/* NULL constructor */
MP_Mpd_demix_Core_c::MP_Mpd_demix_Core_c()
{
  /* Manipulated objects */
  dictArray = NULL;
  bookArray = NULL;
  mixer = NULL;
  maxAtom  = NULL;
  maxAmp = 0;
  amp = NULL;
  maxSrc = 0;
  srcSeqFileName = NULL;
  bookFileName = NULL;
  approxFileNames = NULL;
}

/**************/
/* Destructor */
MP_Mpd_demix_Core_c::~MP_Mpd_demix_Core_c()
{
  if ( sigArray )
    {
      for ( unsigned int j = 0; j < mixer->numSources; j++ ) if ( sigArray->at(j) ) delete sigArray->at(j);
      delete sigArray;
    }

}


/***************************/
/* SET OBJECTS             */
/***************************/

/************/
/* Set dict */
std::vector<MP_Dict_c*>* MP_Mpd_demix_Core_c::change_dict( std::vector<MP_Dict_c*> *setDictArray )
{

  const char* func = "MP_Mpd_demix_Core_c::change_dict( std::vector<MP_Dict_c*> * )";
  std::vector<MP_Dict_c*> *oldDict = dictArray;
  /* If there was a non-NULL dictionary before, detach the residual
     to avoid its destruction: */
  if ( setDictArray->size() == mixer->numSources )
    {


      for (unsigned int i=0; i< setDictArray->size() ; i++)
        {

          if ( setDictArray->at(i)->signal != NULL )
            {
              mp_error_msg( func, "Dict number [%d] in Dict array has a pluged signal.\n", i );
              return( NULL );
            }

        }

      if ( oldDict && ( oldDict->size()>0 ) )
        for (unsigned int i=0; i< setDictArray->size() ; i++)
          { 
            sigArray->at(i) = oldDict->at(i)->detach_signal();
          }

      /* Set the new dictionary: */
      dictArray = setDictArray ;
      /* If the new dictionary is not NULL, replug the residual: */
      for (unsigned int i=0; i< setDictArray->size() ; i++)
        { char line[1024]; 
          sprintf( line, "%s_%02u.xml", "Z:\\workspace\\build-MPTK-plugin\\bin\\dict", i );
            dictArray->at(i)->print( line );
          dictArray->at(i)->plug_signal( sigArray->at(i) );
        }
      /* Note:
         - if a NULL dictionary is given, the residual is kept alive
         in the residual variable;
         - at the first use of set_dict(dict), the oldDict is NULL and
         the residual is copy-constructed from the signal at
         the mpdCore->init(signal,book) pahse. */

      return( oldDict );
    }
  else
    {
      mp_error_msg( func, "Dict number [%d] is not the same than source number [%d].\n", setDictArray->size(), mixer->numSources );
      return( NULL );
    }
}


bool MP_Mpd_demix_Core_c::plug_approximant( std::vector<MP_Signal_c*> *setApproxArray )
{

  const char* func = "plug_approximant( std::vector<MP_Signal_c*> *approxArray )";

  if (setApproxArray && setApproxArray->size() == mixer->numSources){ approxArray = setApproxArray;

  for ( unsigned int j = 0; j < mixer->numSources; j++ )
    {
      bookArray->at(j)->substract_add( NULL, approxArray->at(j), NULL );
    }
    return true;
    }
    else return false;
}

/***************************/
/* OTHER METHODS           */
/***************************/

/********************************/
/* Save the book/residual/decay */
void MP_Mpd_demix_Core_c::save_result()
{

  const char* func = "Save info";

  /* - Save the book: */
  if ( bookFileName && (strcmp( bookFileName, "-" ) != 0) )
    {
      for (unsigned int j = 0; j < mixer->numSources; j++ )
        {
          sprintf( line, "%s_%02u.bin", bookFileName, j );
          bookArray->at(j)->print( line, MP_BINARY);
          if ( verbose )
            {
              if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : Saved book number [%02u] in file [%s], in binary mode.\n"
                                              , numIter, j, line );
              else mp_info_msg( func, "Saved book number [%02u] in file [%s], in binary mode.\n",
                                  j, line );
            }
        }
    }
  if ( verbose )
    {
      if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the book to file [%s].\n", numIter, bookFileName );
      else mp_info_msg( func, "Saved the book to file [%s]...\n", bookFileName );
    }
  else if ( bookFileNames && bookFileNames->size() == mixer->numSources)
    {
      for (unsigned int j = 0; j < mixer->numSources; j++ )
        {
          bookArray->at(j)->print( bookFileNames->at(j), MP_BINARY);
          if ( verbose )
            {
              if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : Saved book number [%02u] in file [%s], in binary mode.\n"
                                              , numIter, j, line );
              else mp_info_msg( func, "Saved book number [%02u] in file [%s], in binary mode.\n",
                                  j, line );
            }
        }
    }

  /* - Save the approximant: */

  if ( (approxFileNames && approxFileNames->size()==mixer->numSources) && (approxArray && approxArray->size()==mixer->numSources))
    {
      for ( unsigned int j = 1; j < mixer->numSources; j++ )
        {
          if (approxArray->at(j)->wavwrite( approxFileNames->at(j) ) == 0 )
            {
              mp_error_msg( func, "Can't write approximant signal to file [%s]. for source [%u]\n", approxFileNames->at(j), j );
            }
          else

            if ( verbose )
              {
                if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the approximant for source [%u] to file [%s].\n", numIter , j , approxFileNames->at(j) );

                else
                  {
                    mp_info_msg( func, "Saved the approximant signal for source [%u] to file [%s]...\n", j , approxFileNames->at(j) );
                    mp_info_msg( func, "The resulting signal has [%lu] samples in [%d] channels, with sample rate [%d]Hz.\n",
                                 approxArray->at(j)->numSamples, approxArray->at(j)->numChans, approxArray->at(j)->sampleRate );

                  }
              }
        }

    }

  /* - Save the residual: */
  if ( resFileName )
    {
      if ( residual->wavwrite( resFileName ) == 0 )
        {
          mp_error_msg( func, "Can't write residual signal to file [%s].\n", resFileName );
        }
      else
        if ( verbose )
          {
            if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the residual signal to file [%s].\n", numIter , resFileName );
            else mp_info_msg( func, "Saved the residual signal to file [%s]...\n", resFileName );

          }

    }
  /* - the decay: */
  if ( decayFileName )
    {
      unsigned long int nWrite;
      nWrite = decay.save( decayFileName );
      if ( nWrite != (numIter+1) )
        {
          mp_warning_msg( func, "Wrote less than the expected number of doubles to the energy decay file.\n" );
          mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter+1, nWrite );
        }
      if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the energy decay to file [%s].\n", numIter , decayFileName );
    }
  /* - the src sequance file: */
  if ( srcSeqFileName )
    {
      unsigned long int nWritesrcseq;
      nWritesrcseq = srcSequences.save_ui_to_text( srcSeqFileName );
      if ( nWritesrcseq  != (numIter) )
        {
          mp_warning_msg( func, "Wrote less than the expected number of unsigned int to src sequence file.\n" );
          mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter, nWritesrcseq  );
        }
      if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the source sequence to file [%s].\n", numIter , srcSeqFileName );
    }


}
unsigned long int MP_Mpd_demix_Core_c::save_source_sequence(const char* fileName){
	const char* func = "MP_Mpd_demix_Core_c::save_source_sequence()";
	      unsigned long int nWritesrcseq;
      nWritesrcseq = srcSequences.save_ui_to_text( fileName );
      if ( nWritesrcseq  != (numIter) )
        {
          mp_warning_msg( func, "Wrote less than the expected number of unsigned int to src sequence file.\n" );
          mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter, nWritesrcseq  );
        }
return nWritesrcseq;
}

/*************************/
/* Make one MP iteration */
unsigned short int MP_Mpd_demix_Core_c::step()
{

  const char* func = "MP_Mpd_Core_c::step()";
  unsigned int j;
  int k;
  /* Reset the state info */
  state = 0;

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
                10*log10(currentSnr), 10*log10(stopAfterSnr) );

  /* 1) Iterate: */

  /*-------------------------------------------*/
  /* -- Seek the best atom across the sources: */
  /*-------------------------------------------*/
  /* Init with source 0 */
  dictArray->at(0)->update();
  maxBlock =  dictArray->at(0)->blockWithMaxIP;
  max =  dictArray->at(0)->block[maxBlock]->maxIPValue;
  maxSrc = 0;

  /* Follow through the remaining sources */
  for ( unsigned int j = 1; j < mixer->numSources; j++ )
    {
      dictArray->at(j)->update();
      blockIdx = dictArray->at(j)->blockWithMaxIP;
      val = dictArray->at(j)->block[blockIdx]->maxIPValue;
      if ( val > max )
        {
          max = val;
          maxSrc = j;
          maxBlock = blockIdx;
        }
    }

  srcSequences.append(maxSrc);

  /*----------------------------*/
  /* -- Create the best atom:   */
  /*----------------------------*/
  dictArray->at(maxSrc)->create_max_atom( (MP_Atom_c**)(&maxAtom) );
#ifndef NDEBUG
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "mpd_demix DEBUG -- MAX ATOM HAS:\n" );
  maxAtom->info( stderr );
#endif
  /* Backup the atom's amplitude */
  maxAmp = maxAtom->amp[0];

  /*----------------------------*/
  /* -- Update the signals:     */
  /*----------------------------*/

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "mpd_demix DEBUG -- Updating the signals..." );
  /* - update the multiple signal */
  for ( j = 0; j < mixer->numSources; j++ )
    {
      maxAtom->amp[0] = maxAmp * (*(mixer->Ah + j*(mixer->numSources) + maxSrc));
      if (approxArray && approxArray->size() == mixer->numSources) maxAtom->substract_add( dictArray->at(j)->signal, approxArray->at(maxSrc) );
      else maxAtom->substract_add( dictArray->at(j)->signal, NULL );
    }
  /* Restore the initial atom's amplitude */

  maxAtom->amp[0] = maxAmp;

  /* - update the input signal
     (note that maxAmp will be used in build_waveform, when calling
     substract_add_var_amp. that's why amp[k] is not multiplied by
     maxAmp) */
  for ( k = 0; k < residual->numChans; k++)
    {
      amp[k] = (*(mixer->mixer + k*(mixer->numSources) + maxSrc));
    }

  maxAtom->substract_add_var_amp( amp, residual->numChans, residual, NULL );

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, " Done.\n" );

  /* Store the max atom (with its original amplitude) */
  bookArray->at(maxSrc)->append( maxAtom );

  /*-----------------------------------------------------------------*/
  /* -- Keep track of the support where the signal has been modified */
  /*-----------------------------------------------------------------*/
  for ( j = 0; j < mixer->numSources; j++ )
    {
      dictArray->at(j)->touch[0].pos = maxAtom->support[0].pos;
      dictArray->at(j)->touch[0].len = maxAtom->support[0].len;
    }
  residual->refresh_energy();
  residualEnergyBefore = residualEnergy;
  residualEnergy = (double)residual->energy;
  if ( decayFileName ) decay.append( residualEnergy );

  numIter++;

  /* 2) Check for possible breakpoints: */
  if ( numIter == nextSnrHit )
    {
      currentSnr = initialEnergy / residualEnergy;
      nextSnrHit += snrHit;
    }

  if ( numIter == nextReportHit )
    {
      mp_progress_msg( "At iteration", "[%lu] : the residual energy is [%g] and the SNR is [%g].\n",
                       numIter, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
      nextReportHit += reportHit;
    }

  if ( numIter == nextSaveHit )
    {
      save_result();
      nextSaveHit += saveHit;
    }

  if ( numIter == ULONG_MAX ) state = ( state | MP_ITER_EXHAUSTED );

  /* 3) Check for possible stopping conditions: */
  if ( useStopAfterIter && (numIter >= stopAfterIter) )   state = ( state | MP_ITER_CONDITION_REACHED );
  if ( useStopAfterSnr  && (currentSnr >= stopAfterSnr) ) state = ( state | MP_SNR_CONDITION_REACHED );
  if ( residualEnergy < 0.0 ) state = ( state | MP_NEG_ENERGY_REACHED );
  if ( residualEnergy >= residualEnergyBefore )
    {
      mp_warning_msg( func, "Iteration [%lu] increases the energy of the residual ! Before: [%g] Now: [%g]\n",
                      numIter, residualEnergyBefore, residualEnergy );
      mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    }

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
MP_Bool_t MP_Mpd_demix_Core_c::can_step( void )
{


  for (unsigned int j = 0; j < mixer->numSources; j++ )
    {
      if ( dictArray->at(j)->signal == NULL ) return MP_FALSE;
    }

  /* Check that all of dict, book and signal are not NULL */
  return( ( dictArray && dictArray->size() == mixer->numSources ) && ( bookArray && bookArray->size() == mixer->numSources ) && residual );
}

unsigned long int MP_Mpd_demix_Core_c::book_append(MP_Book_c *newBook)
{
  return ((*bookArray)[0]->append(newBook));
}

void MP_Mpd_demix_Core_c::info_result( void )
{
  const char* func = "Result";

  mp_info_msg( func, "[%lu] iterations have been performed.\n", numIter );
  mp_info_msg( func, "([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
               numIter, dictArray->at(0)->num_atoms() );
  mp_info_msg( func, "The initial signal energy was [%g].\n", initialEnergy );
  mp_info_msg( func, "The residual energy is now [%g].\n", residualEnergy );
  mp_info_msg( func, "The SNR is now [%g].\n", 10*log10( initialEnergy / residualEnergy ) );

}

void MP_Mpd_demix_Core_c::info_conditions( void )
{
  const char* func = "Conditions";


  if ( useStopAfterIter ) mp_info_msg( func, "This run will perform [%lu] iterations, using [%lu] atoms.\n",
                                         stopAfterIter, dictArray->at(0)->num_atoms() );
  if ( useStopAfterSnr ) mp_info_msg( func, "This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
                                        10*log10( stopAfterSnr ), dictArray->at(0)->num_atoms() );
  if ( bookFileName )
    {
      if ( strcmp( bookFileName, "-" ) == 0 ) mp_info_msg( func, "The resulting books will be written"
            " to the standard output .\n");
      else
        {
          for (unsigned int j = 0; j < mixer->numSources; j++ )

            {
              sprintf( line, "%s_%02u.bin", bookFileName, j );
              mp_info_msg( func, "The resulting book for source [%u] will be written to book file [%s].\n", j, line);

            }
        }
    }

  if ( resFileName ) mp_info_msg( func, "The residual will be written to file [%s].\n", resFileName );
  else mp_info_msg( func, "The residual will not be saved.\n" );
  if ( decayFileName ) mp_info_msg( func, "The energy decay will be written to file [%s].\n", decayFileName );
  else mp_info_msg( func, "The energy decay will not be saved.\n" );
  if ( srcSeqFileName ) mp_info_msg( func, "The source sequence will be written to file [%s].\n", srcSeqFileName);
  else mp_info_msg( func, "The source sequence will not be saved.\n" );
  mp_info_msg( func, "-------------------------\n" );
  mp_info_msg( func, "The original signal has [%d] channels, with [%u] sources.\n",
               residual->numChans, mixer->numSources );
  mp_info_msg( func, "mpd_demix msg -- The original signal has [%lu] samples in each channel.\n",
               residual->numSamples );


}

void MP_Mpd_demix_Core_c::set_save_hit( const unsigned long int setSaveHit,
                                        const char* setBookFileName,
                                        const char* setResFileName,
                                        const char* setDecayFileName,
                                        const char* setSrcSeqFileName )
{
  const char* func = "set_save_hit";
  char* newBookFileName;
  char* newResFileName;
  char* newDecayFileName;
  char* newSrcSeqFileName;
  if (setSaveHit>0) saveHit = setSaveHit;
  if (setSaveHit>0) nextSaveHit = numIter + setSaveHit;

//reallocate memory and copy name
  if (setBookFileName && strlen(setBookFileName) >1 )
    {
      newBookFileName = (char*) realloc((void *)bookFileName  , ((strlen(setBookFileName)+1 ) * sizeof(char)));
      if ( newBookFileName == NULL )
        {
          mp_error_msg( func,"Failed to re-allocate book file name to store book [%s] .\n",
                        setBookFileName );
        }
      else bookFileName = newBookFileName;
      strcpy(bookFileName, setBookFileName);
    }

  if ( setResFileName && strlen(setResFileName) > 1 )
    {
      newResFileName = (char*) realloc((void *)resFileName  , ((strlen(setResFileName)+1 ) * sizeof(char)));
      if ( newResFileName == NULL )
        {
          mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
                        setResFileName);
        }
      else resFileName = newResFileName;
      strcpy(resFileName, setResFileName);
    }
  if ( setDecayFileName && strlen(setDecayFileName)> 1 )
    {
      newDecayFileName = (char*) realloc((void *)decayFileName  , ((strlen(setDecayFileName)+1 ) * sizeof(char)));
      if ( newDecayFileName == NULL )
        {
          mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
                        setDecayFileName);
        }
      else decayFileName = newDecayFileName;
      strcpy(decayFileName, setDecayFileName);
    }
  if ( setSrcSeqFileName && strlen(setSrcSeqFileName)> 1 )
    {
      newSrcSeqFileName = (char*) realloc((void *)srcSeqFileName  , ((strlen(setSrcSeqFileName)+1 ) * sizeof(char)));
      if ( newSrcSeqFileName == NULL )
        {
          mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
                        setSrcSeqFileName);
        }
      else srcSeqFileName = newSrcSeqFileName;
      strcpy(srcSeqFileName, setSrcSeqFileName);
    }


}
