/******************************************************************************/
/*                                                                            */
/*                                  dll.h                                     */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/* Benjamin Roy                                               Mon Feb 21 2005 */
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

/*****************************************************/
/*                                                   */
/* DEFINITION OF THE DYNAMIC LIBRARY LOADER CLASS    */
/*                                                   */
/*****************************************************/
#ifndef dll_h_
#define dll_h_
#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <io.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <sstream>
#include <string>
#include <iostream>
#endif
#include <vector>
#include "mptk.h"

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

using namespace std;

/***************************/
/* MP_Dll_Manager Class    */
/***************************/

/** \brief The MP_Dll_Manager class is a simple C++ Library manager.
 * It offers to dynamically load the shared library contained ine a specified folder.
 * and get registry symbol to register create functions in appropried factory.
 * 
 * Call LastError() before doing anything.  If it returns NULL there is no error.
 * 
 * This code is based on the Dynamically Loaded C++ Plugins for all platforms
 * By Alex Holkner, xander@yifan.net
 * Building on work by Jeff Koftinoff, jeffk@jdkoftinoff.com
 * http://yallara.cs.rmit.edu.au/~aholkner/dynload/index.html
 *
 */

class MP_Dll_Manager_c
  {

    /********/
    /* DATA */
    /********/

#ifdef _WIN32

   /** \brief handle to the library for win32 */
    HMODULE h;
#else
    /** \brief handle to the library for others OS */
    void *h;
    
#endif /* __WIN32__ */

    /** \brief array of char for error description */
    char *err;
    
    /** \brief vector to stock all the name of the found DLL  */
    vector <string> dllVectorName;

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:
  
    /** \brief Public constructor  */
    MP_Dll_Manager_c();
    
    /** \brief Public destructor  */
    virtual ~MP_Dll_Manager_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/
    
    /** \brief Method load the shared library with fname name
    *   \param fname : name of the name of the DLL to load
    */   
    void get_dll(const char *fname);
    
    /** \brief Method load the symbol with sym_name name
    *   \param void ** handle on the symbol of library, NULL if open failed
    *   \param sym_name : name of the symbol to load
    */   
    bool get_symbol( void **, const char *sym_name );
    
    /** \brief Method to load all dll contained in the path directory */
    bool load_dll();
       
    /** \brief Methode to get the dll type, for concerned  */
    static const char *get_dll_type();  
    
    /** \brief Method to get the last error  */
    const char *last_error();
    
    /** \brief Method to search all the dynamic load library in the path directory
    *   \param lib_names :vector to store the name of the found DLL
    *   \param path : path of the directory where the DLL are
    */
    bool search_library(vector<string> * lib_names, const char * path);
    
  };

#endif /* __DLL_H */




