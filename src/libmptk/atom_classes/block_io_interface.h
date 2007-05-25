/******************************************************************************/
/*                                                                            */
/*                          block_io_interface.h                              */
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

#ifndef __block_io_interface_h_
#define __block_io_interface_h_


/*****************************************/
/*                                       */
/* CONSTANTS                             */
/*                                       */
/*****************************************/

/* Scanner return values (for flow control) */
#define NULL_EVENT           0
#define ERROR_EVENT          1
#define DICT_OPEN            2
#define COMPLETED_BLOCK      3
#define DICT_CLOSE           4
#define REACHED_END_OF_FILE  5


/*****************************************/
/*                                       */
/* class MP_Scan_Info_c                  */
/*                                       */
/*****************************************/

class MP_Scan_Info_c {

  /********/
  /* DATA */
  /********/

public:
  
  unsigned int blockCount;
  
  char libVersion[MP_MAX_STR_LEN];
  
  char type[MP_MAX_STR_LEN];
  
  unsigned long int windowLen;
  bool windowLenIsSet;
  unsigned long int globWindowLen;
  bool globWindowLenIsSet;
  
  unsigned long int windowShift;
  bool windowShiftIsSet;
  unsigned long int globWindowShift;
  bool globWindowShiftIsSet;
  
  double windowRate;
  bool windowRateIsSet;
  double globWindowRate;
  bool globWindowRateIsSet;

  unsigned long int fftSize;
  bool fftSizeIsSet;
  unsigned long int globFftSize;
  bool globFftSizeIsSet;

  unsigned long int blockOffset;
  bool blockOffsetIsSet;
  unsigned long int globBlockOffset;
  bool globBlockOffsetIsSet;
  
  unsigned char windowType;
  bool windowTypeIsSet;
  unsigned char globWindowType;
  bool globWindowTypeIsSet;

  double windowOption;
  bool windowOptionIsSet;
  double globWindowOption;
  bool globWindowOptionIsSet;
  
  double f0Min;
  bool f0MinIsSet;
  double globF0Min;
  bool globF0MinIsSet;
  
  double f0Max;
  bool f0MaxIsSet;
  double globF0Max;
  bool globF0MaxIsSet;
  
  unsigned int numPartials;
  bool numPartialsIsSet;
  unsigned int globNumPartials;
  bool globNumPartialsIsSet;
  
  char tableFileName[MP_MAX_STR_LEN];
  bool tableFileNameIsSet;
  char globTableFileName[MP_MAX_STR_LEN];
  bool globTableFileNameIsSet;

  unsigned int numFitPoints;
  bool numFitPointsIsSet;
  unsigned int globNumFitPoints;
  bool globNumFitPointsIsSet;
  
  unsigned int numIter;
  bool numIterIsSet;
  unsigned int globNumIter;
  bool globNumIterIsSet;
  
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief Constructor + reset_all */
  MP_Scan_Info_c( );
  /* Destructor */
  ~MP_Scan_Info_c();

  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Reset all the fields of the structure, including the global fields */
  void reset_all( void );

  /** \brief Reset the local fields of the structure */
  void reset( void );
  
  /** \brief Pop a block respecting the fields of the structure, and reset the local fields */
  MP_Block_c* pop_block( MP_Signal_c *signal );
  
};


/*****************************************/
/*                                       */
/* Generic block output function         */
/*                                       */
/*****************************************/

/** \brief Writes a block to a stream in text format
 *
 * \param  fid A writeable stream
 * \param  block A reference to the block to be written
 * \return The number of written characters. 
 *
 * Enclosing XML tags <block type="*"> ... </block> are written for 
 * compatibility with read_block().
 * \sa read_block() MP_Block_c::print_struct()
 */
int write_block( FILE *fid, MP_Block_c *block );


#endif /* __block_io_interface_h_ */
