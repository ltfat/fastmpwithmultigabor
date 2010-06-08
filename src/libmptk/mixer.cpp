/******************************************************************************/
/*                                                                            */
/*                                 mixer.cpp                                  */
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

/**************************************************************/
/*                                                            */
/* mixer.cpp: generic interface for creating mixer            */
/*                                                            */
/**************************************************************/

#include "mptk.h"
#include "mp_system.h"

/*********************************/
/*                               */
/* GENERIC INTERFACE             */
/*                               */
/*********************************/

/***************************/
/* FACTORY METHOD          */
/***************************/

MP_Abstract_Mixer_c* MP_Abstract_Mixer_c::creator( FILE * mixerFID )
{

  const char* func = "MP_Abstract_Mixer_c::init(char *mixerFileName)";
  mp_debug_msg( MP_DEBUG_FUNC_ENTER, func, "Entering.\n" );

  MP_Mixer_c* mixer = NULL;

// if ( (mixerFID = fopen( mixerFileName, "r" )) == NULL )
  // {
  // mp_error_msg( "MP_Abstract_Mixer_c::init()", "Failed to open the mixer matrix file [%s] for reading.\n",
  //               mixerFileName );
  // return (NULL);
  // }
  //else

  //if ( !strcmp(type,"linear") )
  //mixer = (MP_Mixer_c*) MP_Mixer_c::creator(mixerFID);
  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Leaving.\n" );
  return mixer;
  //else
  // {
  //  mp_error_msg( func, "Cannot read mixer of type '%s'\n",type);
  // return( NULL );
  //}

}

int MP_Abstract_Mixer_c::read( FILE * mixerFID ){
return 0;
}

//int MP_Abstract_Mixer_c::info( FILE *fid ){
//return 0;
//}
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
MP_Abstract_Mixer_c::MP_Abstract_Mixer_c( void ) {

  


}
/**************/
/* Destructor */
MP_Abstract_Mixer_c::~MP_Abstract_Mixer_c() {
}
/**********************************/
/*                                */
/* MIXER-DEPENDENT IMPLEMENTATION */
/*                                */
/**********************************/


/***************************/
/* FACTORY METHOD          */
/***************************/
MP_Mixer_c* MP_Mixer_c::creator_from_txt_file( const char *mixerFileName )
{
	const char* func = "MP_Linear_Mixer_c::init( const char *mixerFileName)";
	FILE *mixerFID;
     if ( (mixerFID = fopen( mixerFileName, "r" )) == NULL )
    {
       mp_error_msg( func, "Failed to open the mixer matrix file [%s] for reading.\n",
               mixerFileName );
      return( NULL );
    } 
    else return creator_from_txt_file(mixerFID );
}    


MP_Mixer_c* MP_Mixer_c::creator_from_txt_file( FILE * mixerFID )
{

  const char* func = "MP_Linear_Mixer_c::init( FILE * mixerFID)";

  /* Instantiate and check */
  MP_Mixer_c* linearMixer = new MP_Mixer_c();
  if ( linearMixer == NULL )
    {
      mp_error_msg( func, "Failed to create a new Linear mixer.\n" );
      return( NULL );
    }

  /* Read and check */
  if ( linearMixer->read( mixerFID ) == 1)
    {
      mp_error_msg( func, "Failed to read the new Gabor atom.\n" );
      delete( linearMixer );
      return( NULL );
    }

  return( linearMixer );


}
int MP_Mixer_c::read( FILE * mixerFID )
{
  const char* func = "MP_Linear_Mixer_c::read( FILE * mixerFID )";
  unsigned long int i;
  unsigned short int j;
  unsigned int k;
  char line[1024];
  float scanVal;
  double val;

  if ( ( fgets( line, MP_MAX_STR_LEN, mixerFID ) == NULL ) ||
       ( sscanf( line,"%hu %hu\n", &numChans, &numSources ) != 2 ) )
    {
      fprintf( stderr, "mpd_demix error -- Failed to read numChans and numSources from the mixer matrix file.\n" );
      return( 1 );
    }
  else
    if ( (mixer = (MP_Real_t*) malloc( numChans*numSources*sizeof(MP_Real_t) )) == NULL )
      {
        fprintf( stderr, "mpd_demix error -- Can't allocate an array of [%lu] MP_Real_t elements"
                 "for the mixer matrix.\n", (unsigned long int)(numChans)*numSources );
       
        return( 1 );
      }

    else for ( k = 0, p = mixer; k < numChans; k++ )
        {
          for ( j = 0; j<numSources; j++ )
            {
              if ( fscanf( mixerFID, "%f", &scanVal ) != 1 )
                {
                  mp_error_msg( func, "Can't read element [%i,%u] of the mixer matrix in file \n",
                                k, j);
                  fclose( mixerFID );
                  return( 1 );
                }
              else
                {
                  *p = (MP_Real_t)(scanVal);
                  p++;
                }
            }
        }

  /* Normalize the columns */
  for ( j = 0; j < numSources; j++ )
    {
      for ( k = 0, p = mixer+j, val = 0.0; k < numChans; k++, p += numSources ) val += (*p)*(*p);
      val = sqrt( val );
      for ( k = 0, p = mixer+j; k < numChans; k++, p += numSources ) (*p) = (*p) / val;
    }
#ifndef NDEBUG
  fprintf( stderr, "mpd_demix DEBUG -- Normalized mixer matrix:\n" );
  for ( k = 0, p = mixer; k < numChans; k++ )
    {
      for ( j = 0; j < numSources; j++, p++ )
        {
          fprintf( stderr, "%.4f ", *p );
        }
      fprintf( stderr, "\n" );
    }
  fprintf( stderr, "mpd_demix DEBUG -- End mixer matrix.\n" );
#endif

  /* Pre-compute the squared mixer */
  if ( (Ah = (MP_Real_t*) malloc( numSources*numSources*sizeof(MP_Real_t) )) == NULL )
    {
      fprintf( stderr, "mpd_demix error -- Can't allocate an array of [%u] MP_Real_t elements"
               "for the squared mixer matrix.\n", numSources*numSources );

      return( 1 );
    }
  else
    {
      for ( i = 0, p = Ah; i < (unsigned long int)numSources; i++ )
        {
          for ( j = 0; j < numSources; j++, p++ )
            {
              for ( k = 0, val = 0.0; k < numChans; k++ )
                {
                  val += (double)(*(mixer + k*numSources + i)) * (double)(*(mixer + k*numSources + j));
                }
              *p = (MP_Real_t)(val);
            }
        }
    }

  return( 0 );

}
//int MP_Linear_Mixer_c::info( FILE *fid ){
//return 0;
//}
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
MP_Mixer_c::MP_Mixer_c( void )
    :MP_Abstract_Mixer_c() {
  numSources = 0;
  numChans= 0;
  mixer = NULL;
  Ah =NULL;
  p = NULL;

}
/**************/
/* Destructor */
MP_Mixer_c::~MP_Mixer_c() {
  if (mixer) free(mixer);
  if (Ah) free(Ah);
}
 
void  MP_Mixer_c::applyAdjoint( std::vector<MP_Signal_c*> *sourceSignalArray, const MP_Signal_c *mixedSignal ) {
 /* Fill the signal array: multiply the input signal by the transposed mixer */
  unsigned long int i;
  unsigned short int j;
  int k;
  for ( j = 0; j < numSources; j++ )
    {
      MP_Real_t *s = sourceSignalArray->at(j)->storage;
      for ( i = 0; i < mixedSignal->numSamples; i++, s++ )
        {
          MP_Real_t val;
          MP_Real_t in;
          for ( k = 0, val = 0.0, p = mixer+j;
                k < mixedSignal->numChans;
                k++, p += numSources )
            {
              in = *(mixedSignal->channel[k] + i);
              val += ( (*p) * (MP_Real_t)(in) );
            }
          *s = (MP_Real_t)( val );
        }
}

}	
void MP_Mixer_c::applyDirect( const std::vector<MP_Signal_c*> *sourceSignalArray, MP_Signal_c *mixedSignal ){

}



