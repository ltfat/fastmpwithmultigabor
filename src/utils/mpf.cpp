/******************************************************************************/
/*                                                                            */
/*                                 mpf.cpp                                    */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* R�mi Gribonval                                                             */
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

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_SWITCH     3
#define ERR_WRITE      4
#define ERR_MALLOC     5


/********************/
/* Global variables */
/********************/
int MPF_QUIET      = MP_FALSE;
int MPF_VERBOSE    = MP_FALSE;

/* Table of arguments */
char MPF_USE[MP_NUM_PROPS+1] = { MP_FALSE, MP_FALSE, MP_FALSE,
				 MP_FALSE, MP_FALSE, MP_FALSE,
				 MP_FALSE };

MP_Real_t MPF_MIN[MP_NUM_PROPS+1] = { 0.0, 0.0, 0.0,
				      0.0, 0.0, 0.0,
				      0.0 };

char MPF_INCLUDE_MIN[MP_NUM_PROPS+1] = { MP_TRUE, MP_TRUE, MP_TRUE,
					 MP_TRUE, MP_TRUE, MP_TRUE,
					 MP_TRUE };

MP_Real_t MPF_MAX[MP_NUM_PROPS+1] = { 0.0, 0.0, 0.0,
				      0.0, 0.0, 0.0,
				      0.0 };

char MPF_INCLUDE_MAX[MP_NUM_PROPS+1] = { MP_TRUE, MP_TRUE, MP_TRUE,
					 MP_TRUE, MP_TRUE, MP_TRUE,
					 MP_TRUE };

char MPF_NEGATE_PROP[MP_NUM_PROPS+1] = { MP_FALSE, MP_FALSE, MP_FALSE,
					 MP_FALSE, MP_FALSE, MP_FALSE,
					 MP_FALSE };

char MPF_USE_UCF = MP_FALSE;
char MPF_USE_LCF = MP_FALSE;
char MPF_USE_UCL = MP_FALSE;
char MPF_USE_LCL = MP_FALSE;
char MPF_USE_UCP = MP_FALSE;
char MPF_USE_LCP = MP_FALSE;

/* Atom type */
char *MPF_TEST_TYPE = NULL;

/* Input/output file names: */
char *bookInName  = NULL;
char *bookYesName = NULL;
char *bookNoName  = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) {

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpf --PROPERTY1=[min:max] ... --PROPERTY_N=[min:max] (bookIn.bin|-) [bookYes.bin|-] [bookNo.bin]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Filters the atoms contained in bookIn.bin (or stdin), stores those which satisfy\n" );
  fprintf( stdout, "     the indicated properties in bookYes.bin (or stdout) and the others in bookNo.bin.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Mandatory arguments:\n" );
  fprintf( stdout, "     (bookIn.bin|-)      A book of input atoms, or stdin.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Optional arguments:\n" );
  fprintf( stdout, "     (bookYes.bin|-)     A file (or stdout) to store the book of atoms which satisfy the indicates properties.\n" );
  fprintf( stdout, "     bookNo.bin          A file to store a book of atoms which do not satisfy the indicated properties.\n" );
  fprintf( stdout, "     If no output files are given, the atoms are just counted and their number is reported in stderr.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     One or more of the following switches:\n" );
  fprintf( stderr, "     --index=[min:max]    / -i [min:max] : keep the atoms ordered from min to max in the book\n");
  fprintf( stderr, "     --length=[min:max]   / -l [min:max] : keep a specific range of atom lengths (in number of samples)\n");
  fprintf( stderr, "     --Length=[min:max]   / -L [min:max] : keep a specific range of atom lengths (in seconds)\n");
  fprintf( stderr, "     --position=[min:max] / -p [min:max] : keep a specific range of atom positions (in number of samples)\n");
  fprintf( stderr, "     --Position=[min:max] / -P [min:max] : keep a specific range of atom positions (in seconds)\n");
  fprintf( stderr, "     --freq=[min:max]     / -f [min:max] : keep a specific frequency range (in normalized values between 0 and 0.5)\n");
  fprintf( stderr, "     --Freq=[min:max]     / -F [min:max] : keep a specific frequency range (in Hz)\n");
  fprintf( stderr, "     --amp=[min:max]      / -a [min:max] : keep a specific range of amplitudes\n");
  fprintf( stderr, "     --chirp=[min:max]    / -c [min:max] : keep a specific range of chirp factors\n");
  fprintf( stderr, "     The intervals can exclude the min or max value by using reverted braces,\n" );
  fprintf( stdout, "     e.g. ]min:max] will exclude the min value.\n");
  fprintf( stderr, "     The intervals can be negated with prepending the '^' character, e.g. ^[min:max].\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     --type=gabor|harmonic|dirac / -t gabor|harmonic|dirac : test the atom type.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Other optional arguments are:\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, "     -q, --quiet          No text output.\n" );
  fprintf( stdout, "     -v, --verbose        Verbose.\n" );
  fprintf( stdout, "     -V, --version        Output the version and exit.\n" );
  fprintf( stdout, "     -h, --help           This help.\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Example:\n" );
  fprintf( stdout, "     Take all the atoms with a frequency lower than 50Hz and higher than 1000Hz\n" );
  fprintf( stdout, "     among the first 100 atoms of bookIn.bin, store them in bookYes.bin\n" );
  fprintf( stdout, "     and store all the others in bookNo.bin:\n" );
  fprintf( stdout, "     mpf --index=[0:100[ --Freq=^[50:1000] bookIn.bin bookYes.bin bookNo.bin\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Note:\n" );
  fprintf( stdout, "     Only one instance of each property is allowed. If you want to elaborate more complicated domains,\n" );
  fprintf( stdout, "     use a pipe.\n" );
  fprintf( stdout, " \n" );

  exit(0);
}


/**************************************************/
/* PARSING OF THE ARGUMENTS                       */
/**************************************************/
/* Local parsing of a [min:max] interval */
int parse_interval( char* arg, MP_Real_t* min, MP_Real_t* max, char* includeMin, char* includeMax, char* negate ) {

  double val;
  char* p;
  char* ep;

  /* Point the beginning of the interval */
  p = arg;
  /* Check a potential negation with '^' */
  if ( *p == '^' ) { *negate = MP_TRUE;  p++; }
  else               *negate = MP_FALSE;
  /* Check the opening '[' */
  switch( *p ) {
  case '[': *includeMin = MP_TRUE;  break;
  case ']': *includeMin = MP_FALSE; break;
  default:
    fprintf( stderr, "mpf error -- Missing '[' or ']' character at the beginning of the interval (%s, pointing at %s).", arg, p );
    return( 1 );
  }
  /* Get the min value */
  p++; val = strtod( p, &ep );
  if ( p == ep ) {
    fprintf( stderr, "mpf error -- Could not read a min value in the interval (%s, pointing at %s).", arg, p );
    return( 1 );
  }
  else *min = (MP_Real_t)( val );
  /* Check the middle ':' */
  p = ep;
  if ( *p != ':' ) {
    fprintf( stderr, "mpf error -- Missing ':' character between min and max in the interval (%s, pointing at %s).", arg, p );
    return( 1 );
  }
  /* Get the max value */
  p++; val = strtod( p, &ep );
  if ( p == ep ) {
    fprintf( stderr, "mpf error -- Could not read a min value in the interval (%s, pointing at %s).", arg, p );
    return( 1 );
  }
  else *max = (MP_Real_t)( val );
  /* Check the ending ']' */
  p = ep;
  switch( *p ) {
  case '[': *includeMax = MP_FALSE; break;
  case ']': *includeMax = MP_TRUE;  break;
  default:
    fprintf( stderr, "mpf error -- Missing '[' or ']' character at the end of the interval (%s, pointing at %s).", arg, p );
    return( 1 );
  }

#ifndef NDEBUG
  fprintf( stderr, "mpf DEBUG -- Parsed interval %s: min=%g max=%g includeMin=%d includeMax=%d negate=%d\n",
	   arg, *min, *max, *includeMin, *includeMax, *negate );
#endif

  return( 0 );
}

/* General parsing */
int parse_args(int argc, char **argv) {

  int c, i;
  MP_Real_t min, max;
  char includeMin, includeMax, negate;

  struct option longopts[] = {
    {"amp",     required_argument, NULL, 'a'},
    {"chirp",   required_argument, NULL, 'c'},
    {"freq",    required_argument, NULL, 'f'},
    {"Freq",    required_argument, NULL, 'F'},
    {"index",   required_argument, NULL, 'i'},
    {"length",  required_argument, NULL, 'l'},
    {"Length",  required_argument, NULL, 'L'},
    {"position",required_argument, NULL, 'p'},
    {"Position",required_argument, NULL, 'P'},
    {"type",    required_argument, NULL, 't'},

    {"quiet",   no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"version", no_argument, NULL, 'V'},
    {"help",    no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "a:c:f:i:l:p:t:F:L:P:qvVh", longopts, &i)) != -1 ) {

    switch (c) {

    case 'a':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --amp=/-a (argument is: %s).\n",
		 optarg );
      }
      else {
	MPF_USE[MP_AMP_PROP] = MP_TRUE;
	MPF_MIN[MP_AMP_PROP] = min;
	MPF_MAX[MP_AMP_PROP] = max;
	MPF_INCLUDE_MIN[MP_AMP_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_AMP_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_AMP_PROP] = negate;
      }
      break;


    case 'c':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --chirp=/-s (argument is: %s).\n",
		 optarg );
      }
      else {
	MPF_USE[MP_CHIRP_PROP] = MP_TRUE;
	MPF_MIN[MP_CHIRP_PROP] = min;
	MPF_MAX[MP_CHIRP_PROP] = max;
	MPF_INCLUDE_MIN[MP_CHIRP_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_CHIRP_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_CHIRP_PROP] = negate;
      }
      break;


    case 'f':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --freq=/-f (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (min > 0.5) || (max < 0) || (max > 0.5) ) {
	  fprintf( stderr, "mpf error -- In argument --freq=/-f (argument is: %s): the normalized frequencies must have"
		   " values between 0 and 0.5.\n", optarg );
	  fprintf( stderr, "mpf error -- (If you meant frequencies in Hertz, use --Freq=/-F.)\n" );
	  return( ERR_ARG );
	}
	MPF_USE[MP_FREQ_PROP] = MP_TRUE;
	MPF_MIN[MP_FREQ_PROP] = min;
	MPF_MAX[MP_FREQ_PROP] = max;
	MPF_INCLUDE_MIN[MP_FREQ_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_FREQ_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_FREQ_PROP] = negate;
	MPF_USE_LCF = MP_TRUE;
      }
      break;


    case 'F':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --Freq=/-F (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --Freq=/-F (argument is: %s): frequencies can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_FREQ_PROP] = MP_TRUE;
	MPF_MIN[MP_FREQ_PROP] = min;
	MPF_MAX[MP_FREQ_PROP] = max;
 	MPF_INCLUDE_MIN[MP_FREQ_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_FREQ_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_FREQ_PROP] = negate;
	MPF_USE_UCF = MP_TRUE;
     }
      break;


    case 'i':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --index=/-i (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --index=/-i (argument is: %s): atom indexes can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_NUM_PROPS] = MP_TRUE;
	MPF_MIN[MP_NUM_PROPS] = min;
	MPF_MAX[MP_NUM_PROPS] = max;
  	MPF_INCLUDE_MIN[MP_NUM_PROPS] = includeMin;
 	MPF_INCLUDE_MAX[MP_NUM_PROPS] = includeMax;
	MPF_NEGATE_PROP[MP_NUM_PROPS] = negate;
     }
      break;


    case 'l':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --length=/-l (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --length=/-l (argument is: %s): atom lengths can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_LEN_PROP] = MP_TRUE;
	MPF_MIN[MP_LEN_PROP] = min;
	MPF_MAX[MP_LEN_PROP] = max;
 	MPF_INCLUDE_MIN[MP_LEN_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_LEN_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_LEN_PROP] = negate;
	MPF_USE_LCL = MP_TRUE;
      }
      break;


    case 'L':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --Length=/-L (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --Length=/-L (argument is: %s): atom lengths can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_LEN_PROP] = MP_TRUE;
	MPF_MIN[MP_LEN_PROP] = min;
	MPF_MAX[MP_LEN_PROP] = max;
 	MPF_INCLUDE_MIN[MP_LEN_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_LEN_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_LEN_PROP] = negate;
	MPF_USE_UCL = MP_TRUE;
      }
      break;


    case 'p':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --position=/-p (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --position=/-p (argument is: %s): atom positions can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_POS_PROP] = MP_TRUE;
	MPF_MIN[MP_POS_PROP] = min;
	MPF_MAX[MP_POS_PROP] = max;
  	MPF_INCLUDE_MIN[MP_POS_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_POS_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_POS_PROP] = negate;
	MPF_USE_LCP = MP_TRUE;
      }
      break;


    case 'P':
      if ( parse_interval( optarg, &min, &max, &includeMin, &includeMax, &negate ) ) {
	fprintf( stderr, "mpf error -- Can't parse an interval after --Position=/-P (argument is: %s).\n",
		 optarg );
      }
      else {
	if ( (min < 0) || (max < 0) ) {
	  fprintf( stderr, "mpf error -- In argument --Position=/-P (argument is: %s): atom positions can't be negative.\n",
		   optarg );
	  return( ERR_ARG );
	}
	MPF_USE[MP_POS_PROP] = MP_TRUE;
	MPF_MIN[MP_POS_PROP] = min;
	MPF_MAX[MP_POS_PROP] = max;
  	MPF_INCLUDE_MIN[MP_POS_PROP] = includeMin;
 	MPF_INCLUDE_MAX[MP_POS_PROP] = includeMax;
	MPF_NEGATE_PROP[MP_POS_PROP] = negate;
	MPF_USE_UCP = MP_TRUE;
      }
      break;


    case 't':
#ifndef NDEBUG
      fprintf( stderr, "mpf DEBUG -- switch -t : optarg is [%s].\n", optarg );
#endif
      if (optarg == NULL) {
	fprintf( stderr, "mpf error -- After switch -t or switch --type=.\n" );
	fprintf( stderr, "mpf error -- the argument is NULL.\n" );
	fprintf( stderr, "mpf error -- (Did you use --type without the '=' character ?).\n" );
	return( ERR_ARG );
      }
      else MPF_TEST_TYPE = optarg;
#ifndef NDEBUG
      fprintf( stderr, "mpf DEBUG -- Read atom type [%s].\n", MPF_TEST_TYPE );
#endif
      break;


      /* General options */

    case 'h':
      usage();
      break;

    case 'q':
      MPF_QUIET = MP_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpf DEBUG -- MPF_QUIET is TRUE.\n" );
#endif
      break;

    case 'v':
      MPF_VERBOSE = MP_TRUE;
#ifndef NDEBUG
      fprintf( stderr, "mpf DEBUG -- MPF_VERBOSE is TRUE.\n" );
#endif
      break;


    case 'V':
      fprintf(stdout, "mpf -- Matching Pursuit library version %s -- mpf %s\n", VERSION, cvsid);
      exit(0);
      break;


    default:
      fprintf( stderr, "mpf error -- The command line contains the unrecognized option [%s].\n",
	       argv[optind-1] );
      return( ERR_ARG );

    } /* end switch */

  } /* end while */


#ifndef NDEBUG
      fprintf( stderr, "mpf DEBUG -- When exiting getopt, optind is [%d].\n", optind );
      fprintf( stderr, "mpf DEBUG -- (argc is [%d].)\n", argc );
#endif

  /* Check if some file names are following the options */
  if ( (argc-optind) < 1 ) {
    fprintf(stderr, "mpf error -- You must indicate a file name (or - for stdin) for an input book file.\n");
    return( ERR_ARG );
  }

  /* Read the file names after the options */
  bookInName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpf DEBUG -- Read input book file name [%s].\n", bookInName );
#endif
  bookYesName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpf DEBUG -- Read output bookYes file name [%s].\n", bookYesName );
#endif
  if (optind < argc) {
    bookNoName = argv[optind++];
#ifndef NDEBUG
  fprintf( stderr, "mpf DEBUG -- Read output bookNo file name [%s].\n", bookNoName );
#endif
  }


  /***********************/
  /* Basic options check */

  /* Can't have two times the same property different units */
  if ( MPF_USE_LCF && MPF_USE_UCF ) {
    fprintf(stderr, "mpf error -- Choose either one of --freq=/-f or --Freq=/-F.\n");
    return( ERR_ARG );
  }
  if ( MPF_USE_LCL && MPF_USE_UCL ) {
    fprintf(stderr, "mpf error -- Choose either one of --length=/-l or --Length=/-l.\n");
    return( ERR_ARG );
  }
  if ( MPF_USE_LCP && MPF_USE_UCP ) {
    fprintf(stderr, "mpf error -- Choose either one of --position=/-p or --Position=/-P.\n");
    return( ERR_ARG );
  }

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPF_QUIET && MPF_VERBOSE ) {
    fprintf(stderr, "mpf error -- Choose either one of --quiet or --verbose.\n");
    return( ERR_ARG );
  }

  return(0);
}


/**************************************************/
/* MISC                                           */
/**************************************************/
int test_satisfaction( int k, unsigned long int n, MP_Atom_c* atom ) {

  int retVal = MP_TRUE;;

  /* Testing versus the MIN */
  /* - if not testing the index, call atom.satisfies(): */
  if ( k != MP_NUM_PROPS ) {
    if ( MPF_INCLUDE_MIN[k] ) retVal = (retVal && atom->satisfies( k, MP_SUPEQ, MPF_MIN[k] ) );
    else                      retVal = (retVal && atom->satisfies( k, MP_SUPER, MPF_MIN[k] ) );
  }
  /* - else, if testing the index, cook it yourself: */
  else {
    if ( MPF_INCLUDE_MIN[k] ) retVal = retVal && ( n >= MPF_MIN[k] );
    else                      retVal = retVal && ( n >  MPF_MIN[k] );
  }
    
  /* Testing versus the MAX */
  /* - if not testing the index, call atom.satisfies(): */
  if ( k != MP_NUM_PROPS ) {
    if ( MPF_INCLUDE_MAX[k] ) retVal = (retVal && atom->satisfies( k, MP_INFEQ, MPF_MAX[k] ) );
    else                      retVal = (retVal && atom->satisfies( k, MP_INFER, MPF_MAX[k] ) );
  }
  /* - else, if testing the index, cook it yourself: */
  else {
    if ( MPF_INCLUDE_MAX[k] ) retVal = retVal && ( n <= MPF_MAX[k] );
    else                      retVal = retVal && ( n <  MPF_MAX[k] );
  }
    
  /* Negate if needed */
  if ( MPF_NEGATE_PROP[k] ) retVal = !retVal;

  return( retVal );
}

/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) {

  MP_Book_c *book;
  int decision = MP_TRUE;
  unsigned long int numPositive = 0;

  unsigned long int n; /* loop variable for atoms */
  int k;
  MP_Mask_c* mask = NULL;

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) ) {
    fprintf (stderr, "mpf error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
    fflush( stderr );
    exit( ERR_ARG );
  }

  /* Make the book */
  book = MP_Book_c::init();
  if ( book == NULL ) {
      fprintf( stderr, "mpr error -- Can't create a new book.\n" );
      fflush( stderr );
      return( ERR_BOOK );
  }

  /* Load the book */
  if ( !strcmp( bookInName, "-" ) ) book->load( stdin );
  else                              book->load( bookInName );

  /* Rectify the min/max if asking for non-normed values */
  if ( MPF_USE_UCF && MPF_USE[MP_FREQ_PROP] ) {
    MPF_MIN[MP_FREQ_PROP] = MPF_MIN[MP_FREQ_PROP]/book->sampleRate;
    MPF_MAX[MP_FREQ_PROP] = MPF_MAX[MP_FREQ_PROP]/book->sampleRate;
  }
  if ( MPF_USE_UCL && MPF_USE[MP_LEN_PROP] ) {
    MPF_MIN[MP_LEN_PROP] = MPF_MIN[MP_LEN_PROP]*book->sampleRate;
    MPF_MAX[MP_LEN_PROP] = MPF_MAX[MP_LEN_PROP]*book->sampleRate;
  }
  if ( MPF_USE_UCP && MPF_USE[MP_POS_PROP] ) {
    MPF_MIN[MP_POS_PROP] = MPF_MIN[MP_POS_PROP]*book->sampleRate;
    MPF_MAX[MP_POS_PROP] = MPF_MAX[MP_POS_PROP]*book->sampleRate;
  }

  /* Allocate the mask */
  if ( (mask = MP_Mask_c::init( book->numAtoms )) == NULL ) {
    fprintf( stderr, "mpf error -- Can't create a new mask with [%lu] elements.\n", book->numAtoms );
    return( ERR_MALLOC );
  }
  
  /* Fill the mask */
  for (n = 0; n < book->numAtoms; n++) {
    /* Reset the decision for the current atom */
    decision = MP_TRUE;
    /* Test the atom type */
    if (MPF_TEST_TYPE) decision = ( !strcmp( MPF_TEST_TYPE, book->atom[n]->type_name() ) );
    /* Browse the properties */
    for ( k = 0; ( (k <= MP_NUM_PROPS) && decision ); k++ ) {
      if ( MPF_USE[k] ) decision = ( decision && test_satisfaction( k, n, book->atom[n] ) );
    }
    /* Fill the mask */
    mask->sieve[n] = decision;
    if ( decision ) numPositive++;
  }

  /* Report */
  if ( !MPF_QUIET ) fprintf( stderr, "mpf msg -- Out of the [%lu] original atoms, [%lu] atoms satisfy the required properties.\n",
			     book->numAtoms, numPositive );
  if ( !MPF_QUIET ) fprintf( stderr, "mpf msg -- Out of the [%lu] original atoms, [%lu] atoms DO NOT satisfy the required properties.\n",
			     book->numAtoms, book->numAtoms - numPositive );

  /* Write the YES book */
  if ( bookYesName ) {
    if ( !strcmp( bookYesName, "-" ) ) {
      n = book->print( stdout, MP_TEXT, mask );
      fflush( stdout );
      if ( MPF_VERBOSE ) fprintf( stderr, "mpf msg -- [%lu] atoms were written to stdout.\n", n );
    }
    else {
      n = book->print( bookYesName, MP_BINARY, mask );
      if ( MPF_VERBOSE ) fprintf( stderr, "mpf msg -- [%lu] atoms were written to file [%s].\n", n, bookYesName );
    }
  }

  /* Write the NO book */
  if ( bookNoName ) {
    /* Revert the mask */
    //for (n = 0; n < book->numAtoms; n++) mask[n] = !(mask[n]);
    *mask = !(*mask);
    /* Write the book */
    n = book->print( bookNoName, MP_BINARY, mask );
    /* Report */
    if ( MPF_VERBOSE ) fprintf( stderr, "mpf msg -- [%lu] atoms were written to file [%s].\n", n, bookNoName );
  }

  /* Clean the house */
  delete( book );
  delete( mask );

  return( 0 );
}
