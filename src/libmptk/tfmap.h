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


#include <stdio.h>


/********************************/
/* Time-Freq Map  class         */
/********************************/
class MP_TF_Map_c {

  /* DATA */
public : 
  /* STORAGE FOR THE MAP */
  /** \brief Number of columns in the time-frequency map */
  int numCols;
  /** \brief Number of rows in the time-frequency map */
  int numRows;
  /** \brief Number of channels in the time-frequency map */
  unsigned char numChans;
  /** \brief Big storage space to store the map */
  MP_Real_t  *storage;
  /** \brief Pointers to each channel of the map */
  MP_Real_t **channel;

  /* CONVERSION BETWEEN PIXEL COORDINATES AND TIME-FREQUENCY COORDINATES */
  /** \brief Sample coordinate of the lower left corner of the map */
  MP_Real_t tMin;
  /** \brief Normalized frequency coordinate (between 0 and 0.5) of the lower left corner of the map */
  MP_Real_t fMin;
  /** \brief Distance between adjacent pixels in sample coordinates */
  MP_Real_t dt;
  /** \brief Distance between adjacent pixels in normalized frequency coordinates */
  MP_Real_t df;

  /* METHODS */
public :
  /** \brief Constructor that allocates and folds the storage space */
  MP_TF_Map_c(int setNumCols,int setNumRows, unsigned char setNumChans, 
	      MP_Real_t setTMin, MP_Real_t setFMin,
	      MP_Real_t setTMax, MP_Real_t setFMax);

  /* Destructor */
  ~MP_TF_Map_c();

  /** \brief Print human readable information about the atom to a stream
   *
   * \param  fid A writable stream
   * \return The number of characters written to the stream */
  int info( FILE *fid );

  /** \brief Write to a file as raw data
   * \param fName the file name 
   * \param flagUpsideDown if yes writes the columns upside down 
   * \return nonzero upon success, zero otherwise */
  char dump_to_float_file( const char *fName , char flagUpsideDown);

  
  /** \brief Converts real coordinates into pixel coordinates */
  void pixel_coordinates(MP_Real_t t,MP_Real_t f, int *n, int *k);
  /** \brief Converts pixel coordinates into real coordinates */
  void tf_coordinates(int n, int k, MP_Real_t *t,MP_Real_t *f);
};


#endif /* __tfmap_h_ */
