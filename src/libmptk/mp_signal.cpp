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
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/*********************************************/
/*                                           */
/* signal.cpp: methods for class MP_Signal_c */
/*                                           */
/*********************************************/

#include "mptk.h"
#include "mp_system.h"

#include <sndfile.h>


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/********************/
/* Void constructor */
MP_Signal_c::MP_Signal_c(void) {

  mp_debug_msg( "MP_Signal_c::MP_Signal_c(void)", "New empty signal.\n" );

  set_null();
}


/************************/
/* Specific constructor */
MP_Signal_c::MP_Signal_c( const int setNumChans,
			  const unsigned long int setNumSamples ,
			  const int setSampleRate ) {
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( 3 params )", "Setting a new signal...\n");

  set_null();
  init( setNumChans, setNumSamples, setSampleRate );

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( 3 params )", "Done.\n");
}


/********************/
/* File constructor */
MP_Signal_c::MP_Signal_c( const char *fName ) {

  SNDFILE *file;
  SF_INFO sfinfo;

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )",
		"New signal from fName=[%s]...\n", fName );

  set_null();

  /* open the file */
  if ( fName == NULL ) {
    mp_error_msg( "MP_Signal_c( fName )", "Invalid file name [%s] was passed"
		  " to a signal constructor.\n", fName );
    return;
  }
  else {

    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "Doing sf_open on file [%s]...\n", fName );

    sfinfo.format  = 0; /* -> See the libsndfile manual. */
    file = sf_open( fName, SFM_READ, &sfinfo );

    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "Done.\n" );

  }
  /* Check */
  if ( file == NULL ) {
    mp_error_msg( "MP_Signal_c( fName )", "sf_open could not open the sound file [%s] for reading."
		  " New signal is left un-initialized\n", fName );
    return;
  }

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "sfinfo contains:\n");
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- seekable : %d\n", sfinfo.seekable) ;
  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- end sfinfo.\n");

  /* actually read the file if allocation is OK */
  if ( init(sfinfo.channels,sfinfo.frames,sfinfo.samplerate) ) {

    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "After init, signal values are:\n");
    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- sampleRate : %d\n", sampleRate) ;
    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- numChans   : %d\n", numChans) ;
    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- numSamples : %lu\n", numSamples) ;
    mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "-- end after init.\n");

    double frame[numChans];
    unsigned long int sample;
    int chan;
    for (sample=0; sample < numSamples; sample++) { /* loop on frames           */
      sf_readf_double (file, frame, 1 );            /* read one frame at a time */
      for (chan = 0; chan < numChans; chan++) {     /* de-interleave it         */
	channel[chan][sample] = frame[chan];
      }
    }
  }
  /* close the file */
  sf_close(file);

  /* Refresh the energy */
  energy = compute_energy();

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( fName )", "Done.\n");

}


/********************************/
/* Copy constructor (deep copy) */
MP_Signal_c::MP_Signal_c( const MP_Signal_c &from ) {

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( copy )", "Copying-constructing a new signal...\n");

  set_null();

  /* If the input signal is empty, we have nothing to do */
  if ( ( from.numChans == 0 ) || ( from.numSamples == 0 ) ) return;

  /* If every allocation went OK, copy the data */
  if ( init( from.numChans, from.numSamples, from.sampleRate ) ) {
    memcpy( storage, from.storage, numChans*numSamples*sizeof(MP_Sample_t) );
  }

  /* Copy the energy */
  energy = from.energy;

  mp_debug_msg( "MP_Signal_c::MP_Signal_c( copy )", "Done.\n");

}


/**************/
/* Destructor */
MP_Signal_c::~MP_Signal_c() {

  mp_debug_msg( "MP_Signal_c::~MP_Signal_c()", "Deleting the signal...");

  if (storage) free(storage);
  if (channel) free(channel);

  mp_debug_msg( "MP_Signal_c::~MP_Signal_c()", "Done.\n");

}


/***************************/
/* I/O METHODS             */
/***************************/

/*************************************/
/* Reading from a file of raw floats */
unsigned long int MP_Signal_c::read_from_float_file( const char *fName ) {

  FILE *fid;
  float buffer[numChans*numSamples];
  unsigned long int nRead = 0;
  unsigned long int i;
 
  /* Open the file in read mode */
  if ( ( fid = fopen( fName, "r" ) ) == NULL ) {
    mp_error_msg( "MP_Signal_c::read_from_float_file(file)",
		  "Can't open file [%s] for reading a signal.\n", fName );
    return(0);
  }

  /* Read the samples,
     and emit a warning if we can't read enough samples to fill the whole signal: */
  if ( (nRead = mp_fread( buffer, sizeof(float), numChans*numSamples, fid ))
       != (numChans*numSamples) ) {
    mp_warning_msg( "MP_Signal_c::read_from_float_file(file)",
		    "Can't read more than [%lu] samples from file [%s] "
		    "to fill signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
		    nRead, fName, numChans, numSamples, numChans*numSamples );
  }
  
  /* If some samples remain in the file, emit a warning: */
  if ( !feof(fid) ) {
    mp_warning_msg( "MP_Signal_c::read_from_float_file(file)",
		    " Some samples seem to remain after reading [%lu] samples from file [%s] used "
		    "to fill the signal with [%d]channels*[%lu]samples=[%lu] samples.\n",
		    nRead, fName, numChans, numSamples, numChans*numSamples );
  }
  
  /* Cast the read samples */
  for ( i=0; i<nRead; i++ ) { *(storage+i) = (MP_Sample_t)(*(buffer+i)); }

  /* Compute the energy */
  energy = compute_energy();

  /* Clean the house */
  fclose(fid);
  return(nRead);
}


/***********************************/
/* Dumping to a file of raw floats */
unsigned long int MP_Signal_c::dump_to_float_file( const char *fName ) {

  FILE *fid;
  float buffer[numChans*numSamples];
  unsigned long int nWrite = 0;
  unsigned long int i;
 
  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL ) {
    mp_error_msg( "MP_Signal_c::dump_to_float_file(file)",
		  "Can't open file [%s] for writing a signal.\n", fName );
    return(0);
  }

  /* Cast the samples */
  for ( i=0; i<(numChans*numSamples); i++ ) { *(buffer+i) = (float)(*(storage+i)); }
  
  /* Write to the file,
     and emit a warning if we can't write all the signal samples: */
  if ( (nWrite = mp_fwrite( buffer, sizeof(float), numChans*numSamples, fid ))
       != (numChans*numSamples) ) {
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
unsigned long int MP_Signal_c::dump_to_double_file( const char *fName ) {

  FILE *fid;
  double buffer[numChans*numSamples];
  unsigned long int nWrite = 0;
  unsigned long int i;
 
  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL ) {
    mp_error_msg( "MP_Signal_c::dump_to_double_file(file)",
		  "Can't open file [%s] for writing a signal.\n", fName );
    return(0);
  }

  /* Cast the samples */
  for ( i=0; i<(numChans*numSamples); i++ ) { *(buffer+i) = (double)(*(storage+i)); }
  
  /* Write to the file,
     and emit a warning if we can't write all the signal samples: */
  if ( (nWrite = mp_fwrite( buffer, sizeof(double), numChans*numSamples, fid ))
       != (numChans*numSamples) ) {
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
unsigned long int MP_Signal_c::wavwrite( const char *fName ) {
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

  if (sf_format_check (&sfinfo)==0) {
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "Bad output format\n" );
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- format   : %d\n", sfinfo.format) ;
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- sections : %d\n", sfinfo.sections);
    mp_error_msg( "MP_Signal_c::wavwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
    return(0);
  }

#ifndef NDEBUG
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( "MP_Signal_c::wavwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
#endif

  /* open the file */
  file = sf_open(fName,SFM_WRITE,&sfinfo);
  if (file == NULL) {
    mp_error_msg( "MP_Signal_c::wavwrite(file)",
		  " Cannot open sound file %s for writing\n",fName);
    return(0);
  }

  /* write the file */
  {
    double frame[numChans];
    unsigned long int sample;
    int chan;
    for (sample=0; sample < numSamples; sample++) {                     /* loop on frames                       */
      for (chan = 0; chan < numChans; chan++) {     /* interleave the channels in one frame */
	frame[chan] = channel[chan][sample];
      }
      numFrames += sf_writef_double (file, frame, 1 );
    }
  }
  /* close the file */
  sf_close(file);
  return(numFrames);
}


/*************************/
/* Writing to a Mat file */
unsigned long int MP_Signal_c::matwrite( const char *fName ) {
  SNDFILE *file;
  SF_INFO sfinfo;
  unsigned long int numFrames = 0;

  /* 1) fill in the sound file information */
  sfinfo.samplerate = sampleRate;
  sfinfo.frames     = numSamples;
  sfinfo.channels   = numChans;
  if (sizeof(MP_Sample_t)==sizeof(double))
    sfinfo.format     = SF_FORMAT_MAT5 | SF_FORMAT_DOUBLE;
  else
    sfinfo.format     = SF_FORMAT_MAT5 | SF_FORMAT_FLOAT;
  sfinfo.sections   = 0;
  sfinfo.seekable   = 0;

  if (sf_format_check (&sfinfo)==0) {
    mp_error_msg( "MP_Signal_c::matwrite(file)", "Bad output format\n");
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- format   : %d\n", sfinfo.format) ;
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- sections : %d\n", sfinfo.sections);
    mp_error_msg( "MP_Signal_c::matwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
    return(0);
  }

#ifndef NDEBUG
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- srate    : %d\n", sfinfo.samplerate) ;
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- frames   : %d\n", (int)sfinfo.frames) ;
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- channels : %d\n", sfinfo.channels) ;
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- format   : %d\n", sfinfo.format) ;
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- sections : %d\n", sfinfo.sections);
  mp_debug_msg( "MP_Signal_c::matwrite(file)", "-- seekable : %d\n", sfinfo.seekable) ;
#endif

  /* open the file */
  file = sf_open(fName,SFM_WRITE,&sfinfo);
  if (file == NULL) {
    mp_error_msg( "MP_Signal_c::matwrite(file)", "Cannot open sound file %s for writing\n",fName);
    return(0);
  }

  /* write the file */
  {
    double frame[numChans];
    unsigned long int sample;
    int chan;
    for (sample=0; sample < numSamples; sample++) { /* loop on frames                       */
      for (chan = 0; chan < numChans; chan++) {     /* interleave the channels in one frame */
	frame[chan] = channel[chan][sample];
      }
      numFrames += sf_writef_double (file, frame, 1 );
    }
  }
  /* close the file */
  sf_close(file);
  return(numFrames);
}

/*************************/
/* Readable text dump    */
int MP_Signal_c::info( FILE *fid ) {

  int nChar = 0;

  nChar += mp_info_msg_str( fid, "MP_Signal_c::info()",
			    "This signal object has [%lu] samples on [%d] channels;"
			    " its sample rate is [%d]Hz.\n",
			    numSamples, numChans, sampleRate );
  return( nChar );
}


/***************************/
/* MISC METHODS            */
/***************************/


/******************************************/
/* Set everything to default NULL values. */
/* (WARNING: it DOES NOT and SHOULD NOT be used to deallocate
   the storage and channel arrays, and it should only be called
   by the constructors.) */
inline void MP_Signal_c::set_null( void ) {

  mp_debug_msg( "MP_Signal_c::set_null()", "Setting the signal to NULL...\n" );

  sampleRate = MP_SIGNAL_DEFAULT_SAMPLERATE;
  numChans   = 0;
  numSamples = 0;
  storage = NULL;
  channel = NULL;
  energy = 0;

  mp_debug_msg( "MP_Signal_c::set_null()", "Done.\n" );

}


/**********************************/
/* Initialization with allocation */
int MP_Signal_c::init( const int setNumChans, const unsigned long int setNumSamples, const int setSampleRate ) {

  mp_debug_msg( "MP_Signal_c::init()", "Initializing the signal:  [%d] chans [%lu] samples...\n",
		setNumChans, setNumSamples );

  sampleRate = setSampleRate;

  if ( storage ) free( storage );
  if ( channel ) free( channel );

  /* Allocate the storage space */
  if ( (storage = (MP_Sample_t*) calloc( setNumChans*setNumSamples , sizeof(MP_Sample_t) )) == NULL ) {
    mp_error_msg( "MP_Signal_c::init()",
		  "Can't allocate storage space for new signal with "
		  "[%d] channel(s) and [%lu] samples per channel. New signal is left "
		  "un-initialized.\n", setNumChans, setNumSamples );
    channel = NULL;
    numChans = 0;
    numSamples = 0;
    return(0);
  }

  /* "Fold" the storage space into separate channels */
  if ( (channel = (MP_Sample_t**) calloc( setNumChans , sizeof(MP_Sample_t*) )) == NULL ) {
    mp_error_msg( "MP_Signal_c::init()",
		  "Can't allocate an array of [%d] signal pointers "
		  "to fold the signal storage space. Storage will be freed and new signal "
		  "will be left un-initialized.\n", setNumChans );
    free(storage);
    numChans = 0;
    numSamples = 0;
    return(0);
  }
  /* If every allocation went OK, fold the storage space and set the size values. */
  {      
    int i;
    /* Set the size values */
    numChans   = setNumChans;
    numSamples = setNumSamples;
    /* Fold the storage space */
    for ( i=0; i<numChans; i++ ) channel[i] = storage + i*numSamples;
  }

  mp_debug_msg( "MP_Signal_c::init()", "Done.\n");

  return( 1 );
}


/***********************/
/* Total signal energy */
MP_Real_t MP_Signal_c::compute_energy( void ) {

  double retEnergy = 0.0;
  double val;
  MP_Real_t *p;
  unsigned long int i;

  assert( storage != NULL );

  for ( i = 0, p = storage;
	i < (numChans*numSamples);
	i++, p++ ) {

    val = (double)(*p);
    retEnergy += (val * val);
  }

  return( (MP_Real_t)retEnergy );
}


/*****************************************/
/* Signal energy over a specific channel */
MP_Real_t MP_Signal_c::compute_energy_in_channel( int numChan ) {

  double retEnergy = 0.0;
  double val;
  MP_Real_t *p;
  unsigned long int i;

  assert( storage != NULL );
  assert( numChan < numChans );
  assert( channel[numChan] != NULL );

  for ( i = 0, p = (storage + numChan*numSamples);
	i < numSamples;
	i++, p++ ) {

    val = (double)(*p);
    retEnergy += (val * val);
  }

  return( (MP_Real_t)retEnergy );
}

/*****************************************/
/* Pre_emphasis                          */
MP_Real_t MP_Signal_c::preemp( double coeff ) {

  unsigned long int i;
  int chan;

  MP_Real_t *p;
  double val, valBefore;
  double result;

  double retEnergy = 0.0;

  assert( storage != NULL );

  for (chan = 0; chan < numChans; chan++ ) {

    p = (storage + chan*numSamples);
    valBefore = 0.0;

    for ( i = 0; i < numSamples; i++, p++ ) {
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
MP_Real_t MP_Signal_c::deemp( double coeff ) {

  unsigned long int i;
  int chan;

  MP_Real_t *p;
  double result;

  double retEnergy = 0.0;

  assert( storage != NULL );

  for (chan = 0; chan < numChans; chan++ ) {

    p = (storage + chan*numSamples);
    result = 0.0;

    for ( i = 0; i < numSamples; i++, p++ ) {
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
MP_Signal_c& MP_Signal_c::operator=( const MP_Signal_c& from ) {

  mp_debug_msg( "MP_Signal_c::operator=()", "Assigning a signal...\n" );

  /* If every allocation went OK, copy the data */
  if ( init( from.numChans, from.numSamples, from.sampleRate ) ) {
    memcpy( storage, from.storage, numChans*numSamples*sizeof(MP_Sample_t) );
  }

  /* Copy the energy */
  energy = from.energy;

  mp_debug_msg( "MP_Signal_c::operator=()", "Assignment done.\n" );

  return( *this );
}
