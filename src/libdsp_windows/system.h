/******************************************************************************/
/*                                                                            */
/*                                system.h                                    */
/*                                                                            */
/*                    Digital Signal Processing Windows                       */
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
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/*
 * System dependent includes and defines.
 */

#ifndef _system_h_
# define _system_h_


# ifdef HAVE_CONFIG_H
#  include <config.h>
# endif

# include <stdio.h>

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

/* Assertions */
# ifdef HAVE_ASSERT_H
#  include <assert.h>
# else
#  define assert(expr) 0
# endif

#endif /* _system_h_ */
