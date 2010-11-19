/******************************************************************************/
/*                                                                            */
/*                          mdct_atom.cpp    	                    	      */
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


/****************************************************************/
/*                                                              */
/* mdct_atom.cpp: methods for mclt atoms			*/
/*                                                              */
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mdct_atom_plugin.h"

#include <dsp_windows.h>


/*************/
/* CONSTANTS */
/*************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Atom_c* MP_Mdct_Atom_Plugin_c::mdct_atom_create_empty(void)
{

  return new MP_Mdct_Atom_Plugin_c;

}

/*************************/
/* File factory function */
MP_Atom_c* MP_Mdct_Atom_Plugin_c::create( FILE *fid, const char mode )
{

  const char* func = "MP_Mdct_Atom_Plugin_c::init(fid,mode)";

  MP_Mdct_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Mdct_Atom_Plugin_c();
  if ( newAtom == NULL )
    {
      mp_error_msg( func, "Failed to create a new atom.\n" );
      return( NULL );
    }

  /* Read and check */
  if ( newAtom->read( fid, mode ) )
    {
      mp_error_msg( func, "Failed to read the new MDCT atom.\n" );
      delete( newAtom );
      return( NULL );
    }

  return( (MP_Atom_c*)newAtom );
}


/********************/
/* Void constructor */
MP_Mdct_Atom_Plugin_c::MP_Mdct_Atom_Plugin_c( void )
    :MP_Atom_c()
{
  windowType = DSP_UNKNOWN_WIN;
  windowOption = 0.0;
  freq  = 0.0;
}


/***************/
/* File reader */
int MP_Mdct_Atom_Plugin_c::read( FILE *fid, const char mode )
{

  const char* func = "MP_Mdct_Atom_Plugin_c::read(fid,mode)";
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  double fidFreq;

  /* Go up one level */
  if ( MP_Atom_c::read( fid, mode ) )
    {
      mp_error_msg( func, "Reading of MDCT atom fails at the generic atom level.\n" );
      return( 1 );
    }

  /* Alloc at local level */
  /* if ( local_alloc( numChans ) ) {
    mp_error_msg( func, "Allocation of MDCT atom failed at the local level.\n" );
    return( 1 );
    }*/
  /* NOTE: no local alloc needed here because no vectors are used at this level. */

  /* Then read this level's info */
  switch ( mode )
    {

    case MP_TEXT:
      /* Read the window type */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
           ( sscanf( line, "\t\t<window type=\"%[a-z]\" opt=\"%lg\"></window>\n", str, &windowOption ) != 2 ) )
        {
          mp_error_msg( func, "Failed to read the window type and/or option in a Mdct atom structure.\n");
          windowType = DSP_UNKNOWN_WIN;
          return( 1 );
        }
      else
        {
          /* Convert the window type string */
          windowType = window_type( str );
        }
      /* freq */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
           ( sscanf( str, "\t\t<par type=\"freq\">%lg</par>\n", &fidFreq ) != 1 ) )
        {
          mp_error_msg( func, "Cannot scan freq.\n" );
          return( 1 );
        }
      else
        {
          freq = (MP_Real_t)fidFreq;
        }
      break;

    case MP_BINARY:
      /* Try to read the atom window */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
           ( sscanf( line, "%[a-z]\n", str ) != 1 ) )
        {
          mp_error_msg( func, "Failed to scan the atom's window type.\n");
          windowType = DSP_UNKNOWN_WIN;
          return( 1 );
        }
      else
        {
          /* Convert the window type string */
          windowType = window_type( str );
        }
      /* Try to read the window option */
      if ( mp_fread( &windowOption,  sizeof(double), 1, fid ) != 1 )
        {
          mp_error_msg( func, "Failed to read the atom's window option.\n");
          windowOption = 0.0;
          return( 1 );
        }
      /* Try to read the freq */
      if ( mp_fread( &freq,  sizeof(MP_Real_t), 1 , fid ) != (size_t)1 )
        {
          mp_error_msg( func, "Failed to read the freq.\n" );
          freq = 0.0;
          return( 1 );
        }
      break;

    default:
      mp_error_msg( func, "Unknown read mode met in MP_Mdct_Atom_c( fid , mode )." );
      return( 1 );
      break;
    }

  return( 0 );
}


/**************/
/* Destructor */
MP_Mdct_Atom_Plugin_c::~MP_Mdct_Atom_Plugin_c()
{}

/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Mdct_Atom_Plugin_c::write( FILE *fid, const char mode )
{
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other MDCT-specific parameters */
  switch ( mode )
    {

    case MP_TEXT:
      /* Window name */
      nItem += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%g\"></window>\n", window_name(windowType), windowOption );
      /* print the freq*/
      nItem += fprintf( fid, "\t\t<par type=\"freq\">%g</par>\n",  (double)freq );
      break;

    case MP_BINARY:
      /* Window name */
      nItem += fprintf( fid, "%s\n", window_name(windowType) );
      /* Window option */
      nItem += mp_fwrite( &windowOption,  sizeof(double), 1, fid );
      /* Binary parameters */
      nItem += mp_fwrite( &freq,  sizeof(MP_Real_t), 1, fid );
      break;

    default:
      break;
    }

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

///** Get the identifying parameters of the block to use with sorted books
//  */
//
//map<string, string, mp_ltstring>* MP_Mdct_Atom_Plugin_c::get_block_param( void ){
//    char buffer[100];
//    map<string, string, mp_ltstring>* res = new map<string, string, mp_ltstring>;
//
//    res->insert(pair<const string,string>(string("name"), string("mdct")));
//
//    sprintf(buffer,"%lu",support[0].len);
//    res->insert(pair<const string,string>(string("filterLen"), string(buffer)));
//
//    sprintf(buffer,"%c",windowType);
//    res-> insert(pair<const string,string>(string("windowType"), string(buffer)));
//
//    sprintf(buffer, "%f", windowOption);
//    res->insert(pair<const string,string>(string("windowOption"), string(buffer)));
//
//    return res;
//}
   
/** Get the identifying parameters of the atom inside a block to use with sorted books
   */
    
MP_Atom_Param_c* MP_Mdct_Atom_Plugin_c::get_atom_param( void )const{
  //cerr << "Mdct_atom->get_atom_param\t" << "freq = " << freq << endl;
  return new MP_Freq_Atom_Param_c(freq);
}


/*************/
/* Type name */
const char * MP_Mdct_Atom_Plugin_c::type_name(void)
{
  return ("mdct");
}

/**********************/
/* Readable text dump */
int MP_Mdct_Atom_Plugin_c::info( FILE *fid )
{

  int nChar = 0;
  FILE* bakStream;
  void (*bakHandler)( void );

  /* Backup the current stream/handler */
  bakStream = get_info_stream();
  bakHandler = get_info_handler();
  /* Redirect to the given file */
  set_info_stream( fid );
  set_info_handler( MP_FLUSH );
  /* Launch the info output */
  nChar += info();
  /* Reset to the previous stream/handler */
  set_info_stream( bakStream );
  set_info_handler( bakHandler );

  return( nChar );
}

/**********************/
/* Readable text dump */
int MP_Mdct_Atom_Plugin_c::info()
{

  unsigned int i = 0;
  int nChar = 0;

  nChar += mp_info_msg( "MDCT ATOM", "%s window (window opt=%g)\n", window_name(windowType), windowOption );
  nChar += mp_info_msg( "        |-", "[%d] channel(s)\n", numChans );
  nChar += mp_info_msg( "        |-", "Freq %g\n", (double)freq);
  for ( i=0; i<numChans; i++ )
    {
      nChar += mp_info_msg( "        |-", "(%d/%d)\tSupport= %lu %lu\tAmp %g\n",
                            i+1, numChans, support[i].pos, support[i].len,
                            (double)amp[i]);
    }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Mdct_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer )
{
	dict->block[blockIdx]->build_atom_waveform_amp(this, outBuffer);
}

MP_Real_t wigner_ville(MP_Real_t t, MP_Real_t f, unsigned char windowType)
{

  static double factor = 1/MP_PI;
  double x = t-0.5;

  switch (windowType)
    {
    case DSP_GAUSS_WIN :
      return(factor*exp(-(x*x)/DSP_GAUSS_DEFAULT_OPT -f*f*DSP_GAUSS_DEFAULT_OPT));
      /* \todo : add pseudo wigner_ville for other windows */
    default :
      //
      //    fprintf ( stderr, "Warning : wigner_ville defaults to Gaussian one for window type [%s]\n", window_name(windowType));
      return(factor*exp(-(x*x)/DSP_GAUSS_DEFAULT_OPT -f*f*DSP_GAUSS_DEFAULT_OPT));
      //    return(1);
    }
}

/********************************************************/
/** Addition of the atom to a time-frequency map */

int MP_Mdct_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType )
{
  const char* func = "MP_Mclt_Atom_Plugin_c:add_to_tfmap(tfmap,type)";
  unsigned char chanIdx;
  unsigned long int tMin,tMax;
  MP_Real_t fMin,fMax,df;
  unsigned long int nMin,nMax,kMin,kMax;
  unsigned long int i, j;
  //  unsigned long int t; MP_Real_t f;
  MP_Real_t t;
  MP_Real_t f;

  MP_Tfmap_t *column;
  MP_Real_t val;

  assert(numChans == tfmap->numChans);

  for (chanIdx = 0; chanIdx < numChans; chanIdx++)
    {

      /* 1/ Is the atom support inside the tfmap ?
         (in real time-frequency coordinates) */
      /* Time interval [tMin tMax) that contains the time support: */
      tMin = support[chanIdx].pos;
      tMax = tMin + support[chanIdx].len;
      if ( (tMin > tfmap->tMax) || (tMax < tfmap->tMin) ) return( 0 );
      /* Freq interval [fMin fMax] that (nearly) contains the freq support : */
      df   = 40 / ( (MP_Real_t)(support[chanIdx].len) ); /* freq bandwidth */
      /** \todo: determine the right constant factor to replace '40' in the computation of the freq width of a Gabor atom*/

      fMin = freq - df/2;
      fMax = freq + df/2;


      if ( (fMin > tfmap->fMax) || (fMax < tfmap->fMin) ) return( 0 );

      mp_debug_msg( MP_DEBUG_ATOM, func, "Atom support in tf  coordinates: [%lu %lu]x[%g %g]\n",
                    tMin, tMax, fMin, fMax );
      //    mp_info_msg( func, "Atom support in tf  coordinates: [%lu %lu]x[%g %g]\n",
      //		  tMin, tMax, fMin, fMax );

      /* 2/ Clip the support if it reaches out of the tfmap */
      if ( tMin < tfmap->tMin ) tMin = tfmap->tMin;
      if ( tMax > tfmap->tMax ) tMax = tfmap->tMax;
      if ( fMin < tfmap->fMin ) fMin = tfmap->fMin;
      if ( fMax > tfmap->fMax ) fMax = tfmap->fMax;

      mp_debug_msg( MP_DEBUG_ATOM, func, "Atom support in tf  coordinates, after clipping: [%lu %lu]x[%g %g]\n",
                    tMin, tMax, fMin, fMax );
      //    mp_info_msg( func, "Atom support in tf  coordinates, after clipping: [%lu %lu]x[%g %g]\n",
      //		  tMin, tMax, fMin, fMax );

      /** \todo add a generic method MP_Atom_C::add_to_tfmap() that tests support intersection */

      /* 3/ Convert the real coordinates into pixel coordinates */
      nMin = tfmap->time_to_pix( tMin );
      nMax = tfmap->time_to_pix( tMax );
      kMin = tfmap->freq_to_pix( fMin );
      kMax = tfmap->freq_to_pix( fMax );

      if (nMax==nMin) nMax++;
      if (kMax==kMin) kMax++;

      mp_debug_msg( MP_DEBUG_ATOM, func, "Clipped atom support in pix coordinates [%lu %lu)x[%lu %lu)\n",
                    nMin, nMax, kMin, kMax );
      //    mp_info_msg( func, "Clipped atom support in pix coordinates [%lu %lu)x[%lu %lu)\n",
      //		  nMin, nMax, kMin, kMax );

      /* 4/ Fill the TF map: */
      switch ( tfmapType )
        {

          /* - with rectangles, with a linear amplitude scale: */
        case MP_TFMAP_SUPPORTS:
          for ( i = nMin; i < nMax; i++ )
            {
              column = tfmap->channel[chanIdx] + i*tfmap->numRows; /* Seek the column */
              for ( j = kMin; j < kMax; j++ )
                {
                  val = (MP_Real_t)(column[j]) + amp[chanIdx]*amp[chanIdx];
                  column[j] = (MP_Tfmap_t)( val );
                  /* Test the min/max */
                  if ( tfmap->ampMax < val ) tfmap->ampMax = val;
                  if ( tfmap->ampMin > val ) tfmap->ampMin = val;
                }
            }
          break;

          /* - with pseudo-Wigner, with a linear amplitude scale: */
        case MP_TFMAP_PSEUDO_WIGNER:
          for ( i = nMin; i < nMax; i++ )
            {
              column = tfmap->channel[chanIdx] + i*tfmap->numRows; /* Seek the column */
              //	t = tfmap->pix_to_time(i);
              //	if (nMax==nMin+1) t = tMin;
              t = ((MP_Real_t)tfmap->tMin)+(((MP_Real_t)i)*tfmap->dt);
              for ( j = kMin; j < kMax; j++ )
                {
                  f = tfmap->pix_to_freq(j);
                  if (kMax==kMin+1) f = fMin;
                  val = (MP_Real_t)(column[j]) +
                        amp[chanIdx]*amp[chanIdx]
                        * wigner_ville( ((double)(t - tMin)) / ((double)support[chanIdx].len),
                                        (f - freq ), windowType );
                  column[j] = (MP_Tfmap_t)( val );
                  /* Test the min/max */
                  if ( tfmap->ampMax < val ) tfmap->ampMax = val;
                  if ( tfmap->ampMin > val ) tfmap->ampMin = val;
                }
            }
          break;

        default:
          mp_error_msg( func, "Asked for an incorrect tfmap type.\n" );
          break;

        } /* End switch tfmapType */

    } /* End foreach channel */

  return( 0 );
}
/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   along one channel */
int MP_Mdct_Atom_Plugin_c::has_field( int field )
{

  if ( MP_Atom_c::has_field( field ) ) return ( MP_TRUE );
  else switch (field)
      {
      case MP_FREQ_PROP :
        return( MP_TRUE );
      case MP_WINDOW_TYPE_PROP :
        return( MP_TRUE );
      case MP_WINDOW_OPTION_PROP :
        return( MP_TRUE );
      default :
        return( MP_FALSE );
      }
}

MP_Real_t MP_Mdct_Atom_Plugin_c::get_field( int field, MP_Chan_t chanIdx )
{
	MP_Real_t x;
	if ( MP_Atom_c::has_field( field ) ) return ( MP_Atom_c::get_field(field,chanIdx) );
	else switch (field)
	{
	case MP_POS_PROP :
		x = (MP_Real_t)(support[chanIdx].pos);
		break;
	case MP_FREQ_PROP :
		x = freq;
		break;
	case MP_WINDOW_TYPE_PROP :
		x = (MP_Real_t) windowType;
		break;
	case MP_WINDOW_OPTION_PROP :
		x = windowOption;
		break;
	default :
		mp_warning_msg( "MP_Mdct_Atom_c::get_field()", "Unknown field: %d. Returning ZERO.\n",field );
		x = 0.0;
	}

	return( x );

}

/******************************************************/
/* Registration of new atom (s) in the atoms factory */

DLL_EXPORT void registry(void)
{
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom_empty("mdct",&MP_Mdct_Atom_Plugin_c::mdct_atom_create_empty);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom("mdct",&MP_Mdct_Atom_Plugin_c::create);

}
