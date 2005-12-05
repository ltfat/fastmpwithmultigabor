/******************************************************************************/
/*                                                                            */
/*                              mp_messaging.h                                */
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

/********************************************************/
/*                                                      */
/* PRETTY PRINTING OF ERROR/WARNING/INFO/DEBUG MESSAGES */
/*                                                      */
/********************************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#ifndef __mp_messaging_h_
#define __mp_messaging_h_

#include "mp_system.h"

/*******************/
/* DEFAULT STREAMS */
/*******************/

/** \brief The default global output stream. */
#define MP_MSG_STREAM stderr

/** \brief The default error stream. */
#define MP_ERR_STREAM MP_MSG_STREAM

/** \brief The default warning stream. */
#define MP_WARNING_STREAM MP_MSG_STREAM

/** \brief The default info stream. */
#define MP_INFO_STREAM MP_MSG_STREAM

/** \brief The default debug stream. */
#define MP_DEBUG_STREAM MP_MSG_STREAM


/***********************/
/* MESSAGING FUNCTIONS */
/***********************/

/** \brief Pretty-printing of the libmptk error messages
 *  in the default error stream.
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the stream defaulted to MP_ERR_STREAM
 * and the type defaulted to "error".
 */
int mp_error_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk error messages
 *  in a specific stream.
 *
 * \param stream the output stream
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the type defaulted to "error".
 */
int mp_error_msg_str( FILE* stream, const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk warning messages
 *  in the default warning stream.
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the stream defaulted to MP_WARNING_STREAM
 * and the type defaulted to "warning".
 */
int mp_warning_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk warning messages
 *  in a specific stream.
 *
 * \param stream the output stream
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the type defaulted to "warning".
 */
int mp_warning_msg_str( FILE* stream, const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk info messages
 *  in the default info stream.
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the stream defaulted to MP_INFO_STREAM
 * and the type defaulted to "info".
 */
int mp_info_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk info messages
 *  in a specific stream.
 *
 * \param stream the output stream
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the type defaulted to "info".
 */
int mp_info_msg_str( FILE* stream, const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk debug messages
 *  in the default debug stream. Does nothing if the NDEBUG preprocessor
 *  variable is set.
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the stream defaulted to MP_DEBUG_STREAM
 * and the type defaulted to "DEBUG".
 */
#ifndef NDEBUG
int mp_debug_msg( const char *funcName, const char *format, ... );
#else
#define mp_debug_msg( X, Y, ... ) void(0)
#endif

/** \brief Pretty-printing of the libmptk debug messages
 *  in a specific stream. Does nothing if the NDEBUG  preprocessor
 *  variable is set.
 *
 * \param stream the output stream
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa This function calls mp_msg_str() with the type defaulted to "DEBUG".
 */
#ifndef NDEBUG
int mp_debug_msg_str( FILE* stream, const char *funcName, const char *format, ... );
#else
#define mp_debug_msg_str( X, Y, Z, ... ) void(0)
#endif

/** \brief Pretty-printing of the libmptk messages.
 *
 * \param stream the output stream
 * \param type a prefix (such as "error", "warning", "info" etc.)
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 */
//int mp_msg_str( FILE* stream, const char *type, const char *funcName, const char *format, ... );
int mp_msg_str( FILE* stream, const char *type, const char *funcName, const char *format, va_list arg );


#endif /* __mp_messaging_h_ */
