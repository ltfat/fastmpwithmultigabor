/******************************************************************************/
/*                                                                            */
/*                                test_win.c                                  */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
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
 * $Author: sacha $
 * $Date: 2005/02/21 15:43:38 $
 * $Revision: 1.5 $
 *
 */

#include <stdio.h>
#include <dsp_windows.h>
#include <math.h>

/* 
 * BEGIN :
 * Extracted from LastWave 2.0.4 code for computing the Gaussian window 
 * with minor modifications
 */
//#define LW_Win_t float
#define LW_Win_t double
double theGaussianSigma2 = 0.02;  
void LWGauss(LW_Win_t *window,unsigned long int size)
{
  LW_Win_t i;
  unsigned long int   j;
  LW_Win_t x;
  LW_Win_t energy = 0.0;
  LW_Win_t factor;
  
  /* The window */
  for(i = 0.0, j=0; j < size; j++,i++) {
    x = (i-size/2)/size;
    window[j] = exp(-x*x/(2*theGaussianSigma2));
    energy += window[j]*window[j];
  }
  
  /* Putting the first point to zero */
  energy -= window[0]*window[0];
  window[0] = 0.0;
  
  /* Normalizing */
  factor = 1/sqrt(energy);
  for(j=0;j<size;j++) {
    window[j] *= factor;
  }
}

void LWExponential(LW_Win_t *window,unsigned long size)
{
  LW_Win_t decay = 1e4;
  unsigned long int j;
  LW_Win_t i;
  LW_Win_t energy = 0.0;
  LW_Win_t factor;
  LW_Win_t a;
  LW_Win_t expon;
  
  /* Damping factor */
  a = log(decay);
  expon = a/size;  /* scaled */
  
  /* The window */
  for(j=0, i=-1; j < size; j++, i++) {
    window[j] = exp(-expon*i);
    energy += window[j]*window[j];
  }
  
  /* Putting the first point to zero*/
  energy -= window[0]*window[0];
  window[0] = 0.0;
  
  /* Normalizing */
  factor = 1/sqrt(energy);
  
  for(j = 0; j < size; j++) {
    window[j] *= factor;
  }
}

LW_Win_t decayFoF = 1e5; /* 1e5 */ 
LW_Win_t betaFoF = M_PI/0.25; /* pi/0.25 */

void LWFoF(LW_Win_t *window,unsigned long int size)
{ 
  unsigned long int j;
  LW_Win_t limit;
  LW_Win_t i;
  LW_Win_t beta;
  LW_Win_t energy = 0.0;
  LW_Win_t factor;
  LW_Win_t a;
  LW_Win_t expon;
  
  a = log(decayFoF);
  expon = a/size; 
  beta = betaFoF/size;
  limit = M_PI/beta;
  for(j=0, i=0.0; i <= limit ; j++, i += 1.0) {
    window[j] = 0.5*(1-cos(beta*i))*exp(-expon*i);
    energy += window[j]*window[j];
  }
  
  for(; i < size; j++, i += 1.0) {
    window[j] = exp(-expon*i);
    energy += window[j]*window[j];
  }
  
  energy -= window[0]*window[0];
  window[0] = 0.0;
  
  factor = 1/sqrt(energy);
  for(j = 0; j < size; j++) {
    window[j] *= factor;
  }
}

/*
 * END
 */
void Test1(void) 
{
  unsigned char windowType;
  Dsp_Win_t out[1];
  unsigned long int retval;
  printf("Test 1 : generate one point windows for all types\n");
  for(windowType = DSP_RECTANGLE_WIN; windowType <= DSP_FOF_WIN; windowType++){
    retval = make_window(out,1,windowType,0.0);
    printf("One point window of type %d has center at %lu and value %g\n",
	   windowType,retval,out[0]);
  }
}

#define TEST_WIN_LENGTH 543
void Test2(unsigned long int length)
{
  unsigned char windowType;
  Dsp_Win_t out[TEST_WIN_LENGTH];
  double optional = 0.02;
  unsigned long int retval;
  unsigned long int i;
  double energy;

  printf("Test 2 : generate windows of size %lu for all types\n",length);
  printf("         and check that they are of unit energy\n");

  for(windowType = DSP_RECTANGLE_WIN; windowType <= DSP_FOF_WIN; windowType++){
    retval = make_window(out,length,windowType,optional);
    for (i=0, energy = 0.0; i< length; i++) {
      energy += (double) ( out[i]*out[i] );
    }
    printf("Window of type %d has center at %lu and energy %g\n",
	   windowType,retval,energy);
  }
}

void Test3(unsigned long int length,unsigned char windowType)
{
  LW_Win_t lwwin[TEST_WIN_LENGTH];
  char filename[256];
  FILE *fid;
  double optional = 0.02;
  Dsp_Win_t out[TEST_WIN_LENGTH];
  unsigned long int retval;
  unsigned long int i;

  printf("Test 3 : compare some windows with those of LastWave\n");
  
  switch(windowType) {
  case DSP_GAUSS_WIN :
    LWGauss(lwwin,length);
    sprintf(filename,"gauss%d_window_test.flt",(int)length);
    printf("Gaussian window of size %lu\n",length);
    optional = 0.02;
    break;
    printf("Exponential window of size %lu\n",length);
  case DSP_EXPONENTIAL_WIN :
    LWExponential(lwwin,length);
    sprintf(filename,"expon%d_window_test.flt",(int)length);
    printf("Exponential window of size %lu\n",length);
    optional = log(1e4);
    break;
  case DSP_FOF_WIN :
    LWFoF(lwwin,length);
    sprintf(filename,"fof%d_window_test.flt",(int)length);
    printf("FoF window of size %lu\n",length);
    break;
  default :
    printf("Error : bad window type for this test\n");
    return;
  }

  fid = fopen( filename, "w" );
  fwrite( lwwin, sizeof(LW_Win_t), length, fid );
  fclose(fid);

  retval = make_window( out, length-1, windowType, optional);
  printf("lw\t\tmplib\t\tdiff\n");
  printf("%g\t\t%g\n",(double)lwwin[0],(double)0.0);
  for (i= 0; i < length-1; i++) {
    /*    out[i] -= lwwin[i+1];*/
    printf("%g\t%g\t%g\n",(double)lwwin[i+1],(double)out[i],(double)lwwin[i+1]-(double)out[i]);
  }
}

void Test4(void)
{
  Dsp_Win_t out[TEST_WIN_LENGTH];
  unsigned long int retval;
  FILE *fid;

  printf("Test 4 : generate a more or less arbitrary window\n");
  retval = make_window( out, TEST_WIN_LENGTH, DSP_FOF_WIN, 0.0 );

  fid = fopen( "window_test.out", "w" );
  fwrite( out, sizeof(Dsp_Win_t), TEST_WIN_LENGTH, fid );
  fclose(fid);

  printf("%d points output to window_test.out . Center location is [%lu].\n",
	 TEST_WIN_LENGTH, retval );
}


int main( void ) {

  Test1();
  Test2(10);
  Test2(11);
  //  Test3(4,DSP_GAUSS_WIN); // These windows match those of Lastwave up to numerical accuracy
  //  Test3(8,DSP_GAUSS_WIN);
  //  Test3(16,DSP_GAUSS_WIN);
  //  Test3(32,DSP_GAUSS_WIN);
  //  Test3(4,DSP_EXPONENTIAL_WIN); // These windows are different from those of LastWave
  //  Test3(8,DSP_EXPONENTIAL_WIN); 
  //  Test3(16,DSP_EXPONENTIAL_WIN);
  //  Test3(32,DSP_EXPONENTIAL_WIN);
  Test3(4,DSP_FOF_WIN); // These windows match those of Lastwave up to numerical accuracy
  Test3(8,DSP_FOF_WIN);
  Test3(16,DSP_FOF_WIN);
  Test3(32,DSP_FOF_WIN);
  Test4();
  return(0);
}
