/******************************************************************************/
/*                                                                            */
/*                                mp_core.h                                   */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/*                                                                            */
/* Roy Benjamin                                               Mon Feb 21 2005 */
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


/***********************************************/
/*                                             */
/* set the has map container                   */
/*                                             */
/***********************************************/
#ifndef MP_HASH_CONTAINER_HEADER_H_
#define MP_HASH_CONTAINER_HEADER_H_
#include <iostream>



#ifdef _WIN32
#	ifdef __MINGW32__
# 	 define MINGW_STL
#   else //__MINGW32__
#		if defined(_MSC_VER)
#  		 pragma warning(disable: 4786)
#   	  if _MSC_VER < 1300
#    	   define NO_STL_EXT
#         else
#			if defined(_STLP_MSVC)
#            define MSC_STLP
#           else
#            define MSC_STL
#           endif
#         endif
#		endif //__MINGW32__
#	endif //__MINGW32__
#elif defined(__GNUC__)//__WIN32__
#  		if __GNUC__ >= 3
#    	 define GNU_STL_IN_EXT
#    		if __GNUC__ == 3 && __GNUC_MINOR__ == 0
#      		 define STL_EXT_NM std
#    		endif
#  		endif
#endif //_WIN32


#if defined(NO_STL_EXT)
#  define SGI_STL
#  define STL_EXT_NM std
#  error "no hash_map"
#elif defined(GNU_STL_IN_EXT)
#  define SGI_STL
#  if !defined(STL_EXT_NM)
#    define STL_EXT_NM __gnu_cxx
#  endif
#  include <ext/hash_map>
#  include <ext/hash_set>
#endif
#  if defined(MSC_STLP)
#    define STL_EXT_NM std
#  include <hash_map>
#  include <hash_set>
#elif defined(MSC_STL)
#    define STL_EXT_NM stdext
#  include <hash_map>
#  include <hash_set>
#endif
#  if defined(MINGW_STL)
#    define SGI_STL
#    define STL_EXT_NM __gnu_cxx
#include <ext/hash_map>
#endif

struct mp_hash_fun
 { // define hash function for strings
 enum
  { // parameters for hash table
  bucket_size = 4, // 0 < bucket_size
  min_buckets = 8}; // min_buckets = 2 ^^ N, 0 < N

 size_t operator()(const char *s1) const
  { // hash string s1 to size_t value
  const unsigned char *p = (const unsigned char *)s1;
  size_t hashval = 5381;
        int c;

        while (c = *p++)
            hashval = ((hashval << 5) + hashval) + c; /* hash * 33 + c */

  return (hashval);
  }

 }; 

struct mp_eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};
#endif /*MP_HASH_CONTAINER_HEADER_H_*/
