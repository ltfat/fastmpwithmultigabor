/******************************************************************************/
/*                                                                            */
/*                                  mptk_env.h                                */
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




#ifndef MPTK_ENV_C_H_
#define MPTK_ENV_C_H_

#include "mptk.h"
#include "mp_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string>
#include <iostream>


using namespace std;


/***********************/
/* MPTK_Env CLASS      */
/***********************/
/** \brief The MPTK_Env_c class implements a standard singleton template
 * this class is used to set the mptk environnement before using matching pursuit
 */
class MPTK_Env_c
  {
  public:

    virtual ~MPTK_Env_c()
    {
      instanceFlag=false;

    };
    /** \brief Boolean set to true when when the fftw wisdom file is correctly
    * loaded and the wisdom is well formed
    */

    /** \brief Allocating and initializing
     *  myEnv's static data member
     *  (the ptr, not a myEnv inst)
     */
    static MPTK_Env_c * getEnv();
    /** \brief Get the complete path of the Wisdom file
    */
    char * get_FFTW_Wisdom_File();
    /** \brief Get boolean defining if Wisdom file was correctly loaded
     */
    bool get_FFTW_Wisdom_loaded();
    /** \brief Set boolean defining if Wisdom file was correctly loaded
     */
    bool set_FFTW_Wisdom_loaded();
    /** \brief Change env if necessary
     */
    bool setEnv(string filename);
  protected:

    /** \brief ptr on myEnv's
     * Can only be accessed by getEnv()
     */
    static MPTK_Env_c * myEnv;

  private:

    /** \brief Boolean set to true when an instance is created */
    static bool instanceFlag;
    /** \brief Boolean set to true when when the fftw wisdom file is correctly
     * loaded and the wisdom is well formed
     */
    static bool fftw_file_loaded;
    /** \brief Private constructor*/
    MPTK_Env_c()
    {};

  };

#endif /*MPTK_ENV_C_H_*/
