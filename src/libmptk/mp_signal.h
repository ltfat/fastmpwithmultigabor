/******************************************************************************/
/*                                                                            */
/*                               mp_signal.h                                  */
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

/**********************************/
/*                                */
/* DEFINITION OF THE SIGNAL CLASS */
/*                                */
/**********************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __signal_h_
#define __signal_h_


/***********************/
/* CONSTANTS           */
/***********************/

/** \brief The default sample rate of signals */
#define MP_SIGNAL_DEFAULT_SAMPLERATE 44100


/***********************/
/* SIGNAL CLASS        */
/***********************/
/** \brief Mono or multichannel signals can be read from/written to files and analyzed. */
class MP_Signal_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief Number of channels */
  int numChans;                 
  /** \brief Number of samples IN EACH CHANNEL */
  unsigned long int numSamples; 
  /** \brief Sample rate in Hertz */
  int sampleRate;

  /** \brief Big storage space with room for
      numChans*numSamples samples */
  MP_Sample_t *storage;  

  /** \brief Pointers to access each channel separately
      in storage as channel[0] ... channel[numChans-1];

      Example:
      3rd sample of second channel is
      channel[1][2] or *(channel[1]+2). */
  MP_Sample_t **channel; 

  /** \brief The signal's energy */
  MP_Real_t energy;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief A plain constructor which does not allocate anything */
  MP_Signal_c( void );
  /** \brief A constructor which allocates storage space and initializes it to zero
   *
   * \param setNumChans The desired number of channels
   * \param setNumSamples The desired number of samples per channel 
   * \param setSampleRate The desired sample rate 
   */
  MP_Signal_c( const int setNumChans,
	       const unsigned long int setNumSamples ,
	       const int setSampleRate);

  /** \brief A contructor that reads from a sound file 
   *
   * \param fName the file name
   */
  MP_Signal_c( const char *fName );

  /** \brief A copy constructor
   *
   * \param from A reference to the copied signal
   */
  MP_Signal_c( const MP_Signal_c &from );

  /* Destructor */
  ~MP_Signal_c();


  /***************************/
  /* I/O METHODS             */
  /***************************/

  /** \brief Read a signal from a file as raw data in float precision
   * \param fName the file name 
   * \return the number of floats read (zero if failure to read the float file)
   * \remark the number of floats read is \a numChans times \a numSamples, 
   * and a warning is emitted if we can't read enough samples
   * \remark the \a sampleRate is not modified
   */
  unsigned long int read_from_float_file( const char *fName );


  /** \brief Write a signal to a file as raw data in float precision
   * \param fName the file name
   * \return the number of floats written (zero if failure to write the float file)
   */
  unsigned long int dump_to_float_file( const char *fName );


  /** \brief Write a signal to a file as raw data in double precision
   * \param fName the file name
   * \return the number of doubles written (zero if failure to write the float file)
   */
  unsigned long int dump_to_double_file( const char *fName );


  /** \brief Write a signal to a WAV file 
   * \param fName the file name 
   * \return the number of frames written (zero if failure to write the WAV file)
   */
  unsigned long int wavwrite( const char *fName );


  /** \brief Write a signal to a MAT file 
   * \param fName the file name 
   * \return the number of frames written (zero if failure to write the MAT file)
   */
  unsigned long int matwrite( const char *fName );


  /** \brief Send a brief info about the signal to a stream
   * \param fid A writeable stream
   * \return the number of written characters
   */
  int info( FILE *fid );


  /***************************/
  /* OTHER METHODS           */
  /***************************/

private:

  /* \brief A utility to set all data to default or NULL values. */
  void set_null( void );

public:

  /* \brief A utility to clear and reallocate the storage at a given
   * size without, possibly changing the sampling rate
   *
   * \param setNumChans The desired number of channels
   * \param setNumSamples The desired number of samples per channel
   * \param setSampleRate The desired sample rate
   */
  int init( const int setNumChans, const unsigned long int setNumSamples, const int setSampleRate );


  /** \brief Compute the total signal energy over all channels,
   *  using the signal samples.
   *  \return the computed energy */
  MP_Real_t compute_energy( void );


  /** \brief Compute the signal energy over a specified channel
   * \param numChan the channel number 
   * \return the computed energy
   */
  MP_Real_t compute_energy_in_channel( int numChan );


  /** \brief Pre-emphasize the signal (in all the channels)
   * \param coeff The pre-emphasis coefficient
   * \return the new energy of the signal
   *
   * \note signal.energy is set to the new energy.
   */
  MP_Real_t preemp( double coeff );


  /** \brief De-emphasize the signal (in all the channels)
   * \param coeff The former pre-emphasis coefficient
   * \return the new energy of the signal
   *
   * \note signal.energy is set to the new energy.
   */
  MP_Real_t deemp( double coeff );


  /***************************/
  /* OPERATORS               */
  /***************************/

  /** \brief Assignment operator */
  MP_Signal_c& operator=(  const MP_Signal_c& from );

};


#endif /* __signal_h_ */
