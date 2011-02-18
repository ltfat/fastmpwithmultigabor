/******************************************************************************/
/*                                                                            */
/*                           test_convolution.cpp                             */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Mar 14 2006 */
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
 * $Author: remi $
 * $Date: 2006-03-23 14:57:33 +0100 (Thu, 23 Mar 2006) $
 * $Revision: 545 $
 *
 */

/** \file test_convolution.cpp
 * 	INFORMATION : the datas tested by test_convolution.cpp (anywave_results_1.bin, 
 *  anywave_results_2.bin and anywave_results_3.bin) are written by a matlab 
 * 	script file named "prepareConvolution.m" situated under matlab/utils
 */
#include <mptk.h>
#include <stdio.h>
#include <stdlib.h>

double relative_error( double a, double b) 
{
	double c;
	double d;
	
	c = a-b;
	if (c < 0) 
		c = -c;
	
	if (a < 0) 
		d = -a;
	else if (a > 0) 
		d = a;
	else 
	{
		if (b < 0) d = -b;
		else if (b > 0) d = b;
		else return( 0.0 );
	}
	
	return ( c / d  );
}

double relative_error( unsigned long int a, unsigned long int b) 
{
	return(relative_error((double)a , (double)b));
}

double relative_errorbis( double a, double b) 
{
	double c;
	double d;
	fprintf(stdout, "ON ENTRE");
	
	c = a-b;
	if (c < 0) 
		c = -c;
	
	if (a < 0) 
		d = -a;
	else if (a > 0) 
		d = a;
	else 
	{
		if (b < 0) d = -b;
		else if (b > 0) d = b;
		else return( 0.0 );
	}
	
	return ( c / d  );
}


int is_the_same( unsigned long int a, unsigned long int b) 
{
	return( is_the_same( (double)a, (double)b ) );
}

int is_the_same( double a, double b) 
{
	double c;
	double d;
	double seuil = 0.00001;
	
	c = a-b;
	if (c < 0) 
		c = -c;
	
	if (a < 0) 
		d = -a;
	else if (a > 0) 
		d = a;
	else 
	{
		if (b < 0) d = -b;
		else if (b > 0) d = b;
		else return(1);
	}
	
	if ( c / d  > seuil ) 
		return(0);
	else 
		return(1);
}

/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv )
{
	const char					*func = "test convolution";
	const char					*anywaveTableFileName, *configFileName, *signalFileName;
	const char					*resFile1, *resFile2, *resFile3;
	MP_Anywave_Table_c			*anywaveTable, *anywaveRealTable;	
	MP_Convolution_Direct_c		*dirConv;
	MP_Convolution_FFT_c		*fftConv;
	MP_Convolution_Fastest_c	*fasConv;
	MP_Signal_c					*signal;
	MP_Real_t					*input;
	FILE						*fid;
	double						*outputTrue1, *trueAmp2, *trueAmp3, *fftAmp3, *fasAmp3;
    unsigned long int			*trueIdx2, *trueIdx3, *fftIdx3, *fasIdx3;
    double						dirMaxRelErr = 0.0, fftMaxRelErr = 0.0, fasMaxRelErr = 0.0;
	double						dirAmpMaxRelErr = 0.0, fftAmpMaxRelErr = 0.0, fasAmpMaxRelErr = 0.0;
	double						dirIdxMaxRelErr = 0.0, fftIdxMaxRelErr = 0.0, fasIdxMaxRelErr = 0.0;
	double						seuil = 0.00001;
	unsigned long int			chanIdx = 0, filterIdx = 0, frameIdx = 0, sampleIdx = 0, filterShift = 0;
	unsigned long int			inputLen = 0, numFrames = 0, numFramesSamples = 0, fromSample = 0;
    int							dir1Res,fft1Res,fas1Res,dir2Res,fft2Res,fas2Res,fft3Res,fas3Res;

	mp_info_msg( func, "------------------------------------------------------\n" );
	mp_info_msg( func, "TEST CONVOLUTION - TESTING CONVOLUTION FUNCTIONALITIES\n" );
	mp_info_msg( func, "------------------------------------------------------\n" );
	
	//-------------------------
	// Parsing the arguments                     
	//-------------------------
	if (argc != 2)
    {
		mp_error_msg( func, "Bad Number of arguments, test_convolution require \"configFileName\" (path.xml) as argument.\n");
		return(-1);
	}
	
	configFileName = argv[1];
	mp_info_msg( func, "The argument for \"configFileName\" is [%s].\n",configFileName);

	//-------------------------
	// Loading MPTK environment                     
	//-------------------------
	if(! (MPTK_Env_c::get_env()->load_environment_if_needed(configFileName)) ) 
		return (-1);
	
	anywaveTableFileName = MPTK_Env_c::get_env()->get_config_path("defaultAnyWaveTable");
	mp_info_msg( func, "The retrieved value for \"anywaveTableFileName\" is [%s].\n",anywaveTableFileName);
	signalFileName = MPTK_Env_c::get_env()->get_config_path("exampleSignal");
	mp_info_msg( func, "The retrieved value for \"exampleSignal\" is [%s].\n",signalFileName);
	resFile1 = MPTK_Env_c::get_env()->get_config_path("defaultAnyWaveResult1");
	mp_info_msg( func, "The retrieved value for \"defaultAnyWaveResult1\" is [%s].\n",resFile1);
	resFile2 = MPTK_Env_c::get_env()->get_config_path("defaultAnyWaveResult2");
	mp_info_msg( func, "The retrieved value for \"defaultAnyWaveResult2\" is [%s].\n",resFile2);
	resFile3 = MPTK_Env_c::get_env()->get_config_path("defaultAnyWaveResult3");
	mp_info_msg( func, "The retrieved value for \"defaultAnyWaveResult3\" is [%s].\n",resFile3);

	//-----------------------------------
	// Initiating the Anywave parameters                      
	//-----------------------------------
	anywaveTable = new MP_Anywave_Table_c((char *)anywaveTableFileName);
	signal = MP_Signal_c::init(signalFileName);
	input = signal->channel[0];
	
	//-----------------------------------------------------------------------------------------
	// 1) Inner product between the frames of signal and every filter, with a filterShift of 3
	//-----------------------------------------------------------------------------------------
    fprintf(stdout,"\nStep 1 : Computing inner products\n");
    fprintf(stdout,"-----------------------------------\n");

    inputLen = signal->numSamples;
    
    /* load the results */
	if ( ( fid = fopen( resFile1, "rb" ) ) == NULL ) 
	{
		mp_error_msg( func,"Could not open file %s to print a book.\n", resFile1 );
		return(-1);
	}

	if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) 
	{
		mp_error_msg(func, "Cannot read the filterShift from the file [%s].\n", resFile1 );
		fclose(fid);
		return(-1);
    }

    numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    numFramesSamples = anywaveTable->numFilters * numFrames;
	
    if ((outputTrue1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the outputTrue1 array.\n", numFramesSamples );
		fclose(fid);
		return(-1);
    } 
    if ( fread ( outputTrue1, sizeof(double), numFramesSamples, fid) != numFramesSamples ) 
	{
		mp_error_msg(func, "Cannot read the [%lu] samples from the file [%s].\n", numFramesSamples, resFile1 );
		fclose(fid);
		return(-1);
    }
    fclose(fid);

    dirConv = new MP_Convolution_Direct_c(anywaveTable,filterShift );
    fftConv = new MP_Convolution_FFT_c(anywaveTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveTable,filterShift );

    /* experimental results */
    double* outputFas1;
    double* outputDir1;
    double* outputFft1;
    if ((outputFas1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the outputFas1 array.\n", numFramesSamples );
		return(-1);
    } 
    if ((outputDir1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the outputDir1 array.\n", numFramesSamples );
		return(-1);
    } 
    if ((outputFft1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the outputFft1 array.\n", numFramesSamples );
		return(-1);
    } 

    dirConv->compute_IP( input, inputLen, chanIdx, &outputDir1 );
    fftConv->compute_IP( input, inputLen, chanIdx, &outputFft1 );
    fasConv->compute_IP( input, inputLen, chanIdx, &outputFas1 );

    /* Comparison to the true result */
    dirMaxRelErr = 0.0;
    fftMaxRelErr = 0.0;
    fasMaxRelErr = 0.0;
    seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) 
	{
		for (filterIdx = 0; filterIdx < anywaveTable->numFilters; filterIdx++ ) 
		{
			sampleIdx = frameIdx*anywaveTable->numFilters + filterIdx;
			//fprintf(stdout, "Frame[%lu]Filter[%lu] - true [%2.8lf] - dir [%2.8lf] - fft [%2.8lf] - fas [%2.8lf]\n",frameIdx,filterIdx,outputTrue1[sampleIdx],outputDir1[sampleIdx],outputFft1[sampleIdx],outputFas1[sampleIdx]);

			if ( relative_error(outputTrue1[sampleIdx],outputDir1[sampleIdx]) > dirMaxRelErr ) 
				dirMaxRelErr = relative_error(outputTrue1[sampleIdx],outputDir1[sampleIdx]);
			if ( relative_error(outputTrue1[sampleIdx],outputFft1[sampleIdx]) > fftMaxRelErr ) 
				fftMaxRelErr = relative_error(outputTrue1[sampleIdx],outputFft1[sampleIdx]);
			if ( relative_error(outputTrue1[sampleIdx],outputFas1[sampleIdx]) > fasMaxRelErr ) 
				fasMaxRelErr = relative_error(outputTrue1[sampleIdx],outputFas1[sampleIdx]);

		}
    }

    /* Calculating the difference result */
    dir1Res = (dirMaxRelErr > seuil)?0:1;
    fft1Res = (fftMaxRelErr > seuil)?0:1; 
    fas1Res = (fasMaxRelErr > seuil)?0:1; 

    /* Printing the verdict */
    fprintf(stdout,"\nusing direct convolution  : ");
	fprintf(stdout,(dir1Res == 1)?"[OK]\n":"[ERROR] (max relative error [%e])\n", dirMaxRelErr);
    fprintf(stdout,"using fft convolution     : ");
	fprintf(stdout,(fft1Res == 1)?"[OK]\n":"[ERROR] (max relative error [%e])\n", fftMaxRelErr);
    fprintf(stdout,"using fastest convolution : ");
	fprintf(stdout,(fas1Res == 1)?"[OK]\n":"[ERROR] (max relative error [%e])\n", fasMaxRelErr);

    delete(dirConv);
    delete(fftConv);
    delete(fasConv);
    free(outputTrue1); outputTrue1 = NULL;
    free(outputDir1); outputDir1 = NULL;
    free(outputFft1); outputFft1 = NULL;
    free(outputFas1); outputFas1 = NULL;

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Find the filterIdx, and the value corresponding to the max (in energy) inner product between each frame of signal and all the filters, with a filterShift of 3
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    fprintf(stdout,"\nStep 2 : finding the max inner product between each frame and all the filters\n");
    fprintf(stdout,"---------------------------------------------------------------------------------\n");

	/* load the results */
	if ( ( fid = fopen( resFile2, "rb" ) ) == NULL ) 
	{
		mp_error_msg( func,"Could not open file %s to print a book.\n", resFile2 );
		return(-1);
	}

	if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) 
	{
		mp_error_msg(func, "Cannot read the filterShift from the file [%s].\n", resFile2 );
		fclose(fid);
		return(-1);
    }
	
    numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
	
    if ((trueAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the trueAmp2 array.\n", numFrames );
		fclose(fid);
		return(-1);
    } 
    if ((trueIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the trueIdx2 array.\n", numFrames );
		fclose(fid);
		return(-1);
    } 
    if ( fread ( trueAmp2, sizeof(double), numFrames, fid) != numFrames ) 
	{
		mp_error_msg(func, "Cannot read the [%lu] amplitudes from the file [%s].\n", numFrames, resFile2 );
		fclose(fid);
		return(-1);
    }
    if ( fread ( trueIdx2, sizeof(unsigned long int), numFrames, fid) != numFrames ) 
	{
		mp_error_msg(func, "Cannot read the [%lu] indices from the file [%s].\n", numFrames, resFile2 );
		fclose(fid);
		return(-1);
    }
    fclose(fid);

    dirConv = new MP_Convolution_Direct_c(anywaveTable,filterShift );
    fftConv = new MP_Convolution_FFT_c(anywaveTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveTable,filterShift );

    /* experimental results */
    double* dirAmp2;
    double* fftAmp2;
    double* fasAmp2;
    unsigned long int* dirIdx2;
    unsigned long int* fftIdx2;
    unsigned long int* fasIdx2;

    if ((dirAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the dirAmp2 array.\n", numFrames );
		return(1);
    } 
    if ((fftAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the fftAmp2 array.\n", numFrames );
		return(1);
    } 
    if ((fasAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the fasAmp2 array.\n", numFrames );
		return(1);
    } 
    if ((dirIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] unsigned long int blocks for the dirIdx2 array.\n", numFrames );
		return(1);
    } 
    if ((fftIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] unsigned long int blocks for the fftIdx2 array.\n", numFrames );
		return(1);
    } 
    if ((fasIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] unsigned long int blocks for the fasIdx2 array.\n", numFrames );
		return(1);
    } 

    fromSample = 0;

    dirConv->compute_max_IP( signal, inputLen, fromSample, dirAmp2, dirIdx2 );
    fftConv->compute_max_IP( signal, inputLen, fromSample, fftAmp2, fftIdx2 );
    fasConv->compute_max_IP( signal, inputLen, fromSample, fasAmp2, fasIdx2 );

    /* Comparison to the true result */
    dirAmpMaxRelErr = 0.0;
    fftAmpMaxRelErr = 0.0;
    fasAmpMaxRelErr = 0.0;
    dirIdxMaxRelErr = 0.0;
    fftIdxMaxRelErr = 0.0;
    fasIdxMaxRelErr = 0.0;
    seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) 
	{
		//fprintf(stdout, "Frame[%lu] [max nrg|max idx] - true [%2.8lf|%lu] - dir [%2.8lf|%lu] - fft [%2.8lf|%lu] - fas [%2.8lf|%lu]\n",frameIdx,trueAmp2[frameIdx],trueIdx2[frameIdx],dirAmp2[frameIdx],dirIdx2[frameIdx],fftAmp2[frameIdx],fftIdx2[frameIdx],fasAmp2[frameIdx],fasIdx2[frameIdx]);
		if(relative_error(trueAmp2[frameIdx],dirAmp2[frameIdx]) > dirAmpMaxRelErr)
			dirAmpMaxRelErr = relative_error(trueAmp2[frameIdx],dirAmp2[frameIdx]);
		if ( relative_error(trueAmp2[frameIdx],fftAmp2[frameIdx]) > fftAmpMaxRelErr ) 
			fftAmpMaxRelErr = relative_error(trueAmp2[frameIdx],fftAmp2[frameIdx]);
		if ( relative_error(trueAmp2[frameIdx],fasAmp2[frameIdx]) > fasAmpMaxRelErr ) 
			fasAmpMaxRelErr = relative_error(trueAmp2[frameIdx],fasAmp2[frameIdx]);
		if ( relative_error(trueIdx2[frameIdx],dirIdx2[frameIdx]) > dirIdxMaxRelErr ) 
			dirIdxMaxRelErr = relative_error(trueIdx2[frameIdx],dirIdx2[frameIdx]);
		if ( relative_error(trueIdx2[frameIdx],fftIdx2[frameIdx]) > fftIdxMaxRelErr ) 
			fftIdxMaxRelErr = relative_error(trueIdx2[frameIdx],fftIdx2[frameIdx]);
		if ( relative_error(trueIdx2[frameIdx],fasIdx2[frameIdx]) > fasIdxMaxRelErr ) 
			fasIdxMaxRelErr = relative_error(trueIdx2[frameIdx],fasIdx2[frameIdx]);
	}

    /* Calculating the difference result */
    dir2Res = ((dirAmpMaxRelErr > seuil) && (dirIdxMaxRelErr > seuil))?0:1;
    fft2Res = ((fftAmpMaxRelErr > seuil) && (fftIdxMaxRelErr > seuil))?0:1; 
    fas2Res = ((fasAmpMaxRelErr > seuil) && (fasIdxMaxRelErr > seuil))?0:1; 

    /* Printing the verdict */
    fprintf(stdout,"\nusing direct convolution  : ");
    fprintf(stdout,(dir2Res == 1)?"[OK]\n":"[ERROR] (max relative error: amp [%e] idx [%e])\n", dirAmpMaxRelErr, dirIdxMaxRelErr);
    fprintf(stdout,"using fft convolution     : ");
    fprintf(stdout,(fft2Res == 1)?"[OK]\n":"[ERROR] (max relative error: amp [%e] idx [%e])\n",fftAmpMaxRelErr,fftIdxMaxRelErr);
    fprintf(stdout,"using fastest convolution : ");
    fprintf(stdout,(fas2Res == 1)?"[OK]\n":"[ERROR] (max relative error: amp [%e] idx [%e])\n",fasAmpMaxRelErr,fasIdxMaxRelErr);

    delete(dirConv);
    delete(fftConv);
    delete(fasConv);
    free(trueAmp2); trueAmp2 = NULL;
    free(dirAmp2); dirAmp2 = NULL;
    free(fftAmp2); fftAmp2 = NULL;
    free(fasAmp2); fasAmp2 = NULL;
    free(trueIdx2); trueIdx2 = NULL;
    free(dirIdx2); dirIdx2 = NULL;
    free(fftIdx2); fftIdx2 = NULL;
    free(fasIdx2); fasIdx2 = NULL;

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 3) Find the filterIdx, and the value corresponding to the max (in energy) inner product between each frame of signal and all the filters, in the sense of Hilbert, with a filterShift of 3
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    fprintf(stdout,"\nStep 3 : finding the max inner product (in the Hilbert sense) between each frame and all the filters\n");
    fprintf(stdout,"--------------------------------------------------------------------------------------------------------\n");

    anywaveRealTable = anywaveTable->copy();
    anywaveRealTable->center_and_denyquist();
    anywaveRealTable->normalize();

    /* load the results */
	if ( ( fid = fopen( resFile3, "rb" ) ) == NULL ) 
	{
		mp_error_msg( func,"Could not open file %s to print a book.\n", resFile3 );
		return(-1);
	}

    if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) 
	{
		mp_error_msg(func, "Cannot read the filterShift from the file [%s].\n", resFile3 );
		fclose(fid);
		return(-1);
    }
    numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    if ((trueAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the trueAmp3 array.\n", numFrames );
		fclose(fid);
		return(-1);
    } 
    if ((trueIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the trueIdx3 array.\n", numFrames );
		fclose(fid);
		return(-1);
    } 
    if ( fread ( trueAmp3, sizeof(double), numFrames, fid) != numFrames ) 
	{
		mp_error_msg(func, "Cannot read the [%lu] amplitudes from the file [%s].\n", numFrames, resFile3 );
		fclose(fid);
		return(-1);
    }
    if ( fread ( trueIdx3, sizeof(unsigned long int), numFrames, fid) != numFrames ) 
	{
		mp_error_msg(func, "Cannot read the [%lu] indices from the file [%s].\n", numFrames, resFile3 );
		fclose(fid);
		return(-1);
    }
    fclose(fid);

    fftConv = new MP_Convolution_FFT_c(anywaveRealTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveRealTable,filterShift );

    /* experimental results */
     if ((fftAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the fftAmp3 array.\n", numFrames );
		return(-1);
    } 
    if ((fasAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] double blocks for the fasAmp3 array.\n", numFrames );
		return(-1);
    } 
    if ((fftIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] unsigned long int blocks for the fftIdx3 array.\n", numFrames );
		return(-1);
    } 
    if ((fasIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) 
	{
		mp_error_msg(func, "Cannot alloc [%lu] unsigned long int blocks for the fasIdx3 array.\n", numFrames );
		return(-1);
    } 

    fromSample = 0;

    fftConv->compute_max_hilbert_IP( signal, inputLen, fromSample, fftAmp3, fftIdx3 );
    fasConv->compute_max_hilbert_IP( signal, inputLen, fromSample, fasAmp3, fasIdx3 );
    
    /* Comparison to the true result */
    fftAmpMaxRelErr = 0.0;
    fasAmpMaxRelErr = 0.0;
    fftIdxMaxRelErr = 0.0;
    fasIdxMaxRelErr = 0.0;
    seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) 
	{
		//fprintf(stdout, "Frame[%lu] [max nrg|max idx] - true [%2.8lf|%lu] - fft [%2.8lf|%lu] - fas [%2.8lf|%lu]\n",frameIdx,trueAmp3[frameIdx],trueIdx3[frameIdx],fftAmp3[frameIdx],fftIdx3[frameIdx],fasAmp3[frameIdx],fasIdx3[frameIdx]);
		if ( relative_error(trueAmp3[frameIdx],fftAmp3[frameIdx]) > fftAmpMaxRelErr ) 
			fftAmpMaxRelErr = relative_error(trueAmp3[frameIdx],fftAmp3[frameIdx]);
		if ( relative_error(trueAmp3[frameIdx],fasAmp3[frameIdx]) > fasAmpMaxRelErr ) 
			fasAmpMaxRelErr = relative_error(trueAmp3[frameIdx],fasAmp3[frameIdx]);
		if ( relative_error(trueIdx3[frameIdx],fftIdx3[frameIdx]) > fftIdxMaxRelErr ) 
			fftIdxMaxRelErr = relative_error(trueIdx3[frameIdx],fftIdx3[frameIdx]);
		if ( relative_error(trueIdx3[frameIdx],fasIdx3[frameIdx]) > fasIdxMaxRelErr ) 
			fasIdxMaxRelErr = relative_error(trueIdx3[frameIdx],fasIdx3[frameIdx]);
    }

    /* Calculating the difference result */
    fft3Res = ((fftAmpMaxRelErr > seuil) && (fftIdxMaxRelErr > seuil))?0:1; 
    fas3Res = ((fasAmpMaxRelErr > seuil) && (fasIdxMaxRelErr > seuil))?0:1; 

    /* Printing the verdict */
    fprintf(stdout,"\nusing fft convolution     : ");
	fprintf(stdout,(fft3Res == 1)?"[OK]\n":"[ERROR] (max relative error: amp [%e] idx [%e])\n",fftAmpMaxRelErr,fftIdxMaxRelErr);
    fprintf(stdout,"using fastest convolution : ");
	fprintf(stdout,(fas3Res == 1)?"[OK]\n":"[ERROR] (max relative error: amp [%e] idx [%e])\n",fasAmpMaxRelErr,fasIdxMaxRelErr);

    delete(fftConv);
    delete(fasConv);
    delete(anywaveRealTable);
	delete(anywaveTable);
    free(trueAmp3); trueAmp3 = NULL;
    free(fftAmp3); fftAmp3 = NULL;
    free(fasAmp3); fasAmp3 = NULL;
    free(trueIdx3); trueIdx3 = NULL;
    free(fftIdx3); fftIdx3 = NULL;
    free(fasIdx3); fasIdx3 = NULL;

	/* Release Mptk environnement */
	MPTK_Env_c::get_env()->release_environment();

	/* Sending the result */
	if(dir1Res && fft1Res && fas1Res && dir2Res && fft2Res && fas2Res && fft3Res && fas3Res)
		return(0);
	else
		return(-1);
}