/******************************************************************************/
/*                                                                            */
/*                               dll_elf.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* Roy Benjamin                                               Mon Feb 21 2007 */
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

#ifndef __WIN32__


#include "dll.h"


/********************/
/* NULL constructor */
MP_Dll_Manager_c::MP_Dll_Manager_c()
{
	dllVectorName = new vector <string>();
}

/**************/
/* Destructor */
MP_Dll_Manager_c::~MP_Dll_Manager_c()
{
	if (dllVectorName) delete dllVectorName;
	dllVectorName = NULL;
  /* close the library if it isn't null */
  if ( h!=0 ) dlclose(h);
}

/***********************************/
/* Load and registrate all the dll */
bool MP_Dll_Manager_c::load_dll()
{
	if ( MPTK_Env_c::get_env()->get_config_path("dll_directory")!= NULL){
  if (MP_Dll_Manager_c::search_library(dllVectorName, MPTK_Env_c::get_env()->get_config_path("dll_directory")))
    {
      /* for all the shared lib */
      for (unsigned int k = 0; k < dllVectorName->size(); k++)
        {
          MP_Dll_Manager_c::get_dll((*dllVectorName)[k].c_str());

          if ( last_error()==0 )
            {
              void (*c)(void);
              if (MP_Dll_Manager_c::get_symbol((void **)&c,"registry"))
                {
                  /* test if plugin has the symbol registry */
                  if (c!= NULL)
                    {
                      /* Register the plugin in the concerned factory */
                      c();
                    }
                  else  mp_warning_msg( "MP_Dll_Manager::load_dll","No registry function in '%s' shared library; \n",(*dllVectorName)[k].c_str());
                }
              else  mp_warning_msg( "MP_Dll_Manager::load_dll","No registry symbol in '%s' shared library.\n",(*dllVectorName)[k].c_str());
            }
          else  mp_error_msg( "MP_Dll_Manager::load_dll","Error when loading the dll: '%s' .\n ",last_error());
        }
      return true;
    }
  else
    {
      mp_error_msg( "MP_Dll_Manager::load_dll","No library found in '%s' .\n", MPTK_Env_c::get_env()->get_config_path("dll_directory"));
      return false;
    }
} else return false;
}
/*****************************/
/* Get the symbol in the dll */
bool MP_Dll_Manager_c::get_symbol(void **v, const char *symbol_name )
{
  /* try extract a symbol from the library get any error message is there is any */

  if ( h!=0 )
    {
      *v = dlsym( h, symbol_name );
      err=dlerror();
      if ( err==0 )
        return true;
      else
        return false;
    }
  else
    {
      return false;
    }
}

/************/
/* Open dll */
void MP_Dll_Manager_c::get_dll(const char *fname)
{
  /* Try to open the library now and get any error message. */
  h=dlopen( fname, RTLD_NOW);
  err=dlerror();
}

/*********************************/
/* Search the dll in a directory */
bool MP_Dll_Manager_c::search_library(vector<string> * libraryNames, const char * path)
{
  DIR *dp;
  struct dirent *ep;
  string folder = path;
  dp = opendir (path);
  if (dp != NULL)
    {
      while (ep = readdir(dp))
        { if ( (strcmp( ep->d_name ,"libmptk.so")) && (strcmp( ep->d_name ,"libmptk.dylib"))){
          istringstream iss( ep->d_name );
          string mot;
          const char * c_str;
          while (
            std::getline( iss, mot , '.' ))
            {
              c_str = mot.c_str();
              if ( !strcmp( c_str ,get_dll_type()) )
                {
                  string bufferize;
                  bufferize =  folder +"/" +  ep->d_name;
                  (*libraryNames).push_back(bufferize);
                }
            }
        }
        }
      if ((*libraryNames).size()==0)
        {
          (void) closedir (dp);
          return false;
        }
      else
        { (void) closedir (dp);
          return true;
        }
    }
  else
    {
      mp_error_msg( "MP_Dll_Manager::load_dll","No directory with '%s'.\n", path );
      return false;
    }
}

#ifdef __APPLE__

/********************/
/* Get the dll type */

const char * MP_Dll_Manager_c::get_dll_type()
{
  return "dylib";
}

#else

/********************/
/* Get the dll type */

const char * MP_Dll_Manager_c::get_dll_type()
{
  return "so";
}

#endif /* __APPLE__ */
/******************/
/* Get last error */
const char * MP_Dll_Manager_c::last_error()
{
  return err;
}



#endif /* WIN32 */

