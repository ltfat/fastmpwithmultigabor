/******************************************************************************/
/*                                                                            */
/*                               dll_win32.cpp                                */
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

#ifdef _WIN32

#include "dll.h"

using namespace std;

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
  /* Free error string if allocated */
  LocalFree(err);

  /* close the library if it isn't null */
  if (h != NULL)
    FreeLibrary(h);

}

/***********************************/
/* Load and registrate all the dll */
bool MP_Dll_Manager_c::load_dll()
{
if (MPTK_Env_c::get_env()->get_config_path("dll_directory")!= NULL){
mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Dll_Manager::load_dll", "Plug-in localisation: [%s].\n", MPTK_Env_c::get_env()->get_config_path("dll_directory") );          	
  if (MP_Dll_Manager_c::search_library(dllVectorName, MPTK_Env_c::get_env()->get_config_path("dll_directory")))
  
    {
      for (unsigned int k=0;k<dllVectorName->size();k++ )
        {
          MP_Dll_Manager_c::get_dll((*dllVectorName)[k].c_str());

          if ( last_error()==0 )
            {
              void (*c)(void);
              if (MP_Dll_Manager_c::get_symbol((void **)&c,"registry"))
                {

                  /*test if plugin has the symbol registry */
                  if (c!= NULL)
                    {
                      /*registry the plugin in the concerned factory*/
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
} else { mp_error_msg( "MP_Dll_Manager::load_dll","Problem with config path '%s' .\n", MPTK_Env_c::get_env()->get_config_path("dll_directory"));
	return false;}
}

/************/
/* Open dll */
void  MP_Dll_Manager_c::get_dll(const char *fname)
{
  /* Try to open the library now and get any error message. */

  h = LoadLibrary(fname);

  if (h == NULL)
    {
      DWORD m;
      m = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, 			/* Instance */
                        GetLastError(),   /* Message Number */
                        0,   				/* Language */
                        err,  			/* Buffer */
                        0,    			/* Min/Max Buffer size */
                        NULL);  			/* Arguments */
      mp_error_msg( "MP_Dll_Manager::get_dll","error : [%s]" , err);
    }
  else
    {
      err = NULL;
    }
}

/*****************************/
/* Get the symbol in the dll */
bool MP_Dll_Manager_c::get_symbol( void **v, const char *sym_name )
{
  /* try extract a symbol from the library get any error message is there is any */

  if ( h!=0 )
    {
      *v = (void*)GetProcAddress(h, sym_name);
      if (v != NULL)
        return true;
      else
        {
          FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, 			/* Instance */
                        GetLastError(),   /* Message Number */
                        0,   				/* Language */
                        err,  			/* Buffer */
                        0,    			/* Min/Max Buffer size */
                        NULL);  			/* Arguments */
          return false;
        }
    }
  else
    {
      return false;
    }

}

/*********************************/
/* Search the dll in a directory */

bool MP_Dll_Manager_c::search_library(vector<string> * lib_names, const char * path)
{
  struct _finddata_t c_file;
  long hFile;
  static string fname;
  string buffer;
  buffer = path;
  buffer  += "\\";
  buffer += MP_Dll_Manager_c::get_dll_type();
  if ( (hFile = _findfirst(buffer.c_str(), &c_file)) == -1L )
    {
      mp_error_msg( "MP_Dll_Manager::search_library","No *.dll files in current directory: [%s]\n", buffer.c_str() );
      return false;
    }
  else
    { if (strcmp( c_file.name ,"mptk.dll") && strcmp( c_file.name ,"libmptk.dll")){
      fname = path ;
      fname += "\\";
      fname += c_file.name;


      (*lib_names).push_back(fname);
    }

      while ( _findnext(hFile, &c_file) == 0 )
        {
        if (strcmp( c_file.name ,"mptk.dll")&& strcmp( c_file.name ,"libmptk.dll")){
          fname = path ;
          fname += "\\";
          fname += c_file.name;
          (*lib_names).push_back(fname);}

        }
      _findclose(hFile);
    }

  return true;
}

/********************/
/* Get the dll type */
const char * MP_Dll_Manager_c::get_dll_type()
{
  return "*.dll";
}

/******************/
/* Get last error */
const char * MP_Dll_Manager_c::last_error()
{
  return err;
}

#endif /* WIN32 */
