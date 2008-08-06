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
#include "tinyxml.h"
#include "tinystr.h"
#include "string.h"

using namespace std;

/*boolean flag if MPTK_Env_c instance has been created */
bool MPTK_Env_c::instanceFlag = false;
/*Initialise ptr to MPTK_Env_c instance */
MPTK_Env_c * MPTK_Env_c::myEnv = NULL;
/*Initialise ptr to MP_Dll_Manager_c instance */
MP_Dll_Manager_c* MPTK_Env_c::dll = NULL;
/*Env variable name for path to wisdom file */
const char * mptk_Config_File_Name = "MPTK_CONFIG_FILENAME";
/*boolean to set if the loading of the file containing the wisdow was successful */
bool MPTK_Env_c::fftw_file_loaded= false;
/*boolean to set if the environnement was loading */
bool MPTK_Env_c::environment_loaded= false;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
int bufferSize = 3;

/********************/
/* NULL constructor */
MPTK_Env_c::MPTK_Env_c()
{
  //configPath = STL_EXT_NM::hash_map<const char*,const char*,mycomp>(1);
}

/**************/
/* Destructor */
MPTK_Env_c::~MPTK_Env_c()
{
  instanceFlag=false;
}

/*Create a singleton of MPTK_Env_c class */
MPTK_Env_c * MPTK_Env_c::get_env()
{
  if (!myEnv)
    {
      myEnv = new MPTK_Env_c();
      instanceFlag = true;
    }
  return  myEnv;
}

/*Destroy a singleton of MPTK_Env_c class */
void MPTK_Env_c::release_environment()
{
  const char * func = "MPTK_Env_c::release_environment()";
  if (environment_loaded){
    if (MP_FFT_Interface_c::save_fft_library_config()) mp_debug_msg( MP_DEBUG_FILE_IO, func, "The fftw Plan is now saved.\n" );

    else mp_debug_msg(MP_DEBUG_FILE_IO, func, "The fftw Plan is not saved.\n" );
    /* delete the dll*/
    if (dll) delete(dll);
    dll = NULL;
    /* Release server */
    MPTK_Server_c::get_server()->release_servers();
 
    if (myEnv)
      {
	delete myEnv;
	myEnv = NULL;
	environment_loaded = false;
      }
  }
}

/* Set some change in the env variable if MyEnv singleton is not set*/
bool MPTK_Env_c::set_env(string filename)

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


/* Get the path using a env variable */

const char * MPTK_Env_c::get_config_path(const char* name)
{
  STL_EXT_NM::hash_map<const char*,const char*,CSTRING_HASHER>::iterator iter;
  // Test if the name exists, because we don't want to add it (by default configPath[name] adds name if it does not yet exist) 
  iter= MPTK_Env_c::get_env()->configPath.find(name);
  if(iter==MPTK_Env_c::get_env()->configPath.end()) 
    return NULL;
  else
    return MPTK_Env_c::get_env()->configPath[name];
}

/* fill a vector with the name of the atoms registred in atom factory */
void MPTK_Env_c::get_registered_path_name( vector< string >* nameVector ){
  char *func = "MPTK_Env_c::get_registered_path_name";
  STL_EXT_NM::hash_map<const char*,const char*,CSTRING_HASHER>::iterator iter;
  if (NULL==nameVector) {
    mp_error_msg(func, "nameVector is NULL!");
  }
  else {
    for( iter = MPTK_Env_c::configPath.begin(); iter != MPTK_Env_c::configPath.end(); iter++ ) {
      nameVector->push_back(string(iter->first)); // the string put in the name vector is a copy of the char* iter->first
    }
  }
}
 

/* Get the path of the configuration file using a env variable */
char * MPTK_Env_c::get_configuration_file()
{
  char * pPath = NULL;
  if (getenv (mptk_Config_File_Name)!=0) pPath = getenv (mptk_Config_File_Name);
  else  mp_error_msg( "MPTK_Env_c::get_configuration_file()", "Could not find MPTK Env variable with path to config file.\n" );
  return pPath;

}


/* Get the boolean environment loaded */
bool MPTK_Env_c::get_environment_loaded(){

  return 	environment_loaded;

}

/* Load Mptk environnement and initialize all the utility class instance */
bool MPTK_Env_c::load_environment(const char * name )
{
  const char * func ="MPTK_Env_c::load_environment()";
  TiXmlElement *elem;
  char path[1024];
	
  if(!environment_loaded){ 	/* Get the name of the configuration file ... */
    if (name!= NULL && strlen(name)>0){ /* ... from the file name which was provided as an argument... */
      FILE *fp = fopen (name, "r");
      if (fp == NULL) {
	mp_error_msg( func, "The config file with name %s doesn't exist.\n", name);
	return false;
      }
      else {   /* ... if it exists ... */
	fclose(fp); 	
	strcpy(path,name);
      }
    } else if (get_configuration_file() != NULL) /* ... otherwise try the default configuration file it it exists ... */
      {
	strcpy(path,get_configuration_file());
      }
    else /* ... but if the configuration file is not found, miserably fail! */
      {
	mp_error_msg(func, "couldn't load the MPTK environment\n");
	mp_info_msg("","The MPTK environment can be specified either by:\n");
	mp_info_msg("","  a) setting the MPTK_CONFIG_FILENAME environment variable, using e.g. 'setenv MPTK_CONFIG_FILENAME <path_to_config_file.xml>')\n");
	mp_info_msg("","  b) using the -C <path_to_configfile.xml> option in many MPTK command line utilities.\n");
	environment_loaded = false;
	return false;
      }
		
    TiXmlDocument configFile(path);
		
    if (!configFile.LoadFile()) /* Try to load the file, and check success */
      {
	mp_error_msg( func, "Could not load the xml file: %s , description: %s .\n", get_configuration_file(), configFile.ErrorDesc() );
	return false;
      }
    else
      {
	TiXmlHandle hdl(&configFile); /* Load and parse the file with TinyXML */
	/* Find the first <configpath> <path .../><configpath> entry */
	elem = hdl.FirstChildElement("configpath").FirstChildElement("path").Element(); 
	if (!elem)
	  {
	    mp_error_msg( func, "the <configpath> <path .../> </configpath> node doesn't exist in file %s",get_configuration_file());
	    return false;
	  }
			
	/* Read each node */
	while (elem)
	  {
	    /* Get the name and path */
	    std::string nameBuffer = elem->Attribute("name");
	    std::string pathBuffer = elem->Attribute("path");
	    /* Allocate a new char *to copy them */
	    char *nameBufferCstr = (char *) malloc(nameBuffer.size()+1);
	    if(NULL==nameBufferCstr) {
	      mp_error_msg( func, "Could not allocate nameBufferCstr");
	      return false;
	    }
	    char *pathBufferCstr = (char *) malloc(pathBuffer.size()+1);
	    if(NULL==pathBufferCstr) {
	      mp_error_msg( func, "Could not allocate pathBufferCstr");
	      return false;
	    }
	    /* Copy */
	    strncpy(nameBufferCstr,nameBuffer.c_str(),nameBuffer.size()+1 );
	    strncpy(pathBufferCstr,pathBuffer.c_str(),pathBuffer.size()+1 );
	
	    /* If the pair (name,path) does not already exist in the configPath, add it */
	    if (NULL == MPTK_Env_c::get_env()->get_config_path(nameBufferCstr)) {
	      MPTK_Env_c::get_env()->configPath[nameBufferCstr] = pathBufferCstr;
	      mp_debug_msg(MP_DEBUG_FILE_IO,func,"Setting %s=%s\n",nameBufferCstr,pathBufferCstr);
	    }
	    /* Otherwise generate an warning */
	    else {
	      mp_warning_msg( func, "Two path variable with the same name=%s in config file\n",nameBufferCstr); 
	    }
	    /* iterate on the next element */
	    elem = elem->NextSiblingElement();
	  }
	/* Create DLL Manager */ 
	dll = new MP_Dll_Manager_c();
	if (NULL== dll)
	  {
	    mp_error_msg( func, "Failed to create a dll manager");
	    return false;
	  }
			
	/* Load DLL */ 
	if (dll->load_dll())
	  {
	    mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Load successfully the following Block type: \n" );
	    vector< string >* nameVector = new vector< string >();
	    MP_Block_Factory_c::get_block_factory()->get_registered_block_name( nameVector );
	    for (unsigned int i= 0; i < nameVector->size(); i++) mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "%s block.\n",nameVector->at(i).c_str()  );
	    delete(nameVector);
	  }
	/* Load FFT wisdom file */ 
	if (MP_FFT_Interface_c::init_fft_library_config()) mp_debug_msg( MP_DEBUG_CONSTRUCTION ,func, "The fftw Plan is now loaded.\n" );
	else mp_debug_msg(MP_DEBUG_CONSTRUCTION, func, "No fftw Plan well formed was found.\n" );
	environment_loaded = true;  
	return true;
			
      }
  } return false;
}


/* Get if the wisdom file was loaded correctly and if the wisdom was well formed */
bool MPTK_Env_c::get_fftw_wisdom_loaded()
{
  return fftw_file_loaded;
}

/* Set the boolean if the wisdom file was loaded correctly and if the wisdom was well formed */
bool MPTK_Env_c::set_fftw_wisdom_loaded()
{
  fftw_file_loaded= true;
  return fftw_file_loaded;
}

/*Initialise ptr to MPTK_Server_c instance */
MPTK_Server_c * MPTK_Server_c::myServer = NULL;

/*Initialise ptr to MP_Msg_Server_c instance */
MP_Msg_Server_c * MPTK_Server_c::myMsgServer = NULL;

/*Initialise ptr to MP_Win_Server_c instance */
MP_Win_Server_c * MPTK_Server_c::myWinServer = NULL;

/*Initialise ptr to MP_Anywave_Server_c instance */
MP_Anywave_Server_c* MPTK_Server_c::myAnywaveServer = NULL;

/*MPTK_Server_c constructor */
MPTK_Server_c::MPTK_Server_c()
{}

/*MPTK_Server_c destructor */
MPTK_Server_c::~MPTK_Server_c()
{}



/*Get MPTK_Server_c instance */
MPTK_Server_c* MPTK_Server_c::get_server()
{
  if (!myServer)
    {
      myServer = new MPTK_Server_c();
    }
  return myServer;
}

/*Get MP_Msg_Server_c instance */
MP_Msg_Server_c* MPTK_Server_c::get_msg_server()
{
  myMsgServer= MP_Msg_Server_c::get_msg_server();
  // MP_Msg_Server_c::get_msg_server()->register_display_function("default_error_message_display",&MPTK_Env_c::default_display_error_function);
  return  myMsgServer;
}

/*Get MP_Win_Server_c instance */
MP_Win_Server_c* MPTK_Server_c::get_win_server()
{
  if (!myServer)
    {
      myServer = new MPTK_Server_c();
    }
  if (!myServer->myMsgServer)
    {
      myMsgServer = MP_Msg_Server_c::get_msg_server();
    }
  if (!myServer->myWinServer)
    {
      myWinServer = new MP_Win_Server_c();
    }
  return myWinServer;
}

/*Get MP_Anywave_Server_c instance */
MP_Anywave_Server_c* MPTK_Server_c::get_anywave_server()
{
  if (!myServer)
    {
      myServer = new MPTK_Server_c();
    }
  if (!myServer->myMsgServer)
    {
      myMsgServer = MP_Msg_Server_c::get_msg_server();
    }
  if (!myServer->myWinServer)
    {
      myWinServer = new MP_Win_Server_c();
    }
  if (!myServer->myAnywaveServer)
    {
      myAnywaveServer = new MP_Anywave_Server_c();
    }

  return myAnywaveServer;

}

/*Release all the server and destroy them in the correct order */
void MPTK_Server_c::release_servers()
{
  if (MPTK_Server_c::myWinServer) delete  MPTK_Server_c::myWinServer;
  MPTK_Server_c::myWinServer = NULL;
  if (MPTK_Server_c::myAnywaveServer) delete  MPTK_Server_c::myAnywaveServer;
  MPTK_Server_c::myAnywaveServer = NULL;
  /* myMsgServer has to be destroy in last just before the intance of  MPTK_Server_c*/
  if (MPTK_Server_c::myMsgServer) delete  MPTK_Server_c::myMsgServer;
  MPTK_Server_c::myMsgServer = NULL;
  if (MPTK_Server_c::myServer) delete  MPTK_Server_c::myServer;
  MPTK_Server_c::myServer = NULL;
}
