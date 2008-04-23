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
 * $Author: broy $
 * $Date: 2007-07-13 16:24:32 +0200 (Fri, 13 Jul 2007) $
 * $Revision: 1112 $
 *
 */

#include <mptk.h>
#include <sstream>
#include <string>

#include "mp_system.h"
#include "getopt.h"

char* func = "mpd_demix";

static char *cvsid = "$Revision: 1112 $";
/********************/
/* Global constants */
/********************/
#define MPD_TRUE  (1==1)
#define MPD_FALSE (0==1)
#define MPD_ALLOC_BLOCK_SIZE   1000

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_DICT       3
#define ERR_SIG        4
#define ERR_CORE       5
#define ERR_DECAY      6
#define ERR_OPEN       7
#define ERR_WRITE      8
#define ERR_READ       9
#define ERR_MALLOC     10
#define ERR_NEW        11
#define ERR_NCHANS     12
#define ERR_SIGINIT    13
#define ERR_MIXER      14

/********************/
/* Global variables */
/********************/

#define MPD_DEFAULT_NUM_ITER   ULONG_MAX
unsigned long int MPD_REPORT_HIT = ULONG_MAX; /* Default: never report during the main loop. */
unsigned long int MPD_SAVE_HIT   = ULONG_MAX; /* Default: never save during the main loop. */
unsigned long int MPD_SNR_HIT    = ULONG_MAX; /* Default: never test the snr during the main loop. */

int MPD_QUIET      = MPD_FALSE;
int MPD_VERBOSE    = MPD_FALSE;

unsigned long int MPD_NUM_ITER = MPD_DEFAULT_NUM_ITER;
#define MPD_DEFAULT_SNR        0.0
double MPD_SNR   = MPD_DEFAULT_SNR;
int MPD_USE_SNR  = MPD_FALSE;
int MPD_USE_ITER = MPD_FALSE;

double MPD_PREEMP = 0.0;

/* Input/output file names: */
char *dictFileName = NULL;
char *sndFileName   = NULL;
char *bookFileName  = NULL;
char *resFileName   = NULL;
char *decayFileName = NULL;
char *mixerFileName = NULL;
char *srcSeqFileName = NULL;
const char *configFileName = NULL;


/* --------------------------------------- */
void usage( void )
{
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
  fprintf( stdout, "     -C<FILE>, --config-file=<FILE>  Use the specified configuration file, \n" );
  fprintf( stdout, "                                     otherwise the MPTK_CONFIG_FILENAME environment variable will be\n" );
  fprintf( stdout, "                                     used to find the configuration file and set up the MPTK environment.\n" );
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
int parse_args(int argc, char **argv)
{
  /* --------------------------------------- */

  int c, i;
  char *p;

  struct option longopts[] =
      {
      	{"config-file",  required_argument, NULL, 'C'},
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

  while ((c = getopt_long(argc, argv, "C:D:E:M:Q:R:S:T:n:s:qvVh", longopts, &i)) != -1 )
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

        case 'D':
          mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -D : optarg is [%s].\n", optarg );
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -D or switch --dictionary=.\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --dictionary without the '=' character ?).\n" );
              return( ERR_ARG );
            }
        else dictFileName = optarg;
         mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read dictionary file name [%s].\n", dictFileName );

          break;


        case 'E':
#ifndef NDEBUG
          fprintf( stderr, "mpd_demix DEBUG -- switch -E : optarg is [%s].\n", optarg );
#endif
          if (optarg == NULL)
            {
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
          if (optarg == NULL)
            {
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
          if (optarg == NULL)
            {
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
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -R or switch --report-hit= :\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --report-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else MPD_REPORT_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
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
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -S or switch --save-hit= :\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --save-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else MPD_SAVE_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
              fprintf( stderr, "mpd_demix error -- After switch -S or switch --save-hit= :\n" );
              fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to an unsigned long value.\n",
                       optarg );
              return( ERR_ARG );
            }
          break;


        case 'T':
#ifndef NDEBUG
          fprintf( stderr, "mpd_demix DEBUG -- switch -T : optarg is [%s].\n", optarg );
#endif
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -T or switch --snr-hit= :\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --snr-hit without the '=' character ?).\n" );
              return( ERR_ARG );
            }
          else MPD_SNR_HIT = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
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
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -n/--num-iter=/--num-atom= :\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --numiter or --numatom without the '=' character ?).\n" );
              fflush( stderr );
              return( ERR_ARG );
            }
          else MPD_NUM_ITER = strtoul(optarg, &p, 10);
          if ( (p == optarg) || (*p != 0) )
            {
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
          if (optarg == NULL)
            {
              fprintf( stderr, "mpd_demix error -- After switch -s/--snr= :\n" );
              fprintf( stderr, "mpd_demix error -- the argument is NULL.\n" );
              fprintf( stderr, "mpd_demix error -- (Did you use --snr without the '=' character ?).\n" );
              fflush( stderr );
              return( ERR_ARG );
            }
          else
            {
              MPD_SNR = strtod(optarg, &p);
            }
          if ( (p == optarg) || (*p != 0) )
            {
              fprintf( stderr, "mpd_demix error -- After switch -s/--snr= :\n" );
              fprintf( stderr, "mpd_demix error -- failed to convert argument [%s] to a double value.\n",
                       optarg );
              return( ERR_ARG );
            }
          else
            {
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
  if ( (argc-optind) < 1 )
    {
      fprintf(stderr, "mpd error -- You must indicate a file name (or - for stdin) for the signal to analyze.\n");
      return( ERR_ARG );
    }
  if ( (argc-optind) < 2 )
    {
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
  if (optind < argc)
    {
      resFileName = argv[optind++];
#ifndef NDEBUG
      fprintf( stderr, "mpd_demix DEBUG -- Read residual file name [%s].\n", resFileName );
#endif
    }


  /***********************/
  /* Basic options check */

  /* Can't have quiet AND verbose (make up your mind, dude !) */
  if ( MPD_QUIET && MPD_VERBOSE )
    {
      mp_error_msg( func, "Choose either one of --quiet or --verbose.\n");
      return( ERR_ARG );
    }

  /* Was dictionary file name given ? */
  /* if ( dictFileName == NULL ) {
     fprintf(stderr, "mpd_demix error -- You must specify a dictionary using switch -D/--dictionary= .\n");
     return( ERR_ARG );
     } */

  /* Was mixer file name given ? */
  if ( mixerFileName == NULL )
    {
      mp_error_msg( func, "You must specify a mixer matrix using switch -M/--mix-matrix= .\n");
      return( ERR_ARG );
    }

  /* Must have one of --num-iter or --snr to tell the algorithm where to stop */
  if ( (!MPD_USE_SNR) && (!MPD_USE_ITER) )
    {
      mp_error_msg( func, "You must specify one of : --num-iter=n/--num-atoms=n\n" );
      mp_error_msg( func, "                     or   --snr=%%f\n" );
      return( ERR_ARG );
    }

  /* If snr is given without a snr hit value, test the snr on every iteration */
  if ((MPD_SNR_HIT == ULONG_MAX) && MPD_USE_SNR ) MPD_SNR_HIT = 1;

  /* If having both --num-iter AND --snr, warn */
  if ( (!MPD_QUIET) && MPD_USE_SNR && MPD_USE_ITER )
    {
      mp_warning_msg( func, "The option --num-iter=/--num-atoms= was specified together with the option --snr=.\n" );
      mp_warning_msg( func, "The algorithm will stop when the first of either conditions is reached.\n" );
      mp_warning_msg( func, "(Use --help to get help if this is not what you want.)\n" );
    }

  return(0);
}

  std::vector<MP_Book_c*> *bookArray = NULL;
  std::vector<MP_Dict_c*> *dictArray = NULL;

  MP_Mixer_c* mixer =NULL;
  MP_Mpd_demix_Core_c* mpdDemixCore = NULL;

/*---------------------*/
void free_mem( std::vector<MP_Dict_c*> *dictArray, std::vector<MP_Book_c*> *bookArray, MP_Mixer_c* mixer, MP_Signal_c *inSignal, MP_Mpd_demix_Core_c* mpdDemixCore, char ** dictFileNameList )
{
  /*---------------------*/
  
  if ( mpdDemixCore ) delete mpdDemixCore;
  if ( dictArray )
    {
      for ( unsigned int j = 0; j < mixer->numSources; j++ ) if ( dictArray->at(j) ) delete dictArray->at(j);
     delete (dictArray);
    }
  if ( bookArray )
    {
      for ( unsigned int j = 0; j < mixer->numSources; j++ ) if ( bookArray->at(j) ) delete bookArray->at(j);
      delete (bookArray);
    }
  
 if ( inSignal ) delete ( inSignal );
  
  if(dictFileNameList) { for ( unsigned int j = 0; j < mixer->numSources; j++ ) free(dictFileNameList[j]);
  free(dictFileNameList);
  }

 if ( mixer ) delete mixer;
}


/* --------------------------------------- */
int main( int argc, char **argv )
{
  /* --------------------------------------- */
  std::vector<MP_Book_c*> *bookArray = NULL;
  std::vector<MP_Dict_c*> *dictArray = NULL;
  MP_Signal_c *inSignal = NULL;
  MP_Mixer_c* mixer =NULL;
  MP_Mpd_demix_Core_c* mpdDemixCore = NULL;
  FILE* fid;

  unsigned short int numDictionaries;
  unsigned long int i;
  char line[1024]; 
  char ** dictFileNameList = NULL; 
  
  /* Load Mptk environnement */
  MPTK_Env_c::get_env()->load_environment(configFileName);

  
  /* Parse the command line */
  if ( argc == 1 ) usage();
  if ( parse_args( argc, argv ) )
    {
      mp_error_msg( func, "mpd_demix error -- Please check the syntax of your command line. (Use --help to get some help.)\n" );
      exit( ERR_ARG );
    }

  /* Re-print the command line */
  if ( !MPD_QUIET )
    {
      mp_info_msg( func, "--------------------------------------------------------------------\n" );
      mp_info_msg( func, "MPD_DEMIX - MATCHING PURSUIT DECOMPOSITION FOR BLIND SOURCE SEPARATION\n" );
      mp_info_msg( func, "--------------------------------------------------------------------\n" );
      mp_info_msg( func, "The command line was:\n" );
      for ( i=0; i<(unsigned long int)argc; i++ )
        {
          fprintf( stderr, "%s ", argv[i] );
        }
        fprintf( stderr, "\n");
        fflush( stderr );
      mp_info_msg( func, "End command line.\n" );
    }
  int posDot;
  char * pch;
  pch=strrchr(mixerFileName,'.');
  posDot = pch-mixerFileName+1;
  char last[10] = "not";
  strncpy( last, mixerFileName + posDot, 3 );
if ( !strcmp( last ,"txt" ) ) mixer = MP_Mixer_c::creator_from_txt_file( mixerFileName );
else { 
	mp_error_msg( func, "mixer type file not recognised [%s] .\n", mixerFileName );
	free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
return( ERR_MIXER  );
}
if (mixer ==NULL )
{
mp_error_msg( func, "Can't create can't create a mixer from file [%s] .\n", mixerFileName );
free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
return( ERR_MIXER  );
}

  /* Load the input signal */
  if ( (inSignal = MP_Signal_c::init( sndFileName )) == NULL )
    {
      mp_error_msg( func, "Failed to create a new signal from file [%s].\n",
               sndFileName );
      free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
      return( ERR_SIG );
    }
  if ( inSignal->numChans != mixer->numChans )
    {
      mp_error_msg( func, "Channel mismatch: signal has [%d] channels"
               " whereas mixer matrix has [%d] channels.\n",
               inSignal->numChans, mixer->numChans );
      free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
      return( ERR_NCHANS );
    }
  if ( MPD_VERBOSE )
    {
      mp_error_msg( func, "The signal loaded from file [%s] has:\n", sndFileName );
      inSignal->info( stderr );
    }
    
bookArray = new  std::vector<MP_Book_c*>(mixer->numSources);

for (unsigned int j =0; j <mixer->numSources; j++) bookArray->at(j) = MP_Book_c::create(1, inSignal->numSamples, inSignal->sampleRate );




  /* Build and replicate the dictionary */
  if ( !MPD_QUIET ) mp_info_msg( func, "Loading and replicating the dictionary...\n" );

 
 if ( (dictFileNameList = (char**)malloc(mixer->numSources * sizeof(char*))) == NULL ) {
      mp_error_msg( func, "Can't create the char** array of size [%hu] called dictFileNameList.\n", mixer->numSources );
      free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
      return( ERR_DICT );
    } else {
      for ( unsigned short int j = 0; j < mixer->numSources; j++ ) {
	if ( (dictFileNameList[j] = (char*)malloc(1024 * sizeof(char))) == NULL ) {
	  mp_error_msg( func ,"Can't create the char* array of size [%hu] called dictFileNameList[%hu].\n", 1024, j );
	  free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
	  return( ERR_DICT );	  
	}
      }
    }

      if ( dictFileName != NULL ){ 
        	
          /* If dictFileName is the file */
          if ( ( fid = fopen( dictFileName, "r" ) ) == NULL )
            {
              mp_error_msg( func,"Can't open file %s to read the dictionary.\n", dictFileName );
              free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
              return( ERR_DICT );
            }
          if (fgets( line, MP_MAX_STR_LEN, fid ) == NULL)
            {
              mp_error_msg( func ,"Can't read the file %s.\n", dictFileName );
              free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
              return( ERR_DICT );
            }
          numDictionaries = 0;
          if (sscanf( line, "%hu dictionaries\n", &numDictionaries ) == 1 )
            {
              /* dictFileName contains a list of numDictionaries dictionary file names*/
              if (numDictionaries != mixer->numSources)
                {
                  mp_error_msg( func, "The file %s must contain [%hu] dictionary filenames, instead of [%hu].\n", dictFileName, mixer->numSources, numDictionaries );
                  free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
                  return( ERR_DICT );
                }
              for ( unsigned short int m = 0; m < mixer->numSources; m++ )
                {
                  if ( (fgets( line, MP_MAX_STR_LEN, fid ) == NULL) ||
                       (strlen(line) == 0) )
                    {
                      mp_error_msg( func, "Can't read the name of the [%hu]th dictionary filename from file %s.\n", m, dictFileName );
                      free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
                      return( ERR_DICT );
                    }
                  /* cut the newline character */
                  strncpy(dictFileNameList[m],line,strlen(line) - 1);
                  dictFileNameList[m][strlen(line)-1] = '\0';

                }

            }
          else
            {
              for ( unsigned short int n = 0; n < mixer->numSources; n++ )
                {
                  strcpy(dictFileNameList[n],dictFileName);
                  
                }
            }
          fclose( fid );
        }
      else
        {
          mp_info_msg( func, "dictFileName is NULL, mpd_demix will use the default gabor block.\n" );
          for ( unsigned short int o = 0; o < mixer->numSources; o++ )
            {
              dictFileNameList[o] = NULL;
            }
        }
      /* Build the dictionary array */
     dictArray = new  std::vector<MP_Dict_c*>(mixer->numSources);
     
      for ( unsigned short int p = 0; p < mixer->numSources; p++ )
        {dictArray->at(p)= MP_Dict_c::init();
   
          if ( dictArray->at(p) == NULL )
            {
               mp_error_msg( func, "Can't create a new dictionary for source [%hu].\n", p );
               free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
              return( ERR_DICT );
            }
          if ( dictFileNameList[p] != NULL )
            { 
             dictArray->at(p)->add_blocks( dictFileNameList[p] );
            }
          /* If no file name is given, use the following default dictionnary: */
          else { 
              mp_info_msg( func, "Use the default gabor block.\n" );
              dictArray->at(p)->add_default_block("gabor");
          }
                  
          if ( MPD_VERBOSE )
            {
              if ( dictFileNameList[p] ) mp_info_msg( func, "The dictionary for source [%hu], read from file [%s], contains [%u] blocks:\n",
                                                    p,dictFileNameList[p], dictArray->at(p)->numBlocks );
              else mp_info_msg( func, " The default dictionary for source [%hu] contains [%u] blocks:\n", p, dictArray->at(p)->numBlocks );
              for ( unsigned int q = 0; q < dictArray->at(p)->numBlocks; q++ ) dictArray->at(p)->block[q]->info( stderr );
              mp_info_msg( func, "End of dictionary for source [%hu].\n",p );
            }

        }

    
  if ( !MPD_QUIET ) mp_info_msg( func, "The multiple dictionary is built.\n" );

  if ( MPD_VERBOSE )
    {
      if ( dictFileName ) mp_info_msg( func, "The dictionary read from file [%s] contains [%u] blocks:\n",
                                     dictFileName, dictArray->at(0)->numBlocks );
      else mp_info_msg( func, "The default dictionary contains [%u] blocks:\n", dictArray->at(0)->numBlocks );
      for ( unsigned int i = 0; i < dictArray->at(0)->numBlocks; i++ ) dictArray->at(0)->block[i]->info( stderr );
      mp_info_msg( func, "End of dictionary.\n" );
    }
    
 /****/
  /* Make the mpdCore */
 mpdDemixCore = MP_Mpd_demix_Core_c::create( inSignal, mixer, bookArray );
  if ( mpdDemixCore == NULL )
    {
      mp_error_msg( func, "Failed to create a MPD core object.\n" );
      free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );
      return( ERR_CORE );
    }
    
  /* Build a multiple book */
mpdDemixCore->change_dict( dictArray );

/****/
/* Set the breakpoints and other parameters */
  if ( MPD_USE_ITER ) mpdDemixCore->set_iter_condition( MPD_NUM_ITER );
  if ( MPD_USE_SNR  ) mpdDemixCore->set_snr_condition( MPD_SNR );
  mpdDemixCore->set_save_hit( MPD_SAVE_HIT,
                    bookFileName,   
                    resFileName,
                    decayFileName,
                    srcSeqFileName );
                    
                    
/****/                   
/* Initial report */
  if ( !MPD_QUIET )
    {
      mp_info_msg( func, "-------------------------\n" );
      if (dictFileName) mp_info_msg( func, "Starting Blind Source Separation on signal [%s] with dictionary [%s].\n",
                                     sndFileName, dictFileName );
      else mp_info_msg( func, "Starting Blind Source Separation on signal [%s] with the default dictionary.\n",
                      sndFileName );
      mp_info_msg( func, "-------------------------\n" );
      mpdDemixCore->info_conditions(); 
      mp_info_msg( func, "The initial signal energy is : %g\n", mpdDemixCore->get_initial_energy() );
      mp_info_msg( func, "STARTING TO ITERATE\n" );


}
                    
                    
  /*********************/
  /* Start the pursuit */
  /*********************/
  
  mpdDemixCore->run();
  if ( (!MPD_QUIET) && ( mpdDemixCore->get_residual_energy() < 0.0) )
    {
      mp_warning_msg( func, "The loop has stopped because a negative residual energy has been encountered.\n" );
      mp_warning_msg( func, "You have gone close to the machine precision. On the next run, you should use\n" );
      mp_warning_msg( func, "less iterations or a lower SNR .\n" );
    }

  /**************/
  /* End report */
  if ( !MPD_QUIET ) mpdDemixCore->info_result();
   
  /***************************/
  /* Global save at the end: */
   mpdDemixCore->save_result();
 
  /*******************/
  /* Clean the house */
free_mem(dictArray, bookArray,  mixer, inSignal, mpdDemixCore, dictFileNameList  );

 if ( !MPD_QUIET ) mp_info_msg( func, "Have a nice day !\n" );
 /* Release Mptk environnement */
 MPTK_Env_c::get_env()->release_environment();

  return( 0 );
}
