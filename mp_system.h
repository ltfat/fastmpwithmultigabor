/******************************************************************************/
/*                                                                            */
/*                                mp_system.h                                    */
/*                                                                            */
/*                       The Matching Pursuit ToolKit                         */
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
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2006-01-31 14:51:06 +0100 (Tue, 31 Jan 2006) $
 * $Revision: 300 $
 *
 */

/*****************************************/
/*                                       */
/* SYSTEM DEPENDENT INCLUDES AND DEFINES */
/*                                       */
/*****************************************/

#ifndef _mp_system_h_
# define _mp_system_h_

# ifdef HAVE_CONFIG_H
#  include <config.h>
# endif

# include <stdio.h>
# include <time.h>

/* stdlib stuff */
# if STDC_HEADERS
#  include <stdlib.h>
#  include <stddef.h>
#  include <stdarg.h>
# else
#  if HAVE_STDLIB_H
#   include <stdlib.h>
#  endif
# endif

# if defined STDC_HEADERS || defined _LIBC
#   include <stdlib.h>
# elif defined HAVE_MALLOC_H
#   include <malloc.h>
# endif

/* string stuff */
# if HAVE_STRING_H
#  if !STDC_HEADERS && HAVE_MEMORY_H
#   include <memory.h>
#  endif
#  include <string.h>
# endif

/* mathematics */
# if HAVE_MATH_H
#  include <math.h>
# endif

/* signals */
/* # if HAVE_SIGNAL_H */
#  include <signal.h>
/* # endif */

/* limits */
# if HAVE_LIMITS_H
#  include <limits.h>
# else
/* If limits.h does not exist, define our own limits: */
#   ifndef INT_MAX
#      define INT_MAX 2147483647
#   endif
#   ifndef UINT_MAX
#      define UINT_MAX 4294967295U
#   endif
#   if __WORDSIZE == 64
#    define ULONG_MAX    18446744073709551615UL
#   else
#    define ULONG_MAX    4294967295UL
#   endif
# endif

# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

/* Assertions */
# ifdef HAVE_ASSERT_H
#  include <assert.h>
# else
#  define assert(expr) (void)(0)
# endif

#ifdef _WIN32
#if defined(_MSC_VER)
#define MPTK_LIB_EXPORT __declspec(dllexport)  /* export function out of the lib */
#define MPTK_LIB_IMPORT __declspec(dllimport)  /* import function in the lib */
#else
#define MPTK_LIB_EXPORT
#define MPTK_LIB_IMPORT
#endif
#else
#define MPTK_LIB_EXPORT
#define MPTK_LIB_IMPORT
#endif

#endif /* _mp_system_h_ */
