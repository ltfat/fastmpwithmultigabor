/******************************************************************************/
/*                                                                            */
/*                         mclt_block.cpp      		                      */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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

/****************************************************************/
/*                                               		*/
/* mclt_block.cpp: methods for mclt blocks			*/
/*                                               		*/
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mclt_atom_plugin.h"
#include "mclt_block_plugin.h"
#include "mclt_abstract_block_plugin.h"
#include <sstream>
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

MP_Block_c* MP_Mclt_Block_Plugin_c::create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap )
{

  const char* func = "MP_Mclt_Block_Plugin_c::create( MP_Signal_c *setSignal, map<const char*, const char*, MP_Dict_c::ltstr> *paramMap )";
  MP_Mclt_Block_Plugin_c *newBlock = NULL;
  char*  convertEnd;
  unsigned long int filterLen = 0;
  unsigned long int filterShift =0 ;
  unsigned long int fftSize  = 0;
  unsigned char windowType;
  double windowOption =0.0;
  double windowRate =0.0;
  unsigned long int blockOffset = 0;

  /* Instantiate and check */
  newBlock = new MP_Mclt_Block_Plugin_c();
  if ( newBlock == NULL )
    {
      mp_error_msg( func, "Failed to create a new Mclt block.\n" );
      return( NULL );
    }
  /* Analyse the parameter map */
  if (strcmp((*paramMap)["type"].c_str(),"mclt"))
    {
      mp_error_msg( func, "Parameter map does not define a MCLT block.\n" );
      return( NULL );
    }



  if ((*paramMap)["windowLen"].size()>0)
    {
      /*Convert window length*/
      filterLen=strtol((*paramMap)["windowLen"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowLen in unsigned long int.\n" );
          return( NULL );
        }
    }
  else
    {
      mp_error_msg( func, "No parameter windowLen in the parameter map.\n" );
      return( NULL );
    }
  if ((*paramMap)["fftSize"].size()>0)
    {
      /*Convert fftSize*/
      fftSize=strtol((*paramMap)["fftSize"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter fftSize in unsigned long int.\n" );
          return( NULL );
        }
    }
  else
    {
      if ( is_odd(filterLen) ) fftSize = filterLen + 1;
      else                     fftSize = filterLen;
    }

  if ((*paramMap)["windowtype"].size()>0) windowType = window_type((*paramMap)["windowtype"].c_str());
  else
    {
      mp_error_msg( func, "No parameter windowtype in the parameter map.\n" );
      return( NULL );
    }

  if (( window_needs_option(windowType)) && (!((*paramMap)["windowopt"].size()>0)))
    {
      mp_error_msg( func, "Mclt block "
                    " requires a window option (the opt=\"\" attribute is probably missing"
                    " in the relevant <window> tag). Returning a NULL block.\n" );
      return( NULL );
    }
  else
    {
      /*Convert windowopt*/
      windowOption=strtod((*paramMap)["windowopt"].c_str(), &convertEnd);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter window option in double.\n" );
          return( NULL );
        }
    }

  if ((*paramMap)["windowShift"].size()>0)
    {
      /*Convert windowShift*/
      filterShift=strtol((*paramMap)["windowShift"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
          return( NULL );
        }
    }
  else
    {
      if ((*paramMap)["windowRate"].size()>0)
        {
          windowRate =strtod((*paramMap)["windowRate"].c_str(), &convertEnd);
          if (*convertEnd != '\0')
            {
              mp_error_msg( func, "cannot convert parameter windowRate in unsigned long int.\n");
              return( NULL );
            }
          filterShift = (unsigned long int)( (double)(filterLen)*windowRate + 0.5 );
          filterShift = ( filterShift > 1 ? filterShift : 1 ); /* windowShift has to be 1 or more */

        }
      else
        {
          if ((*paramMap)["windowLen"].size()>0)
            {
              if ( (!strcmp((*paramMap)["windowtype"].c_str(),"rectangle")) || (!strcmp((*paramMap)["windowtype"].c_str(),"cosine")) || (!strcmp((*paramMap)["windowtype"].c_str(),"kbd")) ) filterShift = filterLen / 2;
              else
                {
                  mp_error_msg( func, "Wrong window type. It has to be: rectangle, cosine or kbd.\n" );
                }
            }
          else
            {
              mp_error_msg( func, "No parameter windowShift or windowRate in the parameter map.\n" );
              return( NULL );
            }
        }
    }

  if ((*paramMap)["blockOffset"].size()>0)
    {
      /*Convert windowShift*/
      blockOffset=strtol((*paramMap)["blockOffset"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
          return( NULL );
        }
    }
  /* Set the block parameters (that are independent from the signal) */
  if (  newBlock->init_parameters( filterLen, filterShift, fftSize,
                                   windowType, windowOption, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in the new MCLT block.\n" );
      delete( newBlock );
      return( NULL );
    }
  /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map( filterLen, filterShift, fftSize,
                                     windowType, windowOption, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize parameters map in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );
    }

  /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map( filterLen, filterShift, fftSize,
                                     windowType, windowOption, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize parameters map in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );
    }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) )
    {
      mp_error_msg( func, "Failed to plug a signal in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );
    }

  return( (MP_Block_c*)newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Mclt_Block_Plugin_c::init_parameters( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setFftSize,
    const unsigned char setWindowType,
    const double setWindowOption,
    const unsigned long int setBlockOffset )
{

  const char* func = "MP_Mclt_Block_c::init_parameters()";

  MP_Mclt_Abstract_Block_Plugin_c::init_parameters( setFilterLen, setFilterShift, setFftSize, setWindowType, setWindowOption, setBlockOffset);

  /* Allocate the atom's autocorrelations */
  if ( alloc_correl( &reCorrel, &imCorrel, &sqCorrel, &cstCorrel ) )
    {
      mp_error_msg( func, "Failed to allocate the block's autocorrelations.\n" );
      return( 1 );
    }

  /* Tabulate the atom's autocorrelations */
  if ( fill_correl( reCorrel, imCorrel, sqCorrel, cstCorrel ) )
    {
      mp_error_msg( func, "Failed to tabulate the block's autocorrelations.\n" );
      return( 1 );
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Mclt_Block_Plugin_c::init_parameter_map( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setFftSize,
    const unsigned char setWindowType,
    const double setWindowOption,
    const unsigned long int setBlockOffset )
{
  const char* func = "MP_Gabor_Block_c::init_parameter_map(...)";

  parameterMap = new map< string, string, mp_ltstring>();

  /*Create a stream for convert number into string */
  std::ostringstream oss;

  (*parameterMap)["type"] = type_name();

  /* put value in the stream */
  if (!(oss << setFilterLen))
    {
      mp_error_msg( func, "Cannot convert windowLen in string for parameterMap.\n" );
      return( 1 );
    }
  /* put stream in string */
  (*parameterMap)["windowLen"] = oss.str();
  /* clear stream */
  oss.str("");
  if (!(oss << setFilterShift))
    {
      mp_error_msg( func, "Cannot convert windowShift in string for parameterMap.\n"
                  );
      return( 1 );
    }
  (*parameterMap)["windowShift"] = oss.str();
  oss.str("");
  if (!(oss << setFftSize))
    {
      mp_error_msg( func, "Cannot convert fftSize in string for parameterMap.\n"
                  );
      return( 1 );
    }
  (*parameterMap)["fftSize"] = oss.str();
  oss.str("");
  (*parameterMap)["windowtype"] = window_name(setWindowType);

  if (!(oss << setWindowOption))
    {
      mp_error_msg( func, "Cannot convert windowopt in string for parameterMap.\n"
                  );
      return( 1 );
    }
  (*parameterMap)["windowopt"] = oss.str();
  oss.str("");
  if (!(oss << setBlockOffset))
    {
      mp_error_msg( func, "Cannot convert blockOffset in string for parameterMap.\n"
                  );
      return( 1 );
    }
  (*parameterMap)["blockOffset"] = oss.str();
  oss.str("");

  return (0);
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Mclt_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  MP_Mclt_Abstract_Block_Plugin_c::plug_signal( setSignal );

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Mclt_Block_Plugin_c::nullify_signal( void )
{

  MP_Mclt_Abstract_Block_Plugin_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Mclt_Block_Plugin_c::MP_Mclt_Block_Plugin_c( void )
    :MP_Mclt_Abstract_Block_Plugin_c()
{

  reCorrel = imCorrel = NULL;
  sqCorrel = cstCorrel = NULL;

}


/**************/
/* Destructor */
MP_Mclt_Block_Plugin_c::~MP_Mclt_Block_Plugin_c()
{

  if ( reCorrel  ) free( reCorrel  );
  if ( imCorrel  ) free( imCorrel  );
  if ( sqCorrel  ) free( sqCorrel  );
  if ( cstCorrel ) free( cstCorrel );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
const char * MP_Mclt_Block_Plugin_c::type_name()
{
  return ("mclt");
}

/**********************/
/* Readable text dump */
int MP_Mclt_Block_Plugin_c::info( FILE *fid )
{

  int nChar = 0;

  nChar += mp_info_msg( fid, "MCLT BLOCK", "%s window (window opt=%g)"
                        " of length [%lu], shifted by [%lu] samples,\n",
                        window_name( fft->windowType ), fft->windowOption,
                        filterLen, filterShift );
  nChar += mp_info_msg( fid, "         |-", "projected on [%lu] frequencies;\n",
                        numFilters );
  nChar += mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu], "
                        "the search tree has [%lu] levels.\n", numFrames, numLevels );

  return( nChar );
}

/*****************************************/
/* Allocation of the correlation vectors */
int MP_Mclt_Block_Plugin_c::alloc_correl( MP_Real_t **reCorr, MP_Real_t **imCorr,
    MP_Real_t **sqCorr, MP_Real_t **cstCorr )
{

  const char* func = "MP_Mclt_Block_c::alloc_correl(...)";

  /* Allocate the memory for the correlations and init it to zero */
  *reCorr = *imCorr = *sqCorr = NULL;

  if ( ( *reCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL)
    {
      mp_error_msg( func, "Can't allocate storage space for the real part"
                    " of the atom correlations. Correlations are left un-initialized.\n");
      return( 1 );
    }
  else if ( ( *imCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL)
    {
      mp_error_msg( func, "Can't allocate storage space for the imaginary part"
                    " of the atom correlations. Corrations are left un-initialized.\n");
      free( *reCorr );
      *reCorr = NULL;
      return( 1 );
    }
  else if ( ( *sqCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL)
    {
      mp_error_msg( func, "Can't allocate storage space for the squared"
                    " atom correlations. Correlations are left un-initialized.\n");
      free( *reCorr );
      *reCorr = NULL;
      free( *imCorr );
      *imCorr = NULL;
      return( 1 );
    }
  else if ( ( *cstCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL)
    {
      mp_error_msg( func, "Can't allocate storage space for the pre-computed"
                    " constant of the atom correlations. Correlations are left un-initialized.\n");
      free( *reCorr );
      *reCorr = NULL;
      free( *imCorr );
      *imCorr = NULL;
      free( *sqCorr );
      *sqCorr = NULL;
      return( 1 );
    }

  return( 0 );
}

/******************************************************/
/** Fill the correlation arrays with */
int MP_Mclt_Block_Plugin_c::fill_correl( MP_Real_t *reCorr, MP_Real_t *imCorr,
    MP_Real_t *sqCorr, MP_Real_t *cstCorr )
{

  //const char* func = "MP_Mclt_Block_c::fill_correl(...)";
  double re,im,sq;
  int k, l;

  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );

  for ( k = 0;  k < (int)(numFreqs);  k++ )
    {
      re = 0.0;
      im = 0.0;
      for ( l = 0; l < (int)(filterLen);  l++ )
        {
          if ( filterLen == fftSize )
            {
              re += pow(*(fft->window+l),2) * cos( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k + 0.5 ) );
              im += pow(*(fft->window+l),2) * sin( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k + 0.5 ) );
            }
          else
            {
              re += pow(*(fft->window+l),2) * cos( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k ) );
              im += pow(*(fft->window+l),2) * sin( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k ) );
            }
        }
      *( reCorr + k ) = (MP_Real_t)( re );
      *( imCorr + k ) = (MP_Real_t)( im );
      sq = ( re*re + im*im );
      *( sqCorr + k ) = (MP_Real_t)(   sq );
      *( cstCorr + k ) = (MP_Real_t)( 2.0 / (1.0 - sq) );
    }

  return( 0 );
}

/*************************************/
/* Compute the accurate atom energy  */
void MP_Mclt_Block_Plugin_c::compute_energy( MP_Real_t *in,
    MP_Real_t *reCorr, MP_Real_t *imCorr,
    MP_Real_t *sqCorr, MP_Real_t *cstCorr,
    MP_Real_t *outMag )
{

  const char* func = "MP_Mclt_Block_c::compute_energy(...)";
  int i;
  double re, im, reSq, imSq, energy;
  double correlSq;

  /* Simple buffer check */
  assert( in  != NULL );
  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );
  assert( outMag != NULL );

  /* Execute the mclt transform */
  MP_Mclt_Abstract_Block_Plugin_c::compute_transform( in );

  /*****/
  /* Get the resulting magnitudes: */

  for ( i = 0;  i < ((int)(numFreqs));  i++ )
    {

      /* Get the complex values */
      re = mcltOutRe[i];
      im = mcltOutIm[i];
      reSq = ( re * re );
      imSq = ( im * im );

      /* Get the atom' autocorrelation: */
      correlSq = (double)(*(sqCorr+i));

      /* If the atom's autocorrelation is neglegible: */
      if ( correlSq < MP_ENERGY_EPSILON )
        {
          energy = 2 * ( reSq + imSq );
        }
      /* Else, if the atom's autocorrelation is NOT neglegible: */
      else
        {
          energy  =   ( reSq + imSq )
                      - (double)(*(reCorr+i)) * ( reSq - imSq )
                      + (double)(*(imCorr+i)) * (  2 * re*im  );

          energy = (double)(*(cstCorr+i)) * energy;
          /* The following version appears to be slightly slower,
             but that's not clear cut with our experiments: */
          /* energy = ( 2.0 / (1.0 - correlSq) ) * energy; */
        }

      /* => Compensate for a possible numerical innacuracy
       *    (this case should never happen in practice) */
      if ( energy < 0 )
        {
          mp_warning_msg( func, "A negative energy was met."
                          " (energy = [%g])\nEnergy value is reset to 0.0 .",
                          energy );
          energy = 0.0;
        }

      /* Cast and fill mag */
      *(outMag+i) = (MP_Real_t)(energy);

    }

  /*****/

  return;
}

/********************************************/
/* Frame-based update of the inner products */
void MP_Mclt_Block_Plugin_c::update_frame(unsigned long int frameIdx,
    MP_Real_t *maxCorr,
    unsigned long int *maxFilterIdx)
{
  unsigned long int inShift;
  unsigned long int i;

  MP_Real_t *in;
  MP_Real_t *magPtr;

  double sum;
  double max;
  unsigned long int maxIdx;

  int chanIdx;
  int numChans;

  assert( s != NULL );
  numChans = s->numChans;
  assert( mag != NULL );

  inShift = frameIdx*filterShift + blockOffset;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0, magPtr = mag;    /* <- for each channel */
        chanIdx < numChans;
        chanIdx++,   magPtr += numFreqs )
    {

      assert( s->channel[chanIdx] != NULL );

      /* Hook the signal and the inner products to the fft */
      in  = s->channel[chanIdx] + inShift;

      /* Execute the FFT (including windowing, conversion to energy etc.) */
      compute_energy( in,
                      reCorrel, imCorrel, sqCorrel, cstCorrel,
                      magPtr );

    } /* end foreach channel */
  /*----*/

  /*----*/
  /* Make the sum and find the maxcorr: */
  /* --Element 0: */
  /* - make the sum */
  sum = (double)(*(mag));                     /* <- channel 0      */
  for ( chanIdx = 1, magPtr = mag+numFreqs; /* <- other channels */
        chanIdx < numChans;
        chanIdx++,   magPtr += numFreqs )   sum += (double)(*(magPtr));
  /* - init the max */
  max = sum;
  maxIdx = 0;
  /* -- Following elements: */
  for ( i = 1; i<numFreqs; i++)
    {
      /* - make the sum */
      sum = (double)(*(mag+i));                     /* <- channel 0      */
      for ( chanIdx = 1, magPtr = mag+numFreqs+i; /* <- other channels */
            chanIdx < numChans;
            chanIdx++,   magPtr += numFreqs ) sum += (double)(*(magPtr));
      /* - update the max */
      if ( sum > max )
        {
          max = sum;
          maxIdx = i;
        }
    }
  *maxCorr = (MP_Real_t)max;
  *maxFilterIdx = maxIdx;
}

/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Mclt_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int freqIdx )
{

  const char* func = "MP_Mclt_Block_c::create_atom(...)";
  MP_Mclt_Atom_Plugin_c *matom = NULL;
  /* Time-frequency location: */
  unsigned long int pos = frameIdx*filterShift + blockOffset;
  /* Parameters for a new FFT run: */
  MP_Real_t *in;
  /* Parameters for the atom waveform : */
  double re, im;
  double amp, phase;
  double reCorr, imCorr, sqCorr;
  double real, imag, energy;
  /* Misc: */
  int chanIdx;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples )
    {
      mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
                    " Returning a NULL atom.\n" );
      *atom = NULL;
      return( 0 );
    }
  /* Allocate the atom */
  *atom = NULL;
  MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("mclt");
  if (NULL == emptyAtomCreator)
    {
      mp_error_msg( func, "Mclt atom is not registred in the atom factory" );
      return( 0 );
    }

  if ( (matom =  (MP_Mclt_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL )
    {
      mp_error_msg( func, "Can't create a new Mclt atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
  if ( matom->alloc_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
      return( 0 );

    }
  if ( matom->alloc_mclt_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
      return( 0 );

    }
  if ( window_type_is_ok( fft->windowType) )
    {
      matom->windowType   = fft->windowType;
      matom->windowOption = fft->windowOption;
    }
  else
    {
      mp_error_msg( func, "The window type is unknown. Returning a NULL atom.\n" );
      return( 0 );
    }

  /* Set the default freq */
  matom->freq  = 0.0;

  /* 1) set the frequency of the atom */
  if ( filterLen == fftSize )
    {
      matom->freq  = (MP_Real_t)( (double)(freqIdx + 0.5 ) / (double)(fft->fftSize) );
    }
  else
    {
      matom->freq  = (MP_Real_t)( (double)(freqIdx) / (double)(fft->fftSize) );
    }

  matom->numSamples = pos + filterLen;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {

      /* 2) set the support of the atom */
      matom->support[chanIdx].pos = pos;
      matom->support[chanIdx].len = filterLen;
      matom->totalChanLen += filterLen;

      /* 3) seek the right location in the signal */
      in  = s->channel[chanIdx] + pos;

      /* 4) recompute the inner product of the complex atom */
      MP_Mclt_Abstract_Block_Plugin_c::compute_transform( in );
      re  = (double)( *(mcltOutRe + freqIdx) );
      im  = (double)( *(mcltOutIm + freqIdx) );

      /* 5) set the amplitude an phase */
      /* This is equivalent to: complex2amp_and_phase( re, im, reCorrel, imCorrel, &amp, &phase ); */
      energy = re*re + im*im;
      reCorr = reCorrel[freqIdx];
      imCorr = imCorrel[freqIdx];
      sqCorr = sqCorrel[freqIdx];
      assert( sqCorr <= 1.0 );
      if ( filterLen == fftSize )
        {
          real = (1.0 - reCorr)*re + imCorr*im;
          imag = (1.0 + reCorr)*im + imCorr*re;
          amp   = 2.0 * sqrt( real*real + imag*imag );
          phase = atan2( imag, real );
        }
      else
        {
          if ( (freqIdx != 0) )
            {
              real = (1.0 - reCorr)*re + imCorr*im;
              imag = (1.0 + reCorr)*im + imCorr*re;
              amp   = 2.0 * sqrt( real*real + imag*imag );
              phase = atan2( imag, real ); /* the result is between -M_PI and MP_PI */
            }
          else
            {
              amp = sqrt( energy );
              if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
              else             phase = MP_PI; /* corresponds to the '-' sign exp(i\pi) */
            }
        }

      /* 6) fill in the atom parameters */
      matom->amp[chanIdx]   = (MP_Real_t)( amp   );
      matom->phase[chanIdx] = (MP_Real_t)( phase );

    }

  *atom = matom;

  return( 1 );

}

/*********************************************/
/* get Paramater type map defining the block */
void MP_Mclt_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType)
{

  const char * func = "void MP_Mclt_Block_Plugin_c::get_parameters_type_map()";

  if ((*parameterMapType).empty())
    {
      (*parameterMapType)["type"] = "string";
      (*parameterMapType)["windowLen"] = "ulong";
      (*parameterMapType)["windowShift"] = "ulong";
      (*parameterMapType)["fftSize"] = "ulong";
      (*parameterMapType)["windowtype"] = "string";
      (*parameterMapType)["windowopt"] = "real";
      (*parameterMapType)["blockOffset"] = "ulong";
      (*parameterMapType)["windowRate"] = "real";

    }
  else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Mclt_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo )
{

  const char * func = "void MP_Mclt_Block_Plugin_c::get_parameters_info_map()";

  if ((*parameterMapInfo).empty())
    {
      (*parameterMapInfo)["type"] = "the type of blocks";
      (*parameterMapInfo)["windowLen"] = "The common length of the atoms (which is the length of the signal window), in number of samples.";
      (*parameterMapInfo)["windowShift"] = "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
      (*parameterMapInfo)["fftSize"] = "the size of the FFT, including zero padding";
      (*parameterMapInfo)["windowtype"] = "The window type, which determines its shape. Examples include 'gauss', 'rect', 'hamming' (see the developper documentation of libdsp_windows.h). A related parameter is <windowopt>.";
      (*parameterMapInfo)["windowopt"] = "The optional window shape parameter (see the developper documentation oflibdsp_windows.h).";
      (*parameterMapInfo)["blockOffset"] = "Offset between beginning of signal and beginning of first atom, in number of samples.";
	  (*parameterMapInfo)["windowRate"] = "The shift between atoms on adjacent time frames, in proportion of the <windowLen>. For example, windowRate = 0.5 corresponds to half-overlapping signal windows.";

    }
  else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Mclt_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault )
{

  const char * func = "void MP_Mclt_Block_Plugin_c::get_parameters_default_map()";

  if ((*parameterMapDefault).empty())
    {
      (*parameterMapDefault)["type"] = "mclt";
      (*parameterMapDefault)["windowLen"] = "1024";
      (*parameterMapDefault)["windowShift"] = "512";
      (*parameterMapDefault)["fftSize"] = "1024";
      (*parameterMapDefault)["windowtype"] = "rectangle";
      (*parameterMapDefault)["windowopt"] = "0";
      (*parameterMapDefault)["blockOffset"] = "0";
      (*parameterMapDefault)["windowRate"] = "0.5";
    }

  else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}

/******************************************************/
/* Registration of new block (s) in the block factory */

DLL_EXPORT void registry(void)
{
  MP_Block_Factory_c::get_block_factory()->register_new_block("mclt",&MP_Mclt_Block_Plugin_c::create, &MP_Mclt_Block_Plugin_c::get_parameters_type_map, &MP_Mclt_Block_Plugin_c::get_parameters_info_map, &MP_Mclt_Block_Plugin_c::get_parameters_default_map );
}
