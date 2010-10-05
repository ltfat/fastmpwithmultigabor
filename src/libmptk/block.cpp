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
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-06-25 18:20:30 +0200 (Mon, 25 Jun 2007) $
 * $Revision: 1077 $
 *
 */

/**********************************************/
/*                                            */
/* blocks.cpp: generic methods for MP_Bloc_c  */
/*                                            */
/**********************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Block_c::init_parameters( const unsigned long int setFilterLen,
                                 const unsigned long int setFilterShift,
                                 const unsigned long int setNumFilters,
                                 const unsigned long int setBlockOffset )
{

  const char* func = "MP_Block_c::init_parameters(...)";

  /* Check the input parameters */
  if ( setFilterLen == 0 )
    { /* filterLen must be at least 1 sample */
      mp_error_msg( func, "filterLen [%lu] is null: filterLen must be at least 1 sample.\n" ,
                    setFilterLen );
      return( 1 );
    }
  if ( setFilterShift == 0 )
    { /* filterShift must be at least 1 sample */
      mp_error_msg( func, "filterShift [%lu] is null: filterShift must be at least 1 sample.\n" ,
                    setFilterShift );
      return( 1 );
    }
  if ( setNumFilters == 0 )
    { /* numFilters must be at least 1 */
      mp_error_msg( func, "numFilters [%lu] is null: numFilters must be at least 1.\n" ,
                    setNumFilters );
      return( 1 );
    }

  /* Set the block parameters */
  filterLen = setFilterLen;
  filterShift = setFilterShift;
  numFilters = setNumFilters;
  blockOffset = setBlockOffset;

  return( 0 );
}


/*************************************************************/
/* Internal allocations of signal-dependent block parameters */
int MP_Block_c::plug_signal( MP_Signal_c *setSignal )
{

  const char *func = "MP_Block_c::plug_signal( signal )";
  unsigned long int nNow;
  unsigned long int i, nel, nBase;
  unsigned long int offset;

  /* Reset any potential previous signal */
  nullify_signal();

  /* Set the new signal and related parameters/allocations */
  if ( setSignal != NULL )
    {

      /* Bind the signal */
      s = setSignal;

      /* Set frame count */
      if ( blockOffset > s->numSamples )
        {
          mp_error_msg( func, "The block offset can't be superior to the signal length.\n");
        }
      else
        {
          numFrames = len2numFrames( s->numSamples - blockOffset, filterLen, filterShift );
        }

      /* Parameters for the research of the max */
      maxIPValue = -1.0;
      maxIPFrameIdx = 0;

      /* maxIPValueInFrame array */
      if ( (maxIPValueInFrame = (MP_Real_t*) malloc( numFrames*sizeof(MP_Real_t) ))
           == NULL )
        {
          mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                        "for the storage of maxIPValueInFrame.\n", numFrames );
          nullify_signal();
          return( 1 );
        }
      else for ( i = 0; i < numFrames; i++ ) *(maxIPValueInFrame+i) = 0.0;

      /* maxIPIdxInFrame array */
      if ( (maxIPIdxInFrame = (unsigned long int*) malloc( numFrames*sizeof(unsigned long int) ))
           == NULL )
        {
          mp_error_msg( func, "Can't allocate an array of [%lu] unsigned long int elements"
                        "for the storage of maxIPIdxInFrame.\n", numFrames );
          nullify_signal();
          return( 1 );
        }
      else memset( maxIPIdxInFrame, 0, numFrames*sizeof(unsigned long int) );

      /*******************************************************/
      /* Management of the tree structure for the max search */
      assert( numFrames != 0 );
      nBase = ((numFrames-1) / MP_BLOCK_FRAMES) + 1;
      if ( numFrames == 1 ) numLevels = 1;
      else numLevels = (unsigned long int)( ceil( log( (double)nBase ) / log( (double)MP_NUM_BRANCHES ) ) ) + 1;

      mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                    "NFRAMES %lu (LOG %g) NBLOCKS %lu NBASE %lu NUMBRANCHES %lu (LOG %g) :"
                    " numLevels=%lu \n",
                    numFrames, log((double)numFrames), (unsigned long int)MP_BLOCK_FRAMES, nBase,
                    (unsigned long int)MP_NUM_BRANCHES, log((double)MP_NUM_BRANCHES),
                    numLevels  );

      /* Compute the total number of elements in the elevators */
      nNow = nBase;
      nel = 0;
      for ( i = 0; i < numLevels; i++ )
        {

          mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "LEVEL %lu/%lu : vecSize=%lu\n",
                        i, numLevels, nNow );

          nel += nNow;
#ifdef MP_NUM_BRANCHES_IS_POW2
          nNow = ( (nNow-1) >> MP_LOG2_NUM_BRANCHES ) + 1;
#else
          nNow = ( (nNow-1) / MP_NUM_BRANCHES ) + 1;
#endif

        }

      /* Allocate the max elevator */
      if ( (elevSpace = (MP_Real_t*) malloc( nel*sizeof(MP_Real_t) )) == NULL )
        {
          mp_error_msg( func, "Can't allocate %lu MP_Real_t values for the max tree.\n",
                        nel );
          nullify_signal();
          return( 1 );
        }
      else for ( i = 0; i < nel; i++ ) elevSpace[i] = -1.0;

      /* Allocate the frame index elevator */
      if ( (elevFrameSpace = (unsigned long int*) malloc( nel*sizeof(unsigned long int) )) == NULL )
        {
          mp_error_msg( func, "Can't allocate %lu unsigned long int values"
                        " for the frame index tree.\n", nel );
          nullify_signal();
          return( 1 );
        }
      else for ( i = 0; i < nel; i++ ) elevFrameSpace[i] = 0;

      /* Allocate the pointers for the levels of the max elevator */
      if ( (elevator = (MP_Real_t**) malloc( numLevels*sizeof(MP_Real_t*) )) == NULL )
        {
          mp_error_msg( func, "Can't allocate %lu pointers for the levels of the max tree.\n",
                        numLevels );
          nullify_signal();
          return( 1 );
        }

      /* Allocate the pointers for the levels of the frame index elevator */
      if ( (elevatorFrame = (unsigned long int**) malloc( numLevels*sizeof(unsigned long int*) )) == NULL )
        {
          mp_error_msg( func, "Can't allocate %lu pointers for the levels of the index tree.\n",
                        numLevels );
          nullify_signal();
          return( 1 );
        }

      /* Fold the elevator spaces */
      elevator[0] = elevSpace;
      elevatorFrame[0] = elevFrameSpace;
      nNow = nBase;
      offset = 0;
      for ( i = 1; i < numLevels; i++ )
        {
          offset += nNow;

          mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,
                        "Folding: at level %lu, offset is %lu/%lu.\n",
                        i, offset, nel );

          elevator[i] = elevSpace + offset;
          elevatorFrame[i] = elevFrameSpace + offset;
#ifdef MP_NUM_BRANCHES_IS_POW2
          nNow = ( (nNow-1) >> MP_LOG2_NUM_BRANCHES ) + 1;
#else
          nNow = ( (nNow-1) / MP_NUM_BRANCHES ) + 1;
#endif

        }

    } /* end if( setSignal != NULL ) */

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Block_c::nullify_signal( void )
{

  s = NULL;
  numFrames = 0;

  if ( maxIPValueInFrame )
    {
      free( maxIPValueInFrame );
      maxIPValueInFrame = NULL;
    }
  if ( maxIPIdxInFrame )
    {
      free( maxIPIdxInFrame );
      maxIPIdxInFrame = NULL;
    }
  if ( elevator )
    {
      free( elevator );
      elevator = NULL;
    }
  if ( elevSpace )
    {
      free( elevSpace );
      elevSpace = NULL;
    }
  if ( elevatorFrame )
    {
      free( elevatorFrame );
      elevatorFrame = NULL;
    }
  if ( elevFrameSpace )
    {
      free( elevFrameSpace );
      elevFrameSpace = NULL;
    }

}


/********************/
/* NULL constructor */
MP_Block_c::MP_Block_c( void )
{

  filterLen = filterShift = numFilters = numFrames = blockOffset = 0;

  maxIPValue = 0.0;
  maxIPFrameIdx = 0;
  maxIPValueInFrame = NULL;
  maxIPIdxInFrame = NULL;

  numLevels = 0;
  elevator = NULL;
  elevSpace = NULL;
  elevatorFrame = NULL;
  elevFrameSpace = NULL;
  parameterMap = NULL;
  
}


/**************/
/* Destructor */
MP_Block_c::~MP_Block_c()
{

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "~MP_Block_c()", "Deleting basic_block...\n" );

  if ( maxIPValueInFrame ) free( maxIPValueInFrame );
  if ( maxIPIdxInFrame )   free( maxIPIdxInFrame );
  if ( elevator )          free( elevator );
  if ( elevSpace )         free( elevSpace );
  if ( elevatorFrame )     free( elevatorFrame );
  if ( elevFrameSpace )    free( elevFrameSpace );
  if (  parameterMap ){
  	 	 while( !(*parameterMap).empty() ) {
    (*parameterMap).erase( (*parameterMap).begin() );
  }
  	 delete( parameterMap );}
  mp_debug_msg( MP_DEBUG_DESTRUCTION, "~MP_Block_c()", "Done.\n" );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/****************************************************************/
/* Finds the max of the cross-channel sum of the inner products */
MP_Real_t MP_Block_c::update_max( const MP_Support_t frameSupport )
{

#ifndef NDEBUG
  const char* func = "MP_Block_c::update_max( frameSupport )";
#endif

  unsigned long int fromFrame, toFrame;
  unsigned long int curFrom, curTo, nMax, level, cur, i, nextMaxReset;
  MP_Real_t *p;
  MP_Real_t val;
  MP_Real_t max;
  unsigned long int maxIdx;

  fromFrame = frameSupport.pos;
  toFrame = fromFrame + frameSupport.len - 1;

#ifndef NDEBUG
  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func, "Before update, elevators say that"
                " max is %g in frame %lu at position %lu.\n",
                elevator[numLevels-1][0], elevatorFrame[numLevels-1][0],
                maxIPIdxInFrame[ elevatorFrame[numLevels-1][0] ] );
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
  for ( cur = curFrom; cur <= curTo; cur++ )
    {
      nextMaxReset += MP_BLOCK_FRAMES;
      if ( nextMaxReset > numFrames ) nextMaxReset = numFrames;
      max = *p++;
      maxIdx = i++;
      for ( ; i < nextMaxReset; i++ )
        {
          val = *p++;
          if ( val > max )
            {
              max = val;
              maxIdx = i;
            }
        }
      /* Register the max at the upper level */
#ifndef NDEBUG
      mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func, "Propagating max %g (frame %lu)"
                    " at level 0, position %lu.\n",
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
  for ( level = 0; level < (numLevels-1); level++ )
    {

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
      for ( cur = curFrom; cur <= curTo; cur++ )
        {
          nextMaxReset += MP_NUM_BRANCHES;
          if ( nextMaxReset > nMax ) nextMaxReset = nMax;
          max = *p++;
          maxIdx = elevatorFrame[level][i];
          i++;
          for ( ; i < nextMaxReset; i++ )
            {
              val = *p++;
              if ( val > max )
                {
                  max = val;
                  maxIdx = elevatorFrame[level][i];
                }
            }
          /* Register the max at the upper level */
#ifndef NDEBUG
          mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func, "Propagating max %g"
                        " (frame %lu) at level %lu, position %lu.\n",
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
#ifndef NDEBUG
  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func, "After update, elevators say that max is %g"
                " in frame %lu at position %lu.\n",
                maxIPValue, maxIPFrameIdx, maxIPIdxInFrame[maxIPFrameIdx] );
#endif

  return( maxIPValue );
}


/****************************************/
/* Partial update of the inner products */
MP_Support_t MP_Block_c::update_ip( const MP_Support_t *touch )
{

#ifndef NDEBUG
  const char* func = "MP_Block_c::update_ip( touch )";
#endif

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;

  int chanIdx;
  unsigned long int frameIdx;
  MP_Real_t maxCorr;
  unsigned long int maxFilterIdx;

  MP_Support_t frameSupport;

  assert( s != NULL );

  /*---------------------------*/
  /* Computes the interval [fromFrame,toFrame] where
     the frames need an IP+maxCorr update
    
     WARNING: toFrame is INCLUDED. See the LOOP below.

     THIS IS CRITICAL CODE. MODIFY WITH CARE.
  */

  /* -- If touch is NULL, we ask for a full update: */
  if ( touch == NULL )
    {
      fromFrame = 0;
      toFrame   = numFrames - 1;
    }
  /* -- If touch is not NULL, we specify a touched support: */
  else
    {
      /* Initialize fromFrame and toFrame using the support on channel 0 */
      if (blockOffset>touch[0].pos)
        {
          fromSample = 0;
        }
      else
        {
          fromSample = touch[0].pos-blockOffset;
        }
      fromFrame = len2numFrames( fromSample, filterLen, filterShift );

      toSample = ( fromSample + touch[0].len - 1 );
      toFrame  = toSample / filterShift ;
      if ( toFrame >= numFrames )  toFrame = numFrames - 1;
      /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
      for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ )
        {
          if (blockOffset>touch[chanIdx].pos)
            {
              fromSample = 0;
            }
          else
            {
              fromSample = touch[chanIdx].pos-blockOffset;
            }
          tmpFromFrame = len2numFrames( fromSample, filterLen, filterShift );
          if ( tmpFromFrame < fromFrame ) fromFrame = tmpFromFrame;

          toSample = ( fromSample + touch[chanIdx].len - 1 );
          tmpToFrame  = toSample / filterShift ;
          if ( tmpToFrame >= numFrames )  tmpToFrame = numFrames - 1;
          if ( tmpToFrame > toFrame ) toFrame = tmpToFrame;
        }
    }
  /*---------------------------*/


#ifndef NDEBUG
  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,"Updating frames from %lu to %lu / %lu.\n",
                fromFrame, toFrame, numFrames );
#endif

  /*---------------------------*/
  /* LOOP : Browse the frames which need an update. */
  for ( frameIdx = fromFrame; frameIdx <= toFrame; frameIdx++ )
    {

      update_frame(frameIdx, &maxCorr, &maxFilterIdx);

      /* Register the maxCorr value for the current frame */
      maxIPValueInFrame[frameIdx] = maxCorr;
      maxIPIdxInFrame[frameIdx]   = maxFilterIdx;

#ifndef NDEBUG
      mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,"In frame %lu, maxcorr is"
                    " %g at position %lu.\n",
                    frameIdx, maxCorr, maxFilterIdx );
#endif

    } /* end foreach frame */


  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = toFrame - fromFrame + 1;

  return( frameSupport );
}

MP_Support_t MP_Block_c::update_ip( const MP_Support_t *touch, GP_Pos_Book_c* book )
{

#ifndef NDEBUG
  const char* func = "MP_Block_c::update_ip( touch, book )";
#endif

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;

  int chanIdx;
  unsigned long int frameIdx;
  MP_Real_t maxCorr;
  unsigned long int maxFilterIdx;

  MP_Support_t frameSupport;
  unsigned long int pos=1;
  GP_Param_Book_c* subBook;
  
  if (!book)
    return update_ip(touch);

  assert( s != NULL );

  /*---------------------------*/
  /* Computes the interval [fromFrame,toFrame] where
     the frames need an IP+maxCorr update
    
     WARNING: toFrame is INCLUDED. See the LOOP below.

     THIS IS CRITICAL CODE. MODIFY WITH CARE.
  */

  /* -- If touch is NULL, we ask for a full update: */
  if ( touch == NULL )
    {
      fromFrame = 0;
      toFrame   = numFrames - 1;
    }
  /* -- If touch is not NULL, we specify a touched support: */
  else
    {
      /* Initialize fromFrame and toFrame using the support on channel 0 */
      if (blockOffset>touch[0].pos)
        {
          fromSample = 0;
        }
      else
        {
          fromSample = touch[0].pos-blockOffset;
        }
      fromFrame = len2numFrames( fromSample, filterLen, filterShift );

      toSample = ( fromSample + touch[0].len - 1 );
      toFrame  = toSample / filterShift ;
      if ( toFrame >= numFrames )  toFrame = numFrames - 1;
      /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
      for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ )
        {
          if (blockOffset>touch[chanIdx].pos)
            {
              fromSample = 0;
            }
          else
            {
              fromSample = touch[chanIdx].pos-blockOffset;
            }
          tmpFromFrame = len2numFrames( fromSample, filterLen, filterShift );
          if ( tmpFromFrame < fromFrame ) fromFrame = tmpFromFrame;

          toSample = ( fromSample + touch[chanIdx].len - 1 );
          tmpToFrame  = toSample / filterShift ;
          if ( tmpToFrame >= numFrames )  tmpToFrame = numFrames - 1;
          if ( tmpToFrame > toFrame ) toFrame = tmpToFrame;
        }
    }
  /*---------------------------*/


#ifndef NDEBUG
  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,"Updating frames from %lu to %lu / %lu.\n",
                fromFrame, toFrame, numFrames );
#endif

  /*---------------------------*/
  /* LOOP : Browse the frames which need an update. */
  for ( frameIdx = fromFrame; frameIdx <= toFrame; frameIdx++ )
    {

      pos = frameIdx*filterShift + blockOffset;
      subBook = book->get_sub_book(pos);
      update_frame(frameIdx, &maxCorr, &maxFilterIdx, subBook);

      /* Register the maxCorr value for the current frame */
      maxIPValueInFrame[frameIdx] = maxCorr;
      maxIPIdxInFrame[frameIdx]   = maxFilterIdx;

#ifndef NDEBUG
      mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,"In frame %lu, maxcorr is"
                    " %g at position %lu.\n",
                    frameIdx, maxCorr, maxFilterIdx );
#endif

    } /* end foreach frame */


  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = toFrame - fromFrame + 1;

  return( frameSupport );
}

/********************************/
/* Number of atoms in the block */
unsigned long int MP_Block_c::num_atoms(void)
{
  return( numFilters*numFrames );
}

/****************************************/
/* get Paramater map defining the block */
 map<string, string, mp_ltstring>* MP_Block_c::get_block_parameters_map()
{
  return ( parameterMap );
}

void MP_Block_c::update_frame( unsigned long int frameIdx, 
                                       MP_Real_t *maxCorr, 
                                       unsigned long int *maxFilterIdx,
                                       GP_Param_Book_c* touchBook){
   update_frame(frameIdx, maxCorr, maxFilterIdx);
}

/**********************************************/
/* Substract / add a monochannel atom from / to multichannel signals with amplitude proportional to its correlation
 * with the residual. */
void MP_Block_c::substract_add_grad( GP_Param_Book_c& book, MP_Real_t step, 
                                     MP_Signal_c *sigSub, MP_Signal_c *sigAdd ) {
    GP_Param_Book_Iterator_c iter;
    for (iter = book.begin(); iter != book.end(); ++iter)
        iter->substract_add_grad(step, sigSub, sigAdd);
}
