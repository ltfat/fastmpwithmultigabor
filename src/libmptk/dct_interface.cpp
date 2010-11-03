/******************************************************************************/
/*                                                                            */
/*                             fft_interface.cpp                              */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Boris Mailhé                                               Tue Nov 02 2010 */
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

#include "mptk.h"
#include "mp_system.h"
#include <fstream>


/*********************************/
/*                               */
/* GENERIC INTERFACE             */
/*                               */
/*********************************/

/***************************/
/* FACTORY METHOD          */
/***************************/
MP_DCT_Interface_c* MP_DCT_Interface_c::init(const unsigned long int setDctSize){
  MP_DCT_Interface_c* dct = NULL;

  /* Create the adequate FFT and check the returned address */
#ifdef USE_FFTW3
  dct = (MP_DCT_Interface_c*) new MP_DCTW_Interface_c(setDctSize);
  if ( dct == NULL ) mp_error_msg( "MP_DCT_Interface_c::init()",
                                     "Instanciation of DCTW_Interface failed."
                                     " Returning a NULL fft object.\n" );
#else
#  error "No FFT implementation was found !"
#endif

  if ( dct == NULL){ 
    mp_error_msg( "MP_DCT_Interface_c::init()",
                    "DCT window is NULL. Returning a NULL dct object.\n");
  	return( NULL );
  
  }

  /* If everything went OK, just pop it ! */
  return( dct );
}

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***********************************/
/* Constructor with a typed window */
MP_DCT_Interface_c::MP_DCT_Interface_c(const unsigned long int setDctSize){

  /* Set values */
  dctSize = setDctSize;
  buffer = new MP_Real_t[dctSize];
}


/**************/
/* Destructor */
MP_DCT_Interface_c::~MP_DCT_Interface_c( ){
    if (buffer)
        delete[] buffer;
}

/***************************/
/* OTHER METHODS           */
/***************************/

/***************************/
/* EXECUTION METHODS       */
/***************************/

/**************************/
/* Get the magnitude only */
void MP_DCT_Interface_c::exec_mag( MP_Real_t *in, MP_Real_t *mag )
{

  unsigned long int i;
  MP_Real_t coef;

  /* Simple buffer check */
  assert( in  != NULL );
  assert( mag != NULL );

  /* Execute the FFT */
  exec_dct( in, buffer );

  /* Get the resulting magnitudes */
  for ( i=0; i<dctSize; i++ )
    {
        coef = *(buffer+i);
        
#ifdef MP_MAGNITUDE_IS_SQUARED
      *(mag+i) = (MP_Real_t)( coef*coef );
#else
      *(mag+i) = (MP_Real_t)( fabs(coef) );
#endif
    }

}

/*********************************/
/*                               */
/*             GENERIC TEST      */
/*                               */
/*********************************/
int MP_DCT_Interface_c::test( const double precision,
                              const unsigned long int setDctSize ,
                              MP_Real_t *samples)
{

  MP_DCT_Interface_c* dct = MP_DCT_Interface_c::init( setDctSize );
  unsigned long int i;
  MP_Real_t amp,energy1,energy2,tmp;
  MP_Real_t* buffer = new MP_Real_t[setDctSize];

  /* -1- Compute the energy of the analyzed signal multiplied by the analysis window */
  energy1 = 0.0;
  for (i=0; i < setDctSize; i++)
    {
      amp = samples[i];
      energy1 += amp*amp;
    }
  /* -2- The resulting DCT should be of the same energy multiplied by windowSize */
  energy2 = 0.0;
  dct->exec_dct(samples,buffer);
  
  energy2 = 0;
  for (i=0; i<setDctSize; i++)
    {
      energy2 += buffer[i]*buffer[i];
    }

  tmp = fabsf((float)energy2 /((float)(energy1))-1);
  delete[] buffer;
  if ( tmp < precision )
    {
      mp_info_msg( "MP_DCT_Interface_c::test()","SUCCESS for DCT size [%ld] energy in/out = 1+/-%g\n",
             setDctSize,tmp);
      return(0);
    }
  else
    {
     mp_error_msg( "MP_DCT_Interface_c::test()",
                        "FAILURE for DCT size [%ld] energy |in/out-1|= %g > %g\n",
             setDctSize, tmp, precision);
      return(1);
    }

}

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/****/
/* Constructor where the window is actually generated */
MP_DCTW_Interface_c::MP_DCTW_Interface_c( const unsigned long int setDctSize )
    :MP_DCT_Interface_c( setDctSize )
{

  /* FFTW takes integer FFT sizes => check if the cast (int)(fftCplxSize) will overflow. */
  assert( dctSize <= INT_MAX );
  /* Allocate the necessary buffers */
  inPrepared =       (double*) fftw_malloc( sizeof(double)       * dctSize );
  out        = (double*) fftw_malloc( sizeof(double) * dctSize );

  /* Create plans */
  p = fftw_plan_r2r_1d( (int)(dctSize), inPrepared, out, FFTW_REDFT11, FFTW_MEASURE );
  scale = 1/sqrt(dctSize);
}

/**************/
/* Destructor */
MP_DCTW_Interface_c::~MP_DCTW_Interface_c()
{
    fftw_destroy_plan( p );
    fftw_free( inPrepared );
    fftw_free( out );
}

/***************************/
/* OTHER METHODS           */
/***************************/

/**************************/
/* Get the complex result */

void MP_DCTW_Interface_c::exec_dct( MP_Real_t *in, MP_Real_t *out )
{

  unsigned long int i;

  /* Simple buffer check */
  assert( in != NULL );
  assert( out != NULL );

  /* Copy and window the input signal */
  for ( i=0; i<dctSize; i++ )
    {
      *(inPrepared+i) = (double)(*(in+i));      
    }

  /* Execute the FFT described by plan "p"
     (which itself points to the right input/ouput buffers,
     such as buffer inPrepared etc.) */
  fftw_execute( p );

  /* Cast and copy the result */
  for ( i=0; i<dctSize; i++ )
    {
      *(out+i) = (MP_Real_t)( *(this->out)+i );
    }
}

bool MP_DCT_Interface_c::init_dct_library_config()
{
#ifdef USE_FFTW3
  const char * func =  "MP_DCT_Interface_c::init_fft_library_config()";
  int wisdom_status;
  FILE * wisdomFile = NULL;

  /* Check if file path is defined in env variable */
  const char *filename = MPTK_Env_c::get_env()->get_config_path("fftw_wisdomfile");
	
  if (NULL != filename)
    wisdomFile= fopen(filename,"r");
  /* Check if file exists */
  if (wisdomFile!=NULL)
    {
      /* Try to load the wisdom file for creating fftw plan */
      wisdom_status = fftw_import_wisdom_from_file(wisdomFile);
      
      /* Check if wisdom file is well formed */
      if (wisdom_status==0)
        {
          mp_error_msg( func, "wisdom file is ill formed\n");
          /* Close the file anyway */
          fclose(wisdomFile);
          return false;
        }
      else
        {MPTK_Env_c::get_env()->set_fftw_wisdom_loaded();
          /* Close the file  */
          fclose(wisdomFile);
          return true;
        }

    }
   
  else{  
    mp_warning_msg( func, "fftw wisdom file with path %s  doesn't exist.\n", filename);
    mp_warning_msg( func, "It will be created.\n");
    mp_warning_msg( func, "NB: An fftw wisdom file allows MPTK to run slighty faster,\n");
    mp_warning_msg( func, "    however its absence is otherwise harmless.\n");
    mp_warning_msg( func, "    YOU CAN SAFELY IGNORE THIS WARNING MESSAGE.\n");
  	return false;
	}
#else

  return false;
#endif


}

bool MP_DCT_Interface_c::save_dct_library_config()
{
#ifdef USE_FFTW3

  FILE * wisdomFile = NULL;
  /* Check if fftw wisdom file has to be saved
   * and if the load of this files  succeed when init the fft library config */
   const char *filename = MPTK_Env_c::get_env()->get_config_path("fftw_wisdomfile");
  if (NULL!=filename && !MPTK_Env_c::get_env()->get_fftw_wisdom_loaded() )
    wisdomFile = fopen(filename,"w");
  /* Check if file exists or if the files could be created */
  if (wisdomFile!=NULL)
    {
      /* Export the actual wisdom to the file */
      fftw_export_wisdom_to_file(wisdomFile);
      /* Close the file */
      fclose(wisdomFile);
      return true;
    }
  else
    {

      return false;
    }
#else
  return false;
#endif

}

MP_Real_t MP_DCTW_Interface_c::test(){
    return 0;
}
