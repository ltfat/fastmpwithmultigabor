/******************************************************************************/
/*                                                                            */
/*                              gabor_atom.cpp                                */
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


/*************************************************/
/*                                               */
/* gabor_atom.cpp: methods for gabor atoms       */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "gabor_atom_plugin.h"
#include "harmonic_atom_plugin.h"
#include "mp_system.h"

#include <dsp_windows.h>

using namespace std;


/*************/
/* CONSTANTS */
/*************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************************/
/* Factory empty atom function     */
MP_Atom_c* MP_Gabor_Atom_Plugin_c::gabor_atom_create_empty(MP_Dict_c* dict)
{

  return new MP_Gabor_Atom_Plugin_c(dict);

}

/**************************/
/* File factory function */
MP_Atom_c* MP_Gabor_Atom_Plugin_c::create_fromxml( TiXmlElement *xmlobj, MP_Dict_c *dict)
{
  const char* func = "MP_Gabor_Atom_c::create_fromxml(fid,mode)";
  MP_Gabor_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Gabor_Atom_Plugin_c(dict);
  if ( newAtom == NULL )
    {
      mp_error_msg( func, "Failed to create a new atom.\n" );
      return( NULL );
    }

	// Read and check
	if ( newAtom->init_fromxml( xmlobj ) )
	{
		mp_error_msg( func, "Failed to read the new Gabor atom.\n" );
		delete( newAtom );
		return( NULL );
	}

	return newAtom;
}
MP_Atom_c* MP_Gabor_Atom_Plugin_c::create_frombinary( FILE *fid, MP_Dict_c *dict)
{
  const char* func = "MP_Gabor_Atom_c::create_frombinary(fid,mode)";
  MP_Gabor_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Gabor_Atom_Plugin_c(dict);
  if ( newAtom == NULL )
    {
      mp_error_msg( func, "Failed to create a new atom.\n" );
      return( NULL );
    }

  /* Read and check */
  if ( newAtom->init_frombinary( fid ) )
    {
      mp_error_msg( func, "Failed to read the new Gabor atom.\n" );
      delete( newAtom );
      return( NULL );
    }

  return( (MP_Atom_c*)newAtom );
}

/********************/
/* Void constructor */
MP_Gabor_Atom_Plugin_c::MP_Gabor_Atom_Plugin_c( MP_Dict_c* dict ):MP_Atom_c(dict)
{
  windowType = DSP_UNKNOWN_WIN;
  windowOption = 0.0;
  freq  = 0.0;
  chirp = 0.0;
  phase = NULL;
}


/************************/
/* Local allocations    */
int MP_Gabor_Atom_Plugin_c::alloc_gabor_atom_param( const MP_Chan_t setNumChans )
{

  const char* func = "MP_Gabor_Atom_c::local_alloc(numChans)";

  /* phase */
  if ( (phase = (MP_Real_t*)calloc( setNumChans, sizeof(MP_Real_t)) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate the phase array.\n" );
      return( 1 );
    }

  return( 0 );
}

/********************/
/* File reader      */
int MP_Gabor_Atom_Plugin_c::init_fromxml(TiXmlElement* xmlobj)
{
  const char* func = "MP_Gabor_Atom_c::read(fid,mode)";

  /* Go up one level */
  if ( MP_Atom_c::init_fromxml( xmlobj ) )
    {
      mp_error_msg( func, "Reading of Gabor atom fails at the generic atom level.\n" );
      return( 1 );
    }

  /* Alloc at local level */
  if ( MP_Gabor_Atom_Plugin_c::alloc_gabor_atom_param( numChans ) )
    {
      mp_error_msg( func, "Allocation of Gabor atom failed at the local level.\n" );
      return( 1 );
    }

  /* Then read this level's info */

  // First, MONOPHONIC FEATURES
  // Iterate children and discover:
  TiXmlNode* kid = 0;
  TiXmlElement* kidel;
  const char* datatext;
  while((kid = xmlobj->IterateChildren(kid))){
    kidel = kid->ToElement();
    if(kidel != NULL){
      // window[type=x][opt=x]
      if(strcmp(kidel->Value(), "window")==0){
        datatext = kidel->Attribute("type");
        if(datatext != NULL){
	  windowType = window_type(datatext);
        }
        else{ mp_error_msg(func, "datatext==NULL in window[type]\n"); return 1; }
        datatext = kidel->Attribute("opt");
        if(datatext != NULL){
	  windowOption = strtod(datatext, NULL);
        }
        else{ mp_error_msg(func, "datatext==NULL in window[opt]\n"); return 1; }
      }
      // par[type=freq]
      if((strcmp(kidel->Value(), "par")==0) && (strcmp(kidel->Attribute("type"), "freq")==0)){
        datatext = kidel->GetText();
        if(datatext != NULL){
	  freq = strtod(datatext, NULL);
        }
        else{ mp_error_msg(func, "datatext==NULL in freq\n"); return 1; }
      }
      // par[type=chirp]
      if((strcmp(kidel->Value(), "par")==0) && (strcmp(kidel->Attribute("type"), "chirp")==0)){
        datatext = kidel->GetText();
        if(datatext != NULL){
	  chirp = strtod(datatext, NULL);
        }
        else{ mp_error_msg(func, "datatext==NULL in chirp\n"); return 1; }
      }
    }
  }

  if(windowType == DSP_UNKNOWN_WIN){
    mp_error_msg( func, "Failed to read the window type and/or option in a Gabor atom structure.\n");
    return( 1 );
  }

  // Then, MULTICHANNEL FEATURES
  // Iterate children and:
  //      if item is par[type=phase][chan=x] then store that
  kid = 0;
  int count_phase=0;
  while((kid = xmlobj->IterateChildren("par", kid))){
    kidel = kid->ToElement();
    if(kidel != NULL){
      //      if item is par[type=phase][chan=x] then store that
      if((strcmp(kidel->Value(), "par")==0) && (strcmp(kidel->Attribute("type"), "phase")==0)){
        ++count_phase;
        // Get the channel, and check bounds (could cause bad mem writes otherwise)
	datatext = kidel->Attribute("chan");
        long chan = strtol(datatext, NULL, 0);
        if((chan<0) || (chan >= numChans)){
            mp_error_msg( func, "Found a <par type='amp'> tag with channel number %i, which is outside the channel range for this atom [0,%i).\n", chan, numChans);
            return( 1 );
        }
	datatext = kidel->GetText();
	phase[chan] = strtod(datatext, NULL);
      }
    }
  }

  if(count_phase != numChans){
    mp_error_msg( func, "Scanned an atom with %i channels, but failed to get that number of 'phase' values (%i).\n",
		numChans, count_phase);
    return( 1 );
  }

  return 0;
}

int MP_Gabor_Atom_Plugin_c::init_frombinary( FILE *fid )
{
  const char* func = "MP_Gabor_Atom_c::read(fid,mode)";
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  //double fidFreq,fidChirp,fidPhase;
  //MP_Chan_t i, iRead;
  /* Go up one level */
  if ( MP_Atom_c::init_frombinary( fid ) )
    {
      mp_error_msg( func, "Reading of Gabor atom fails at the generic atom level.\n" );
      return( 1 );
    }

  /* Alloc at local level */
  if ( MP_Gabor_Atom_Plugin_c::alloc_gabor_atom_param( numChans ) )
    {
      mp_error_msg( func, "Allocation of Gabor atom failed at the local level.\n" );
      return( 1 );
    }

  /* Then read this level's info */
      /* Window type */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
           ( sscanf( line, "%[a-z]\n", str ) != 1 ) )
        {
          mp_error_msg( func, "Failed to scan the atom's window type.\n");
          return( 1 );
        }
      else
        {
          /* Convert the window type string */
          windowType = window_type( str );
        }
      /* Window option */
      if ( mp_fread( &windowOption,  sizeof(double), 1, fid ) != 1 )
        {
          mp_error_msg( func, "Failed to read the atom's window option.\n");
          return( 1 );
        }
      /* Try to read the freq, chirp, phase */
      if ( mp_fread( &freq,  sizeof(MP_Real_t), 1 , fid ) != (size_t)1 )
        {
          mp_error_msg( func, "Failed to read the freq.\n" );
          return( 1 );
        }
      if ( mp_fread( &chirp, sizeof(MP_Real_t), 1, fid ) != (size_t)1 )
        {
          mp_error_msg( func, "Failed to read the chirp.\n" );
          return( 1 );
        }
      if ( mp_fread( phase, sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans )
        {
          mp_error_msg( func, "Failed to read the phase array.\n" );
          return( 1 );
        }

  return( 0 );
}


/**************/
/* Destructor */
MP_Gabor_Atom_Plugin_c::~MP_Gabor_Atom_Plugin_c()
{

  if (phase) free( phase );
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Gabor_Atom_Plugin_c::write( FILE *fid, const char mode )
{

  MP_Chan_t i;
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other Gabor-specific parameters */
  switch ( mode )
    {

    case MP_TEXT:
      /* Window name */
      nItem += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%g\"></window>\n",
                        window_name(windowType), windowOption );
      /* print the freq, chirp, phase */
      nItem += fprintf( fid, "\t\t<par type=\"freq\">%g</par>\n",  (double)freq );
      nItem += fprintf( fid, "\t\t<par type=\"chirp\">%g</par>\n", (double)chirp );
      for (i = 0; i<numChans; i++)
        {
          nItem += fprintf(fid, "\t\t<par type=\"phase\" chan=\"%u\">%lg</par>\n", i, (double)phase[i]);
        }
      break;

    case MP_BINARY:
      /* Window name */
      nItem += fprintf( fid, "%s\n", window_name(windowType) );
      /* Window option */
      nItem += (int)mp_fwrite( &windowOption,  sizeof(double), 1, fid );
      /* Binary parameters */
      nItem += (int)mp_fwrite( &freq,  sizeof(MP_Real_t), 1, fid );
      nItem += (int)mp_fwrite( &chirp, sizeof(MP_Real_t), 1, fid );
      nItem += (int)mp_fwrite( phase, sizeof(MP_Real_t), numChans, fid );
      break;

    default:
      break;
    }

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/*************/
/* Type name */
const char * MP_Gabor_Atom_Plugin_c::type_name(void)
{
  return ("gabor");
}

/**********************/
/* Readable text dump */
int MP_Gabor_Atom_Plugin_c::info( FILE *fid )
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
int MP_Gabor_Atom_Plugin_c::info()
{
	unsigned int i = 0;
	int nChar = 0;

	nChar += (int)mp_info_msg( "GABOR ATOM", "%s window (window opt=%g)\n", window_name(windowType), windowOption );
	nChar += (int)mp_info_msg( "        |-", "[%d] channel(s)\n", numChans );
	nChar += (int)mp_info_msg( "        |-", "Freq %g\tChirp %g\n", (double)freq, (double)chirp);
	for ( i=0; i<numChans; i++ )
    {
		nChar += (int)mp_info_msg( "        |-", "(%d/%d)\tSupport= %lu %lu\tAmp %g\tPhase %g\n", i+1, numChans, support[i].pos, support[i].len, (double)amp[i], (double)phase[i] );
	}
	
	return( nChar );
}

/********************/
/* Waveform builder */
void MP_Gabor_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer )
{

  MP_Real_t *window = NULL;
  MP_Real_t *atomBuffer = NULL;
  /*unsigned long int windowCenter = 0;*/
  /* Parameters for the atom waveform : */
  MP_Chan_t chanIdx;
  unsigned int t;
  unsigned long int len;
  double dHalfChirp, dAmp, dFreq, dPhase, dT;
  double argument;

  assert( outBuffer != NULL );

  for ( chanIdx = 0 , atomBuffer = outBuffer;
        chanIdx < numChans;
        chanIdx++  )
    {
      /* Dereference the atom length in the current channel once and for all */
      len = support[chanIdx].len;

      /** make the window */
      /*windowCenter = */ MPTK_Server_c::get_win_server()->get_window( &window, len, windowType, windowOption );
      assert( window != NULL );

      /* Dereference the arguments once and for all */
      dHalfChirp = (double)( chirp ) * MP_PI;
      dFreq      = (double)(  freq ) * MP_2PI;
      dPhase     = (double)( phase[chanIdx] );
      dAmp       = (double)(   amp[chanIdx] );

      /** Multiply by the desired modulation to get
       * \f[
       * \mbox{window}(t) \cdot \mbox{amp} \cdot 
       * \cos\left(2\pi \left (\mbox{chirp} \cdot \frac{t^2}{2}
       *      + \mbox{freq} \cdot t\right)+ \mbox{phase}\right)
       * \f]
       */
      for ( t = 0; t<len; t++ )
        {

          /* Compute the cosine's argument */
          dT = (double)(t);
          argument = (dHalfChirp*dT + dFreq)*dT + dPhase;
          /* The above does:
          argument = dHalfChirp*dT*dT + dFreq*dT + dPhase
          but saves a multiplication.
          */
          /* Compute the waveform samples */
          *(atomBuffer+t) = (MP_Real_t)( (double)(*(window+t)) * dAmp * cos(argument) );

        }

      /* Go to the next channel */
      atomBuffer += len;
    }

}

#ifdef LASTWAVE
/*
 * The Wigner-Ville distribution of a Gaussian atom is
 *  GWV(2*(u/2^o)) x GWV(2*(2*pi*\sigma^2* k*2^o/GABOR_MAX_FREQID))
 *
 *  GWV(x) = e^{-x^2/4\sigma^2}
 */
static float *GaussianWignerVille(int sizeGWV)
{
  float *GWV;
  float c = 1/(4*theGaussianSigma2*sizeGWV*sizeGWV);
  int i;

  /* Allocation */
  if ((GWV = FloatAlloc(sizeGWV)) == NULL)
    Errorf("GaussianWignerVilleTime : Mem. Alloc. failed!");

  /* Computation of e^{-x^2/\sigma^2}, x = i/sizeGWV*/
  for (i = 0; i < sizeGWV; i++)
    {
      GWV[i] = exp(-c*i*i);
    }
  return(GWV);
}

/*
 * The Wigner-Ville distribution of a FoF atom is
 *  FWV(2*(u/2^o)) x GWV(2*(2*pi*\sigma^2* k*2^o/GABOR_MAX_FREQID))
 *
 *  FWV(x) = ???
 */
static float *FoFWignerVille(int sizeFWV)
{
  float *FWV;
  float a,expon,beta,max;
  int i;

  /* Allocation */
  if ((FWV = FloatAlloc(sizeFWV)) == NULL)
    Errorf("FoFWignerVilleTime : Mem. Alloc. failed!");

  /* Damping factor */
  a = log(decayFoF);

  /* scale */
  expon = a/sizeFWV;
  beta = betaFoF/sizeFWV;

  max=0.0;

  /* Computation of FoF window */
  for (i = 0; i <= M_PI/beta; i++)
    {
      FWV[i] = 0.5*(1-cos(beta*i))*exp(-expon*i);
      if (FWV[i] > max) max=FWV[i];
    }
  for (; i<sizeFWV; i++)
    {
      FWV[i] = exp(-expon*i);
      if (FWV[i] > max) max=FWV[i];
    }

  /* Normalizing to the maximum */
  for (i = 0; i <sizeFWV; i++)
    {
      FWV[i]=FWV[i]/max;
    }
  return(FWV);
}
#endif

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
int MP_Gabor_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType )
{

  const char* func = "MP_Gabor_Atom_c::add_to_tfmap(tfmap,type)";
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
      if (chirp >= 0.0)
        {
          fMin = freq - df/2;
          fMax = freq + df/2 + chirp*(tMax-tMin);
        }
      else
        {
          fMax = freq + df/2;
          fMin = freq - df/2 + chirp*(tMax-tMin);
        }
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
                                        (f - freq - chirp*(t-(MP_Real_t)tMin)) * (MP_Real_t)support[chanIdx].len,
                                        windowType );
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

MP_Real_t MP_Gabor_Atom_Plugin_c::dist_to_tfpoint( MP_Real_t time, MP_Real_t freq , MP_Chan_t chanIdx )
{

  MP_Real_t duration,tcenter,fcenter,deltat,deltaf,a2,b2;

  /* Compute distance to current atom */
  duration = support[chanIdx].len;
  tcenter  = (float)(support[chanIdx].pos) +0.5*duration;
  fcenter  = freq+chirp*0.5*duration;
  deltat = (time-tcenter);
  deltaf = (freq-fcenter);
  a2 = (deltat+chirp*deltaf)*(deltat+chirp*deltaf)/(1+chirp*chirp);
  b2 = (-chirp*deltat+deltaf)*(-chirp*deltat+deltaf)/(1+chirp*chirp);
  //	dist = a2/(duration*duration)+b2*duration*duration;
  return(a2+b2);
}


/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   along one channel */
int MP_Gabor_Atom_Plugin_c::has_field( int field )
{

  if ( MP_Atom_c::has_field( field ) ) return ( MP_TRUE );
  else switch (field)
      {
      case MP_FREQ_PROP :
        return( MP_TRUE );
      case MP_PHASE_PROP :
        return( MP_TRUE );
      case MP_CHIRP_PROP :
        return( MP_TRUE );
      case MP_WINDOW_TYPE_PROP :
        return( MP_TRUE );
      case MP_WINDOW_OPTION_PROP :
        return( MP_TRUE );
      default :
        return( MP_FALSE );
      }
}

MP_Real_t MP_Gabor_Atom_Plugin_c::get_field( int field, MP_Chan_t chanIdx )
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
      case MP_PHASE_PROP :
        x = phase[chanIdx];
        break;
      case MP_CHIRP_PROP :
        x = chirp;
        break;
      case MP_WINDOW_TYPE_PROP :
	x = (MP_Real_t) windowType;
	break;
      case MP_WINDOW_OPTION_PROP :
	x = windowOption;
	break;
      default :
        mp_warning_msg( "MP_Gabor_Atom_Plugin_c::get_field()", "Unknown field: %d. Returning ZERO.\n",field );
        x = 0.0;
      }

  return( x );

}



/******************************************************/
/* Registration of new atom (s) in the atoms factory */


DLL_EXPORT void registry(void)
{
  const char *func = "registry (gabor_atom_plugin.cpp)";
  mp_debug_msg( MP_DEBUG_FUNC_ENTER, func, "Entering\n" );
  MP_Atom_Factory_c::register_new_atom_empty("gabor",&MP_Gabor_Atom_Plugin_c::gabor_atom_create_empty);
  MP_Atom_Factory_c::register_new_atom("gabor",&MP_Gabor_Atom_Plugin_c::create_fromxml,&MP_Gabor_Atom_Plugin_c::create_frombinary);
  MP_Atom_Factory_c::register_new_atom_empty("harmonic",&MP_Harmonic_Atom_Plugin_c::harmonic_atom_create_empty);
  MP_Atom_Factory_c::register_new_atom("harmonic",&MP_Harmonic_Atom_Plugin_c::create_fromxml,&MP_Harmonic_Atom_Plugin_c::create_frombinary);
  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Leaving\n" );
}

