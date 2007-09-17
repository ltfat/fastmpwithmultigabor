/******************************************************************************/
/*                                                                            */
/*                                mp_types.h                                  */
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

/***********************/
/*                     */
/* CONSTANTS AND TYPES */
/*                     */
/***********************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-06-25 18:20:30 +0200 (Mon, 25 Jun 2007) $
 * $Revision: 1077 $
 *
 */


#ifndef __mp_types_h_
#define __mp_types_h_

#include <stdlib.h>
#include <string>

using namespace std;

/********************/
/* Basic data types */
/********************/
//typedef double MP_Sample_t; /* Signal data type */
typedef double MP_Real_t;   /* Inner products data type */
typedef char   MP_Bool_t;         /* Boolean data type */
typedef char   MP_Bool_On_Disk_t; /* Disk version of Boolean data type */
typedef unsigned short int MP_Chan_t; /* Type of all the channel indexes */

//typedef unsigned short int MP_Tfmap_t;  /* Type of the Tfmap data */
typedef float  MP_Tfmap_t;  /* Type of the Tfmap data */
#define TFMAP_NUM_DISCRETE_LEVELS 65536 /* Number of levels when discretizing
					   real values in a tfmap */

/** \brief Support of an atom, in terms of its first sample and size */
typedef struct {            
  /** \brief sample index of the first sample of the support (starting at zero)
   */
  unsigned long int pos;
  /** \brief number of samples in the support (zero if the support is empty) */
  unsigned long int len;
} MP_Support_t;

 /** \brief Struct use compare key in the hash map for block parameter */ 
typedef struct 
{
  bool operator()(string s1, string s2) const
  {
    return strcmp(s1.c_str(), s2.c_str()) < 0;
  }
} mp_ltstring;

/*************/
/* Constants */
/*************/

/* Booleans */
#define MP_TRUE  (1==1)
#define MP_FALSE (1==0)

/* File reading modes */
#define MP_TEXT   1
#define MP_BINARY 2

/* Max line length when doing text i/o */
#define MP_MAX_STR_LEN 1024

/* Useful math constants */
#define MP_PI    3.14159265358979323846
#define MP_2PI   6.28318530717958647692

#define MP_PI_SQ     9.8696044010893579923049401
#define MP_INV_PI_SQ 0.1013211836423377754101693
/* Note: the two above constants have been computed with doubles
   and copied from a printf( "%.25f", x ) display. Their accuracy
   could probably be enhanced.
*/

/* Minimum significant energy level */
#define MP_ENERGY_EPSILON 1e-10

/*****/
/* Bounds of integer types */

const unsigned short int MP_MAX_BITS_SHORT_INT = 8*sizeof(short int) - 1; /* 15 */
const short int MP_MIN_SHORT_INT = (short int)   ( 1 << MP_MAX_BITS_SHORT_INT ); /*-32768*/
const short int MP_MAX_SHORT_INT = (short int) ( ( 1 << MP_MAX_BITS_SHORT_INT ) - 1); /*32767*/

const unsigned short int MP_MAX_BITS_UNSIGNED_SHORT_INT = 8*sizeof(unsigned short int); /* 16 */
const unsigned short int MP_MIN_UNSIGNED_SHORT_INT = 0;
const unsigned short int MP_MAX_UNSIGNED_SHORT_INT = (unsigned short int) (-1); /* 65535 */

const unsigned short int MP_MAX_BITS_LONG_INT = 8*sizeof(long int) - 1; /* 31 */
const long int MP_MIN_LONG_INT = (long int)   ( (long)(1) << MP_MAX_BITS_LONG_INT ); /*-2147483648*/
const long int MP_MAX_LONG_INT = (long int) ( ( (long)(1) << MP_MAX_BITS_LONG_INT ) - (long)(1) ); /* 2147483647 */

const unsigned short int MP_MAX_BITS_UNSIGNED_LONG_INT = 8*sizeof(unsigned long int); /* 32 */
const unsigned long int MP_MIN_UNSIGNED_LONG_INT = 0;
const unsigned long int MP_MAX_UNSIGNED_LONG_INT = (unsigned long int) (-1); /* 4294967295 */

const unsigned short int MP_MAX_BITS_SIZE_T = 8*sizeof(size_t);
const size_t MP_MIN_SIZE_T = 0;
const size_t MP_MAX_SIZE_T = (size_t) (-1);

/*****/
/* Constants for the dimensioning of the max search tree: */

#define MP_BLOCK_FRAMES_IS_POW2

#define MP_BLOCK_FRAMES 32
#define MP_LOG2_BLOCK_FRAMES 5


#define MP_NUM_BRANCHES_IS_POW2

#define MP_NUM_BRANCHES 4
#define MP_LOG2_NUM_BRANCHES 2

//#define MP_NUM_BRANCHES 8
//#define MP_LOG2_NUM_BRANCHES 3

//#define MP_NUM_BRANCHES 16
//#define MP_LOG2_NUM_BRANCHES 4

//#define MP_NUM_BRANCHES 32
//#define MP_LOG2_NUM_BRANCHES 5

//#define MP_NUM_BRANCHES 64
//#define MP_LOG2_NUM_BRANCHES 6

//#define MP_NUM_BRANCHES 128
//#define MP_LOG2_NUM_BRANCHES 7

//#define MP_NUM_BRANCHES 256
//#define MP_LOG2_NUM_BRANCHES 8

//#define MP_NUM_BRANCHES 512
//#define MP_LOG2_NUM_BRANCHES 9

//#define MP_NUM_BRANCHES 4096
//#define MP_LOG2_NUM_BRANCHES 12

#endif /* __mp_types_h_ */
