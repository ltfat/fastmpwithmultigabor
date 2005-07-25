/******************************************************************************/
/*                                                                            */
/*                                 mpd.cpp                                    */
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

#include "system.h"
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

/********************/
/* Global variables */
/********************/

unsigned long int MPD_REPORT_HIT = ULONG_MAX; /* Default: never report during the main loop. */
unsigned long int MPD_SAVE_HIT   = ULONG_MAX; /* Default: never save during the main loop. */
unsigned long int MPD_SNR_HIT    = ULONG_MAX; /* Default: never test the snr during the main loop. */

int MPD_QUIET      = MPD_FALSE;
int MPD_VERBOSE    = MPD_FALSE;

unsigned long int MPD_NUM_ITER = MPD_DEFAULT_NUM_ITER;
int MPD_USE_ITER = MPD_FALSE;

double MPD_SNR  = MPD_DEFAULT_SNR;
int MPD_USE_SNR = MPD_FALSE;

double MPD_PREEMP = 0.0;

/* Input/output file names: */
char *dictFileName  = NULL;
char *sndFileName   = NULL;
char *bookFileName  = NULL;
char *resFileName   = NULL;
char *decayFileName = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) {

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpd [options] -D dictFILE.xml (-n N|-s SNR) (sndFILE.wav|-) (bookFILE.bin|-) [residualFILE.wav]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Iterates Matching Pursuit on signal sndFILE.wav with dictionary dictFile.xml\n" );
  fprintf( stdout, "     and gives the resulting book bookFILE.bin (and an optional residual signal)\n" );
  fprintf( stdout, "     after N iterations or after reaching the signal-to-residual ratio SNR.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     -D<FILE>, --dictionary=<FILE>  Read the dictionary from xml file FILE.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -n<N>, --num-iter=<N>|--num-atoms=<N>    Stop after N iterations.\n" );
  fprintf( stdout, "AND/OR -s<SNR>, --snr=<SNR>                   Stop when the SNR value SNR is reached.\n" );
  fprintf( stdout, "                                              If both options are used together, the algorithm stops\n" );
  fprintf( stdout, "                                              as soon as either one is reached.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     (sndFILE.wav|-)                          The signal to analyze or stdin (in WAV format).\n" );
  fprintf( stdout, "     (bookFILE.bin|-)                         The file to store the resulting book of atoms, or stdout.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     -E<FILE>, --energy-decay=<FILE>  Save the energy decay as doubles to file FILE.\n" );
  fprintf( stdout, "     -R<N>,    --report-hit=<N>       Report some progress info (in stderr) every N iterations.\n" );
  fprintf( stdout, "     -S<N>,    --save-hit=<N>         Save the output files every N iterations.\n" );
  fprintf( stdout, "     -T<N>,    --snr-hit=<N>          Test the SNR every N iterations only (instead of each iteration).\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -p<double>, --preemp=<double>    Pre-emphasize the input signal with coefficient <double>.\n" );
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


/**************************************************/
/* PARSING OF THE ARGUMENTS                       */
/**************************************************/
int parse_args(int argc, char **argv) {

  int c, i;
  char *p;

  struct option longopts[] = {
    {"dictionary",   required_argument, NULL, 'D'},
    {"energy-decay", required_argument, NULL, 'E'},
    {"report-hit",   required_argument, NULL, 'R'},
    {"save-hit",     required_argument, NULL, 'S'},
    {"snr-hit",      required_argument, NULL, 'T'},

    {"num-atoms",    required_argument, NULL, 'n'},
    {"num-iter",     required_argument, NULL, 'n'},
    {"preemp",       required_argument, NULL, 'p'},
    {"snr",          required_argument, NULL, 's'},

    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "D:E:R:S:T:n:p:s:qvVh", longopts, &i)) != -1 ) {

    switch (c) {


    case 'D':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -D : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -D or switch --dictionary=.\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --dictionary without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else dictFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read dictionary file name [%s].\n", dictFileName );
#endif
      break;


    case 'E':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -E : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -E or switch --energy-decay= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --energy-decay without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else decayFileName = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read decay file name [%s].\n", decayFileName );
#endif
      break;


    case 'R':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -R : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -R or switch --report-hit= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --report-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_REPORT_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd error -- After switch -R or switch --report-hit= :\n" );
        fprintf( stderr, "mpd error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read report hit [%lu].\n", MPD_REPORT_HIT );
#endif
      break;


    case 'S':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -S : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -S or switch --save-hit= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --save-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_SAVE_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd error -- After switch -S or switch --save-hit= :\n" );
        fprintf( stderr, "mpd error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read save hit [%lu].\n", MPD_SAVE_HIT );
#endif
      break;


    case 'T':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -T : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -T or switch --snr-hit= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --snr-hit without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPD_SNR_HIT = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
	fprintf( stderr, "mpd error -- After switch -T or switch --snr-hit= :\n" );
        fprintf( stderr, "mpd error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read snr hit [%lu].\n", MPD_SNR_HIT );
#endif
      break;



    case 'n':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -n : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
        fprintf( stderr, "mpd error -- After switch -n/--num-iter=/--num-atom= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --numiter or --numatom without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else MPD_NUM_ITER = strtoul(optarg, &p, 10);
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpd error -- After switch -n/--num-iter=/--num-atom= :\n" );
	fprintf( stderr, "mpd error -- failed to convert argument [%s] to an unsigned long value.\n",
		 optarg );
        return( ERR_ARG );
      }
      MPD_USE_ITER = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read numIter [%lu].\n", MPD_NUM_ITER );
#endif
      break;


    case 'p':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -p : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -p/--preemp= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --preemp without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else MPD_PREEMP = strtod(optarg, &p);
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpd error -- After switch -p/--preemp= :\n" );
	fprintf( stderr, "mpd error -- failed to convert argument [%s] to a double value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read preemp coeff [%g].\n", MPD_PREEMP );
#endif
      break;


    case 's':
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- switch -s : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpd error -- After switch -s/--snr= :\n" );
	fprintf( stderr, "mpd error -- the argument is NULL.\n" );
	fprintf( stderr, "mpd error -- (Did you use --snr without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else MPD_SNR = strtod(optarg, &p);
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpd error -- After switch -s/--snr= :\n" );
	fprintf( stderr, "mpd error -- failed to convert argument [%s] to a double value.\n",
		 optarg );
        return( ERR_ARG );
      }
      MPD_SNR = pow( 10.0, MPD_SNR/20 ); /* Translate the snr in linear energy scale */
      MPD_USE_SNR = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- Read SNR [%g].\n", MPD_SNR );
#endif
      break;



    case 'h':
      usage();
      break;


    case 'q':
      MPD_QUIET = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- MPD_QUIET is TRUE.\n" );
#endif
      break;


    case 'v':
      MPD_VERBOSE = MPD_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- MPD_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpd -- Matching Pursuit library version %s -- mpd %s\n", VERSION, cvsid);
      exit(0);
      break;


    default:
      fprintf( stderr, "mpd error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
      fprintf( stderr, "mpd DEBUG -- When exiting getopt, optind is [%d].\n", optind );
      fprintf( stderr, "mpd DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some file names are following the options */
  if ( (argc-optind) < 1 ) {
    fprintf(stderr, "mpd error -- You must indicate a file name (or - for stdin) for the signal to analyze.\n");
    return( ERR_ARG );
  }
  if ( (argc-optind) < 2 ) {
    fprintf(stderr, "mpd error -- You must indicate a file name (or - for stdout) for the book file.\n");
    return( ERR_ARG );
  }

  /* Read the file names after the options */
  sndFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpd DEBUG -- Read sound file name [%s].\n", sndFileName );
#endif
  bookFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpd DEBUG -- Read book file name [%s].\n", bookFileName );
#endif
  if (optind < argc) {
    resFileName = argv[optind++];
#ifndef NDEBUG
    fprintf( stderr, "mpd DEBUG -- Read residual file name [%s].\n", resFileName );
#endif
  }


  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPD_QUIET && MPD_VERBOSE ) {
    fprintf(stderr, "mpd error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  /* Was dictionary file name given ? */
  if ( dictFileName == NULL ) {
    fprintf(stderr, "mpd error -- You must specify a dictionary using switch -D/--dictionary= .\n");
    return( ERR_ARG );
  }

  /* Must have one of --num-iter or --snr to tell the algorithm where to stop */
  if ( (!MPD_USE_SNR) && (!MPD_USE_ITER) ) {
    fprintf(stderr, "mpd error -- You must specify one of : --num-iter=n/--num-atoms=n\n" );
    fprintf(stderr, "mpd error --                      or   --snr=%%f\n" );
    return( ERR_ARG );
  }

  /* If snr is given without a snr hit value, test the snr on every iteration */
  if ((MPD_SNR_HIT == ULONG_MAX) && MPD_USE_SNR ) MPD_SNR_HIT = 1;

  /* If having both --num-iter AND --snr, warn */
  if ( (!MPD_QUIET) && MPD_USE_SNR && MPD_USE_ITER ) {
    fprintf(stderr, "mpd warning -- The option --num-iter=/--num-atoms= was specified together with the option --snr=.\n" );
    fprintf(stderr, "mpd warning -- The algorithm will stop when the first of either conditions is reached.\n" );
    fprintf(stderr, "mpd warning -- (Use --help to get help if this is not what you want.)\n" );
  }

  return(0);
}


/**************************************************/
/* GLOBAL FUNCTIONS                               */
/**************************************************/
void free_mem( MP_Dict_c* dict, double* decay ) {
  if ( dict )  delete dict;    dict = NULL;
  if ( decay ) free( decay ); decay = NULL;
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) {

  MP_Dict_c  *dict = NULL;
  MP_Book_c book;

  double *decay = NULL;
  double *newDecay = NULL;
  unsigned long int decaySize = 0;
  FILE *decayFID;
  unsigned long int nWrite = 0;

  unsigned long int i;

  double residualEnergy = 0.0;
  double initialEnergy  = 0.0;
  double currentSnr = MPD_DEFAULT_SNR;

  unsigned long int nextReportHit = 0;
  unsigned long int nextSaveHit = 0;
  unsigned long int nextSnrHit = 0;


  /**************************************************/
  /* PRELIMINARIES                                  */
  /**************************************************/

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpd error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    fflush( stderr );
    exit( ERR_ARG );
  }

  if ( !MPD_QUIET ) nextReportHit = MPD_REPORT_HIT;
  else              nextReportHit = ULONG_MAX; /* If quiet, never report */

  nextSaveHit = MPD_SAVE_HIT;

  if ( MPD_USE_SNR ) nextSnrHit = MPD_SNR_HIT - 1;
  else               nextSnrHit = ULONG_MAX;

  /* Re-print the command line */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd msg -- ------------------------------------\n" );
    fprintf( stderr, "mpd msg -- MPD - MATCHING PURSUIT DECOMPOSITION\n" );
    fprintf( stderr, "mpd msg -- ------------------------------------\n" );
    fprintf( stderr, "mpd msg -- The command line was:\n" );
    for ( i=0; i<(unsigned long int)argc; i++ ) {
      fprintf( stderr, "%s ", argv[i] );
    }
    fprintf( stderr, "\nmpd msg -- End command line.\n" );
    fflush( stderr );
  }

  /* Load the dictionary */
  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- Loading the signal and the dictionary...\n" ); fflush( stderr );
  dict = new MP_Dict_c( sndFileName );
  if ( dict->signal->storage == NULL ) {
    fprintf( stderr, "mpd error -- Failed to load a signal from file [%s].\n",
	     sndFileName );
    free_mem( dict, decay );
    return( ERR_SIG );
  }
  /* Pre-emphasize the signal if needed */
  if (MPD_PREEMP != 0.0) {
    if ( MPD_VERBOSE ) { fprintf( stderr, "mpd msg -- Pre-emphasizing the signal..." ); fflush( stderr ); }
    dict->signal->preemp( MPD_PREEMP );
    if ( MPD_VERBOSE ) { fprintf( stderr, "Done.\n" ); fflush( stderr ); }
  }
  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- The signal is now loaded.\n" ); fflush( stderr );
  /* Add the blocks to the dictionnary */
  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- Parsing the dictionary...\n"
			     "mpd msg -- (In the following, spurious output of dictionary pieces"
			     " would be a symptom of parsing errors.)\n" ); fflush( stderr );
  if ( dict->add_blocks( dictFileName ) == 0 ) {
    fprintf( stderr, "mpd error -- Can't read blocks from file [%s].\n", dictFileName );
    free_mem( dict, decay );
    return( ERR_DICT );
  }
  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- The dictionary is now loaded.\n" );

  if ( MPD_VERBOSE ) {
    fprintf( stderr, "mpd msg -- The signal loaded from file [%s] has:\n", sndFileName );
    dict->signal->info( stderr );
    fprintf( stderr, "mpd msg -- The dictionary read from file [%s] contains [%u] blocks:\n",
	     dictFileName, dict->numBlocks );
    for ( i = 0; i < dict->numBlocks; i++ ) dict->block[i]->info( stderr );
    fprintf( stderr, "mpd msg -- End of dictionary.\n" );
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
      fprintf( stderr, "mpd error -- Failed to allocate a decay array of [%lu] doubles.\n", decaySize+1 );
      free_mem( dict, decay );
      return( ERR_DECAY );
    }
    else for ( i = 0; i < (decaySize+1); i++ ) *(decay+i) = 0.0;
  }

  /* Initial report */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd msg -- -------------------------\n" );
    fprintf( stderr, "mpd msg -- Starting Matching Pursuit on signal [%s] with dictionary [%s].\n",
	     sndFileName, dictFileName );
    fprintf( stderr, "mpd msg -- -------------------------\n" );
    if ( MPD_USE_ITER ) fprintf( stderr, "mpd msg -- This run will perform [%lu] iterations, using [%lu] atoms.\n",
				 MPD_NUM_ITER, dict->size() );
    if ( MPD_USE_SNR ) fprintf( stderr, "mpd msg -- This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
				20*log10(MPD_SNR), dict->size() );
    if ( MPD_VERBOSE ) {
      fprintf( stderr, "mpd msg -- The resulting book will be written to book file [%s].\n", bookFileName );
      if ( resFileName ) fprintf( stderr, "mpd msg -- The residual will be written to file [%s].\n", resFileName );
      else fprintf( stderr, "mpd msg -- The residual will not be saved.\n" );
      if ( decayFileName ) fprintf( stderr, "mpd msg -- The energy decay will be written to file [%s].\n", decayFileName );
      else fprintf( stderr, "mpd msg -- The energy decay will not be saved.\n" );
    }
    fflush( stderr );
  }
  
  /* Set the book number of samples and sampling rate */
  book.numSamples = dict->signal->numSamples;
  book.sampleRate = dict->signal->sampleRate;

  /* Start storing the residual energy */
  residualEnergy = initialEnergy = (double)dict->signal->energy;
  if ( decay ) decay[0] = initialEnergy;
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd msg -- The initial signal energy is : %g\n", initialEnergy );
    fflush( stderr );
  }


  /**************************************************/
  /* MAIN PURSUIT LOOP                              */
  /**************************************************/

  /* Report */
  if ( !MPD_QUIET ) { fprintf( stderr, "mpd msg -- STARTING TO ITERATE\n" ); fflush( stderr ); }

  /* Start the pursuit */
  for ( i = 0; ((i < MPD_NUM_ITER) && (currentSnr <= MPD_SNR) && (residualEnergy > 0.0)); i++ ) {

#ifndef NDEBUG
    fprintf( stderr, "mpd DEBUG -- ENTERING iteration [%lu]/[%lu].\n", i, MPD_NUM_ITER );
    fprintf( stderr, "mpd DEBUG -- Next report hit is [%lu].\n", nextReportHit );
    fprintf( stderr, "mpd DEBUG -- Next save hit is   [%lu].\n", nextSaveHit );
    fprintf( stderr, "mpd DEBUG -- Next snr hit is    [%lu].\n", nextSnrHit );
    fprintf( stderr, "mpd DEBUG -- SNR is [%g]/[%g].\n", currentSnr, MPD_SNR );
    fflush( stderr );
#endif

    /* ---- Actual iteration */
    dict->iterate_mp( &book , NULL );
    residualEnergy = (double)dict->signal->energy;
    /* Note: the residual energy may go negative when you use
       monstruous SNRs or too many iterations. */

    /* ---- Save the decay/compute the snr if needed */
    if ( decay ) {
      /* Increase the array size if needed */
      if ( i == decaySize ) {
#ifndef NDEBUG
	fprintf( stderr, " Reallocating the decay.\n" );
	fflush( stderr );
#endif
	decaySize += MPD_ALLOC_BLOCK_SIZE;
	newDecay = (double*) realloc( decay, (decaySize+1)*sizeof(double) );
	if ( newDecay == NULL ) {
	  fprintf( stderr, "mpd error -- Failed to re-allocate the decay array to store [%lu] doubles.\n",
		   decaySize+1 );
	  free_mem( dict, decay );
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

    /* ---- Report */
    if ( i == nextReportHit ) {
      fprintf( stderr, "mpd progress -- At iteration [%lu] : the residual energy is [%g] and the SNR is [%g].\n",
	       i, residualEnergy, 20*log10( initialEnergy / residualEnergy ) );
      fflush( stderr );
      nextReportHit += MPD_REPORT_HIT;
    }

    /* ---- Save */
    if ( i == nextSaveHit ) {
      /* - the book: */
      if ( strcmp( bookFileName, "-" ) != 0 ) {
	book.print( bookFileName, MP_BINARY);
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- At iteration [%lu] : saved the book.\n", i );	  
      }
      /* - the residual: */
      if ( resFileName ) {
	dict->signal->wavwrite( resFileName );
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- At iteration [%lu] : saved the residual.\n", i );	  
      }
      /* - the decay: */
      if ( decayFileName ) {
	if ( (decayFID = fopen( decayFileName, "w" )) == NULL ) {
	  fprintf( stderr, "mpd error -- Failed to open the energy decay file [%s] for writing.\n",
		   decayFileName );
	  free_mem( dict, decay );
	  return( ERR_OPEN );
	}
	else {
	  nWrite = fwrite( decay, sizeof(double), i+1, decayFID );
	  fclose( decayFID );
	  if (nWrite != (i+1)) {
	    fprintf( stderr, "mpd warning -- Wrote less than the expected number of doubles to the energy decay file.\n" );
	    fprintf( stderr, "mpd warning -- ([%lu] expected, [%lu] written.)\n", i+1, nWrite );
	  }
	}
	if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- At iteration [%lu] : saved the energy decay.\n", i );	  
      }
      /* Compute the next save hit */
      nextSaveHit += MPD_SAVE_HIT;
    }

#ifndef NDEBUG
    fprintf( stderr, "mpd DEBUG -- EXITING iteration  [%lu]/[%lu].\n", i, MPD_NUM_ITER );
    fprintf( stderr, "mpd DEBUG -- Next report hit is [%lu].\n", nextReportHit );
    fprintf( stderr, "mpd DEBUG -- Next save hit is   [%lu].\n", nextSaveHit );
    fprintf( stderr, "mpd DEBUG -- Next snr hit is    [%lu].\n", nextSnrHit );
    fprintf( stderr, "mpd DEBUG -- SNR is [%g]/[%g].\n", currentSnr, MPD_SNR );
    fflush( stderr );
#endif

  }
  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- [%lu] ITERATIONS DONE.\n", i );

  if ( (!MPD_QUIET) && (residualEnergy < 0.0) ) {
      fprintf( stderr, "mpd warning -- The loop has stopped because a negative residual energy has been encountered.\n" );
      fprintf( stderr, "mpd warning -- You have gone close to the machine precision. On the next run, you should use\n" );
      fprintf( stderr, "mpd warning -- less iterations or a lower SNR .\n" );
      fflush( stderr );
  }
  /*********************/
  

  /**************************************************/
  /* FINAL SAVES AND CLEANUP                        */
  /**************************************************/

  /**************/
  /* End report */
  if ( !MPD_QUIET ) {
    fprintf( stderr, "mpd msg -- ------------------------\n" );
    fprintf( stderr, "mpd msg -- MATCHING PURSUIT RESULTS:\n" );
    fprintf( stderr, "mpd msg -- ------------------------\n" );
    fprintf( stderr, "mpd result -- [%lu] iterations have been performed.\n", i );
    fprintf( stderr, "mpd result -- ([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
	     i, dict->size() );
    fprintf( stderr, "mpd result -- The initial signal energy was [%g].\n", initialEnergy );
    residualEnergy = dict->signal->energy;
    fprintf( stderr, "mpd result -- The residual energy is now [%g].\n", residualEnergy );
    currentSnr = 20*log10( initialEnergy / residualEnergy );
    fprintf( stderr, "mpd result -- The SNR is now [%g].\n", currentSnr );
    fflush( stderr );
  }

  /***************************/
  /* Global save at the end: */
  /* - the residual: */
  if ( resFileName ) {
    dict->signal->wavwrite( resFileName );
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- Saved the residual.\n" );	  
  }
  /* - the book: */
  if ( strcmp( bookFileName, "-" ) != 0 ) {
    book.print( bookFileName, MP_BINARY);
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- Saved the book in binary mode.\n" );	  
  }
  else {
    book.print( stdout, MP_TEXT );
    fflush( stdout );
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- Sent the book to stdout in text mode.\n" );	  
  }
  /* - the decay: */
  if ( decayFileName ) {
    if ( (decayFID = fopen( decayFileName, "w" )) == NULL ) {
      fprintf( stderr, "mpd error -- Failed to open the energy decay file [%s] for writing.\n",
	       decayFileName );
      free_mem( dict, decay );
      return( ERR_OPEN );
    }
    else {
      nWrite = fwrite( decay, sizeof(double), i+1, decayFID );
      fclose( decayFID );
      if (nWrite != (i+1)) {
	fprintf( stderr, "mpd warning -- Wrote less than the expected number of doubles to the energy decay file.\n" );
	fprintf( stderr, "mpd warning -- ([%lu] expected, [%lu] written.)\n", i+1, nWrite );
      }
    }
    if ( MPD_VERBOSE ) fprintf( stderr, "mpd msg -- Saved the energy decay.\n" );	  
  }

  /*******************/
  /* Clean the house */
  free_mem( dict, decay );

  if ( !MPD_QUIET ) fprintf( stderr, "mpd msg -- Have a nice day !\n" );
  fflush( stderr );
  fflush( stdout );

  return( 0 );
}
