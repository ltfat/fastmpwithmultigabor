/******************************************************************************/
/*                                                                            */
/*                                 mpr.cpp                                    */
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
#define MPR_TRUE  (1==1)
#define MPR_FALSE (0==1)

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_RES        3
#define ERR_SIG        4
#define ERR_BUILD      5
#define ERR_WRITE      6

/********************/
/* Global variables */
/********************/
int MPR_QUIET      = MPR_FALSE;
int MPR_VERBOSE    = MPR_FALSE;

double MPR_DEEMP = 0.0;

/* Input/output file names: */
char *bookFileName = NULL;
char *sigFileName  = NULL;
char *resFileName  = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) {

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpr [options] (bookFILE.bin|-) (reconsFILE.wav|-) [residualFILE.wav]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Rebuilds a signal reconsFile.wav from the atoms contained in the book file bookFile.bin.\n" );
  fprintf( stdout, "     An optional residual can be added.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     (bookFILE.bin|-)     A book of atoms, or stdin.\n" );
  fprintf( stdout, "     (reconsFILE.wav|-)   A file to store the rebuilt signal, or stdout (in WAV format).\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     residualFILE.wav     A residual signal that was obtained from a Matching Pursuit decomposition.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -d, --deemp          De-emphasize the signal.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -q, --quiet          No text output.\n" );
  fprintf( stdout, "     -v, --verbose        Verbose.\n" );
  fprintf( stdout, "     -V, --version        Output the version and exit.\n" );
  fprintf( stdout, "     -h, --help           This help.\n" );
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

    {"deemp",   required_argument, NULL, 'd'},

    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "d:qvVh", longopts, &i)) != -1 ) {

    switch (c) {

    case 'd':
#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- switch -d : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpr error -- After switch -d/--deemp= :\n" );
	fprintf( stderr, "mpr error -- the argument is NULL.\n" );
	fprintf( stderr, "mpr error -- (Did you use --deemp without the '=' character ?).\n" );
	fflush( stderr );
	return( ERR_ARG );
      }
      else MPR_DEEMP = strtod(optarg, &p);
      if ( (p == optarg) || (*p != 0) ) {
        fprintf( stderr, "mpr error -- After switch -d/--deemp= :\n" );
	fprintf( stderr, "mpr error -- failed to convert argument [%s] to a double value.\n",
		 optarg );
        return( ERR_ARG );
      }
#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- Read deemp coeff [%g].\n", MPR_DEEMP );
#endif
      break;


    case 'h':
      usage();
      break;

    case 'q':
      MPR_QUIET = MPR_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- MPR_QUIET is TRUE.\n" );
#endif
      break;

    case 'v':
      MPR_VERBOSE = MPR_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- MPR_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpr -- Matching Pursuit library version %s -- mpr %s\n", VERSION, cvsid);
      exit(0);
      break;


    default:
      fprintf( stderr, "mpr error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- When exiting getopt, optind is [%d].\n", optind );
      fprintf( stderr, "mpr DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some file names are following the options */
  if ( (argc-optind) < 1 ) {
    fprintf(stderr, "mpr error -- You must indicate a file name (or - for stdin) for the book to use.\n");
    return( ERR_ARG );
  }
  if ( (argc-optind) < 2 ) {
    fprintf(stderr, "mpr error -- You must indicate a file name (or - for stdout) for the rebuilt signal.\n");
    return( ERR_ARG );
  }

  /* Read the file names after the options */
  bookFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpr DEBUG -- Read book file name [%s].\n", bookFileName );
#endif
  sigFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpr DEBUG -- Read rebuilt signal file name [%s].\n", sigFileName );
#endif
  if (optind < argc) {
    resFileName = argv[optind++];
#ifndef NDEBUG
    fprintf( stderr, "mpr DEBUG -- Read residual file name [%s].\n", resFileName );
#endif
  }


  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPR_QUIET && MPR_VERBOSE ) {
    fprintf(stderr, "mpr error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  return(0);
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) {

  MP_Book_c book;
  MP_Signal_c *sig;
  int i;

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpr error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    fflush( stderr );
    exit( ERR_ARG );
  }

  /* Report */
  if ( !MPR_QUIET ) {
    fprintf( stderr, "mpr msg -- -------------------------------------\n" );
    fprintf( stderr, "mpr msg -- MPR - MATCHING PURSUIT RECONSTRUCTION\n" );
    fprintf( stderr, "mpr msg -- -------------------------------------\n" );
    fprintf( stderr, "mpr msg -- The command line was:\n" );
    for ( i = 0; i < argc; i++ ) fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\nmpr msg -- End command line.\n" );
    if ( resFileName ) fprintf( stderr, "mpr msg -- Rebuilding signal [%s] from book [%s] plus residual [%s].\n",
				sigFileName, bookFileName, resFileName );
    else               fprintf( stderr, "mpr msg -- Rebuilding signal [%s] from book [%s] (no residual given).\n",
				sigFileName, bookFileName );
    fflush( stderr );
  }

  /* Read the book */
  if ( !strcmp( bookFileName, "-" ) ) {
    if ( MPR_VERBOSE ) fprintf( stderr, "mpr msg -- Reading the book from stdin..." );
    if ( book.load(stdin) == 0 ) {
      fprintf( stderr, "mpr error -- No atoms were found in stdin.\n" );
      fflush( stderr );
      return( ERR_BOOK );
    }
  }
  else {
    if ( MPR_VERBOSE ) fprintf( stderr, "mpr msg -- Reading the book from file [%s]...", bookFileName );
    if ( book.load( bookFileName ) == 0 ) {
      fprintf( stderr, "mpr error -- No atoms were found in the book file [%s].\n", bookFileName );
      fflush( stderr );
      return( ERR_BOOK );
    }
  }
  if ( MPR_VERBOSE ) { fprintf( stderr, "Done.\n" ); fflush( stderr ); }

  /* Read the residual */
  if ( resFileName ) {
    if ( MPR_VERBOSE ) {
      if ( !strcmp( resFileName, "-" ) ) fprintf( stderr, "mpr msg -- Reading the residual from stdin..." );
      else                               fprintf( stderr, "mpr msg -- Reading the residual from file [%s]...", resFileName );
    }
    sig = new MP_Signal_c( resFileName );
    if ( sig == NULL ) {
      fprintf( stderr, "mpr error -- Can't read a residual signal from file [%s].\n", resFileName );
      fflush( stderr );
      return( ERR_RES );
    }
    if ( MPR_VERBOSE ) { fprintf( stderr, "Done.\n" ); fflush( stderr ); }
  }
  /* If no file name was given, make an empty signal */
  else {
    sig = new MP_Signal_c( book.numChans, book.numSamples, book.sampleRate );
    if ( sig == NULL ) {
      fprintf( stderr, "mpr error -- Can't make a new signal.\n" );
      fflush( stderr );
      return( ERR_SIG );
    }
  }

  /* Reconstruct in addition to the residual */
  if ( MPR_VERBOSE ) fprintf( stderr, "mpr msg -- Rebuilding the signal..." );
  if ( book.substract_add( NULL, sig, NULL ) == 0 ) {
    fprintf( stderr, "mpr error -- No atoms were found in the book to rebuild the signal.\n" );
    fflush( stderr );
    return( ERR_BUILD );
  }
  if ( MPR_VERBOSE ) fprintf( stderr, "Done.\n" );

  /* De-emphasize the signal if needed */
  if (MPR_DEEMP != 0.0) {
    if ( MPR_VERBOSE ) { fprintf( stderr, "mpd msg -- De-emphasizing the signal..." ); fflush( stderr ); }
    sig->deemp( MPR_DEEMP );
    if ( MPR_VERBOSE ) { fprintf( stderr, "Done.\n" ); fflush( stderr ); }
  }

  /* Write the reconstructed signal */
  if ( MPR_VERBOSE ) {
    if ( !strcmp( sigFileName, "-" ) ) fprintf( stderr, "mpr msg -- Writing the rebuilt signal to stdout..." );
    else                               fprintf( stderr, "mpr msg -- Writing the rebuilt signal to file [%s]...", sigFileName );
  }
  if ( sig->wavwrite(sigFileName) == 0 ) {
    fprintf( stderr, "mpr error -- Can't write rebuilt signal to file [%s].\n", sigFileName );
    fflush( stderr );
    return( ERR_WRITE );
  }
  if ( MPR_VERBOSE ) fprintf( stderr, "Done.\n" );

  /* Clean the house */
  delete sig;

  /* End report */
  if ( !MPR_QUIET ) {
    fprintf( stderr, "mpr msg -- The resulting signal has [%lu] samples in [%d] channels, with sample rate [%d]Hz.\n",
	     book.numSamples, book.numChans, book.sampleRate );
    fprintf( stderr, "mpr msg -- Have a nice day !\n" );
  }
  fflush( stderr );
  fflush( stdout );

  return( 0 );
}
