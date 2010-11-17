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
}

/**************/
/* Destructor */
MPTK_Env_c::~MPTK_Env_c()
{
	instanceFlag=false;
	map<const char*,const char*,mp_ltstring>::iterator iter;
	const char *a,*b;
	while(!MPTK_Env_c::get_env()->configPath.empty()) 
	{ 
		iter = MPTK_Env_c::get_env()->configPath.begin();
		a = iter->first;
		b = iter->second;
		MPTK_Env_c::get_env()->configPath.erase(iter); 
		free((void *)a); 
		free((void *)b); 
	}
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
    // Release server
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
  map<const char*,const char*,mp_ltstring>::iterator iter;
  // Test if the name exists, because we don't want to add it (by default configPath[name] adds name if it does not yet exist) 
  iter= MPTK_Env_c::get_env()->configPath.find(name);
  if(iter==MPTK_Env_c::get_env()->configPath.end()) 
    return NULL;
  else
    return MPTK_Env_c::get_env()->configPath[name];
}

/* fill a vector with the name of the atoms registred in atom factory */
void MPTK_Env_c::get_registered_path_name( vector< string >* nameVector ){
  const char *func = "MPTK_Env_c::get_registered_path_name";
  map<const char*,const char*,mp_ltstring>::iterator iter;
  if (NULL==nameVector) {
    mp_error_msg(func, "nameVector is NULL!");
  }
  else {
    for( iter = MPTK_Env_c::configPath.begin(); iter != MPTK_Env_c::configPath.end(); iter++ ) {
		if(NULL!=iter->first) {
			nameVector->push_back(string(iter->first)); 
			// the string put in the name vector is a copy of the char* iter->first
		}
		else {
			mp_error_msg(func,"Cannot push a NULL string into nameVector\n");
		}
    }
  }
}

/* fill a vector with the name of the atoms registred in atom factory */
void MPTK_Env_c::get_registered_path_names( char **pathNames ){
	int iIndex = 0;
	const char *func = "MPTK_Env_c::get_registered_path_names";
	map<const char*,const char*,mp_ltstring>::iterator iter;
	for( iter = MPTK_Env_c::configPath.begin(); iter != MPTK_Env_c::configPath.end(); iter++ ) 
	{
		if(NULL!=iter->first) 
		{
			pathNames[iIndex++]=(char *)iter->first;
		}
		else 
		{
			mp_error_msg(func,"Cannot push a NULL string into nameVector\n");
		}
	}
}

/* Returns the size of the atom vector */
int MPTK_Env_c::get_path_size( void ){
	return MPTK_Env_c::configPath.size();
}	 

/* Get the path of the configuration file using a env variable */
char * MPTK_Env_c::get_configuration_file()
{
	char *pPath;
	pPath = getenv (mptk_Config_File_Name);
	if(pPath == NULL)
		   mp_error_msg( "MPTK_Env_c::get_configuration_file()", "Could not find MPTK Env variable with path to config file.\n");

	return pPath;
}


/* Get the boolean environment loaded */
bool MPTK_Env_c::get_environment_loaded(){

  return 	environment_loaded;

}

bool MPTK_Env_c::load_environment_if_needed(const char * name) 
{
  const char *func = "MPTK_Env_c::load_environment_if_needed()";
  if (environment_loaded) {
    return true;
  } else {
    if (!load_environment(name)) {
      mp_error_msg(func,"Could not load the MPTK environment.\n");
      mp_info_msg(func,"The most common reason is a missing or erroneous MPTK_CONFIG_FILENAME variable.\n");
      mp_info_msg(func,"The current value is MPTK_CONFIG_FILENAME = [%s].\n",MPTK_Env_c::get_env()->get_configuration_file());
      mp_info_msg("","The MPTK environment can be specified either by:\n");
      mp_info_msg("","  a) setting the MPTK_CONFIG_FILENAME environment variable\n");
      mp_info_msg("","     using e.g. 'setenv MPTK_CONFIG_FILENAME <path_to_config_file.xml>'\n");
      mp_info_msg("","     in a shell terminal, or\n");
      mp_info_msg("","     'setenv('MPTK_CONFIG_FILENAME','<path to configuration file.xml>')\n");
      mp_info_msg("","      from the Matlab command line\n");
      mp_info_msg("","  b) using the -C <path_to_configfile.xml> option in many MPTK command line utilities.\n");
      return false;
    } 
	else 
	{
      return true;
    }
  }
}

/* Load Mptk environnement and initialize all the utility class instance */
bool MPTK_Env_c::load_environment(const char * name )
{
	const char * func ="MPTK_Env_c::load_environment()";
	TiXmlElement *elem;
	char path[1024];
	
	// If the environment is already loaded, do nothing and exit gracefully
	if(environment_loaded){ 	
		return false;
	}
	
	// Get the name of the configuration file
	if(NULL==name || 0==strlen(name)) 
	{ 
		// If no argument is provided, try the default configuration file  ... 

		if (NULL== get_configuration_file()) // ... if it does not exist either, miserably fail
		{ 
			mp_error_msg(func, "couldn't retrieve the name of the MPTK environment configuration file\n");
			mp_info_msg("","This name can be specified either by:\n");
			mp_info_msg("","  a) setting the MPTK_CONFIG_FILENAME environment variable, using e.g. 'setenv MPTK_CONFIG_FILENAME <path_to_config_file.xml>')\n");
			mp_info_msg("","  b) using the -C <path_to_configfile.xml> option in many MPTK command line utilities.\n");
			mp_info_msg("","In a standard installation of MPTK, <path_to_config_file.xml> is /usr/local/mptk/path.xml\n");
			environment_loaded = false;
			return false;
		} 
		strcpy(path,get_configuration_file());
	} 
	else 
	{
		strcpy(path,name);
	}

	// Check that the configuration file exists and is readable
	FILE *fp = fopen (path, "r");
	if (NULL== fp) 
	{
		mp_error_msg( func, "The config file with name %s doesn't exist.\n", path);
		return false;
	} 
	
	// Try to load the XML configuration file
	TiXmlDocument configFile(path);		
	if (!configFile.LoadFile()) 
	{
		/* Try to load the file, and check success */
		mp_error_msg( func, "Could not load the xml config file : %s , description: %s .\n",
					path, configFile.ErrorDesc() );
		return false;
	} 
	
	// Load and parse the file with TinyXML 
	TiXmlHandle hdl(&configFile); 
	/* Find the first <configpath> <path .../><configpath> entry */
	elem = hdl.FirstChildElement("configpath").FirstChildElement("path").Element(); 
	if (!elem) 
	{
		mp_error_msg( func, "the <configpath> <path .../> </configpath> node doesn't exist in file %s",	path);
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
		if(NULL==pathBufferCstr) 
		{
			mp_error_msg( func, "Could not allocate pathBufferCstr");
			return false;
		}
		/* Copy */
		strncpy(nameBufferCstr,nameBuffer.c_str(),nameBuffer.size()+1 );
		strncpy(pathBufferCstr,pathBuffer.c_str(),pathBuffer.size()+1 );
		/* If the pair (name,path) does not already exist in the configPath, add it */
		if (NULL == MPTK_Env_c::get_env()->get_config_path(nameBufferCstr)) 
		{
			MPTK_Env_c::get_env()->configPath[nameBufferCstr] = pathBufferCstr;
			mp_debug_msg(MP_DEBUG_FILE_IO,func,"Setting %s=%s\n",nameBufferCstr,pathBufferCstr);
		} 
		else 
		{ 				
		/* Otherwise generate an warning */
			mp_warning_msg( func, 
							"Two path variable with the same name=%s in config file\n",
							nameBufferCstr); 
		}
		/* iterate on the next element */
		elem = elem->NextSiblingElement();
	}
	/* Create DLL Manager */ 
	dll = new MP_Dll_Manager_c();
	if (NULL== dll) {
		mp_error_msg( func, "Failed to create a dll manager");
		return false;
	}
			
	/* Load plugins */ 
	if (dll->load_dll()) 
	{
	  vector< string >* blockNameVector = new vector< string >();
	  MP_Block_Factory_c::get_block_factory()->get_registered_block_name( blockNameVector );
	  if(0==blockNameVector->size()) 
	  {
	    mp_error_msg( func, "No block type was loaded, even though plugins were found in the dll_directory '%s' specified by the configuration file '%s'\n", MPTK_Env_c::get_env()->get_config_path("dll_directory") , path);
	    mp_info_msg("","The most common reason is a configuration file which does not match the installed version of MPTK\n");
	    delete blockNameVector; blockNameVector = NULL;
	    return false;
	  }
	  vector< string >* atomNameVector = new vector< string >();
	  MP_Atom_Factory_c::get_atom_factory()->get_registered_atom_name( atomNameVector );
	  if(0==atomNameVector->size()) 
	  {
	    mp_error_msg( func, "No atom type was loaded, even though plugins were found in the dll_directory specified by the configuration file\n" );
	    mp_info_msg("","The most common reason is a configuration file which does not match the installed version of MPTK\n");
	    delete atomNameVector; atomNameVector = NULL;
	    return false;
	  }

	  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Load successfully the following Block type: \n" );
	  for (unsigned int i= 0; i < blockNameVector->size(); i++) 
	  {
	    mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "%s block.\n",blockNameVector->at(i).c_str()  );
	  }
	  delete(blockNameVector); blockNameVector = NULL;
	  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Load successfully the following Atom type: \n" );
	  for (unsigned int i= 0; i < atomNameVector->size(); i++) 
	  {
	    mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "%s atom.\n",atomNameVector->at(i).c_str()  );
	  }
	  delete(atomNameVector); atomNameVector = NULL;
	} 
	else 
	{
	  mp_error_msg(func, "Failed to load any plugin\n"); 
	  mp_info_msg(func, "The most common reason is an ill-formed configuration file %s\n",path);
	  mp_info_msg(func, "The XML file %s should define 'dll_directory' to be a path where the plugins for MPTK are available\n");  
	  return false;
	}
	/* Load FFT wisdom file */ 
	if (MP_FFT_Interface_c::init_fft_library_config()) 
		mp_debug_msg( MP_DEBUG_CONSTRUCTION ,func, "The fftw Plan is now loaded.\n" );
	else 
		mp_debug_msg(MP_DEBUG_CONSTRUCTION, func, "No well formed fftw Plan was found.\n" );
	environment_loaded = true;  
	return true;
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
