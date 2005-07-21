/******************************************************************************/
/*                                                                            */
/*                                 block.cpp                                  */
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
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/**********************************************/
/*                                            */
/* blocks.cpp: generic methods for MP_Bloc_c  */
/*                                            */
/**********************************************/

#include "mptk.h"
#include "system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Specific constructor */
MP_Block_c::MP_Block_c( MP_Signal_c *setSignal,
			const unsigned long int setFilterLen,
			const unsigned long int setFilterShift,
			const unsigned long int setNumFilters ) {

  if ( alloc_block( setSignal, setFilterLen, setFilterShift, setNumFilters ) ) {
    fprintf( stderr, "mplib warning -- MP_Block_c() - Memory allocation failed."
	     " Returning an invalid block with NULL pointers.\n" );
  }

}


/**************/
/* Destructor */
MP_Block_c::~MP_Block_c() {
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Block_c() - Deleting basic_block...");
#endif
  if ( maxIPValueInFrame ) free( maxIPValueInFrame );
  if ( maxIPIdxInFrame )   free( maxIPIdxInFrame );
  if ( elevator )          free( elevator );
  if ( elevSpace )         free( elevSpace );
  if ( elevatorFrame )     free( elevatorFrame );
  if ( elevFrameSpace )    free( elevFrameSpace );
#ifndef NDEBUG
  fprintf( stderr, "Done\n");
#endif
}


/***************************/
/* OTHER METHODS           */
/***************************/

/************************/
/* Internal allocations */
int MP_Block_c::alloc_block( MP_Signal_c *setSignal,
			  const unsigned long int setFilterLen,
			  const unsigned long int setFilterShift,
			  const unsigned long int setNumFilters ) {

  unsigned long int nNow;
  unsigned long int i, nel, nBase;
  unsigned long int offset;

  assert( setSignal != NULL );

  /* Init the pointers */
  maxIPValueInFrame = NULL;
  maxIPIdxInFrame = NULL;
  elevator = NULL;
  elevSpace = NULL;
  elevatorFrame = NULL;
  elevFrameSpace = NULL;

  /* Bind the signal */
  s = setSignal;

  /* Set frame counts */
  filterLen = setFilterLen;
  filterShift = setFilterShift;
  numFrames = len2numFrames( s->numSamples, filterLen, filterShift );
  numFilters = setNumFilters;

  /* Parameters for the research of the max */
  maxIPValue = -1.0;
  maxIPFrameIdx = 0;
  maxAtomIdx = 0;

  /* maxIPValueInFrame array */
  if ( (maxIPValueInFrame = (MP_Real_t*) malloc( numFrames*sizeof(MP_Real_t) ))
       == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block() - Can't allocate an array of [%lu] MP_Real_t elements"
	     "for the storage of maxIPValueInFrame.\n", numFrames );
    return( 1 );
  }
  else for ( i = 0; i < numFrames; i++ ) *(maxIPValueInFrame+i) = 0.0;

  /* maxIPIdxInFrame array */
  if ( (maxIPIdxInFrame = (unsigned long int*) malloc( numFrames*sizeof(unsigned long int) ))
       == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block() - Can't allocate an array of [%lu] unsigned long int elements"
	     "for the storage of maxIPIdxInFrame.\n", numFrames );
    return( 1 );
  }
  else memset( maxIPIdxInFrame, 0, numFrames*sizeof(unsigned long int) );

  /*******************************************************/
  /* Management of the tree structure for the max search */
  assert( numFrames != 0 );
  nBase = ((numFrames-1) / MP_BLOCK_FRAMES) + 1;
  if ( numFrames == 1 ) numLevels = 1;
  else numLevels = (unsigned long int)( ceil( log( (double)nBase ) / log( (double)MP_NUM_BRANCHES ) ) ) + 1;
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Block_c::alloc_block() -"
	   " NFRAMES %lu (LOG %g) NBLOCKS %lu NBASE %lu NUMBRANCHES %lu (LOG %g) : numLevels=%lu \n",
	   numFrames, log((double)numFrames), (unsigned long int)MP_BLOCK_FRAMES, nBase,
	   (unsigned long int)MP_NUM_BRANCHES, log(MP_NUM_BRANCHES), numLevels );
#endif

  /* Compute the total number of elements in the elevators */
  nNow = nBase;
  nel = 0;
  for ( i = 0; i < numLevels; i++ ) {
#ifndef NDEBUG
    fprintf( stderr, "mplib DEBUG -- MP_Block_c::alloc_block() - LEVEL %lu/%lu : vecSize=%lu\n",
	     i, numLevels, nNow );
    fflush( stderr );
#endif
    nel += nNow;
#ifdef MP_NUM_BRANCHES_IS_POW2
    nNow = ( (nNow-1) >> MP_LOG2_NUM_BRANCHES ) + 1;
#else
    nNow = ( (nNow-1) / MP_NUM_BRANCHES ) + 1;
#endif
  }
  
  /* Allocate the max elevator */
  if ( (elevSpace = (MP_Real_t*) malloc( nel*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block() - Can't allocate %lu MP_Real_t values for the max tree.\n",
	     nel );
    free( maxIPIdxInFrame ); maxIPIdxInFrame = NULL;
    return( 1 );
  }
  else for ( i = 0; i < nel; i++ ) elevSpace[i] = -1.0;
  /* Allocate the frame index elevator */
  if ( (elevFrameSpace = (unsigned long int*) malloc( nel*sizeof(unsigned long int) )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block() - Can't allocate %lu unsigned long int values"
	     " for the frame index tree.\n", nel );
    free( maxIPIdxInFrame ); maxIPIdxInFrame = NULL;
    free( elevSpace );        elevSpace = NULL;
    return( 1 );
  }
  else for ( i = 0; i < nel; i++ ) elevFrameSpace[i] = 0;
  /* Allocate the pointers for the levels of the max elevator */
  if ( (elevator = (MP_Real_t**) malloc( numLevels*sizeof(MP_Real_t*) )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block()- Can't allocate %lu pointers for the levels of the max tree.\n",
	     numLevels );    
    free( maxIPIdxInFrame ); maxIPIdxInFrame = NULL;
    free( elevSpace );        elevSpace = NULL;
    free( elevFrameSpace );   elevFrameSpace = NULL;
    return( 1 );
  }
  /* Allocate the pointers for the levels of the frame index elevator */
  if ( (elevatorFrame = (unsigned long int**) malloc( numLevels*sizeof(unsigned long int*) )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Block_c::alloc_block() - Can't allocate %lu pointers for the levels of the index tree.\n",
	     numLevels );    
    free( maxIPIdxInFrame ); maxIPIdxInFrame = NULL;
    free( elevSpace );       elevSpace = NULL;
    free( elevFrameSpace );  elevFrameSpace = NULL;
    free( elevator );        elevator = NULL;
    return( 1 );
  }

  /* Fold the elevator spaces */
  elevator[0] = elevSpace;
  elevatorFrame[0] = elevFrameSpace;
  nNow = nBase;
  offset = 0;
  for ( i = 1; i < numLevels; i++ ) {
    offset += nNow;
#ifndef NDEBUG
    fprintf( stderr, "mplib DEBUG -- MP_Block_c::alloc_block() - Folding: at level %lu, offset is %lu/%lu.\n",
	     i, offset, nel );
    fflush( stderr );
#endif
    elevator[i] = elevSpace + offset;
    elevatorFrame[i] = elevFrameSpace + offset;
#ifdef MP_NUM_BRANCHES_IS_POW2
    nNow = ( (nNow-1) >> MP_LOG2_NUM_BRANCHES ) + 1;
#else
    nNow = ( (nNow-1) / MP_NUM_BRANCHES ) + 1;
#endif
  }

  return( 0 );
}


/****************************************************************/
/* Finds the max of the cross-channel sum of the inner products */
MP_Real_t MP_Block_c::update_max( const MP_Support_t frameSupport ) {

  unsigned long int fromFrame, toFrame;
  unsigned long int curFrom, curTo, nMax, level, cur, i, nextMaxReset;
  MP_Real_t *p;
  MP_Real_t val;
  MP_Real_t max;
  unsigned long int maxIdx;

  fromFrame = frameSupport.pos;
  toFrame = fromFrame + frameSupport.len - 1;

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- update_max() - before update, elevators say that max is %g in frame %lu at position %lu.\n",
	   elevator[numLevels-1][0], elevatorFrame[numLevels-1][0], maxIPIdxInFrame[ elevatorFrame[numLevels-1][0] ] );
#endif

  /* First pass at level 0 (without a registration of the max frame number) */
#ifdef MP_BLOCK_FRAMES_IS_POW2
  curFrom = ( fromFrame >> MP_LOG2_BLOCK_FRAMES );
  curTo   = ( toFrame   >> MP_LOG2_BLOCK_FRAMES );
  i       = ( curFrom   << MP_LOG2_BLOCK_FRAMES );
#else
  curFrom = ( fromFrame / MP_BLOCK_FRAMES );
  curTo   = ( toFrame   / MP_BLOCK_FRAMES );
  i       = ( curFrom   * MP_BLOCK_FRAMES ); /* Note: this is not equal to fromFrame
						because of the integer divisions */
#endif
  p = maxIPValueInFrame + i;
  nextMaxReset = i;
  for ( cur = curFrom; cur <= curTo; cur++ ) {
    nextMaxReset += MP_BLOCK_FRAMES;
    if ( nextMaxReset > numFrames ) nextMaxReset = numFrames;
    max = *p++;
    maxIdx = i++;
    for ( ; i < nextMaxReset; i++ ) {
      val = *p++;
      if ( val > max ) { max = val; maxIdx = i; }
    }
    /* Register the max at the upper level */
#ifndef NDEBUG
    fprintf( stderr, "mplib DEBUG -- update_max() - Propagating max %g (frame %lu) at level 0, position %lu.\n",
	     max, maxIdx, cur );
#endif
    elevator[0][cur] = max;
    elevatorFrame[0][cur] = maxIdx;
  }
  
#ifdef MP_BLOCK_FRAMES_IS_POW2
  nMax    = ( (numFrames-1) >> MP_LOG2_BLOCK_FRAMES ) + 1;  
#else
  nMax    = ( (numFrames-1) / MP_BLOCK_FRAMES ) + 1;  
#endif

  /* Subsequent passes for levels above 0 */
  for ( level = 0; level < (numLevels-1); level++ ) {
    
#ifdef MP_NUM_BRANCHES_IS_POW2
    curFrom = ( curFrom >> MP_LOG2_NUM_BRANCHES );
    curTo   = ( curTo   >> MP_LOG2_NUM_BRANCHES );
    i       = ( curFrom << MP_LOG2_NUM_BRANCHES );
#else
    curFrom = ( curFrom / MP_NUM_BRANCHES );
    curTo   = ( curTo   / MP_NUM_BRANCHES );
    i       = ( curFrom * MP_NUM_BRANCHES ); /* Note: this is not equal to curFrom
						because of the integer divisions */
#endif
    p = elevator[level] + i;
    nextMaxReset = i;
    for ( cur = curFrom; cur <= curTo; cur++ ) {
      nextMaxReset += MP_NUM_BRANCHES;
      if ( nextMaxReset > nMax ) nextMaxReset = nMax;
      max = *p++;
      maxIdx = elevatorFrame[level][i]; i++;
      for ( ; i < nextMaxReset; i++ ) {
	val = *p++;
	if ( val > max ) { max = val; maxIdx = elevatorFrame[level][i]; }
      }
      /* Register the max at the upper level */
#ifndef NDEBUG
      fprintf( stderr, "mplib DEBUG -- update_max() - Propagating max %g (frame %lu) at level %lu, position %lu.\n",
	       max, maxIdx, level+1, cur );
#endif
      elevator[level+1][cur] = max;
      elevatorFrame[level+1][cur] = maxIdx;
    }
    
#ifdef MP_NUM_BRANCHES_IS_POW2
    nMax = ( (nMax-1) >> MP_LOG2_NUM_BRANCHES ) + 1;
#else
    nMax = ( (nMax-1) / MP_NUM_BRANCHES ) + 1;
#endif
    
  } /* end for level */

  /* Dereference the top values */
  maxIPValue    = elevator[numLevels-1][0];
  maxIPFrameIdx = elevatorFrame[numLevels-1][0];
  maxAtomIdx = maxIPFrameIdx*numFilters + maxIPIdxInFrame[maxIPFrameIdx];

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- update_max() - after update, elevators say that max is %g"
	   " in frame %lu at position %lu. (maxAtomIdx=%lu)\n",
	   maxIPValue, maxIPFrameIdx, maxIPIdxInFrame[maxIPFrameIdx], maxAtomIdx );
#endif

  return( maxIPValue );
}


/********************************/
/* Number of atoms in the block */
unsigned long int MP_Block_c::size(void) {
  return( numFilters*numFrames );
}
