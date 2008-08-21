/******************************************************************************/
/*                                                                            */
/*                        test_behaviour_in_noise.cpp                         */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* R?mi Gribonval                                                             */
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
 * $Date: 2006-02-06 15:34:52 +0100 (Mon, 06 Feb 2006) $
 * $Revision: 353 $
 *
 */

#include <mptk.h>
#include <time.h>

int main( int argc, char ** argv )
{
  /* Data */
  char* func = "test behaviour in noise";
  std::vector<MP_Mpd_Core_c*> *coreArray;
  std::vector<MP_Dict_c*> *dictArray;
  std::vector<MP_Signal_c*> *approxArray;
  std::vector< string >* nameBlockVector;
  nameBlockVector = new vector< string >();
  unsigned int numIter = 0;
  unsigned int numTestSignals = 0;
  char *p;
  std::vector<MP_Book_c*> *book ;

  /*Parse parameters*/

  if (argc == 2)
    {
      /*Default value */
      numIter = 10;
      numTestSignals = 1;
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
          numTestSignals = strtoul(argv[3], &p, 0);
          if ( (p == argv[3]) || (*p != 0) )
            {
              mp_error_msg( func, "Failed to convert argument [%s] to a unsigned long int value.\n",
                            argv[3] );
              return( -1 );
            }
          mp_info_msg( func, "Test behavior in noise with [%i] ierations on [%i] signals.\n" ,numIter, numTestSignals );
        }
      else mp_error_msg( func, "Bad Number of arguments, test_behaviour_in_noise require confiFile as first argument, and optional (number of iteration and number of testing signals) as argument in unsigned long int.\n"
                         );
    }
  const char *configFileName = argv[1];
  /* Load the MPTK environment */
  if(! (MPTK_Env_c::get_env()->load_environment_if_needed(configFileName)) ) {
    exit(1);
  }

  /* Get registered block */
  MP_Block_Factory_c::get_block_factory()->get_registered_block_name(nameBlockVector);

  dictArray = new  std::vector<MP_Dict_c*>(nameBlockVector->size());
  for (unsigned int n=0; n < nameBlockVector->size(); n++)
    {
      if (strcmp(nameBlockVector->at(n).c_str(),"anywave" )==0)nameBlockVector->erase(nameBlockVector->begin() +n);
    }
  for (unsigned int n=0; n < nameBlockVector->size(); n++)
    {
      if (strcmp(nameBlockVector->at(n).c_str(),"anywavehilbert")==0)nameBlockVector->erase(nameBlockVector->begin() +n);
    }


  /***********************/
  /* Make the dictionary */
  /***********************/
  for (unsigned int m=0; m < numTestSignals; m++)
    {
      for (unsigned int n=0; n < nameBlockVector->size(); n++)
        {
          dictArray->at(n) = MP_Dict_c::init();
          dictArray->at(n)->add_default_block(nameBlockVector->at(n).c_str());
        }
      coreArray= new std::vector<MP_Mpd_Core_c*>(nameBlockVector->size());
      approxArray = new std::vector<MP_Signal_c*>(nameBlockVector->size());
      book = new std::vector<MP_Book_c*>(nameBlockVector->size());

      /*******************/
      /* Make the signal */
      /*******************/
      /* 1 channel,
         3072 samples (5 frames of length 1024 shifted by 512),
         sample rate see below */
#define SAMPLE_RATE 8000
      for (unsigned int n=0; n < nameBlockVector->size(); n++)
        {
          approxArray->at(n) = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
          /* Fill the signal with noise of energy 1.0 */
          approxArray->at(n)->fill_noise( 1.0 );
        }
      for (int n=0; n < nameBlockVector->size(); n++)
        {
          book->at(n) = MP_Book_c::create(1, 3072, SAMPLE_RATE );
          if (book->at(n)==NULL)
            {
              mp_error_msg( func, "failed to create block[%s]\n",nameBlockVector->at(n).c_str());
              return(-1);
            }
        }


      /**************************************/
      /* Make the individual atom waveforms */
      /**************************************/
      mp_info_msg( func, "****************************************\n" );
      mp_info_msg( func, "ENERGIES:\n" );
      mp_info_msg( func, "****************************************\n" );


      for (unsigned int n=0; n < nameBlockVector->size(); n++)
        {
          coreArray->at(n) = MP_Mpd_Core_c::create( approxArray->at(n), book->at(n), dictArray->at(n) );
          if (coreArray->at(n)!=NULL)coreArray->at(n)->set_iter_condition( numIter );
          else
            {
              mp_error_msg( func, "failed to initialise core for block[%s]\n",nameBlockVector->at(n).c_str());
              return (-1);
            }
          if (coreArray->at(n)->can_step())coreArray->at(n)->run();
          if (coreArray->at(n)->get_initial_energy() <= dictArray->at(n)->signal->energy) return(-1);
          else
            {
              mp_info_msg( func, "block[%s]: energy of signal before extraction [%g] after [%g] \n", nameBlockVector->at(n).c_str(), coreArray->at(n)->get_initial_energy() , dictArray->at(n)->signal->energy  );
            }
        }
    }
  /**************************************/
  /* Clean the house */
  /**************************************/
  for (unsigned int n=0; n < nameBlockVector->size(); n++)
    {
      delete(coreArray->at(n));
      delete(approxArray->at(n));
      delete(dictArray->at(n));
      delete(book->at(n));


    }
  delete(coreArray);
  delete(approxArray);
  delete(dictArray);
  delete(book);
  
  /* Release Mptk environnement */
  MPTK_Env_c::get_env()->release_environment();
  return(0);


}
