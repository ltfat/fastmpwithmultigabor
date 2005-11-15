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
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __mp_types_h_
#define __mp_types_h_


/********************/
/* Basic data types */
/********************/
typedef double MP_Sample_t; /* Signal data type */
typedef double MP_Real_t;   /* Inner products data type */
typedef char   MP_Bool_t;   /* Boolean data type */

/** \brief Support of an atom, in terms of its first sample and size */
typedef struct {            
  /** \brief sample index of the first sample of the support 
   *
   * starting at zero.
   */
  unsigned long int pos;
  /** \brief number of samples in the support (zero if the support is empty) */
  unsigned long int len;
} MP_Support_t;


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
#define MP_PI  3.14159265358979323846
#define MP_2PI 6.28318530717958647692

/* Minimum significant energy level */
#define MP_ENERGY_EPSILON 1e-10

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
