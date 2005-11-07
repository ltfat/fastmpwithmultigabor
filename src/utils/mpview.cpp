/******************************************************************************/
/*                                                                            */
/*                                mpview.cpp                                  */
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

#include <stdio.h>
#include <string.h>

#include "mp_system.h"
#include "getopt.h"

static char *cvsid = "$Revision$";

/********************/
/* Global constants */
/********************/
#define MPVIEW_TRUE  (1==1)
#define MPVIEW_FALSE (0==1)

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_COL        3
#define ERR_ROW        4
#define ERR_PIX        5
#define ERR_BUILD      6
#define ERR_WRITE      7

/********************/
/* Global variables */
/********************/
int MPVIEW_QUIET      = MPVIEW_FALSE;
int MPVIEW_VERBOSE    = MPVIEW_FALSE;

/* Input/output file names: */
char *bookFileName    = NULL;
int numCols = 640;
int numRows = 480;
char *pixFileName  = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) {

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpview [options] (bookFILE.bin|-) tfmapFILE.flt\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Makes a time-frequency pixmap fill it with the time-frequency representation\n");
  fprintf( stdout, "     of the atoms contained in the book file bookFile.bin and write it to the file\n");
  fprintf( stdout, "     tfmapFILE.flt as a raw sequence of floats. The pixmap size is %dx%d pixels\n", numCols, numRows );
  fprintf( stdout, "     unless option --size is used.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     (bookFILE.bin|-)     A book of atoms, or stdin.\n" );
  fprintf( stdout, "     tfmapFILE.flt        The file where to write the pixmap in float.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     -s, --size=<numCols>x<numRows> : change the size of the pixmap.\n" );
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
  int val;
  char *p;
  char *ep;

  struct option longopts[] = {

    {"size",   required_argument, NULL, 's'},

    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "s:qvVh", longopts, &i)) != -1 ) {

    switch (c) {

    case 'h':
      usage();
      break;

    case 'q':
      MPVIEW_QUIET = MPVIEW_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpview DEBUG -- MPVIEW_QUIET is TRUE.\n" );
#endif
      break;

    case 'v':
      MPVIEW_VERBOSE = MPVIEW_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpview DEBUG -- MPVIEW_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpview -- Matching Pursuit library version %s -- mpview %s\n", VERSION, cvsid);
      exit(0);
      break;

    case 's':
      p = optarg;
      /* Skip the leading '=' */
      p++;
      /* Get the numCols value */
      val = strtol( p, &ep, 0 );
      if ( p == ep ) {
	fprintf( stderr, "mpview error -- Could not read a numCols value in the size (%s, pointing at %s).", optarg, p );
	return( 1 );
      }
      else numCols = val;
      /* Check the middle 'x' */
      p = ep;
      if ( *p != 'x' ) {
	fprintf( stderr, "mpview error -- Missing 'x' character between numCols and numRows in the size (%s, pointing at %s).", optarg, p );
	return( 1 );
      }
      /* Get the numRows value */
      p++; val = strtol( p, &ep,0 );
      if ( p == ep ) {
	fprintf( stderr, "mpview error -- Could not read a numRows value in the size (%s, pointing at %s).", optarg, p );
	return( 1 );
      }
      else numRows = val ;
      /* Check the ending ']' */
      p = ep;
      if (*p != '\0') {
	fprintf( stderr, "mpview error -- Spurious characters at the end of the size (%s, pointing at %s).", optarg, p );
	return( 1 );
      }
      break;
    default:
      fprintf( stderr, "mpview error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
      fprintf( stderr, "mpview DEBUG -- When exiting getopt, optind is [%d].\n", optind );
      fprintf( stderr, "mpview DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some file names / numbers are following the options */
  if ( (argc-optind) < 1 ) {
    fprintf(stderr, "mpview error -- You must indicate a file name (or - for stdin) for the book to use.\n");
    return( ERR_ARG );
  }
  if ( (argc-optind) < 2 ) {
    fprintf(stderr, "mpview error -- You must indicate a file name where to write the pixmap.\n");
    return( ERR_ARG );
  }

  /* Read the file names and numbers after the options */
  bookFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpview DEBUG -- Read book file name [%s].\n", bookFileName );
#endif
  pixFileName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpview DEBUG -- Read pixmap file name [%s].\n", pixFileName );
#endif

  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPVIEW_QUIET && MPVIEW_VERBOSE ) {
    fprintf(stderr, "mpview error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  return(0);
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) {
  MP_Book_c book;
  int i;
  /* Parse the command line */
  if ( argc == 1 ) usage();

  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpview error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    fflush( stderr );
    exit( ERR_ARG );
  }

  /* Report */
  if ( !MPVIEW_QUIET ) {
    fprintf( stderr, "mpview msg -- -------------------------------------\n" );
    fprintf( stderr, "mpview msg -- MPVIEW - MATCHING PURSUIT DISPLAY    \n" );
    fprintf( stderr, "mpview msg -- -------------------------------------\n" );
    fprintf( stderr, "mpview msg -- The command line was:\n" );
    for ( i = 0; i < argc; i++ ) fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\nmpview msg -- End command line.\n" );
    fprintf( stderr, "mpview msg -- Displaying book [%s] to pixmap [%s].\n",
				bookFileName, pixFileName );
    fflush( stderr );
  }

 /* Read the book */
  if ( !strcmp( bookFileName, "-" ) ) {
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "mpview msg -- Reading the book from stdin..." );
    if ( book.load(stdin) == 0 ) {
      fprintf( stderr, "mpview error -- No atoms were found in stdin.\n" );
      fflush( stderr );
      return( ERR_BOOK );
    }
  }
  else {
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "mpview msg -- Reading the book from file [%s]...", bookFileName );
    if ( book.load( bookFileName ) == 0 ) {
      fprintf( stderr, "mpview error -- No atoms were found in the book file [%s].\n", bookFileName );
      fflush( stderr );
      return( ERR_BOOK );
    }
  }
  if ( MPVIEW_VERBOSE ) { fprintf( stderr, "Done.\n" ); fflush( stderr ); }

  {
    /* Fill the pixmap */
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "mpview msg -- Initializing the pixmap..." );
    MP_TF_Map_c* tfmap = new MP_TF_Map_c(numCols,numRows,book.numChans,0.0,0.0,book.numSamples,0.5);
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "Done.\n" );
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "mpview msg -- Filling the pixmap..." );
    if ( book.add_to_tfmap(tfmap,NULL) == 0 ) {
      fprintf( stderr, "mpview error -- No atoms were found in the book to fill the pixmap.\n" );
      fflush( stderr );
      return( ERR_BUILD );
    }
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "Done.\n" );
    /* Save the pixmap */
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "mpview msg -- Dumping the pixmap to file [%s]...", pixFileName );
    if ( tfmap->dump_to_float_file(pixFileName,1) == 0 ) {
      fprintf( stderr, "\nmpview error -- Can't write filled pixmap to file [%s].\n", pixFileName );
      fflush( stderr );
      return( ERR_WRITE );
    }
    if ( MPVIEW_VERBOSE ) fprintf( stderr, "Done.\n" );

    /* Clean the house */
    delete tfmap;
  }

  /* End report */
  if ( !MPVIEW_QUIET ) {
    fprintf( stderr, "mpview msg -- The resulting pixmap has size [%d x %d] pixels and [%d] channel(s).\n",
	     numCols,numRows, (int)(book.numChans) );
    fprintf( stderr, "mpview msg -- Have a nice day !\n" );
  }
  fflush( stderr );
  fflush( stdout );
  return(0);
}
