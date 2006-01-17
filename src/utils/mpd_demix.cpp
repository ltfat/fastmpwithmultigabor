/******************************************************************************/
/*                                                                            */
/*                              mpd_demix.cpp                                 */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
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

#include <mptk.h>

#include "mp_system.h"
#include "getopt.h"

static char *cvsid = "$Revision$";

/********************/
/* Global constants */
/********************/
#define MPD_TRUE  (1==1)
#define MPD_FALSE (0==1)
#define MPD_DEFAULT_NUM_ITER   ULONG_MAX
#define MPD_DEFAULT_SNR        0.0
#define MPD_ALLOC_BLOCK_SIZE   1000

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_DICT       2
#define ERR_SIG        3
#define ERR_DECAY      4
#define ERR_OPEN       5
#define ERR_WRITE      6
#define ERR_READ       7
#define ERR_MALLOC     8
#define ERR_NEW        9
#define ERR_NCHANS     10
#define ERR_SIGINIT    11

/********************/
/* Global variables */
/********************/

unsigned long int MPD_REPORT_HIT = ULONG_MAX; /* Default: never report during the main loop. */
unsigned long int MPD_SAVE_HIT   = ULONG_MAX; /* Default: never save during the main loop. */
unsigned long int MPD_SNR_HIT    = ULONG_MAX; /* Default: never test the snr during the main loop. */

int MPD_QUIET      = MPD_FALSE;
int MPD_VERBOSE    = MPD_FALSE;

unsigned long int MPD_NUM_ITER = MPD_DEFAULT_NUM_ITER;
double MPD_SNR   = MPD_DEFAULT_SNR;
int MPD_USE_SNR  = MPD_FALSE;
int MPD_USE_ITER = MPD_FALSE;

/* Input/output file names: */
char *dictFileName  = NULL;
char *sndFileName   = NULL;
char *bookFileName  = NULL;
char *resFileName   = NULL;
char *decayFileName = NULL;
char *mixerFileName = NULL;
char *srcSeqFileName = NULL;


/* --------------------------------------- */
void usage( void ) {
/* --------------------------------------- */

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpd_demix [options] -D dictFILE.txt -M matrix.txt (-n N|-s SNR) (sndFILE.wav|-)"
	   " (bookFILE) [residualFILE.wav]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Performs Blind Source Separation on signal sndFILE.wav with dictionary dictFile.txt\n" );
  fprintf( stdout, "     and with the known mixer matrix mixFILE.txt. The result is stored in as many books\n" );
  fprintf( stdout, "     as estimated sources (plus an optional residual signal), after N iterations\n" );
  fprintf( stdout, "     or after reaching the signal-to-residual ratio SNR.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     -M<FILE>, --mix-matrix=<FILE>  Read the mixer matrix from text file FILE.\n" );
  fprintf( stdout, "                                    The first line of the file should indicate the number of rows\n" );
  fprintf( stdout, "                                    and the number of columns, and the following lines should give\n" );
  fprintf( stdout, "                                    space-separated values, with a line break after each row.\n" );
  fprintf( stdout, "                                    EXAMPLE:\n" );
  fprintf( stdout, "                                     2 3\n" );
  fprintf( stdout, "                                     0.92  0.38  0.71\n" );
  fprintf( stdout, "                                     0.71  0.77  1.85\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -n<N>, --num-iter=<N>|--num-atoms=<N>    Stop after N iterations.\n" );
  fprintf( stdout, "AND/OR -s<SNR>, --snr=<SNR>                   Stop when the SNR value SNR is reached.\n" );
  fprintf( stdout, "                                              If both options are used together, the algorithm stops\n" );
  fprintf( stdout, "                                              as soon as either one is reached.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     (sndFILE.wav|-)                          The signal to analyze or stdin (in WAV format).\n" );
  fprintf( stdout, "     (bookFILE)                               The base name of the files to store the books of atoms_n\n" );
  fprintf( stdout, "                                              corresponding to the N estimated sources. These N books\n" );
  fprintf( stdout, "                                              will be named bookFILE_n.bin, n=[0,...,N-1].\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     -D<FILE>, --dictionary=<FILE>    Read the dictionary from text file FILE.\n" );
  fprintf( stdout, "                                      If no dictionary is given, a default dictionary is used.\n" );
  fprintf( stdout, "                                      (Use -v to see the structure of the default dictionary\n" );
  fprintf( stdout, "                                       reported in the verbose information.)\n" );
  fprintf( stdout, "     -E<FILE>, --energy-decay=<FILE>  Save the energy decay as doubles to file FILE.\n" );
  fprintf( stdout, "     -Q<FILE>, --src-sequence=<FILE>  Save the source sequence as unsigned short ints to file FILE.\n" );
  fprintf( stdout, "     -R<N>,    --report-hit=<N>       Report some progress info (in stderr) every N iterations.\n" );
  fprintf( stdout, "     -S<N>,    --save-hit=<N>         Save the output files every N iterations.\n" );
  fprintf( stdout, "     -T<N>,    --snr-hit=<N>          Test the SNR every N iterations only (instead of each iteration).\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     residualFILE.wav                The residual signal after subtraction of the atoms.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -q, --quiet                    No text output.\n" );
  fprintf( stdout, "     -v, --verbose                  Verbose.\n" );
  fprintf( stdout, "     -V, --version                  Output the version and exit.\n" );
  fprintf( stdout, "     -h, --help                     This help.\n" );
  fprintf( stdout, " \n" );

  exit(0);
}


/* --------------------------------------- */
int parse_args(int argc, char **argv) {
/* --------------------------------------- */

  int c, i;
  char *p;

  struct option longopts[] = {
    {"dictionary",   required_argument, NULL, 'D'},
    {"energy-decay", required_argument, NULL, 'E'},
    {"mix-matrix",   required_argument, NULL, 'M'},
    {"src-sequence", required_argument, NULL, 'Q'},
    {"report-hit",   required_argument, NULL, 'R'},
    {"save-hit",     required_argument, NULL, 'S'},
    {"snr-hit",      required_argument, NULL, 'T'},

    {"num-atoms",    required_argument, NULL, 'n'},
    {"num-iter",     required_argument, NULL, 'n'},
    {"snr",          required_argument, NULL, 's'},

    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "D:E:M:Q:R:S:T:n:s:qvVh", longopts, &i)) != -1 ) {

    switch (c) {


    case 'D':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -D : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -D or switch --dictionary=.\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --dictionary without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else dictFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read dictionary file name [%s].\n", dictFileName );
#endif
      break;


    case 'E':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -E : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -E or switch --energy-decay= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --energy-decay without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else decayFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read decay file name [%s].\n", decayFileName );
#endif
      break;


    case 'M':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -M : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -M or switch --mix-matrix=.\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --mix-matrix without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else mixerFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read mixer matrix file name [%s].\n", mixerFileName );
#endif
      break;


    case 'Q':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -Q : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -Q or switch --src-sequence= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --src-sequence without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else srcSeqFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read source sequence file name [%s].\n", srcSeqFileName );
#endif
      break;


    case 'R':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -R : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -R or switch --report-hit= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --report-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_REPORT_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd_demix error -- After switch -R or switch --report-hit= :\n" );
        fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read report hit [%lu].\n", MPD_REPORT_HIT );
#endif
      break;


    case 'S':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -S : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -S or switch --save-hit= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --save-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_SAVE_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd_demix error -- After switch -S or switch --save-hit= :\n" );
        fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read save hit [%lu].\n", MPD_SAVE_HIT );
#endif
      break;


    case 'T':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -T : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -T or switch --snr-hit= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --snr-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_SNR_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd_demix error -- After switch -T or switch --snr-hit= :\n" );
        fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read snr hit [%lu].\n", MPD_SNR_HIT );
#endif
      break;



    case 'n':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -n : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
        fprintf( stderr, "mpd_demix error -- After switch -n/--num-iter=/--num-atom= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --numiter or --numatom without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else MPD_NUM_ITER = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpd_demix error -- After switch -n/--num-iter=/--num-atom= :\n" );
	fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
      MPD_USE_ITER = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read numIter [%lu].\n", MPD_NUM_ITER );
#endif
      break;


    case 's':
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- switch -s : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd_demix error -- After switch -s/--snr= :\n" );
	fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd_demix error -- (Did you use --snr without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else {
	MPD_SNR = strtod(optarg, &p);
      }
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpd_demix error -- After switch -s/--snr= :\n" );
	fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to a double value.\n",
		 optarg );
        return( ERR_ARG );
      }
      else {
	MPD_SNR = pow( 10.0, MPD_SNR/10 ); /* Translate the snr in linear energy scale */
	MPD_USE_SNR = MPD_TRUE;
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read SNR [%g].\n", MPD_SNR );
#endif
      break;



    case 'h':
      usage();
      break;


    case 'q':
      MPD_QUIET = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- MPD_QUIET is TRUE.\n" );
#endif
      break;


    case 'v':
      MPD_VERBOSE = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- MPD_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpd_demix -- Matching Pursuit library version %s -- mpd_demix %s\n", VERSION, cvsid);
      exit(0);
      break;


    default:
      fprintf( stderr, "mpd_demix error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- When exiting getopt, optind is [%d].\n", optind );
      fprintf( stderr, "mpd_demix DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some file names are following the options */
  if ( (argc-optind) < 1 ) {
    fprintf(stderr, "mpd error -- You must indicate a file name (or - for stdin) for the signal to analyze.\n");
    return( ERR_ARG );
  }
  if ( (argc-optind) < 2 ) {
    fprintf(stderr, "mpd_demix error -- You must indicate a base name for the book files.\n");
    return( ERR_ARG );
  }

  /* Read the file names after the options */
  sndFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpd_demix DEBUG -- Read sound file name [%s].\n", sndFileName );
#endif
  bookFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpd_demix DEBUG -- Read book file name [%s].\n", bookFileName );
#endif
  if (optind < argc) {
    resFileName = argv[optind++];
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Read residual file name [%s].\n", resFileName );
#endif
  }


  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPD_QUIET && MPD_VERBOSE ) {
    fprintf(stderr, "mpd_demix error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  /* Was dictionary file name given ? */
  /* if ( dictFileName == NULL ) {
     fprintf(stderr, "mpd_demix error -- You must specify a dictionary using switch -D/--dictionary= .\n");
     return( ERR_ARG );
     } */

  /* Was mixer file name given ? */
  if ( mixerFileName == NULL ) {
    fprintf(stderr, "mpd_demix error -- You must specify a mixer matrix using switch -M/--mix-matrix= .\n");
    return( ERR_ARG );
  }

  /* Must have one of --num-iter or --snr to tell the algorithm where to stop */
  if ( (!MPD_USE_SNR) && (!MPD_USE_ITER) ) {
    fprintf(stderr, "mpd_demix error -- You must specify one of : --num-iter=n/--num-atoms=n\n" );
    fprintf(stderr, "mpd_demix error --                      or   --snr=%%f\n" );
    return( ERR_ARG );
  }

  /* If snr is given without a snr hit value, test the snr on every iteration */
  if ((MPD_SNR_HIT == ULONG_MAX) && MPD_USE_SNR ) MPD_SNR_HIT = 1;

  /* If having both --num-iter AND --snr, warn */
  if ( (!MPD_QUIET) && MPD_USE_SNR && MPD_USE_ITER ) {
    fprintf(stderr, "mpd_demix warning -- The option --num-iter=/--num-atoms= was specified together with the option --snr=.\n" );
    fprintf(stderr, "mpd_demix warning -- The algorithm will stop when the first of either conditions is reached.\n" );
    fprintf(stderr, "mpd_demix warning -- (Use --help to get help if this is not what you want.)\n" );
  }

  return(0);
}


/*---------------------*/
void free_mem( MP_Dict_c** dict, unsigned short int numSources, double* decay ) {
/*---------------------*/
  unsigned short int j;

  if ( dict ) {
    for ( j = 0; j < numSources; j++ ) if ( dict[j] ) delete dict[j];
    delete[] dict;
    dict = NULL;
  }
  if ( decay ) free( decay ); decay = NULL;
}


/* --------------------------------------- */
int main( int argc, char **argv ) {
/* --------------------------------------- */

  MP_Dict_c **dict = NULL;
  MP_Book_c  *book = NULL;
  MP_Gabor_Atom_c *maxAtom       = NULL;
  MP_Gabor_Atom_c *multiChanAtom = NULL;
  MP_Signal_c *inSignal = NULL;
  MP_Signal_c *sigArray = NULL;
  double maxAmp;

  double max, val;
  unsigned short int maxSrc;
  unsigned long int blockIdx, maxBlock;

  MP_Real_t *mixer = NULL;
  MP_Real_t *p = NULL;
  float scanVal;
  FILE *mixerFID;
  MP_Real_t *Ah = NULL;

  unsigned short int numSources;
  int numChans;
  unsigned long int numSamples;
  int sampleRate;

  double *decay = NULL;
  double *newDecay = NULL;
  unsigned long int decaySize = 0;
  FILE *fid;
  unsigned long int nWrite = 0;

  unsigned short int *srcSequence = NULL;
  unsigned long int srcSeqSize = 0;
  unsigned short int *newSeq = NULL;

  unsigned long int i;
  unsigned short int j;
  int k;
  char line[1024];

  double residualEnergy = 0.0;
  double initialEnergy  = 0.0;
  double currentSnr = MPD_DEFAULT_SNR;

  unsigned long int nextReportHit = 0;
  unsigned long int nextSaveHit = 0;
  unsigned long int nextSnrHit = 0;

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpd_demix error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    exit( ERR_ARG );
  }

  if ( !MPD_QUIET ) nextReportHit = MPD_REPORT_HIT;
  else              nextReportHit = ULONG_MAX; /* If quiet, never report */

  nextSaveHit = MPD_SAVE_HIT;

  if ( MPD_USE_SNR ) nextSnrHit = MPD_SNR_HIT - 1;
  else               nextSnrHit = ULONG_MAX;

  /* Re-print the command line */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd_demix msg -- --------------------------------------------------------------------\n" );
    fprintf( stderr, "mpd_demix msg -- MPD_DEMIX - MATCHING PURSUIT DECOMPOSITION FOR BLIND SOURCE SEPARATION\n" );
    fprintf( stderr, "mpd_demix msg -- --------------------------------------------------------------------\n" );
    fprintf( stderr, "mpd_demix msg -- The command line was:\n" );
    for ( i=0; i<(unsigned long int)argc; i++ ) {
      fprintf( stderr, "%s ", argv[i] );
    }
    fprintf( stderr, "\nmpd_demix msg -- End command line.\n" );
  }

  /* Load the mixer matrix */
  if ( (mixerFID = fopen( mixerFileName, "r" )) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Failed to open the mixer matrix file [%s] for reading.\n",
	     mixerFileName );
    return( ERR_OPEN );
  }
  else {
    if ( ( fgets( line, MP_MAX_STR_LEN, mixerFID ) == NULL ) ||
	 ( sscanf( line,"%i %hu\n", &numChans, &numSources ) != 2 ) ) {
      fprintf( stderr, "mpd_demix error -- Failed to read numChans and numSources from the mixer matrix file [%s].\n",
	       mixerFileName );
      fclose( mixerFID );
      return( ERR_READ );
    }
    else {
      if ( (mixer = (MP_Real_t*) malloc( numChans*numSources*sizeof(MP_Real_t) )) == NULL ) {
	fprintf( stderr, "mpd_demix error -- Can't allocate an array of [%lu] MP_Real_t elements"
		 "for the mixer matrix.\n", (unsigned long int)(numChans)*numSources );
	fclose( mixerFID );
	return( ERR_MALLOC );
      }
      else for ( k = 0, p = mixer; k < numChans; k++ ) {
	for ( j = 0; j<numSources; j++ ) {
	  if ( fscanf( mixerFID, "%f", &scanVal ) != 1 ) {
	    fprintf( stderr, "mpd_demix error -- Can't read element [%i,%u] of the mixer matrix in file [%s]\n",
		     k, j, mixerFileName );
	    fclose( mixerFID );
	    free( mixer );
	    return( ERR_READ );
	  }
	  else { *p = (MP_Real_t)(scanVal); p++; }
	}
      }
    }
    fclose( mixerFID );
    /* Normalize the columns */
    for ( j = 0; j < numSources; j++ ) {
      for ( k = 0, p = mixer+j, val = 0.0; k < numChans; k++, p += numSources ) val += (*p)*(*p);
      val = sqrt( val );
      for ( k = 0, p = mixer+j; k < numChans; k++, p += numSources ) (*p) = (*p) / val;
    }
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Normalized mixer matrix:\n" );
    for ( k = 0, p = mixer; k < numChans; k++ ) {
      for ( j = 0; j < numSources; j++, p++ ) {
	fprintf( stderr, "%.4f ", *p );
      }
      fprintf( stderr, "\n" );
    }
    fprintf( stderr, "mpd_demix DEBUG -- End mixer matrix.\n" );
#endif
  }
  /* Pre-compute the squared mixer */
  if ( (Ah = (MP_Real_t*) malloc( numSources*numSources*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Can't allocate an array of [%u] MP_Real_t elements"
	     "for the squared mixer matrix.\n", numSources*numSources );
    free( mixer );
    return( ERR_MALLOC );
  }
  else {
    for ( i = 0, p = Ah; i < (unsigned long int)numSources; i++ ) {
      for ( j = 0; j < numSources; j++, p++ ) {
	for ( k = 0, val = 0.0; k < numChans; k++ ) {
	  val += (double)(*(mixer + k*numSources + i)) * (double)(*(mixer + k*numSources + j));
	}
	*p = (MP_Real_t)(val);
      }
    }
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Squared mixer matrix:\n" );
    for ( i = 0, p = Ah; i < (unsigned long int)(numSources); i++ ) {
      for ( j = 0; j < numSources; j++, p++ ) {
	fprintf( stderr, "%.4f ", *p );
      }
      fprintf( stderr, "\n" );
    }
    fprintf( stderr, "mpd_demix DEBUG -- End squared mixer matrix.\n" );
#endif
  }

  /* Load the input signal */
  if ( (inSignal = new MP_Signal_c( sndFileName )) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Failed to create a new signal from file [%s].\n",
	     sndFileName );
    free( mixer ); free( Ah );
    return( ERR_SIG );
  }
  if ( inSignal->numChans != numChans ) {
    fprintf( stderr, "mpd_demix error -- Channel mismatch: signal has [%d] channels whereas mixer matrix has [%d] channels.\n",
	     inSignal->numChans, numChans );
    free( mixer ); free( Ah );
    return( ERR_NCHANS );
  }
  numSamples = inSignal->numSamples;
  sampleRate = inSignal->sampleRate;
  if ( MPD_VERBOSE ) {
    fprintf( stderr, "mpd_demix msg -- The signal loaded from file [%s] has:\n", sndFileName );
    inSignal->info( stderr );
  }


  /* Make the signal array */
  if ( (sigArray = new MP_Signal_c[numSources]) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Failed to allocate an array of [%u] signals.\n",
	     numSources );
    free( mixer ); free( Ah ); delete(inSignal);
    return( ERR_NCHANS );
  }
  else {
    for ( j = 0; j < numSources; j++ ) {
      if ( !sigArray[j].init( 1, numSamples, sampleRate ) ) {
	fprintf( stderr, "mpd_demix error -- Could not initialize the [%u]-th signal in the signal array.\n",
		 j );
	delete[] sigArray;
	free( mixer ); free( Ah );
	return( ERR_SIGINIT );
      }
    }
  }

  /* Fill the signal array: multiply the input signal by the transposed mixer */
  for ( j = 0; j < numSources; j++ ) {
    MP_Sample_t *s = sigArray[j].storage;
    for ( i = 0; i < numSamples; i++, s++ ) {
      MP_Real_t val;
      MP_Sample_t in;
      for ( k = 0, val = 0.0, p = mixer+j;
	    k < numChans;
	    k++, p += numSources ) {
	in = *(inSignal->channel[k] + i);
	val += ( (*p) * (MP_Real_t)(in) );
      }
      *s = (MP_Sample_t)( val );
    }
#ifndef NDEBUG
    sprintf( line, "sig_DEBUG_%d.dbl", j );
    sigArray[j].dump_to_double_file( line );
#endif
  }

  /* Build and replicate the dictionary */
  if ( !MPD_QUIET ) fprintf( stderr, "mpd_demix msg -- Loading and replicating the dictionary...\n" ); fflush( stderr );
  if ( (dict = new MP_Dict_c*[numSources]) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Can't create an array of [%u] references on dictionaries.\n", numSources );
    free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray);
    free_mem( dict, numSources, decay );
    return( ERR_DICT );
  }
  else {
    /* Init the dictionary array */
    for ( j = 0; j < numSources; j++ ) dict[j] = NULL;
    /* Actually alloc each dictionary */
    for ( j = 0; j < numSources; j++ ) {
      if ( (dict[j] = new MP_Dict_c( &(sigArray[j]), MP_DICT_SIG_HOOK )) == NULL ) {
	fprintf( stderr, "mpd_demix error -- Can't create a new dictionary for source [%u].\n", j );
	free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray);
	free_mem( dict, numSources, decay );
	return( ERR_DICT );
      }
      else {
	if ( dictFileName != NULL ) {
	  if ( dict[j]->add_blocks( dictFileName ) == 0 ) {
	    fprintf( stderr, "mpd_demix error -- Can't read blocks from file [%s].\n", dictFileName );
	    free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray);
	    free_mem( dict, numSources, decay );
	    return( ERR_DICT );
	  }
	}
	/* If no file name is given, use the following default dictionnary: */
	else add_gabor_blocks( dict[j], numSamples, 4, 1, DSP_GAUSS_WIN, DSP_GAUSS_DEFAULT_OPT );
      }
    }
  }
  if ( !MPD_QUIET ) fprintf( stderr, "mpd_demix msg -- The multiple dictionary is built.\n" );

  if ( MPD_VERBOSE ) {
    if ( dictFileName ) fprintf( stderr, "mpd_demix msg -- The dictionary read from file [%s] contains [%u] blocks:\n",
				 dictFileName, dict[0]->numBlocks );
    else fprintf( stderr, "mpd_demix msg -- The default dictionary contains [%u] blocks:\n", dict[0]->numBlocks );
    for ( i = 0; i < dict[0]->numBlocks; i++ ) dict[0]->block[i]->info( stderr );
    fprintf( stderr, "mpd_demix msg -- End of dictionary.\n" );
  }

  /* Build a multiple book */
  if ( (book = new MP_Book_c[numSources]) == NULL ) {
    fprintf( stderr, "mpd_demix error -- Can't create an array of [%u] books.\n", numSources );
    free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray);
    free_mem( dict, numSources, decay );
    return( ERR_NEW );
  }
  else {
    /* Set the book number of samples and sampling rate */
    for ( j = 0; j < numSources; j++ ) {
      book[j].numSamples = numSamples;
      book[j].sampleRate = sampleRate;
    }
  }

  /* Make a multi-channel atom */
  if ( (multiChanAtom = new MP_Gabor_Atom_c( inSignal->numChans, 0, 0.0 )) == NULL ) {
    free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book);
    free_mem( dict, numSources, decay );
    return( ERR_NEW );
  }

  /* Allocate some storage for the decay of the energy  */
  if ( decayFileName ) {
    if ( MPD_USE_ITER ) {
      decay = (double*)malloc( (MPD_NUM_ITER+1)*sizeof(double) );
      decaySize = MPD_NUM_ITER;
    }
    else {
      decay = (double*)malloc( (MPD_ALLOC_BLOCK_SIZE+1)*sizeof(double) );
      decaySize = MPD_ALLOC_BLOCK_SIZE;
    }
    if ( decay == NULL ) {
      fprintf( stderr, "mpd_demix error -- Failed to allocate a decay array of [%lu] doubles.\n", decaySize+1 );
      free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
      free_mem( dict, numSources, decay );
      return( ERR_DECAY );
    }
    else for ( i = 0; i < (decaySize+1); i++ ) *(decay+i) = 0.0;
  }

  /* Allocate the source index sequence */
  if ( srcSeqFileName != NULL ) {
    if ( MPD_USE_ITER ) {
      srcSequence = (unsigned short int*)malloc( MPD_NUM_ITER*sizeof(double) );
      srcSeqSize = MPD_NUM_ITER;
    }
    else {
      srcSequence = (unsigned short int*)malloc( MPD_ALLOC_BLOCK_SIZE*sizeof(double) );
      srcSeqSize = MPD_ALLOC_BLOCK_SIZE;
    }
    if ( srcSequence == NULL ) {
      fprintf( stderr, "mpd_demix error -- Failed to allocate an array of [%lu] unsigned short ints"
	       " to store the sequence of source indexes.\n", srcSeqSize );
      free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
      free_mem( dict, numSources, decay );
      return( ERR_MALLOC );
    }
    else for ( i=0; i<srcSeqSize; i++ ) *(srcSequence+i) = 0;
  }

  /* Initial report */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd_demix msg -- -------------------------\n" );
    if ( dictFileName ) fprintf( stderr, "mpd_demix msg -- Starting Blind Source Separation on signal [%s] with dictionary [%s].\n",
				 sndFileName, dictFileName );
    else fprintf( stderr, "mpd_demix msg -- Starting Blind Source Separation on signal [%s] with the default dictionary.\n",
		  sndFileName );
    fprintf( stderr, "mpd_demix msg -- -------------------------\n" );
    fprintf( stderr, "mpd_demix msg -- The original signal has [%d] channels, with [%u] sources.\n",
	     numChans, numSources );
    fprintf( stderr, "mpd_demix msg -- The original signal has [%lu] samples in each channel.\n",
	     numSamples );
    if ( MPD_USE_ITER ) {
      fprintf( stderr, "mpd_demix msg -- This run will perform [%lu] iterations, using [%lu] atoms per dictionary replica.\n",
	       MPD_NUM_ITER, dict[0]->size() );
    }
    if ( MPD_USE_SNR ) {
      fprintf( stderr, "mpd_demix msg -- This run will iterate until the SNR goes above [%g], using [%lu] atoms per dictionary replica.\n",
	       10*log10(MPD_SNR), dict[0]->size() );
    }
    if ( MPD_VERBOSE ) {
      fprintf( stderr, "mpd_demix msg -- The resulting book will be written to [%u] book files with basename [%s].\n",
	       numSources, bookFileName );
      if ( resFileName ) fprintf( stderr, "mpd_demix msg -- The residual will be written to file [%s].\n", resFileName );
      else fprintf( stderr, "mpd_demix msg -- The residual will not be saved.\n" );
      if ( decayFileName ) fprintf( stderr, "mpd_demix msg -- The energy decay will be written to file [%s].\n", decayFileName );
      else fprintf( stderr, "mpd_demix msg -- The energy decay will not be saved.\n" );
      if ( srcSeqFileName ) fprintf( stderr, "mpd_demix msg -- The source sequence will be written to file [%s].\n", srcSeqFileName );
      else fprintf( stderr, "mpd_demix msg -- The source sequence will not be saved.\n" );
    }
    fflush( stderr );
  }
  
  /* Start storing the residual energy */
  residualEnergy = initialEnergy = (double)inSignal->energy;
  if ( decay ) decay[0] = initialEnergy;
  if ( !MPD_QUIET ) fprintf( stderr, "mpd_demix msg -- The initial signal energy is : %g\n", initialEnergy );

  /*********************/
  /* Start the pursuit */
  /*********************/
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd_demix msg -- STARTING TO ITERATE\n" );
    fflush( stderr );
  }
  /*************/
  /* MAIN LOOP */
  for ( i=0; ((i < MPD_NUM_ITER) && (currentSnr <= MPD_SNR) && (residualEnergy > 0.0)); i++ ) {

#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- ENTERING iteration [%lu]/[%lu].\n", i, MPD_NUM_ITER );
    fprintf( stderr, "mpd_demix DEBUG -- Next report hit is [%lu].\n", nextReportHit );
    fprintf( stderr, "mpd_demix DEBUG -- Next save hit is   [%lu].\n", nextSaveHit );
    fprintf( stderr, "mpd_demix DEBUG -- Next snr hit is    [%lu].\n", nextSnrHit );
    fprintf( stderr, "mpd_demix DEBUG -- SNR is [%g]/[%g].\n", currentSnr, MPD_SNR );
#endif

    /*----------------------------*/
    /* ---- ACTUAL ITERATION ---- */
    /*----------------------------*/

    /*-------------------------------------------*/
    /* -- Seek the best atom across the sources: */
    /*-------------------------------------------*/
    /* Init with source 0 */
    dict[0]->update();
    maxBlock = dict[0]->blockWithMaxIP;
    max = dict[0]->block[maxBlock]->maxIPValue;
    maxSrc = 0;
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Browsing the dictionaries:\n" );
    fprintf( stderr, "mpd_demix DEBUG -- SRC 00\tVAL %g in BLOCK %lu\tMAXSRC 00\tMAX %g in BLOCK %lu\n",
	     max, maxBlock, max, maxBlock ); fflush( stderr );
#endif
    /* Follow through the remaining sources */
    for ( j = 1; j < numSources; j++ ) {
      dict[j]->update();
      blockIdx = dict[j]->blockWithMaxIP;
      val = dict[j]->block[blockIdx]->maxIPValue;
      if ( val > max ) { max = val; maxSrc = j; maxBlock = blockIdx; }
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- SRC %02u\tVAL %g in BLOCK %lu\tMAXSRC %02u\tMAX %g in BLOCK %lu\n",
	       j, val, blockIdx, maxSrc, max, maxBlock ); fflush( stderr );
#endif
    }
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Browsing the dictionaries: done.\n" );
    fprintf( stderr, "mpd_demix DEBUG -- Making max atom from dict %02u\n", maxSrc );
#endif
    /* If needed, store the current max source index */
    if ( srcSeqFileName ) {
      /* Increase the array size if needed */
      if ( i == srcSeqSize ) {
#ifndef NDEBUG
	fprintf( stderr, " Reallocating the source sequence.\n" );
#endif
	srcSeqSize += MPD_ALLOC_BLOCK_SIZE;
	newSeq = (unsigned short int*) realloc( srcSequence, srcSeqSize*sizeof(unsigned short int) );
	if ( newSeq == NULL ) {
	  fprintf( stderr, "mpd_demix error -- Failed to re-allocate the source sequence array to store [%lu] unsigned short ints.\n",
		   srcSeqSize );
	  free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
	  free_mem( dict, numSources, decay );
	  return( ERR_MALLOC );
	}
	else srcSequence = newSeq;
      }
      /* Store the value */
      *(srcSequence+i) = maxSrc;
    }

    /*----------------------------*/
    /* -- Create the best atom:   */
    /*----------------------------*/
    dict[maxSrc]->create_max_atom( (MP_Atom_c**)(&maxAtom) );
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- MAX ATOM HAS:\n" );
    maxAtom->info( stderr );
#endif
    /* Backup the atom's amplitude */
    maxAmp = maxAtom->amp[0];

    /*----------------------------*/
    /* -- Update the signals:     */
    /*----------------------------*/
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- Updating the signals..." );
#endif
    /* - update the multiple signal */
    for ( j = 0; j < numSources; j++ ) {
      maxAtom->amp[0] = maxAmp * (*(Ah + j*numSources + maxSrc));
      maxAtom->substract_add( dict[j]->signal, NULL );
    }
    /* - update the input signal */
    multiChanAtom->totalChanLen = 0;
    multiChanAtom->windowType = maxAtom->windowType;
    multiChanAtom->freq  = maxAtom->freq;
    multiChanAtom->chirp = maxAtom->chirp;
    for ( k = 0; k < numChans; k++) {
      multiChanAtom->amp[k]   = maxAmp * (*(mixer + k*numSources + maxSrc));
      multiChanAtom->phase[k] = maxAtom->phase[0];
      multiChanAtom->support[k] = maxAtom->support[0];
      multiChanAtom->totalChanLen += maxAtom->totalChanLen;
    }
#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- MULTICHAN ATOM HAS:\n" );
    multiChanAtom->info( stderr );
    fprintf( stderr, "mpd_demix DEBUG -- MULTICHAN ATOM TOTLEN IS: %lu\n",
	     multiChanAtom->totalChanLen );
#endif
    multiChanAtom->substract_add( inSignal, NULL );    
    residualEnergy = (double)inSignal->energy;
#ifndef NDEBUG
    fprintf( stderr, " Done.\n" );
#endif
    /* Store the max atom with its original amplitude */
    maxAtom->amp[0] = maxAmp;
    book[maxSrc].append( maxAtom );

    /*----------------------------*/
    /* ---- Save the decay/compute the snr if needed */
    if ( decay ) {
      /* Increase the array size if needed */
      if ( i == decaySize ) {
#ifndef NDEBUG
	fprintf( stderr, " Reallocating the decay.\n" );
#endif
	decaySize += MPD_ALLOC_BLOCK_SIZE;
	newDecay = (double*) realloc( decay, (decaySize+1)*sizeof(double) );
	if ( newDecay == NULL ) {
	  fprintf( stderr, "mpd_demix error -- Failed to re-allocate the decay array to store [%lu] doubles.\n",
		   decaySize+1 );
	  free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
	  free_mem( dict, numSources, decay );
	  return( ERR_DECAY );
	}
	else decay = newDecay;
      }
      /* Store the value */
      decay[i+1] = residualEnergy;
    }

    if ( i == nextSnrHit ) {
      currentSnr = ( initialEnergy / residualEnergy );
      nextSnrHit += MPD_SNR_HIT;
    }

    /*----------------------------*/
    /* ---- Report */
    if ( i == nextReportHit ) {
      fprintf( stderr, "mpd_demix progress -- At iteration [%lu] : the residual energy is [%g] and the SNR is [%g].\n",
	       i, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
      fflush( stderr );
      nextReportHit += MPD_REPORT_HIT;
    }

    /*----------------------------*/
    /* ---- Save */
    if ( i == nextSaveHit ) {
      /* - the books: */
      for (j = 0; j < numSources; j++ ) {
	sprintf( line, "%s_%02u.bin", bookFileName, j );
	book[j].print( line, MP_BINARY);
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved book number [%02u] in file [%s], in binary mode.\n",
				    j, line );	  
      }
      /* - the residual: */
      if ( resFileName != NULL ) {
	inSignal->wavwrite( resFileName );
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- At iteration [%lu] : saved the residual.\n", i );	  
      }
      /* - the decay: */
      if ( decayFileName != NULL ) {
	if ( (fid = fopen( decayFileName, "w" )) == NULL ) {
	  fprintf( stderr, "mpd_demix error -- Failed to open the energy decay file [%s] for writing.\n",
		   decayFileName );
	  free_mem( dict, numSources, decay );
	  return( ERR_OPEN );
	}
	else {
	  nWrite = mp_fwrite( decay, sizeof(double), i+1, fid );
	  fclose( fid );
	  if (nWrite != (i+1)) {
	    fprintf( stderr, "mpd_demix warning -- Wrote less than the expected number of doubles to the energy decay file.\n" );
	    fprintf( stderr, "mpd_demix warning -- ([%lu] expected, [%lu] written.)\n", i+1, nWrite );
	  }
	}
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- At iteration [%lu] : saved the energy decay.\n", i );
      }
      /* - the source sequence: */
      if ( srcSeqFileName != NULL ) {
	if ( (fid = fopen( srcSeqFileName, "w" )) == NULL ) {
	  fprintf( stderr, "mpd_demix error -- Failed to open the source sequence file [%s] for writing.\n",
		   srcSeqFileName );
	  free_mem( dict, numSources, decay );
	  delete[](srcSequence);
	  return( ERR_OPEN );
	}
	else {
	  nWrite = mp_fwrite( srcSequence, sizeof(unsigned short int), i, fid );
	  fclose( fid );
	  if (nWrite != (i)) {
	    fprintf( stderr, "mpd_demix warning -- Wrote less than the expected number of unsigned short ints"
		     " to the source sequence file.\n" );
	    fprintf( stderr, "mpd_demix warning -- ([%lu] expected, [%lu] written.)\n", i, nWrite );
	  }
	}
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved the source sequence.\n" );
      }
      /* Compute the next save hit */
      nextSaveHit += MPD_SAVE_HIT;
    }

#ifndef NDEBUG
    fprintf( stderr, "mpd_demix DEBUG -- EXITING iteration  [%lu]/[%lu].\n", i, MPD_NUM_ITER );
    fprintf( stderr, "mpd_demix DEBUG -- Next report hit is [%lu].\n", nextReportHit );
    fprintf( stderr, "mpd_demix DEBUG -- Next save hit is   [%lu].\n", nextSaveHit );
    fprintf( stderr, "mpd_demix DEBUG -- Next snr hit is    [%lu].\n", nextSnrHit );
    fprintf( stderr, "mpd_demix DEBUG -- SNR is [%g]/[%g].\n", currentSnr, MPD_SNR );
#endif

  }
  /* END OF THE MAIN LOOP*/
  /***********************/
  if ( !MPD_QUIET ) fprintf( stderr, "mpd_demix msg -- [%lu] ITERATIONS DONE.\n", i );

  if ( (!MPD_QUIET) && (residualEnergy < 0.0) ) {
      fprintf( stderr, "mpd_demix warning -- The loop has stopped because a negative residual energy has been encountered.\n" );
      fprintf( stderr, "mpd_demix warning -- You have gone close to the machine precision. On the next run, you should use\n" );
      fprintf( stderr, "mpd_demix warning -- less iterations or a lower SNR .\n" );
  }
  
  /**************/
  /* End report */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd_demix msg -- ------------------------\n" );
    fprintf( stderr, "mpd_demix msg -- MATCHING PURSUIT RESULTS:\n" );
    fprintf( stderr, "mpd_demix msg -- ------------------------\n" );
    fprintf( stderr, "mpd_demix result -- [%lu] iterations have been performed.\n", i );
    fprintf( stderr, "mpd_demix result -- ([%lu] atoms have been selected out of the [%lu] atoms of each dictionary.)\n",
	     i, dict[0]->size() );
    fprintf( stderr, "mpd_demix result -- The initial signal energy was [%g].\n", initialEnergy );
    residualEnergy = inSignal->energy;
    fprintf( stderr, "mpd_demix result -- The residual energy is now [%g].\n", residualEnergy );
    currentSnr = 10*log10( initialEnergy / residualEnergy );
    fprintf( stderr, "mpd_demix result -- The SNR is now [%g].\n", currentSnr );
    fflush( stderr );
  }

  /***************************/
  /* Global save at the end: */
  /* - the books: */
  for (j = 0; j < numSources; j++ ) {
    sprintf( line, "%s_%02u.bin", bookFileName, j );
    book[j].print( line, MP_BINARY);
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved book number [%02u] in file [%s], in binary mode.\n",
				j, line );	  
  }
  /* - the residual: */
  if ( resFileName != NULL ) {
    inSignal->wavwrite( resFileName );
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved the residual.\n" );	  
  }
  /* - the decay: */
  if ( decayFileName != NULL ) {
    if ( (fid = fopen( decayFileName, "w" )) == NULL ) {
      fprintf( stderr, "mpd_demix error -- Failed to open the energy decay file [%s] for writing.\n",
	       decayFileName );
      free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
      free_mem( dict, numSources, decay );
      free( srcSequence );
      return( ERR_OPEN );
    }
    else {
      nWrite = mp_fwrite( decay, sizeof(double), i+1, fid );
      fclose( fid );
      if (nWrite != (i+1)) {
	fprintf( stderr, "mpd_demix warning -- Wrote less than the expected number of doubles to the energy decay file.\n" );
	fprintf( stderr, "mpd_demix warning -- ([%lu] expected, [%lu] written.)\n", i+1, nWrite );
      }
    }
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved the energy decay.\n" );
  }
  /* - the source sequence: */
  if ( srcSeqFileName != NULL ) {
    if ( (fid = fopen( srcSeqFileName, "w" )) == NULL ) {
      fprintf( stderr, "mpd_demix error -- Failed to open the source sequence file [%s] for writing.\n",
	       srcSeqFileName );
      free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
      free_mem( dict, numSources, decay );
      free( srcSequence );
      return( ERR_OPEN );
    }
    else {
      nWrite = mp_fwrite( srcSequence, sizeof(unsigned short int), i, fid );
      fclose( fid );
      if (nWrite != i) {
	fprintf( stderr, "mpd_demix warning -- Wrote less than the expected number of unsigned short ints to the source sequence file.\n" );
	fprintf( stderr, "mpd_demix warning -- ([%lu] expected, [%lu] written.)\n", i, nWrite );
      }
    }
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd_demix msg -- Saved the source sequence.\n" );
  }

  /*******************/
  /* Clean the house */
  free( mixer ); free( Ah ); delete(inSignal); delete[](sigArray); delete[](book); delete(multiChanAtom);
  free_mem( dict, numSources, decay );
  free( srcSequence );

  if ( !MPD_QUIET ) fprintf( stderr, "mpd_demix msg -- Have a nice day !\n" );
  fflush( stderr );
  fflush( stdout );

  return( 0 );
}
