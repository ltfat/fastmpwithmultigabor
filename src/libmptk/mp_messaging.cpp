/******************************************************************************/
/*                                                                            */
/*                               mp_messaging.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* R?mi Gribonval                                                             */
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
 * $Date$
 * $Revision$
 *
 */

/********************************************************/
/*                                                      */
/* PRETTY PRINTING OF ERROR/WARNING/INFO/DEBUG MESSAGES */
/*                                                      */
/********************************************************/

#include "mptk.h"
#include "mp_system.h"


/*********/
/* ERROR */
/*********/

/********************************************************/
/* Error messages sent in the default MPTK error stream */
int mp_error_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( MP_ERR_STREAM, "error", funcName, format, arg );
  va_end ( arg );

  return( done );
}

/*************************************/
/* Error messages sent in any stream */
int mp_error_msg_str( FILE *stream, const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( stream, "error", funcName, format, arg );
  va_end ( arg );

  return( done );
}


/***********/
/* WARNING */
/***********/

/************************************************************/
/* Warning messages sent in the default MPTK warning stream */
int mp_warning_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( MP_WARNING_STREAM, "warning", funcName, format, arg );
  va_end ( arg );

  return( done );
}

/***************************************/
/* Warning messages sent in any stream */
int mp_warning_msg_str( FILE *stream, const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( stream, "warning", funcName, format, arg );
  va_end ( arg );

  return( done );
}


/********/
/* INFO */
/********/

/********************************************************/
/* Info messages sent in the default MPTK info stream */
int mp_info_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( MP_INFO_STREAM, "info", funcName, format, arg );
  va_end ( arg );

  return( done );
}

/*************************************/
/* Info messages sent in any stream */
int mp_info_msg_str( FILE *stream, const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( stream, "info", funcName, format, arg );
  va_end ( arg );

  return( done );
}


/*********/
/* DEBUG */
/*********/

/********************************************************/
/* Debug messages sent in the default MPTK debug stream */
#ifndef NDEBUG
int mp_debug_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( MP_DEBUG_STREAM, "DEBUG", funcName, format, arg );
  va_end ( arg );

  return( done );
}
#endif

/*************************************/
/* Debug messages sent in any stream */
#ifndef NDEBUG
int mp_debug_msg_str( FILE *stream, const char *funcName, const char *format, ...  ) {

  va_list arg;
  int done;
  
  va_start ( arg, format );
  done = mp_msg_str( stream, "DEBUG", funcName, format, arg );
  va_end ( arg );

  return( done );
}
#endif

/***********/
/* GENERIC */
/***********/

/***************************/
/* Generic pretty-printing */
int mp_msg_str( FILE *stream, const char *type, const char *funcName,
		const char *format, va_list arg ) {

  int done = 0;
  
  done += fprintf( stream, "libmptk %s -- %s - ", type, funcName );
  done += vfprintf ( stream, format, arg );

  fflush( stream );

  return( done );
}

