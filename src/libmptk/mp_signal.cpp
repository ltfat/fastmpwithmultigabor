/******************************************************************************/
/*                                                                            */
/*                             mp_signal.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-07-24 17:27:43 +0200 (mar., 24 juil. 2007) $
 * $Revision: 1120 $
 *
 */

/*********************************************/
/*                                           */
/* signal.cpp: methods for class MP_Signal_c */
/*                                           */
/*********************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mtrand.h"
#include <sndfile.h>


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/*******************************/
/* Factory function with sizes */
MP_Signal_c* MP_Signal_c::init( const int setNumChans,
                                const unsigned long int setNumSamples ,
                                const int setSampleRate )
{

  const char* func = "MP_Signal_c::init(3 params)";
  MP_Signal_c *newSig = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Initializing a new signal...\n");

  /* Instantiate and check */
  newSig = new MP_Signal_c();
  if ( newSig == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a new signal.\n" );
      return( NULL );
    }
  /* Do the internal allocations */
  if ( newSig->init_parameters( setNumChans, setNumSamples, setSampleRate ) )
    {
      mp_error_msg( func, "Failed to perform the internal allocations for the new signal.\n" );
      delete( newSig );
      return( NULL );
    }

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Done.\n");

  return( newSig );
}


/***********************************/
/* Factory function from file name */
MP_Signal_c* MP_Signal_c::init( const char *fName )
{

  const char* func = "MP_Signal_c::init(fileName)";
  SNDFILE *file;
  SF_INFO sfinfo;
  MP_Signal_c *newSig = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                "Initializing a new signal from file [%s]...\n", fName );

  /* open the file */
  if ( fName == NULL )
    {
      mp_error_msg( func, "Invalid file name [%s] was passed"
                    " to a signal constructor.\n", fName );
      return( NULL );
    }
  else
    {

      mp_debug_msg( MP_DEBUG_FILE_IO, func,
                    "Doing sf_open on file [%s]...\n", fName );

      sfinfo.format  = 0; /* -> See the libsndfile manual. */
      file = sf_open( fName, SFM_READ, &sfinfo );

      mp_debug_msg( MP_DEBUG_FILE_IO, func, "Done.\n" );

    }
  /* Check */
  if ( file == NULL )
    {
      mp_error_msg( func, "sf_open could not open the sound file [%s] for reading."
                    " Returning NULL.\n", fName );
      return( NULL );
    }

  mp_debug_msg( MP_DEBUG_FILE_IO, func, "sfinfo contains:\n");
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- seekable : %d\n", sfinfo.seekable) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- end sfinfo.\n");

  /* Instantiate the signal */
  newSig = MP_Signal_c::init( sfinfo.channels, sfinfo.frames, sfinfo.samplerate );
  if ( newSig == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a new signal with parameters:"
                    " numChans = %d, numFrames = %lu, sampleRate = %d.\n",
                    sfinfo.channels, sfinfo.frames, sfinfo.samplerate );
      return( NULL );
    }
  /* actually read the file if allocation is OK */
  else
    {

      mp_debug_msg( MP_DEBUG_FILE_IO, func, "After init, signal values are:\n");
      mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- sampleRate : %d\n",  newSig->sampleRate) ;
      mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- numChans   : %d\n",  newSig->numChans) ;
      mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- numSamples : %lu\n", newSig->numSamples) ;
      mp_debug_msg( MP_DEBUG_FILE_IO, func, "-- end after init.\n");

      MP_Chan_t numChans = newSig->numChans;
         /** will initialize initial numCols and numRows with the first value with wich this function is called */
      static unsigned long int allocated_numChans = 0;
      //double frame[numChans];
      double* frame=0;
    if (!frame || allocated_numChans != numChans) {
	  if (frame) free(frame) ;
	  	  allocated_numChans = numChans ; 
		  frame= (double*) malloc (allocated_numChans*sizeof(double)) ;
  }
      
      unsigned long int sample;
      MP_Chan_t chanIdx;
      MP_Real_t** chan = newSig->channel;
      for ( sample = 0; sample < newSig->numSamples; sample++ )
        { /* loop on frames           */
          sf_readf_double ( file, frame, 1 );                       /* read one frame at a time */
          for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
            {         /* de-interleave it         */
              chan[chanIdx][sample] = frame[chanIdx];
            }
        }
    }
  /* close the file */
  sf_close(file);

  /* Refresh the energy */
  newSig->refresh_energy();

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Done.\n");

  return( newSig );
}


/***************************************/
/* Factory function for channel export */
MP_Signal_c* MP_Signal_c::init( MP_Signal_c *sig, MP_Chan_t chanIdx )
{

  const char* func = "MP_Signal_c::init(sig,chanIdx)";
  MP_Signal_c *newSig = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                "Exporting a channel...\n");

  /* Initial checks */
  if ( sig == NULL )
    {
      mp_error_msg( func, "Trying to export a channel from a NULL signal. Returning NULL.\n" );
      return( NULL );
    }
  if ( chanIdx > sig->numChans )
    {
      mp_error_msg( func, "Asked for export of channel [%hu] from a signal with [%hu] channels.\n",
                    chanIdx, sig->numChans );
      return( NULL );
    }

  /* Instantiate and check */
  newSig = MP_Signal_c::init( 1, sig->numSamples, sig->sampleRate );
  if ( newSig == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a new signal.\n" );
      return( NULL );
    }

  /* Copy the relevant channel */
  memcpy( newSig->storage, sig->channel[chanIdx], sig->numSamples*sizeof(MP_Real_t)
        );

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Done.\n");

  return( newSig );
}


/***************************************/
/* Factory function for support export */
MP_Signal_c* MP_Signal_c::init( MP_Signal_c *sig, MP_Support_t supp )
{

  const char* func = "MP_Signal_c::init(sig,support)";
  MP_Signal_c *newSig = NULL;
  MP_Chan_t chanIdx;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                "Exporting a support...\n");

  /* Initial checks */
  if ( sig == NULL )
    {
      mp_error_msg( func, "Trying to export a channel from a NULL signal.\n" );
      return( NULL );
    }
  if ( (supp.pos+supp.len) > sig->numSamples )
    {
      mp_error_msg( func, "Asked for the export of a support which reaches out"
                    " of the initial signal:"
                    " pos[%lu] + len[%lu] = [%lu], sig->numSamples = [%lu]. Returning NULL.\n",
                    supp.pos, supp.len, supp.pos+supp.len, sig->numSamples );
      return( NULL );
    }

  /* Instantiate and check */
  newSig = MP_Signal_c::init( sig->numChans, supp.len, sig->sampleRate );
  if ( newSig == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a new signal.\n" );
      return( NULL );
    }

  /* Copy the relevant support */
  for ( chanIdx = 0; chanIdx < sig->numChans; chanIdx++ )
    {
      memcpy( newSig->channel[chanIdx], sig->channel[chanIdx]+supp.pos, supp.len*sizeof(MP_Real_t) );
    }

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Done.\n");

  return( newSig );
}


/************************************/
/* Factory function for atom export */
MP_Signal_c* MP_Signal_c::init( MP_Atom_c *atom, const int sampleRate )
{

  const char* func = "MP_Signal_c::init(atom)";
  MP_Signal_c *newSig = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                "Exporting an atom...\n");

  /* Initial checks */
  if ( atom == NULL )
    {
      mp_error_msg( func, "Trying to export a signal from a NULL atom.\n" );
      return( NULL );
    }

  /* Instantiate and check */
  newSig = MP_Signal_c::init( atom->numChans, atom->support[0].len, sampleRate );
  if ( newSig == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a new signal.\n" );
      return( NULL );
    }

  /* Make the waveform */
  atom->build_waveform( newSig->storage );

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Done.\n");

  return( newSig );
}


/************************/
/* Internal allocations */
int MP_Signal_c::init_parameters( const int setNumChans,
                                  const unsigned long int setNumSamples,
                                  const int setSampleRate )
{

  const char* func = "MP_Signal_c::init_parameters(...)";
  int i;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                "Initializing the signal:  [%d] chans [%lu] samples...\n",
                setNumChans, setNumSamples );

  /* Initial parameter check */
  if ( setNumChans < 0 )
    {
      mp_error_msg( func, "The signal's number of channels can't be negative." );
      return( 1 );
    }

  if ( setSampleRate < 0 )
    {
      mp_error_msg( func, "The signal's sample rate can't be negative." );
      return( 1 );
    }
  else sampleRate = setSampleRate;

  /* Clear any previous arrays */
  if ( storage ) free( storage );
  if ( channel ) free( channel );

  /* Allocate the storage space */
  if ( (storage = (MP_Real_t*) calloc( setNumChans*setNumSamples , sizeof(MP_Real_t) )) == NULL )
    {
      mp_error_msg( func,
                    "Can't allocate storage space for new signal with "
                    "[%d] channel(s) and [%lu] samples per channel. New signal is left "
                    "un-initialized.\n", setNumChans, setNumSamples );
      channel = NULL;
      numChans = 0;
      numSamples = 0;
      return( 1 );
    }

  /* "Fold" the storage space into separate channels */
  if ( (channel = (MP_Real_t**) calloc( setNumChans , sizeof(MP_Real_t*) )) == NULL )
    {
      mp_error_msg( func,
                    "Can't allocate an array of [%d] signal pointers "
                    "to fold the signal storage space. Storage will be freed and new signal "
                    "will be left un-initialized.\n", setNumChans );
      free(storage);
      numChans = 0;
      numSamples = 0;
      return( 1 );
    }

  /* If every allocation went OK, fold the storage space and set the size values: */
  /* - Set the size values */
  numChans   = setNumChans;
  numSamples = setNumSamples;
  /* - Fold the storage space */
  for ( i=0; i<numChans; i++ ) channel[i] = storage + i*numSamples;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Done.\n");

  return( 0 );
}


/************************************/
/* Fill the storage space with zero */
void MP_Signal_c::fill_zero( void )
{
  /* Set the storage area to zero */
  memset( storage, 0, numSamples*numChans*sizeof( MP_Real_t ) );
  /* Reset the signal's energy */
  energy = 0.0;
}


/*******************************************/
/* Fill the storage space with white noise */
void MP_Signal_c::fill_noise( MP_Real_t energyLevel )
{
  MP_Real_t *p;

  /* Reset the signal's energy */
  energy = 0.0;
  /* Seed the random generator if it has never been done before */
 
  if (MP_Mtrand_c::get_mtrand()->mti == MTRAND_N+1)
    {
      MP_Mtrand_c::get_mtrand()->init_genrand( (unsigned long int)(time(NULL)) );
      mp_debug_msg( MP_DEBUG_GENERAL, "MP_Signal_c::fill_noise(e)",
                    "SEEDING the random generator !\n" );
    }
#ifndef NDEBUG
  else
    {
      mp_debug_msg( MP_DEBUG_GENERAL, "MP_Signal_c::fill_noise(e)",
                    "Re-using the random generator in its previous state,"
                    " no new seeding.\n" );
    }
#endif
  /* Fill the signal with random doubles in the interval [-1.0,1.0] */
  for ( p = storage;
        p < (storage + numSamples*numChans);
        *p++ = (MP_Real_t)(MP_Mtrand_c::get_mtrand()->mt_nrand( 0.0, 1.0 )) );
  /* Normalize */
  apply_gain( (MP_Real_t)sqrt( (double)energyLevel) / l2norm() );

  return;
}


/********************************/
/* Copy constructor (deep copy) */
MP_Signal_c::MP_Signal_c( const MP_Signal_c &from )
{

  const char* func = "MP_Signal_c::MP_Signal_c(copy)";
  mp_debug_msg( MP_DEBUG_FUNC_ENTER, func,
                "Copying-constructing a new signal...\n");

  sampleRate = MP_SIGNAL_DEFAULT_SAMPLERATE;
  numChans   = 0;
  numSamples = 0;
  storage = NULL;
  channel = NULL;
  clipping = false;
  maxclipping = 0.0;
  energy = 0;

  /* If the input signal is empty, we have nothing to do */
  if ( ( from.numChans == 0 ) || ( from.numSamples == 0 ) ) return;

  /* If every allocation went OK, copy the data */
  if ( init_parameters( from.numChans, from.numSamples, from.sampleRate ) )
    {
      mp_warning_msg( func, "Failed to perform the internal allocations"
                      " in the signal's copy constructor. Returning an empty signal.\n" );
      return;
    }
  else
    {
      int i;
      for ( i = 0; i < from.numChans; i++ )
        memcpy( channel[i], from.channel[i], numSamples*sizeof(MP_Real_t) );
      energy = from.energy;
    }

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Done.\n");

}


/********************/
/* NULL constructor */
MP_Signal_c::MP_Signal_c( void )
{

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Signal_c::MP_Signal_c(void)",
                "Constructing an empty signal.\n" );

  sampleRate = MP_SIGNAL_DEFAULT_SAMPLERATE;
  numChans   = 0;
  numSamples = 0;
  storage = NULL;
  channel = NULL;
  clipping = false;
  maxclipping = 0.0;
  energy = 0;

}


/**************/
/* Destructor */
MP_Signal_c::~MP_Signal_c()
{

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Signal_c::~MP_Signal_c()", "Deleting the signal...\n");

  if (storage) free(storage);
  if (channel) free(channel);

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Signal_c::~MP_Signal_c()", "Done.\n");

}


/***************************/
/* I/O METHODS             */
/***************************/

/*************************************/
/* Reading from a file of raw floats */
unsigned long int MP_Signal_c::read_from_float_file( const char *fName )
{

  FILE *fid;
  /** will initialize initial numCols and numRows with the first value with wich this function is called */
  static int allocated_numChans = 0;
  static unsigned long int allocated_numSamples = 0;
  //float buffer[numChans*numSamples];
  static float* buffer = 0;
    if (!buffer|| allocated_numChans != numChans || allocated_numSamples != numSamples) {
	  if (buffer) free(buffer) ;
	  	  allocated_numChans = numChans ; 
	  	  allocated_numSamples = numSamples;
		  buffer= (float*) malloc (allocated_numChans* allocated_numSamples * sizeof(float)) ;
  }
  
  unsigned long int nRead = 0;
  unsigned long int i;

  /* Open the file in read mode */
  if ( ( fid = fopen( fName, "r" ) ) == NULL )
    {
      mp_error_msg( "MP_Signal_c::read_from_float_file(file)",
                    "Can't open file [%s] for reading a signal.\n", fName );
      return(0);
    }

  /* Read the samples,
     and emit a warning if we can't read enough samples to fill the whole signal: */
  if ( (nRead = mp_fread( buffer, sizeof(float), numChans*numSamples, fid ))
       != (numChans*numSamples) )
    {
      mp_warning_msg( "MP_Signal_c::read_from_float_file(file)",
                      "Can't read more than [%lu] samples from file [%s] "
                      "to fill signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
                      nRead, fName, numChans, numSamples, numChans*numSamples );
    }

  /* If some samples remain in the file, emit a warning: */
  if ( !feof(fid) )
    {
      mp_warning_msg( "MP_Signal_c::read_from_float_file(file)",
                      " Some samples seem to remain after reading [%lu] samples from file [%s] used "
                      "to fill the signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
                      nRead, fName, numChans, numSamples, numChans*numSamples );
    }

  /* Cast the read samples */
  for ( i=0; i<nRead; i++ )
    {
      *(storage+i) = (MP_Real_t)(*(buffer+i));
    }

  /* Compute the energy */
  energy = compute_energy();

  /* Clean the house */
  fclose(fid);
  return(nRead);
}


/***********************************/
/* Dumping to a file of raw floats */
unsigned long int MP_Signal_c::dump_to_float_file( const char *fName )
{

  FILE *fid;
  static int allocated_numChans = 0;
  static unsigned long int allocated_numSamples = 0;
  static float* buffer = 0;
    if (!buffer|| allocated_numChans != numChans || allocated_numSamples != numSamples) {
	  if (buffer) free(buffer) ;
	  	  allocated_numChans = numChans ; 
	  	  allocated_numSamples = numSamples;
		  buffer= (float*) malloc (allocated_numChans* allocated_numSamples * sizeof(float)) ;
  }
 // float buffer[numChans*numSamples];
  unsigned long int nWrite = 0;
  unsigned long int i;

  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL )
    {
      mp_error_msg( "MP_Signal_c::dump_to_float_file(file)",
                    "Can't open file [%s] for writing a signal.\n", fName );
      return(0);
    }

  /* Cast the samples */
  for ( i=0; i<(numChans*numSamples); i++ )
    {
      *(buffer+i) = (float)(*(storage+i));
    }

  /* Write to the file,
     and emit a warning if we can't write all the signal samples: */
  if ( (nWrite = mp_fwrite( buffer, sizeof(float), numChans*numSamples, fid ))
       != (numChans*numSamples) )
    {
      mp_warning_msg( "MP_Signal_c::dump_to_float_file(file)",
                      "Can't write more than [%lu] samples to file [%s] "
                      "in float precision, from signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
                      nWrite, fName, numChans, numSamples, numChans*numSamples );
    }

  /* Clean the house */
  fclose(fid);
  return(nWrite);
}


/************************************/
/* Dumping to a file of raw doubles */
unsigned long int MP_Signal_c::dump_to_double_file( const char *fName )
{

  FILE *fid;
  //double buffer[numChans*numSamples];
  static int allocated_numChans = 0;
  static unsigned long int allocated_numSamples = 0;
  static double* buffer = 0;
    if (!buffer|| allocated_numChans != numChans || allocated_numSamples != numSamples) {
	  if (buffer) free(buffer) ;
	  	  allocated_numChans = numChans ; 
	  	  allocated_numSamples = numSamples;
		  buffer= (double*) malloc (allocated_numChans* allocated_numSamples * sizeof(double)) ;
  }
  unsigned long int nWrite = 0;
  unsigned long int i;

  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL )
    {
      mp_error_msg( "MP_Signal_c::dump_to_double_file(file)",
                    "Can't open file [%s] for writing a signal.\n", fName );
      return(0);
    }

  /* Cast the samples */
  for ( i=0; i<(numChans*numSamples); i++ )
    {
      *(buffer+i) = (double)(*(storage+i));
    }

  /* Write to the file,
     and emit a warning if we can't write all the signal samples: */
  if ( (nWrite = mp_fwrite( buffer, sizeof(double), numChans*numSamples, fid ))
       != (numChans*numSamples) )
    {
      mp_warning_msg( "MP_Signal_c::dump_to_double_file(file)",
                      " Can't write more than [%lu] samples to file [%s] in double precision,"
                      " from signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
                      nWrite, fName, numChans, numSamples, numChans*numSamples );
    }

  /* Clean the house */
  fclose(fid);
  return(nWrite);
}


/*************************/
/* Writing to a wav file */
unsigned long int MP_Signal_c::wavwrite( const char *fName )
{
  SNDFILE *file;
  SF_INFO sfinfo;
  unsigned long int numFrames = 0;

  /* 1) fill in the sound file information */
  sfinfo.samplerate = sampleRate;
  sfinfo.frames     = numSamples;
  sfinfo.channels   = numChans;
  sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  sfinfo.sections   = 0;
  sfinfo.seekable   = 0;

  if (sf_format_check (&sfinfo)==0)
    {
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "Bad output format\n" );
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- format   : %d\n", sfinfo.format) ;
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- sections : %d\n", sfinfo.sections);
      mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
      return(0);
    }

  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::wavwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;

  /* open the file */
  file = sf_open(fName,SFM_WRITE,&sfinfo);
  if (file == NULL)
    {
      mp_error_msg( "MP_Signal_c::wavwrite(file)",
                    " Cannot open sound file %s for writing\n",fName);
      return(0);
    }

  /* write the file */
  {
   // double frame[numChans];
  static int allocated_numChans = 0;
  static double* frame = 0;
    if (!frame|| allocated_numChans != numChans) {
	  if (frame) free(frame) ;
	  	  allocated_numChans = numChans ; 
		  frame= (double*) malloc (allocated_numChans * sizeof(double)) ;
  }
    unsigned long int sample;
    int chan;
    for (sample=0; sample < numSamples; sample++)
      {                     /* loop on frames                       */
        for (chan = 0; chan < numChans; chan++)
          {     /* interleave the channels in one frame */
            frame[chan] = channel[chan][sample];
             /* test if clipping may occur */
            if (frame[chan]*frame[chan]>1) {clipping = true;
            	if (frame[chan] < 0) { if(-frame[chan]>maxclipping)maxclipping= -frame[chan]; }
            	else { if(frame[chan]>maxclipping)maxclipping= frame[chan]; }
            }     
          }
        numFrames += sf_writef_double (file, frame, 1 );
      }
  }
  /* close the file */
  sf_close(file);
  if (clipping) mp_warning_msg( "MP_Signal_c::matwrite(file)", "value of frames are major to 1 with max value %f, clipping may occur on the reconstruct wave file.\nYou may apply a gain of %f on the imput signal\n", maxclipping, 0.99/maxclipping);
  return(numFrames);
}


/*************************/
/* Writing to a Mat file */
unsigned long int MP_Signal_c::matwrite( const char *fName )
{
  SNDFILE *file;
  SF_INFO sfinfo;
  unsigned long int numFrames = 0;

  /* 1) fill in the sound file information */
  sfinfo.samplerate = sampleRate;
  sfinfo.frames     = numSamples;
  sfinfo.channels   = numChans;
  if (sizeof(MP_Real_t)==sizeof(double))
    sfinfo.format     = SF_FORMAT_MAT5 | SF_FORMAT_DOUBLE;
  else
    sfinfo.format     = SF_FORMAT_MAT5 | SF_FORMAT_FLOAT;
  sfinfo.sections   = 0;
  sfinfo.seekable   = 0;

  if (sf_format_check (&sfinfo)==0)
    {
      mp_error_msg( "MP_Signal_c::matwrite(file)", "Bad output format\n");
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- format   : %d\n", sfinfo.format) ;
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- sections : %d\n", sfinfo.sections);
      mp_error_msg( "MP_Signal_c::matwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
      return(0);
    }

  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( MP_DEBUG_FILE_IO, "MP_Signal_c::matwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;

  /* open the file */
  file = sf_open(fName,SFM_WRITE,&sfinfo);
  if (file == NULL)
    {
      mp_error_msg( "MP_Signal_c::matwrite(file)", "Cannot open sound file %s for writing\n",fName);
      return(0);
    }

  /* write the file */
  {
    //double frame[numChans];
    static int allocated_numChans = 0;
  static double* frame = 0;
    if (!frame|| allocated_numChans != numChans) {
	  if (frame) free(frame) ;
	  	  allocated_numChans = numChans ; 
		  frame= (double*) malloc (allocated_numChans * sizeof(double)) ;
  }
    unsigned long int sample;
    int chan;
    for (sample=0; sample < numSamples; sample++)
      { /* loop on frames                       */
        for (chan = 0; chan < numChans; chan++)
          {     /* interleave the channels in one frame */
            frame[chan] = channel[chan][sample];
             /* test if clipping may occur */
            if (frame[chan]*frame[chan]>1) {clipping = true;
            	if (frame[chan] < 0) { if(-frame[chan]>maxclipping)maxclipping= -frame[chan]; }
            	else { if(frame[chan]>maxclipping)maxclipping= frame[chan]; }
            }
            
          }
        numFrames += sf_writef_double (file, frame, 1 );
      }
  }
  /* close the file */
  sf_close(file);
  if (clipping) mp_warning_msg( "MP_Signal_c::matwrite(file)", "value of frames are major to 1 with max value %f, clipping may occur on the reconstruct wave file.\nYou may apply a gain of %f on the imput signal\n", maxclipping, 0.99/maxclipping);
  return(numFrames);
}

/*************************/
/* Readable text dump    */
int MP_Signal_c::info( FILE *fid )
{

  int nChar = 0;

  nChar += mp_info_msg( fid, "SIGNAL",
                        "[%lu] samples on [%d] channels; sample rate [%d]Hz; energy [%g].\n",
                        numSamples, numChans, sampleRate, energy );

  return( nChar );
}

/*************************/
/* Readable text dump    */
int MP_Signal_c::info( void )
{

  int nChar = 0;

  nChar += mp_info_msg( "SIGNAL",
                        "[%lu] samples on [%d] channels; sample rate [%d]Hz; energy [%g].\n",
                        numSamples, numChans, sampleRate, energy );

  return( nChar );
}


/***************************/
/* MISC METHODS            */
/***************************/

/***********/
/* L1 norm */
MP_Real_t MP_Signal_c::l1norm( void )
{

  double ret = 0.0;
  MP_Real_t *p;

  assert( storage != NULL );

  for ( p = storage;
        p < (storage + numChans*numSamples);
        p++ )
    {
      ret += fabs( (double)(*p) );
    }

  return( (MP_Real_t)( ret ) );
}


/***********/
/* L2 norm */
MP_Real_t MP_Signal_c::l2norm( void )
{

  double ret = 0.0;
  double val;
  MP_Real_t *p;

  assert( storage != NULL );

  for ( p = storage;
        p < (storage + numChans*numSamples);
        p++ )
    {
      val = (double)(*p);
      ret += (val * val);
    }

  return( (MP_Real_t)sqrt( ret ) );
}


/***********/
/* Lp norm */
MP_Real_t MP_Signal_c::lpnorm( MP_Real_t P )
{

  double ret = 0.0;
  double val;
  MP_Real_t *ptr;

  assert( storage != NULL );

  for ( ptr = storage;
        ptr < (storage + numChans*numSamples);
        ptr++ )
    {
      val = fabs( (double)(*ptr) );
      ret += pow( val, (double)(P) );
    }

  return( (MP_Real_t)pow( ret, 1.0/P ) );
}


/*************/
/* Linf norm */
MP_Real_t MP_Signal_c::linfnorm( void )
{

  double ret;
  double val;
  MP_Real_t *p;

  assert( storage != NULL );

  ret = (double)(*storage);
  for ( p = storage + 1;
        p < (storage + numChans*numSamples);
        p++ )
    {
      val = fabs( (double)(*p) );
      ret = ( val > ret ? val : ret ); /* max( val, ret ) */
    }

  return( (MP_Real_t)( ret ) );
}


/*************************************************/
/* Refresh the energy field in the signal object */
void MP_Signal_c::refresh_energy( void )
{
  energy = compute_energy();
}


/***********************/
/* Total signal energy */
MP_Real_t MP_Signal_c::compute_energy( void )
{

  double retEnergy = 0.0;
  double val;
  MP_Real_t *p;

  assert( storage != NULL );

  for ( p = storage;
        p < (storage + numChans*numSamples);
        p++ )
    {
      val = (double)(*p);
      retEnergy += (val * val);
    }

  return( (MP_Real_t)retEnergy );
}


/*****************************************/
/* Signal energy over a specific channel */
MP_Real_t MP_Signal_c::compute_energy_in_channel( MP_Chan_t chanIdx )
{

  double retEnergy = 0.0;
  double val;
  MP_Real_t *p;

  assert( storage != NULL );
  assert( chanIdx < numChans );
  assert( channel[chanIdx] != NULL );

  for ( p = (storage + chanIdx*numSamples);
        p < (storage + chanIdx*numSamples + numSamples);
        p++ )
    {
      val = (double)(*p);
      retEnergy += (val * val);
    }

  return( (MP_Real_t)retEnergy );
}


/*****************************************/
/* Apply a gain to the whole signal      */
MP_Real_t MP_Signal_c::apply_gain( MP_Real_t gain )
{

  MP_Real_t *p;

  assert( storage != NULL );

  for ( p = storage;
        p < (storage + numChans*numSamples);
        *p++ = (MP_Real_t)( ((double)(*p)) * (double)(gain) ) );

  refresh_energy();

  return( energy );
}


/*****************************************/
/* Pre_emphasis                          */
MP_Real_t MP_Signal_c::preemp( double coeff )
{

  unsigned long int i;
  int chan;

  MP_Real_t *p;
  double val, valBefore;
  double result;

  double retEnergy = 0.0;

  assert( storage != NULL );

  for (chan = 0; chan < numChans; chan++ )
    {

      p = (storage + chan*numSamples);
      valBefore = 0.0;

      for ( i = 0; i < numSamples; i++, p++ )
        {
          val = (double)(*p);
          result = val - coeff*valBefore;
          (*p) = result;
          retEnergy += (result * result);
          valBefore = val;
        }

    }

  /* Register the new signal energy */
  energy = retEnergy;

  return( (MP_Real_t)retEnergy );
}

/*****************************************/
/* De_emphasis                           */
MP_Real_t MP_Signal_c::deemp( double coeff )
{

  unsigned long int i;
  int chan;

  MP_Real_t *p;
  double result;

  double retEnergy = 0.0;

  assert( storage != NULL );

  for (chan = 0; chan < numChans; chan++ )
    {

      p = (storage + chan*numSamples);
      result = 0.0;

      for ( i = 0; i < numSamples; i++, p++ )
        {
          result = (double)(*p) + coeff*result;
          (*p) = result;
          retEnergy += (result * result);
        }

    }

  /* Register the new signal energy */
  energy = retEnergy;

  return( (MP_Real_t)retEnergy );
}


/***************************/
/* OPERATORS               */
/***************************/

/********************************/
/* Assignment operator          */
MP_Signal_c& MP_Signal_c::operator=( const MP_Signal_c& from )
{

  mp_debug_msg( MP_DEBUG_FUNC_ENTER, "MP_Signal_c::operator=()", "Assigning a signal...\n" );

  /* If every allocation went OK, copy the data */
  if ( init( from.numChans, from.numSamples, from.sampleRate ) )
    {
      memcpy( storage, from.storage, numChans*numSamples*sizeof(MP_Real_t) );
    }

  /* Copy the energy */
  energy = from.energy;

  mp_debug_msg( MP_DEBUG_FUNC_EXIT, "MP_Signal_c::operator=()", "Assignment done.\n" );

  return( *this );
}


/*******************/
/* == (COMPARISON) */
MP_Bool_t MP_Signal_c::operator==( const MP_Signal_c& s1 )
{

  int chanIdx;
  unsigned long int i;

  if ( numChans != s1.numChans )     return ( (MP_Bool_t)( MP_FALSE ) );
  if ( numSamples != s1.numSamples ) return ( (MP_Bool_t)( MP_FALSE ) );
  if ( sampleRate != s1.sampleRate ) return ( (MP_Bool_t)( MP_FALSE ) );
  //if ( energy != s1.energy )         return ( (MP_Bool_t)( MP_FALSE ) );
  /* Note: the energy test is optional. Normally, if the samples are the same
     (tested below), the energy should be synchronized as well, unless
     something is screwed up somewhere else in the code. */
  /* Browse until different sample values are found */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {
      for ( i = 0;
            (i < numSamples) && ( channel[chanIdx][i] == s1.channel[chanIdx][i] );
            i++ );
      /* And check where the loop stopped: */
      if ( i != numSamples ){
      	mp_warning_msg( "MP_Signal_c::matwrite(file)", "diff in sample :[%i]\n",i );
      	 return( (MP_Bool_t)( MP_FALSE ) );}
      
    }

  return( (MP_Bool_t)( MP_TRUE ) );
}
/* DIFF */
MP_Bool_t MP_Signal_c::diff( const MP_Signal_c& s1, double precision )
{

  int chanIdx;
  unsigned long int i;

  if ( numChans != s1.numChans )    {
  	mp_warning_msg( "MP_Signal_c::diff", "diff in number of channel");
  	 return ( (MP_Bool_t)( MP_TRUE ) );}
  if ( numSamples != s1.numSamples ) {
  	mp_warning_msg( "MP_Signal_c::diff", "diff in number of samples");
  	return ( (MP_Bool_t)( MP_TRUE ));}
  if ( sampleRate != s1.sampleRate ) {
  	mp_warning_msg( "MP_Signal_c::diff", "diff in sample rate");
  	return ( (MP_Bool_t)( MP_TRUE ) );}
  //if ( energy != s1.energy )         return ( (MP_Bool_t)( MP_FALSE ) );
  /* Note: the energy test is optional. Normally, if the samples are the same
     (tested below), the energy should be synchronized as well, unless
     something is screwed up somewhere else in the code. */
  /* Browse until different sample values are found */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {
      for ( i = 0;
            (i < numSamples);
            i++ ) { 
      /* And check where the loop stopped: */
      if ( ( (channel[chanIdx][i]- s1.channel[chanIdx][i])*(channel[chanIdx][i]- s1.channel[chanIdx][i]) >= precision*precision ) ){
      	mp_warning_msg( "MP_Signal_c::diff", "diff in sample :[%i]\n",i );
      	 return( (MP_Bool_t)( MP_TRUE ) );
    
    }}}

  return( (MP_Bool_t)( MP_FALSE ) );
}

/***********************/
/* != (NEG COMPARISON) */
MP_Bool_t MP_Signal_c::operator!=( const MP_Signal_c& s1 )
{
  return( !( (*this) == s1 ) );
}
