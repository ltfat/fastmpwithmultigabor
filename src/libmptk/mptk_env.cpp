/******************************************************************************/
/*                                                                            */
/*                                mptk_env.cpp                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/*                                                                            */
/* ROY Benjamin                                               Wed Jan 01 2007 */
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

/***********************************************************************/
/*                                                                     */
/*               MANAGEMENT OF MPTK ENVIRONNEMENT                      */
/*                                                                     */
/***********************************************************************/


#include "mptk.h"
#include "mp_system.h"

using namespace std;

/*boolean flag if MPTK_Env_c instance has been created */
bool MPTK_Env_c::instanceFlag = false;
/*Initialise ptr to MPTK_Env_c instance */
MPTK_Env_c * MPTK_Env_c::myEnv = NULL;
/*Env variable name for path to wisdom file */
const char * mptk_FFTW_File_name = "MPTK_FFTW_WISDOM";
/*boolean to set if the loading of the file containing the wisdow was successful */
bool MPTK_Env_c::fftw_file_loaded= false;



/*Create a singleton of MPTK_Env_c class */

MPTK_Env_c * MPTK_Env_c::getEnv()
{
  if (!myEnv)
    {
      myEnv = new MPTK_Env_c();
      instanceFlag = true;
    }
  return  myEnv;
}

/* Set some change in the env variable if MyEnv singleton is not set*/

bool MPTK_Env_c::setEnv(string filename)

{
  if (!myEnv)
    {
      //change the MPTK environnement here

      return true;

    }
  else
    {
      return false;

    }
}

/* Get the path of the wisdom file using a env variable */

char * MPTK_Env_c::get_FFTW_Wisdom_File()
{
  char * pPath = NULL;
  if (getenv (mptk_FFTW_File_name)!=0) pPath = getenv (mptk_FFTW_File_name);
  return pPath;

}

/* Get if the wisdom file was loaded correctly and if the wisdom was well formed */

bool MPTK_Env_c::get_FFTW_Wisdom_loaded()
{
  return fftw_file_loaded;
}

/* Set the boolean if the wisdom file was loaded correctly and if the wisdom was well formed */

bool MPTK_Env_c::set_FFTW_Wisdom_loaded()
{
  fftw_file_loaded= true;
  return fftw_file_loaded;
}


