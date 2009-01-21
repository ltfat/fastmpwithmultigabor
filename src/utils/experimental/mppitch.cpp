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
 * $Author: broy $
 * $Date: 2007-07-13 16:24:32 +0200 (Fri, 13 Jul 2007) $
 * $Revision: 1112 $
 *
 */

#include <mptk.h>
#include "../getopt.h"
//#include <time.h>
//#include <sys/time.h>
#include "../plugin/base/gabor_atom_plugin.h"
#include "../plugin/base/harmonic_atom_plugin.h"


static char *cvsid = "$Revision: 1112 $";

char* func = "mpr";

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

double pitchValue = 0.0;

/* Input/output file names: */


const char *bookFileName = NULL;
const char *sigFileName  = NULL;
const char *resFileName   = NULL;
const char *configFileName = NULL;



/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void )
{

  fprintf( stdout, " \n" );
  fprintf( stdout, " Usage:\n" );
  fprintf( stdout, "     mpd [options] -D dictFILE.xml (-n N|-s SNR) (sndFILE.wav|-) (bookFILE.bin|-) [residualFILE.wav]\n" );
  fprintf( stdout, " \n" );
  fprintf( stdout, " Synopsis:\n" );
  fprintf( stdout, "     Iterates Matching Pursuit on signal sndFILE.wav with dictionary dictFILE.xml\n" );
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
  fprintf( stdout, "     -C<FILE>, --config-file=<FILE>  Use the specified configuration file, \n" );
  fprintf( stdout, "                                     otherwise the MPTK_CONFIG_FILENAME environment variable will be\n" );
  fprintf( stdout, "                                     used to find the configuration file and set up the MPTK environment.\n" );
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
int parse_args(int argc, char **argv)
{

  int c, i;
  char *p;

  struct option longopts[] =
      {
        {"config-file",  required_argument, NULL, 'C'
        },
        {"deemp",   required_argument, NULL, 'd'},
        {"pitch-shifting-value", required_argument, NULL, 'K'},
        {"quiet",   no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"version", no_argument, NULL, 'V'},
        {"help",    no_argument, NULL, 'h'},
        {0, 0, 0, 0}
      };

  opterr = 0;
  optopt = '!';

  while ((c = getopt_long(argc, argv, "C:K:d:qvVh", longopts, &i)) != -1 )
    {

      switch (c)
        {

        case 'C':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -C : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -C or switch --config-file=.\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --config-file without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else configFileName = optarg;
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read config-file name [%s].\n", configFileName );
          break;
        case 'd':
#ifndef NDEBUG
          fprintf( stderr, "mpr DEBUG -- switch -d : optarg is [%s].\n", optarg );
#endif
          if (optarg == NULL)
            {
              fprintf( stderr, "mpr error -- After switch -d/--deemp= :\n" );
              fprintf( stderr, "mpr error -- the argument is NULL.\n" );
              fprintf( stderr, "mpr error -- (Did you use --deemp without the '=' character ?).\n" );
              fflush( stderr );
              return( ERR_ARG );
            }
          else MPR_DEEMP = strtod(optarg, &p);
          if ( (p == optarg) || (*p != 0) )
            {
              fprintf( stderr, "mpr error -- After switch -d/--deemp= :\n" );
              fprintf( stderr, "mpr error -- failed to convert argument [%s] to a double value.\n",
                       optarg );
              return( ERR_ARG );
            }
#ifndef NDEBUG
          fprintf( stderr, "mpr DEBUG -- Read deemp coeff [%g].\n", MPR_DEEMP );
#endif
          break;

        case 'K':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -K : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              mp_error_msg( func, "After switch -K/--pitch-shifting-value= :\n" );
              mp_error_msg( func, "the argument is NULL.\n" );
              mp_error_msg( func, "(Did you use --pitch-shifting-value without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else pitchValue = strtod(optarg, &p);
          if ( (p == optarg) || (*p != 0) )
            {
              mp_error_msg( func, "After switch -K/--pitch-shifting-value= :\n" );
              mp_error_msg( func, "failed to convert argument [%s] to a double value.\n",
                            optarg );
              return( ERR_ARG );
            }
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read Pitch value [%g].\n", pitchValue );
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
  if ( (argc-optind) < 1 )
    {
      fprintf(stderr, "mpr error -- You must indicate a file name (or - for stdin) for the book to use.\n");
      return( ERR_ARG );
    }
  if ( (argc-optind) < 2 )
    {
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
  if (optind < argc)
    {
      resFileName = argv[optind++];
#ifndef NDEBUG
      fprintf( stderr, "mpr DEBUG -- Read residual file name [%s].\n", resFileName );
#endif
    }



  /***********************/
  /* Basic options check */

  /***********************/
  /* Basic options check */


  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPR_QUIET && MPR_VERBOSE )
    {
      mp_error_msg( func, "Choose either one of --quiet or --verbose.\n");
      return( ERR_ARG );
    }

  return( 0 );
}


/**************************************************/
/* GLOBAL FUNCTIONS                               */
/**************************************************/
void free_mem(MP_Book_c* book, MP_Signal_c* sig)
{

  if ( sig  )  delete sig;
  if ( book )  delete book;

}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv )
{


  MP_Signal_c *sig = NULL;
  MP_Book_c *book = NULL;
  MP_Book_c *bookpitched = NULL;
  MP_Gabor_Atom_Plugin_c* newAtom = NULL;
  MP_Harmonic_Atom_Plugin_c* newAtomHarmonic = NULL;
  long int i = 0;

  /**************************************************/
  /* PRELIMINARIES                                  */
  /**************************************************/


  /* Load the MPTK environment */
  if(! (MPTK_Env_c::get_env()->load_environment_if_needed(configFileName)) ) {
    exit(1);
  }

  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) )
    {
      fprintf (stderr, "mpr error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
      fflush( stderr );
      exit( ERR_ARG );
    }

  /* Report */
  if ( !MPR_QUIET )
    {
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



  /* Make the book */
  book = MP_Book_c::create();
  if ( book == NULL )
    {
      fprintf( stderr, "mpr error -- Can't create a new book.\n" );
      fflush( stderr );
      return( ERR_BOOK );
    }

  /* Read the book */

  if ( !strcmp( bookFileName, "-" ) )
    {
      if ( MPR_VERBOSE ) fprintf( stderr, "mpr msg -- Reading the book from stdin...\n" );
      if ( book->load(stdin) == 0 )
        {
          fprintf( stderr, "mpr error -- No atoms were found in stdin.\n" );
          fflush( stderr );
          return( ERR_BOOK );
        }
    }
  else
    {
      if ( MPR_VERBOSE ) fprintf( stderr, "mpr msg -- Reading the book from file [%s]...\n", bookFileName );
      if ( book->load( bookFileName ) == 0 )
        {
          fprintf( stderr, "mpr error -- No atoms were found in the book file [%s].\n", bookFileName );
          fflush( stderr );
          return( ERR_BOOK );
        }
    }
  if ( MPR_VERBOSE )
    {
      fprintf( stderr, "Done.\n" );
      fflush( stderr );
    }

  /* Read the residual */
  if ( resFileName )
    {
      if ( MPR_VERBOSE )
        {
          if ( !strcmp( resFileName, "-" ) ) fprintf( stderr, "mpr msg -- Reading the residual from stdin...\n" );
          else                               fprintf( stderr, "mpr msg -- Reading the residual from file [%s]...\n", resFileName );
        }
      sig = MP_Signal_c::init( resFileName );
      if ( sig == NULL )
        {
          fprintf( stderr, "mpr error -- Can't read a residual signal from file [%s].\n", resFileName );
          fflush( stderr );
          return( ERR_RES );
        }
      if ( MPR_VERBOSE )
        {
          fprintf( stderr, "Done.\n" );
          fflush( stderr );
        }
    }
  /* If no file name was given, make an empty signal */
  else
    {
      sig = MP_Signal_c::init( book->numChans, book->numSamples, book->sampleRate );
      if ( sig == NULL )
        {
          fprintf( stderr, "mpr error -- Can't make a new signal.\n" );
          fflush( stderr );
          return( ERR_SIG );
        }
    }


  /* Make the pitched book */
  if ( !MPR_QUIET ) mp_info_msg( func, "Try to create a new book for pitched signal.\n" );
  bookpitched = MP_Book_c::create(sig->numChans, sig->numSamples, sig->sampleRate );
  if ( bookpitched == NULL )
    {
      mp_error_msg( func, "Failed to create a new book  for pitched signal.\n" );
      free_mem(book, sig);
      return( ERR_BOOK );
    }


  /****/



  if ( !MPR_QUIET ) mp_info_msg( func, "STARTING TO PROCESS PITCH\n" );

  for ( unsigned int nAtom = 0; nAtom < book->numAtoms; nAtom++ )
    {
    	
      if ( strcmp(book->atom[nAtom]->type_name(), "gabor")==0)
        {
        	
         // newAtom = dynamic_cast<MP_Gabor_Atom_Plugin_c*>(book->atom[nAtom]);
         newAtom = (MP_Gabor_Atom_Plugin_c*)book->atom[nAtom];
          if (newAtom->freq*pitchValue<0.5) newAtom->freq = newAtom->freq*pitchValue;
          newAtom->chirp = newAtom->chirp*pitchValue;
          for (i = 0; i<newAtom->numChans; i++)
            {
              newAtom->phase[i] = newAtom->phase[i]*pitchValue;
            }
          bookpitched->append( dynamic_cast<MP_Atom_c*>(newAtom) );
        }
      else
        {
          if ( strcmp(book->atom[nAtom]->type_name(), "harmonic")==0)
            {
              newAtomHarmonic = dynamic_cast<MP_Harmonic_Atom_Plugin_c*>(book->atom[nAtom]);
              if (newAtom->freq*pitchValue<0.5) newAtom->freq = newAtom->freq*pitchValue;
              newAtom->chirp = newAtom->chirp*pitchValue;
              for (i = 0; i<newAtom->numChans; i++)
                {
                  newAtom->phase[i] = newAtom->phase[i]*pitchValue;
                }
              bookpitched->append( dynamic_cast<MP_Atom_c*>(newAtom) );
            }
        
      else
        bookpitched->append(book->atom[nAtom]);
    }
}

if ( bookpitched->substract_add( NULL, sig, NULL ) == 0 )
  {
    fprintf( stderr, "mpr error -- No atoms were found in the book to rebuild the signal.\n" );
    fflush( stderr );
    return( ERR_WRITE );
  }

/* De-emphasize the signal if needed */
if (MPR_DEEMP != 0.0)
  {
    if ( MPR_VERBOSE )
      {
        fprintf( stderr, "mpd msg -- De-emphasizing the signal...\n" );
        fflush( stderr );
      }
    sig->deemp( MPR_DEEMP );
    if ( MPR_VERBOSE )
      {
        fprintf( stderr, "Done.\n" );
        fflush( stderr );
      }
  }

if ( sig->wavwrite(sigFileName) == 0 )
  {
    fprintf( stderr, "mpr error -- Can't write rebuilt signal to file [%s].\n", sigFileName);
    fflush( stderr );
    return( ERR_WRITE );
  }


/**************************************************/
/* FINAL SAVES AND CLEANUP                        */
/**************************************************/




/*******************/
/* Clean the house */
free_mem(book, sig);
if ( !MPR_QUIET ) mp_info_msg( func, "Have a nice day !\n" );
/* Release MPTK environnement */
MPTK_Env_c::get_env()->release_environment();
return( 0 );
}
