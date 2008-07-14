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
#include <string>
#include <iostream>
#include <list>
#include "tinyxml.h"



/** \brief pre-declare MP_Dll_Manager_c class
 */ 
class MP_Dll_Manager_c;

/***********************/
/* MPTK_Env CLASS      */
/***********************/
/** \brief The MPTK_Env_c class implements a standard singleton template
 * this class is used to set the mptk environnement before using matching pursuit
 */
class MPTK_Env_c
  {


    /********/
    /* DATA */
    /********/
public:

  protected:

    /** \brief ptr on myEnv's
     * Can only be accessed by getEnv()
     */
    static MPTK_Env_c * myEnv;
    
     /** \brief ptr on MP_Dll_Manager_c
     */
     static MP_Dll_Manager_c* dll;
  private:
    /** \brief Boolean set to true when an instance is created */
    static bool instanceFlag;

    /** \brief Boolean set to true when when the fftw wisdom file is correctly
     * loaded and the wisdom is well formed
     */
    static bool fftw_file_loaded;
    
    /** \brief Boolean set to true when when the fftw wisdom file is correctly
    * loaded and the wisdom is well formed
    */
    static bool environnement_loaded;

   /** \brief Hash map to store the atom name and the file creation atom method pointer */
STL_EXT_NM::hash_map<const char*,const char*,CSTRING_HASHER> configPath;


 

    /** \brief buffer to store the name of the path */
    char** nameBufferCstr;

    /** \brief buffer to store the value of the path */
    char** pathBufferCstr;

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:

  virtual ~MPTK_Env_c();
    /** \brief Public struct to compare the value in path hash map */

  private:

    /** \brief Private constructor*/
    MPTK_Env_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/

  public:
    /** \brief Allocating and initializing
     *  myEnv's static data member
     *  (the ptr, not a myEnv inst)
     */
    MPTK_LIB_EXPORT static MPTK_Env_c * get_env();

    /** \brief Get boolean defining if Wisdom file was correctly loaded
     */
    MPTK_LIB_EXPORT bool get_fftw_wisdom_loaded();

    /** \brief Set boolean defining if Wisdom file was correctly loaded
     */
    MPTK_LIB_EXPORT bool set_fftw_wisdom_loaded();

    /** \brief Method to change MPTK environnement if necessary
     *  \param filename the name of environment file to change
     */
   MPTK_LIB_EXPORT bool set_env(string filename);

    /** \brief Method to load MPTK environnement
     * \param name the name of xml file containing the environment informations, use an empty string to rely on default name given by \f get_configuration_file()
	 * \return true if the environment was not already loaded and the loading was successful, false otherwise 
    */
    MPTK_LIB_EXPORT bool load_environment(const char * name);

    /** \brief Method to get the name of the configuration file via environnement variable 
    */
    MPTK_LIB_EXPORT char * get_configuration_file();
    
    /** \brief Method to get a boolean that say if environment is loaded
     */ 
    MPTK_LIB_EXPORT bool get_environment_loaded();

    /** \brief Method to get the name of a configuration path 
    *  \param name of the path
    *  \return true if successful
    */
    MPTK_LIB_EXPORT const char * get_config_path(const char * name);

    /** \brief Method to release environnement, desallocate all variables.
    */
    MPTK_LIB_EXPORT static void release_environment();
    
  };

/***********************/
/* MPTK_Server CLASS   */
/***********************/

/** \brief The MPTK_Server_c class implements a standard singleton template
 * this class is used to call the inner server of MPTK.
 */

class MPTK_Server_c
  {

    /********/
    /* DATA */
    /********/
  protected:

    /** \brief ptr on myServer's
     * Can only be accessed by get_server()
     */
    static MP_Msg_Server_c * myMsgServer;
     
    static MPTK_Server_c * myServer;
    /** \brief ptr on myWinServer's
    * Can only be accessed by get_win_server()
    */
    static MP_Win_Server_c * myWinServer;

    /** \brief ptr on myAnywaveServer's
    * Can only be accessed by get_anywave_server()
    */
    static MP_Anywave_Server_c* myAnywaveServer;

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:

  virtual ~MPTK_Server_c();
    /** \brief Public struct to compare the value in path hash map */

  private:

    /** \brief Private constructor*/
    MPTK_Server_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/
     
  public:

    /** \brief Allocating and initializing
    *  myServer's static data member
    *  (the ptr, not a myServer inst)
    */
    static MPTK_Server_c* get_server();
    
    /** \brief Allocating and initializing
    *  myMsgServer's static data member
    *  (the ptr, not a myMsgServer inst)
    */
    MPTK_LIB_EXPORT static MP_Msg_Server_c* get_msg_server();
    
    /** \brief Allocating and initializing
    *  myWinServer's static data member
    *  (the ptr, not a myWinServer inst)
    */
    MPTK_LIB_EXPORT static MP_Win_Server_c* get_win_server();
    
    /** \brief Allocating and initializing
    *  myAnywaveServer's static data member
    *  (the ptr, not a myAnywaveServer inst)
    */
    MPTK_LIB_EXPORT static MP_Anywave_Server_c* get_anywave_server();

    /** \brief Release all the server
    * Note that the chronology of server destruction is critical for Debug Mode
    */
    void release_servers();
  }
;

#endif /*MPTK_ENV_C_H_*/
