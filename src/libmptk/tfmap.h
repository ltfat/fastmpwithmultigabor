/******************************************************************************/
/*                                                                            */
/*                                 tfmap.h                                    */
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

/**********************************************/
/*                                            */
/* DEFINITION OF THE TIME-FREQUENCY MAP CLASS */
/*                                            */
/**********************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

#ifndef __tfmap_h_
#define __tfmap_h_


#include <mp_system.h>


/*********************/
/* CONSTANTS         */
/*********************/
/** \brief Some constants to say what kind of Matching Pursuit representation
    shall be put in the tfmap (in the case of atoms) */
#define MP_TFMAP_SUPPORTS          1
#define MP_TFMAP_LOG_SUPPORTS      2
#define MP_TFMAP_PSEUDO_WIGNER     3
#define MP_TFMAP_LOG_PSEUDO_WIGNER 4
#define MP_TFMAP_NOTHING           255


/********************************/
/* Time-Freq Map  class         */
/********************************/
class MP_TF_Map_c {

  /* DATA */
public : 
  /* STORAGE FOR THE MAP */
  /** \brief Number of columns in the time-frequency map */
  unsigned long int numCols;
  /** \brief Number of rows in the time-frequency map */
  unsigned long int numRows;
  /** \brief Number of channels in the time-frequency map */
  int numChans;
  /** \brief Big storage space to store the map */
  MP_Tfmap_t  *storage;
  /** \brief Pointers to each channel of the map */
  MP_Tfmap_t **channel;

  /* CONVERSION BETWEEN PIXEL COORDINATES AND TIME-FREQUENCY COORDINATES */
  /** \brief Sample coordinate of the lower left corner of the map */
  unsigned long int tMin;
  /** \brief Sample coordinate of the upper right corner of the map */
  unsigned long int tMax;
  /** \brief Normalized frequency coordinate (between 0 and 0.5) of the lower left corner of the map */
  MP_Real_t fMin;
  /** \brief Normalized frequency coordinate (between 0 and 0.5) of the upper right corner of the map */
  MP_Real_t fMax;
  /** \brief Minimum amplitude for the mapping of real values to short ints */
  MP_Real_t ampMin;
  /** \brief 10*log10(ampMin+1) */
  MP_Real_t logAmpMin;
  /** \brief Maximum amplitude for the mapping of real values to short ints */
  MP_Real_t ampMax;
  /** \brief 10*log10(ampMax+1) */
  MP_Real_t logAmpMax;
  /** \brief Distance between adjacent pixels in sample coordinates */
  MP_Real_t dt;
  /** \brief Distance between adjacent pixels in normalized frequency coordinates */
  MP_Real_t df;
  /** \brief Distance between adjacent discrete amplitude values */
  MP_Real_t dAmp;
  MP_Real_t dLogAmp;

  /* METHODS */
public :
  /** \brief Constructor that allocates and folds the storage space */
  MP_TF_Map_c( const unsigned long int setNumCols, const unsigned long int setNumRows,
	       const int setNumChans, 
	       const unsigned long int setTMin,    const unsigned long int setTMax,
	       const MP_Real_t setFMin,            const MP_Real_t setFMax,
	       const MP_Real_t setAmpMin,          const MP_Real_t setAmpMax );

  /* Destructor */
  ~MP_TF_Map_c();


  /** \brief Reset the storage to 0 */
  void reset( void );

  /** \brief Reset the amplitude boundaries (BUT: does not redraw the tfmap) */
  void reset_amp( MP_Real_t setAmpMin, MP_Real_t setAmpMax );


  /** \brief Print human readable information about the tfmap to a stream
   *
   * \param  fid A writable stream
   * \return The number of characters written to the stream */
  int info( FILE *fid );


  /** \brief Write to a file as raw data
   * \param fName the file name 
   * \param flagUpsideDown if yes writes the columns upside down 
   * \return nonzero upon success, zero otherwise */
  unsigned long int dump_to_file( const char *fName , char flagUpsideDown );


  /** \brief Convert between real coordinates and discrete coordinates */
  /* Time: */
  unsigned long int time_to_pix( unsigned long int t );
  unsigned long int pix_to_time( unsigned long int n );
  /* Freq: */
  unsigned long int freq_to_pix( MP_Real_t f );
  MP_Real_t pix_to_freq( unsigned long int k );
  /* Amp: */
  MP_Tfmap_t linmap( MP_Real_t amp );
  MP_Tfmap_t logmap( MP_Real_t amp );

  /** \brief Converts real coordinates into pixel coordinates */
  /*void pixel_coordinates( unsigned long int t, MP_Real_t f,
    unsigned long int *n, unsigned long int *k ); */

  /** \brief Converts pixel coordinates into real coordinates */
  /* void tf_coordinates( unsigned long int n, unsigned long int k,
     unsigned long int *t, MP_Real_t *f ); */

};


#endif /* __tfmap_h_ */
