/******************************************************************************/
/*                                                                            */
/*                                test_mp.cpp                                 */
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
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2006-03-09 18:49:13 +0100 (Thu, 09 Mar 2006) $
 * $Revision: 526 $
 *
 */

#include <mptk.h>

int main( int argc, char ** argv ) {
  char* func = "test_mp";
  std::string strAppDirectory;
  MP_Signal_c *sigref = NULL;
  MP_Signal_c *sigtest = NULL;
  unsigned long int check;
  MP_Dict_c *dico = NULL;
  MP_Book_c *book;
  int i;
  unsigned long int frameIdx, freqIdx;
  char str[1024];
  MP_Mpd_Core_c *mpdCore = NULL;
  unsigned int numIter = 0;
  double precision = 0;
  char *p;
  
  /*Parse parameters*/
  /*Parse parameters*/

  if (argc == 2)
    {
      /*Default value */
      numIter = 10;
      precision = 0.0001;
    }
  else
    {
      if (argc == 4)
        {
          numIter= strtoul(argv[2], &p, 0);
          if ( (p == argv[2]) || (*p != 0) )
            {
              mp_error_msg( func, "Failed to convert argument [%s] to a unsigned long int value.\n",
                            argv[2] );
              return( -1 );
            }
          precision = strtod(argv[3], &p);
          if ( (p == argv[3]) || (*p != 0) )
            {
              mp_error_msg( func, "Failed to convert argument [%s] to a unsigned long int value.\n",
                            argv[3] );
              return( -1 );
            }
         
        }
      else{ mp_error_msg( func, "Bad Number of arguments, test_mp require configFile as first argument, and optional (number of iteration and precision) as argument in unsigned long int and double.\n");
    return( -1 );}
    }

  
  char *configFile = argv[1];

  /* Set environement */
  /* Load the MPTK environment */
  if(! (MPTK_Env_c::get_env()->load_environment_if_needed(configFile)) ) {
    exit(1);
  }

    /*************************/
  const char *refdir = MPTK_Env_c::get_env()->get_config_path("reference");
  if(NULL==refdir) {
    mp_error_msg(func,"Cannot retrieve reference directory\n");
    exit(-1);
  }
  /* Read signal form file */
  strAppDirectory = string(refdir);
  strAppDirectory += "/signal/glockenspiel.wav";
  sigref = MP_Signal_c::init( strAppDirectory.c_str() );
  sigtest = MP_Signal_c::init( strAppDirectory.c_str() );
  
    if ( sigref == NULL )
    {
      mp_error_msg( func, "Failed to load a signal from file [%s].\n",
                    strAppDirectory.c_str() );
 

  return(-1);
    }
  
  /**************************/
  /* Build a new dictionary */
  strAppDirectory = string(refdir);
  strAppDirectory += "/dictionary/dico_test.xml";
  
  dico = MP_Dict_c::init( strAppDirectory.c_str());
  
  if ( dico == NULL )
    {
      mp_error_msg( func, "Failed to create a dictionary from XML file [%s].\n",
                    strAppDirectory.c_str() );
 

  return(-1);
    }
  /*****************/
  /* Make the book */
  book = MP_Book_c::create(sigtest->numChans, sigtest->numSamples, sigtest->sampleRate );;
  if ( book == NULL ) {
    mp_error_msg( func, "Book is NULL.\n" );
    return(-1);
  }
  /*****************/
  /* Make the core */
   mpdCore = MP_Mpd_Core_c::create( sigtest, book, dico );
  if (mpdCore!=NULL)mpdCore->set_iter_condition( numIter );
  else       
    {
      mp_error_msg( func, "Failed to create a MPD core object.\n" );
      return( -1 );
    }

  if (mpdCore->can_step())mpdCore->run();
  
  if ( book->substract_add( NULL, sigtest, NULL ) == 0 ) {
    mp_error_msg( func,"mpr error -- No atoms were found in the book to rebuild the signal.\n" );
   return( -1 );
  }
  
 if(sigref->diff(*sigtest,precision)) {
  mp_error_msg( func, "Difference in reconstruct signal.\n" );
 return (-1);
 }
 

  /* Clean the house */
  if ( sigref  )  delete sigref;
  if ( sigtest  )  delete sigtest;
  if ( mpdCore ) delete mpdCore;
  if (dico) delete dico;
  if ( book )  delete book; 
  /* Release Mptk environnement */
  MPTK_Env_c::get_env()->release_environment();
  
  return(0);
}
