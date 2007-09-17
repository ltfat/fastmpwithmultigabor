/******************************************************************************/
/*                                                                            */
/*                               dsp_windows.c                                */
/*                                                                            */
/*                    Digital Signal Processing Windows                       */
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

#include <dsp_windows.h>
#include <math.h>
#include <string.h>
#include "mp_system.h"


/* Useful constants */
#define DSP_WIN_PI  3.14159265358979323846
#define DSP_WIN_2PI 6.28318530717958647692
#define DSP_EXPON_DEFAULT_DECAY 1e4


/*------------------------*/
/* Function make_window() */
/*------------------------*/
unsigned long int make_window( Dsp_Win_t *out,
			       const unsigned long int length,
			       const unsigned char type,
			       double optional ) {

  unsigned long int i;
  unsigned long int centerPoint = 0;
  double k;
  double newPoint;
  double energy = 0;
  unsigned long int FoFLimit;
  double factor;
  double sumvalue = 0.0;
  Dsp_Win_t *p1, *p2; /** The address of two points of the window,
			  symmetrically located around its center,
			  used to build only half of it when it is symmetric */

  assert(out != NULL);
  assert(length>0);

  /**************************/
  /* 1) Generate the window */

  p1 = out;
  p2 = out + length - 1;

  /* One point window */
  if (length==1) {
    out[0] = 1.0;
    centerPoint = 0;
    return(centerPoint);
  }
  
  switch (type) {

    /*****************/
    /* Basic windows */
    /*****************/


    /* Rectangular window */
  case DSP_RECTANGLE_WIN:
    for ( i = 0; i <length; i++ ) {
      *(out+i) = (Dsp_Win_t)( 1.0 );
    }
    energy = (double)(length);
    /* Locate the center point */
    centerPoint = (length-1)>>1;
    break;


    /* Triangular window */
  case DSP_TRIANGLE_WIN:
    for ( i = 0;             /* -> The window is symmetric, */
	  i < (length >> 1); /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      newPoint = (double)(i);
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = (double)(i);
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );      
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /**************************/
    /* Cosine related windows */
    /**************************/


    /* Cosine window */
  case DSP_COSINE_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      newPoint = sin( DSP_WIN_PI * (double)i / (double)(length-1) );
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = sin( DSP_WIN_PI * (double)i / (double)(length-1) );
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;

    /* Princen-bradley Cosine window */
  case DSP_PBCOSINE_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      newPoint = sin( DSP_WIN_PI * ((double)i+0.5) / (double)(length) );
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = sin( DSP_WIN_PI * ((double)i+0.5) / (double)(length) );
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;
    
    /* Hanning window */
  case DSP_HANNING_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      newPoint = 0.5 - 0.5 * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = 0.5 - 0.5 * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /* Hamming window */
  case DSP_HAMMING_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      newPoint = 0.54 - 0.46 * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      /* newPoint = 0.54 - 0.46 * cos( DSP_WIN_2PI * (double)((length>>1)-i) / (double)(length-1) ); */
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = 0.54 - 0.46 * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /* Generalized Hamming window */
  case DSP_HAMGEN_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      /* The alpha parameter is given by the optional argument. */
      newPoint = optional + (optional-1.0) * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = optional + (optional-1.0) * cos( DSP_WIN_2PI * (double)i / (double)(length-1) );
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /* Blackman window */
  case DSP_BLACKMAN_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      k = DSP_WIN_2PI * (double)i / (double)(length-1);
      newPoint = 0.42 - 0.5*cos(k) + 0.08*cos(2*k);
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      k = DSP_WIN_2PI * (double)i / (double)(length-1);
      newPoint = 0.42 - 0.5*cos(k) + 0.08*cos(2*k);
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /* Flat-top window */
  case DSP_FLATTOP_WIN:
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      k = DSP_WIN_2PI * (double)i / (double)(length-1);
      newPoint = 0.2156 - 0.4160*cos( k ) + 0.2781*cos(2*k) - 0.0836*cos(3*k) + 0.0069*cos(4*k);
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      k = DSP_WIN_2PI * (double)i / (double)(length-1);
      newPoint = 0.2156 - 0.4160*cos( k ) + 0.2781*cos(2*k) - 0.0836*cos(3*k) + 0.0069*cos(4*k);
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;


    /*****************************/
    /* Other symmetrical windows */
    /*****************************/


    /* Gaussian window */
  case DSP_GAUSS_WIN:
    if (optional < 0) {
      fprintf( stderr, "Width of Gaussian window should be strictly positive  in make_window.\n" );
      return(0);
      break;
    }
    if (optional == 0.0) optional = DSP_GAUSS_DEFAULT_OPT;

    optional = 1/(2*optional*(length+1)*(length+1));
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      k = (double)i-((double)(length-1))/2.0;
      newPoint = exp(-k*k*optional);
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      k = (double)i-((double)(length-1))/2.0;
      newPoint = exp(-k*k*optional);
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;

    /* Kaiser Bessel Derived window */
  case DSP_KBD_WIN:
    if (optional < 0) {
      fprintf( stderr, "The parameter of KBD window should be positive  in make_window.\n" );
      return(0);
      break;
    }
    
    for ( i = 0;            /* -> The window is symmetric, */
	  i < (length>>1);  /*    compute only half of it. */
	  i++, p1++, p2-- ) {
      sumvalue += BesselI0(DSP_WIN_PI * optional * sqrt(1.0 - pow(4.0*i/length - 1.0, 2)));
      newPoint = sqrt(sumvalue);
      *p2 = *p1 = (Dsp_Win_t)newPoint;
      energy += ( 2 * newPoint * newPoint );
    }
    /* If the length is odd, add the missing center point */
    if ( (length%2) == 1 ) {
      newPoint = BesselI0(DSP_WIN_PI * optional * sqrt(1.0 - pow(4.0*i/length - 1.0, 2)));
      *p1 = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = (length-1) >> 1;
    break;

    /************************/
    /* Asymmetrical windows */
    /************************/


    /* Exponential window */
  case DSP_EXPONENTIAL_WIN:
    /* The decay parameter is given by the optional argument. */
    if (optional < 0.0) {
      fprintf( stderr, "Decay of exponential window should be strictly positive  in make_window.\n" );
      return(0);
      break;
    }
    if (optional == 0.0) optional = DSP_EXPONENTIAL_DEFAULT_OPT;

    optional /=  (double)(length-1);
    for ( i = 0; i < length; i++ ) {
      newPoint = exp( - (double)optional * (double)i );
      *(out+i) = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    centerPoint = 0;
    break;


    /* FOF window */
  case DSP_FOF_WIN:
#define DSP_FOF_DECAY 1e5
    FoFLimit = (unsigned long int)(((double)length+1)/4);
    optional = log( DSP_FOF_DECAY )  / (double)(length+1);
    factor = DSP_WIN_PI*4/((double)length+1);
    for (i = 0; i < FoFLimit; i++ ) {
      newPoint = 0.5 * 
	( 1.0 - cos( factor * (double)(i+1) ) ) * 
	exp( - (double)optional * (double)(i+1) );
      *(out+i) = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    for( ; i < length; i++ ) {
      newPoint = exp( - (double)optional * (double)(i+1) );
      *(out+i) = (Dsp_Win_t)newPoint;
      energy += ( newPoint * newPoint );
    }
    /* Locate the center point (= the first maximum of the window) */
    {
      Dsp_Win_t max;
      for ( i=0, max=*out; i<length; i++ ) {
	if ( (*(out+i)) > max ) { max = *(out+i); centerPoint = i; }
      }
    }
    break;


    /* Unknown window */
  default:
    fprintf( stderr, "Unknown window type encountered in make_window.\n" );
    return(0);
    break;

  }
  /* End switch */


  /***************************/
  /* 2) Normalize the window */

  factor = 1/sqrt(energy);
  for ( i=0; i<length; i++ ) {
    *(out+i) = (Dsp_Win_t)( (double)(*(out+i)) * factor );
  }


  return( centerPoint );
}


unsigned char window_type_is_ok(const unsigned char type) {
  switch (type) {
  case DSP_RECTANGLE_WIN :
  case DSP_TRIANGLE_WIN :
  case DSP_COSINE_WIN :
  case DSP_PBCOSINE_WIN :
  case DSP_HANNING_WIN :
  case DSP_HAMMING_WIN :
  case DSP_HAMGEN_WIN :
  case DSP_BLACKMAN_WIN :
  case DSP_FLATTOP_WIN :
  case DSP_GAUSS_WIN :
  case DSP_EXPONENTIAL_WIN :
  case DSP_FOF_WIN :
  case DSP_KBD_WIN :
    return(type);
    break;
  default :
    return(DSP_UNKNOWN_WIN);
    break;
  }
}

unsigned char window_needs_option(const unsigned char type) {
  switch (type) {
    /* These do not need the optional parameter: */
  case DSP_RECTANGLE_WIN :
  case DSP_TRIANGLE_WIN :
  case DSP_COSINE_WIN :
  case DSP_PBCOSINE_WIN :
  case DSP_HANNING_WIN :
  case DSP_HAMMING_WIN :
  case DSP_BLACKMAN_WIN :
  case DSP_FLATTOP_WIN :
  case DSP_FOF_WIN :
    return(0==1);
    break;
    /* These do need the optional parameter: */
  case DSP_HAMGEN_WIN :
  case DSP_GAUSS_WIN :
  case DSP_EXPONENTIAL_WIN :
  case DSP_KBD_WIN :
    return(0==0);
    break;
    /* An unknown window doesn't need anything: */
  default :
    return(0==1);
    break;
  }
}

unsigned char window_type(const char * name) {
  if (!strcmp(name,"rectangle"))
    return DSP_RECTANGLE_WIN;
  else if (!strcmp(name,"triangle"))
    return DSP_TRIANGLE_WIN;
  else if (!strcmp(name,"cosine"))
    return DSP_COSINE_WIN;
  else if (!strcmp(name,"pbcosine"))
    return DSP_PBCOSINE_WIN;
  else if (!strcmp(name,"hanning"))
    return DSP_HANNING_WIN;
  else if (!strcmp(name,"hamming"))
    return DSP_HAMMING_WIN;
  else if (!strcmp(name,"hamgen"))
    return DSP_HAMGEN_WIN;
  else if (!strcmp(name,"blackman"))
    return DSP_BLACKMAN_WIN;
  else if (!strcmp(name,"flattop"))
    return DSP_FLATTOP_WIN;
  else if (!strcmp(name,"gauss"))
    return DSP_GAUSS_WIN;
  else if (!strcmp(name,"exponential"))
    return DSP_EXPONENTIAL_WIN;
  else if (!strcmp(name,"fof"))
    return DSP_FOF_WIN;
  else if (!strcmp(name,"kbd"))
    return DSP_KBD_WIN;
  else 
    return DSP_UNKNOWN_WIN;
}


char * window_name(const unsigned char type) {
  switch (type) {
  case DSP_RECTANGLE_WIN :
    return("rectangle");
    break;
  case DSP_TRIANGLE_WIN :
    return("triangle");
    break;
  case DSP_COSINE_WIN :
    return("cosine");
    break;
  case DSP_PBCOSINE_WIN :
    return("pbcosine");
    break;
  case DSP_HANNING_WIN :
    return("hanning");
    break;
  case DSP_HAMMING_WIN :
    return("hamming");
    break;
  case DSP_HAMGEN_WIN :
    return("hamgen");
    break;
  case DSP_BLACKMAN_WIN :
    return("blackman");
    break;
  case DSP_FLATTOP_WIN :
    return("flattop");
    break;
  case DSP_GAUSS_WIN :
    return("gauss");
    break;
  case DSP_EXPONENTIAL_WIN :
    return("exponential");
    break;
  case DSP_FOF_WIN :
    return("fof");
    break;
  case DSP_KBD_WIN :
    return("kbd");
    break;
  default :
    return(NULL);
    break;
  }
}

double BesselI0(double x) {
   double denominator;
   double numerator;
   double z;

   if (x == 0.0) {
      return 1.0;
   } else {
      z = x * x;
      numerator = (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* 
                     (z* 0.210580722890567e-22  + 0.380715242345326e-19 ) +
                         0.479440257548300e-16) + 0.435125971262668e-13 ) +
                         0.300931127112960e-10) + 0.160224679395361e-7  ) +
                         0.654858370096785e-5)  + 0.202591084143397e-2  ) +
                         0.463076284721000e0)   + 0.754337328948189e2   ) +
                         0.830792541809429e4)   + 0.571661130563785e6   ) +
                         0.216415572361227e8)   + 0.356644482244025e9   ) +
                         0.144048298227235e10);

      denominator = (z*(z*(z-0.307646912682801e4)+
                       0.347626332405882e7)-0.144048298227235e10);
   }

   return -numerator/denominator;
}
