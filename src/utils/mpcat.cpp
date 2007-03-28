/******************************************************************************/
/*                                                                            */
/*                                mpcat.cpp                                   */
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
#define MPC_TRUE  (1==1)
#define MPC_FALSE (0==1)

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
int MPC_QUIET      = MPC_FALSE;
int MPC_VERBOSE    = MPC_FALSE;
int MPC_FORCE      = MPC_FALSE;

/* Input/output file names: */
char *bookOutFileName = NULL;
char *bookInFileName  = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) {

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpcat (book1.bin|-) (book2.bin|-) ... (bookN.bin|-) (bookOut.bin|-)\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Concatenates the N books book1.bin...bookN.bin into the book file bookOut.bin.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     (bookN.bin|-)        At least 2 books (or stdin) to concatenate.\n" );
  fprintf( stdout, "     (bookOut.bin|-)      A book where to store the concatenated books, or stdout\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     -f, --force          Force the overwriting of bookOut.bin.\n" );
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
/*
 *  -q, --quiet               don't do any text output
 *  -v, --verbose             output processing info to stderr
 *  -V, --version             print version number
 *  -h, --help
 */

  int c, i;
  FILE *fid;

  struct option longopts[] = {
    {"force",   no_argument, NULL, 'f'},
    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "fqvVh", longopts, &i)) != -1 ) {

    switch (c) {

    case 'h':
      usage();
      break;

    case 'f':
      MPC_FORCE = MPC_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpcat DEBUG -- MPC_FORCE is TRUE.\n" );
#endif
      break;

    case 'q':
      MPC_QUIET = MPC_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpcat DEBUG -- MPC_QUIET is TRUE.\n" );
#endif
      break;

    case 'v':
      MPC_VERBOSE = MPC_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpcat DEBUG -- MPC_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpcat -- Matching Pursuit library version %s -- mpcat %s\n", VERSION, cvsid);
      exit(0);
      break;


    default:
      fprintf( stderr, "mpcat error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
  fprintf( stderr, "mpcat DEBUG -- When exiting getopt, optind is [%d].\n", optind );
  fprintf( stderr, "mpcat DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some books are following the options */
  if ( (argc-optind) < 3 ) {
    fprintf(stderr, "mpcat error -- There must be at least two books (or - for stdin) to concatenate,"
	    " plus a file name (or - for stdout) for the output book.\n");
    return( ERR_ARG );
  }
  
  /* Read the first book file name after the options */
  bookOutFileName = argv[argc-1];
#ifndef NDEBUG
  fprintf( stderr, "mpcat DEBUG -- Read output book file name [%s].\n", bookOutFileName );
#endif

  /***********************/
  /* Basic options check */

  /* Prevent an accidental erasing of an input file */
  if ( strcmp( bookOutFileName, "-" ) && (!MPC_FORCE) ) {
    if ( (fid = fopen( bookOutFileName, "rb" )) != NULL ) {
      fclose( fid );
      fprintf ( stderr, "mpcat error -- Output file [%s] exists. Delete it manually "
		"or use -f/--force if you want to overwrite it.\n",
		bookOutFileName );
      return( ERR_ARG );      
    }
  }

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPC_QUIET && MPC_VERBOSE ) {
    fprintf(stderr, "mpcat error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  return(0);
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) {


  MP_Book_c *book;

  int numBooks = 0;
  unsigned long int n; /* number of read atoms */
  FILE *fid;

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpcat error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    fflush( stderr );
    exit( ERR_ARG );
  }

  /* Make the book */
  book = MP_Book_c::create();
  if ( book == NULL ) {
      fprintf( stderr, "mpr error -- Can't create a new book.\n" );
      fflush( stderr );
      return( ERR_BOOK );
  }



  /* Load all the books and appends them to the first one: */
  while ( optind < (argc-1) ) {
    bookInFileName = argv[optind++];
    numBooks++;

#ifndef NDEBUG
  fprintf( stderr, "mpcat DEBUG -- Read book file name [%s] for book number [%d].\n",
	   bookInFileName, numBooks );
#endif

    if ( !strcmp( bookInFileName, "-" ) ) {
      if ( (n = book->load( stdin )) == 0 ) {
	if ( !MPC_QUIET ) {
	  fprintf ( stderr, "mpcat warning -- Can't read atoms for book number [%d] from stdin. I'm skipping this book.\n",
		    numBooks );
	  fflush( stderr );
	}
      }
      if ( MPC_VERBOSE ) fprintf ( stderr, "mpcat msg -- Loaded [%lu] atoms for book number [%d] from stdin.\n",
				   n, numBooks );

    }
    else {
      if ( (n = book->load( bookInFileName )) == 0 ) {
	if ( !MPC_QUIET ) {
	  fprintf ( stderr, "mpcat warning -- Can't read atoms for book number [%d] from file [%s]. I'm skipping this book.\n",
		    numBooks, bookInFileName );
	  fflush( stderr );
	}
      }
      if ( MPC_VERBOSE ) fprintf ( stderr, "mpcat msg -- Loaded [%lu] atoms for book number [%d] from file [%s].\n",
				   n, numBooks, bookInFileName );
    }
  }

  /* Write the book */
  if ( !strcmp( bookOutFileName, "-" ) ) {
    if ( book->print( stdout, MP_TEXT, NULL ) == 0 ) {
      fprintf ( stderr, "mpcat error -- No atoms could be written to stdout.\n" );
      return( ERR_WRITE );
    }
  }
  else {
    if ( book->print( bookOutFileName, MP_BINARY, NULL ) == 0 ) {
      fprintf ( stderr, "mpcat error -- No atoms could be written to file [%s].\n",
		bookOutFileName );
      return( ERR_WRITE );
    }
  }
  if ( MPC_VERBOSE ) {
    fprintf( stderr, "mpcat msg -- The resulting book contains [%lu] atoms.\n", book->numAtoms );
  }

  /* Clean the house */

  delete( book );

  return(0);
}
