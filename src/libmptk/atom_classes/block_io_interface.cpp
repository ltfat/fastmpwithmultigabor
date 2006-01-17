/******************************************************************************/
/*                                                                            */
/*                         block_io_interface.cpp                             */
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


#include "mptk.h"
#include "mp_system.h"

#include <dsp_windows.h>


/* Constructor */
MP_Scan_Info_c::MP_Scan_Info_c() {
#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- constructing MP_Scan_Info...\n");
#endif

  reset_all();

#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- Done.\n");
#endif
}


/* Destructor */
MP_Scan_Info_c::~MP_Scan_Info_c() {
#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- deleting MP_Scan_Info.\n");
#endif
}


/* Resetting the local variables */
void MP_Scan_Info_c::reset( void ) {

  strcpy( type, "" );
  
  windowLen = 0;
  windowLenIsSet = false;
  
  windowShift = 0;
  windowShiftIsSet = false;
  
  windowRate = 0.0;
  windowRateIsSet = false;
  
  fftSize = 0;
  fftSizeIsSet = false;
  
  windowType = 0;
  windowTypeIsSet = false;

  windowOption = 0.0;
  windowOptionIsSet = false;
  
  f0Min = 0;
  f0MinIsSet = false;
  
  f0Max = 0;
  f0MaxIsSet = false;
  
  numPartials = 0;
  numPartialsIsSet = false;
  
  numFitPoints = 0;
  numFitPointsIsSet = false;
  
  numIter = 0;
  numIterIsSet = false;
  
}

/* Resetting the global variables */
void MP_Scan_Info_c::reset_all( void ) {
  
  reset();
  
  blockCount = 0;

  strcpy( libVersion, VERSION );
  
  globWindowLen = 0;
  globWindowLenIsSet = false;
  
  globWindowShift = 0;
  globWindowShiftIsSet = false;
  
  globWindowRate = 0.0;
  globWindowRateIsSet = false;
  
  globFftSize = 0;
  globFftSizeIsSet = false;
  
  globWindowType = 0;
  globWindowTypeIsSet = false;

  globWindowOption = 0.0;
  globWindowOptionIsSet = false;
  
  globF0Min = 0;
  globF0MinIsSet = false;
  
  globF0Max = 0;
  globF0MaxIsSet = false;
  
  globNumPartials = 0;
  globNumPartialsIsSet = false;
  
  globNumFitPoints = 0;
  globNumFitPointsIsSet = false;
  
  globNumIter = 0;
  globNumIterIsSet = false;
  
}


/* Pop a block */
MP_Block_c* MP_Scan_Info_c::pop_block( MP_Signal_c *signal ) {

  const char* func = "MP_Scan_Info_c::pop_block( signal )";
  MP_Block_c *block;
  unsigned long int fftRealSize = 0;
  unsigned long int maxFundFreqIdx = 0;
  unsigned long int minFundFreqIdx = 0;


  /*******************************************************/
  /* CHECK the block parameters and apply some potential */
  /* defaulting strategies.                              */
  /*******************************************************/

  /* - Dirac block: */
  if ( !strcmp(type,"dirac") ) {
    /* NOP */
  }
  /* - Parameters common to the gabor block and the harmonic block: */
  else if ( (!strcmp(type,"gabor")) || (!strcmp(type,"harmonic")) || (!strcmp(type,"chirp")) ) {
    /* - windowLen: */
    if (!windowLenIsSet) {
      if (globWindowLenIsSet) {
	windowLen = globWindowLen;
	windowLenIsSet = true;
      }
      else {
	fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block) has no windowLen."
		 " Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    /* - windowShift: */
    if (!windowShiftIsSet) {
      if (windowRateIsSet) {
	windowShift = (unsigned long int)( (double)(windowLen)*windowRate + 0.5 ); /* == round(windowLen*windowRate) */
	windowShift = ( windowShift > 1 ? windowShift : 1 ); /* windowShift has to be 1 or more */
	windowShiftIsSet = true;
      }
      else if (globWindowShiftIsSet) {
	windowShift = globWindowShift;
	windowShiftIsSet = true;
      }
      else if (globWindowRateIsSet) {
	windowShift = (unsigned long int)( (double)(windowLen)*globWindowRate + 0.5 ); /* == round(windowLen*globWindowRate) */
	windowShift = ( windowShift > 1 ? windowShift : 1 ); /* windowShift has to be 1 or more */
	windowShiftIsSet = true;
      }
      else {
	fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block) has no windowShift or windowRate."
		 " Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    /* - fftSize: */
    if (!fftSizeIsSet) {
      if (globFftSizeIsSet) {
	fftSize = globFftSize;
	fftSizeIsSet = true;
      }
      else if (windowLenIsSet) {
	if ( is_odd(windowLen) ) fftSize = windowLen + 1;
	else                     fftSize = windowLen;
	fftSizeIsSet = true;
      }
      else {
	fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block) has no fftSize and no windowLen."
		 " Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    /* Check fftSize validity */
    if ( is_odd(fftSize) ) { /* If fftSize is odd (fftSize has to be even) */
      fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block) has an odd fftSize:"
	       " fftSize must be even. Returning a NULL block.\n" , blockCount );
      reset();
      return( NULL );
    }
    if ( is_odd(windowLen) ) { /* If windowLEn is odd, fftSize must be >= windowLen+1 */
      if ( fftSize < (windowLen+1) ) {
	fprintf( stderr, "mplib warning -- pop_block() - In gabor or harmonic block (%u-th block): fftSize must be bigger"
		 " than windowLen+1 when windowLen is odd. Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    else { /* If windowLEn is even, fftSize must be >= windowLen */
      if ( fftSize < windowLen ) {
	fprintf( stderr, "mplib warning -- pop_block() - In gabor or harmonic block (%u-th block): fftSize must be bigger"
		 " than windowLen when windowLen is even. Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    /* Turn fftSize into fftRealSize */
    fftRealSize = (fftSize >> 1) + 1;

    /* - windowType & windowOption: */
    if (!windowTypeIsSet) {

      if (globWindowTypeIsSet) {

	windowType = globWindowType;
	windowTypeIsSet = true;
	windowOption = globWindowOption;
	windowOptionIsSet = globWindowOptionIsSet;

      }
      else {
	fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block)"
		 " has no window specification. Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
      }
    }
    if ( !(window_type_is_ok(windowType)) ) {
	fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block)"
		 " has an invalid window type. Returning a NULL block.\n" , blockCount );
	reset();
	return( NULL );
    }

    if ( window_needs_option(windowType) && (!windowOptionIsSet) ) {
      fprintf( stderr, "mplib warning -- pop_block() - Gabor or harmonic block (%u-th block)"
	       " requires a window option (the opt=\"\" attribute is probably missing"
	       " in the relevant <window> tag). Returning a NULL block.\n" , blockCount );
      reset();
      return( NULL );
    }

    /****************************************************/
    /* - Additional parameters for the harmonic block: */
    if ( !strcmp(type,"harmonic") ) {

      /* - f0Min: */
      if ( !f0MinIsSet ) {
	if ( globF0MinIsSet ) {
	  f0Min = globF0Min;
	  f0MinIsSet = true;
	}
	else { /* Default to just above the DC frequency */
	  f0Min = ( (double)(signal->sampleRate) / (double)(fftSize) );
	  f0MinIsSet = true;
	}
      }
      /* check for neg values */
      if ( f0Min < 0 ) {
	fprintf( stderr, "mplib warning -- pop_block() - Harmonic block (%u-th block) has a negative f0Min [%.2f]:"
		 " f0Min must be a positive frequency value. Returning a NULL block.\n" , blockCount, f0Min );
	reset();
	return( NULL );
      }
      /* Check for going over the Nyquist frequency */
      if ( f0Min > ( (double)(signal->sampleRate) / 2.0 ) - ( (double)(signal->sampleRate)/(double)(fftSize) ) ) {
	fprintf( stderr, "mplib warning -- pop_block() - In harmonic block (%u-th block):"
		 " f0Min [%.2f] has been reduced to the signal's Nyquist frequency [%.2f].\n" ,
		 blockCount, f0Min, ( (double)(signal->sampleRate) / 2.0 ) );
	f0Min = ( (double)(signal->sampleRate) / 2.0 ) - ( (double)(signal->sampleRate)/(double)(fftSize) );
      }
      /* Turn into fft bins */
      minFundFreqIdx = (unsigned long int)( floor( f0Min / ((double)(signal->sampleRate) / (double)(fftSize)) ) );
      if ( minFundFreqIdx == 0 ) {
	fprintf( stderr, "mplib warning -- pop_block() - Harmonic block (%u-th block) has"
		 " f0Min [%.2f]Hz falling into the DC discrete frequency band:"
		 " for this block, f0Min must higher than [%.2f]Hz. Returning a NULL block.\n" ,
		 blockCount, f0Min, ( (double)(signal->sampleRate) / (double)(fftSize) ) );
	reset();
	return( NULL );
      }

      /* - f0Max: */
      if ( !f0MaxIsSet ) {
	if ( globF0MaxIsSet ) {
	  f0Max = globF0Max;
	  f0MaxIsSet = true;
	}
	else { /* Default to the Nyquist frequency */
	  f0Max = ( (double)(signal->sampleRate) / 2.0 );
	  f0MaxIsSet = true;
	}
      }
      /* Check for going over the Nyquist frequency */
      if ( f0Max > ( (double)(signal->sampleRate) / 2.0 ) ) {
	fprintf( stderr, "mplib warning -- pop_block() - In harmonic block (%u-th block):"
		 " f0Max [%.2f] has been reduced to the signal's Nyquist frequency [%.2f].\n",
		 blockCount, f0Max, ( (double)(signal->sampleRate) / 2.0 ) );
	f0Max = ( (double)(signal->sampleRate) / 2.0 );
      }
      /* Check for the position viz. f0Min */
      if ( f0Max <= f0Min ) {
	fprintf( stderr, "mplib warning -- pop_block() - In harmonic block (%u-th block):"
		 " f0Max [%.2f] is smaller than f0Min [%.2f]."
		 " f0Max must be a positive frequency value bigger than f0Min. Returning a NULL block.\n" ,
		 blockCount, f0Max, f0Min );
	reset();
	return( NULL );
      }
      /* Turn into fft bins */
      maxFundFreqIdx = (unsigned long int)( floor( f0Max / ((double)(signal->sampleRate) / (double)(fftSize)) ) );

      /* - numPartials: */
      if ( !numPartialsIsSet ) {
	if ( globNumPartialsIsSet ) {
	  numPartials = globNumPartials;
	  numPartialsIsSet = true;
	}
	else {
	  fprintf( stderr, "mplib warning -- pop_block() - Harmonic block (%u-th block) has no numPartials."
		   " Returning a NULL block.\n" , blockCount );
	  reset();
	  return( NULL );
	}
      }

    }
    /* End additional parameters for the harmonic block  */
    /****************************************************/

    /****************************************************/
    /* - Additional parameters for the chirp block:     */
    if ( !strcmp(type,"chirp") ) {

      /* - numFitPoints: */
      if ( !numFitPointsIsSet ) {
	if ( globNumFitPointsIsSet ) {
	  numFitPoints = globNumFitPoints;
	  numFitPointsIsSet = true;
	}
	else { /* Default to 3 points */
	  numFitPoints = 1;
	  numFitPointsIsSet = true;
	}
      }

      /* - numIter: */
      if ( !numIterIsSet ) {
	if ( globNumIterIsSet ) {
	  numIter = globNumIter;
	  numIterIsSet = true;
	}
	else { /* Default to 1 iteration */
	  numIter = 1;
	  numIterIsSet = true;
	}
      }
    }
    /* End additional parameters for the chirp block    */
    /****************************************************/

  }
  /***************************/
  /* - ADD YOUR BLOCKS HERE: */
  else if ( !strcmp(type,"TEMPLATE") ) {
    // Check the input parameters
  }
  /********************/
  /* - unknown block: */
  else {
    fprintf( stderr, "mplib warning -- pop_block() - Cannot create a block of type \"%s\" (%u-th block)."
	     " Returning a NULL block.\n", type, blockCount );
    reset();
    return( NULL );
  }
  /*                  */
  /********************/


  /*********************************************/
  /* INSTANTIATE the proper block and fill it: */
  /*********************************************/
  /* - Dirac block: */
  if ( !strcmp(type,"dirac") ) {
    block = MP_Dirac_Block_c::init( signal );
    blockCount++;
  }
  /* - Gabor block: */
  else if ( !strcmp(type,"gabor") ) {
    if ( windowLenIsSet && windowShiftIsSet && fftSizeIsSet && windowTypeIsSet ) {
      block = MP_Gabor_Block_c::init( signal, windowLen, windowShift,
				      fftRealSize, windowType, windowOption );
    }
    else {
      mp_error_msg( func, "Missing parameters in gabor block instanciation (%u-th block)."
	       " Returning a NULL block.\n" , blockCount );
      reset();
      return( NULL );
    }
  }
  /* - Harmonic block: */
  else if ( !strcmp(type,"harmonic") ) {
    if ( windowLenIsSet && windowShiftIsSet && fftSizeIsSet && windowTypeIsSet
	 && f0MinIsSet && f0MaxIsSet && numPartialsIsSet ) {
      block = MP_Harmonic_Block_c::init( signal, windowLen, windowShift, fftRealSize,
					 windowType, windowOption,
					 minFundFreqIdx, maxFundFreqIdx, numPartials );
    }
    else {
      mp_error_msg( func, "Missing parameters in harmonic block instanciation (%u-th block)."
		    " Returning a NULL block.\n" , blockCount );
      reset();
      return( NULL );
    }
  }
  /* - Chirp block: */
  else if ( !strcmp(type,"chirp") ) {
    if ( windowLenIsSet && windowShiftIsSet && fftSizeIsSet && windowTypeIsSet
	 && numFitPointsIsSet && numIterIsSet ) {
      block = MP_Chirp_Block_c::init( signal, windowLen, windowShift, fftRealSize,
				      windowType, windowOption,
				      numFitPoints, numIter );
    }
    else {
      mp_error_msg( func, "Missing parameters in chirp block instanciation (%u-th block)."
	       " Returning a NULL block.\n" , blockCount );
      reset();
      return( NULL );
    }
  }
  /* - ADD YOUR BLOCKS HERE: */
  /*else if ( !strcmp(type,"TEMPLATE") ) {
    // block = new MP_TEMPLATE_Block_c( signal, windowLen, windowShift, fftSize );
    block = NULL;
    }*/
  /* - unknown block: */
  else { /* (This case should never be reached, since it should
	    be blocked at the parameter check level above.) */
    fprintf( stderr, "mplib warning -- pop_block() - Cannot create a block of type \"%s\" (%u-th block)."
	     " Returning a NULL block.\n", type, blockCount );
    block = NULL;
  }
  /*                  */
  /********************/

  /********************/
  /* Reset the local block variables in the MP_Scan_Info structure */
  reset();

  /* Return the created block (or NULL) */
  return( block );
}



/*************/
/* Generic function to write blocks to streams */
int write_block( FILE *fid, MP_Block_c *block ) {

  int nChar = 0;
  char *name;

  name = block->type_name();

  /**** - Dirac block: ****/
  if ( !strcmp( name, "dirac" ) ) {
    /* Open the block */
    nChar += fprintf( fid, "\t<block type=\"%s\">\n", name );
    /* Close the block */
    nChar += fprintf( fid, "\t</block>\n" );
  }

  /**** - Gabor block: ****/
  else if ( !strcmp( name, "gabor" ) ) {
    /* Cast the block */
    MP_Gabor_Block_c *gblock;
    gblock = (MP_Gabor_Block_c*)block;
    /* Open the block */
    nChar += fprintf( fid, "\t<block type=\"%s\">\n", name );
    /* Add the parameters */
    nChar += fprintf( fid, "\t\t<par type=\"windowLen\">%lu</par>\n", gblock->filterLen );
    nChar += fprintf( fid, "\t\t<par type=\"windowShift\">%lu</par>\n", gblock->filterShift );
    nChar += fprintf( fid, "\t\t<par type=\"fftSize\">%lu</par>\n", ((gblock->numFilters-1)<<1) );
    nChar += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%lg\"></window>\n",
		      window_name(gblock->fft->windowType), gblock->fft->windowOption );
    /* Close the block */
    nChar += fprintf( fid, "\t</block>\n" );
  }

  /**** - Harmonic block: ****/
  else if ( !strcmp( name, "harmonic" ) ) {
    unsigned long int maxFundFreqIdx;
    unsigned long int fftSize;
    double f0Min, f0Max;
    /* Cast the block */
    MP_Harmonic_Block_c *hblock;
    hblock = (MP_Harmonic_Block_c*)block;
    /* Rectify some parameters */
    maxFundFreqIdx = hblock->minFundFreqIdx + hblock->numFundFreqIdx - 1;
    fftSize = ((hblock->numFilters - 1 - hblock->numFundFreqIdx)<<1);
    f0Min = (double)(hblock->minFundFreqIdx) * ((double)(hblock->s->sampleRate) / (double)(fftSize));
    f0Max = (double)(maxFundFreqIdx) * ((double)(hblock->s->sampleRate) / (double)(fftSize));
    /* Open the block */
    nChar += fprintf( fid, "\t<block type=\"%s\">\n", name );
    /* Add the parameters */
    nChar += fprintf( fid, "\t\t<par type=\"windowLen\">%lu</par>\n", hblock->filterLen );
    nChar += fprintf( fid, "\t\t<par type=\"windowShift\">%lu</par>\n", hblock->filterShift );
    nChar += fprintf( fid, "\t\t<par type=\"fftSize\">%lu</par>\n", fftSize );
    nChar += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%lg\"></window>\n",
		      window_name(hblock->fft->windowType), hblock->fft->windowOption );
    nChar += fprintf( fid, "\t\t<par type=\"f0Min\">%.2f</par>\n", f0Min );
    nChar += fprintf( fid, "\t\t<par type=\"f0Max\">%.2f</par>\n", f0Max );
    nChar += fprintf( fid, "\t\t<par type=\"numPartials\">%u</par>\n", hblock->maxNumPartials );
    /* Close the block */
    nChar += fprintf( fid, "\t</block>\n" );
  }

  /**** - Chirp block: ****/
  else if ( !strcmp( name, "chirp" ) ) {
    /* Cast the block */
    MP_Chirp_Block_c *cblock;
    cblock = (MP_Chirp_Block_c*)block;
    /* Open the block */
    nChar += fprintf( fid, "\t<block type=\"%s\">\n", name );
    /* Add the parameters */
    nChar += fprintf( fid, "\t\t<par type=\"windowLen\">%lu</par>\n", cblock->filterLen );
    nChar += fprintf( fid, "\t\t<par type=\"windowShift\">%lu</par>\n", cblock->filterShift );
    nChar += fprintf( fid, "\t\t<par type=\"fftSize\">%lu</par>\n", ((cblock->numFilters-1)<<1) );
    nChar += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%lg\"></window>\n",
		      window_name(cblock->fft->windowType), cblock->fft->windowOption );
    nChar += fprintf( fid, "\t\t<par type=\"numFitPoints\">%u</par>\n", cblock->numFitPoints );
    /* Close the block */
    nChar += fprintf( fid, "\t</block>\n" );
  }

  /**** - unknown block: ****/
  else {
    fprintf( stderr, "mplib error -- write_block() - Cannot write a block of type \"%s\"."
	     " I'm skipping this block.\n", name );
    return( 0 );
  }

  return( nChar );

}
