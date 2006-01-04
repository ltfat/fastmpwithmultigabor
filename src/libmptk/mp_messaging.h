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

/***********************************************************************/
/*                                                                     */
/* PRETTY PRINTING AND MANAGEMENT OF ERROR/WARNING/INFO/DEBUG MESSAGES */
/*                                                                     */
/***********************************************************************/
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


/*********************/
/* DEFAULT MSG TYPES */
/*********************/
/* - NULL type: */
#define MP_MSG_NULL   0
/* - General types: */
#define MP_ERROR      1
#define MP_WARNING    (1 << 1)
#define MP_INFO       (1 << 2)
/* - Reserved for future use: */
#define MP_RESERVED_1 (1 << 3)
#define MP_RESERVED_2 (1 << 4)
#define MP_RESERVED_3 (1 << 5)
#define MP_RESERVED_4 (1 << 5)
#define MP_RESERVED_5 (1 << 6)
/* - Debugging types (up to 24 types): */
/* these types can be used to fine tune which debug messages should
   or should not be printed at runtime. See the set_debug_mask() macro. */
/* -- general purpose: */
#define MP_DEBUG         (1 << 7)
#define MP_DEBUG_GENERAL MP_DEBUG
/* -- when entering/exiting functions: */
#define MP_DEBUG_FUNC_ENTER (1 << 8)
#define MP_DEBUG_FUNC_EXIT  (1 << 9)
#define MP_DEBUG_FUNC_BOUNDARIES ( MP_DEBUG_FUNC_ENTER + MP_DEBUG_FUNC_EXIT )
/* -- information emitted during loops: */
#define MP_DEBUG_ABUNDANT  (1 << 10)  /* for intensive loops (lots of output, e.g. in blocks) */
#define MP_DEBUG_MEDIUM    (1 << 11) /* for medium frequency loops */
#define MP_DEBUG_SPARSE    (1 << 12) /* loops with sparse output (e.g., dictionary browsing) */
/* -- information related to file I/O: */
#define MP_DEBUG_FILE_IO   (1 << 13)
/* -- construction/deletion of objects: */
#define MP_DEBUG_CONSTRUCTOR_ENTER (1 << 14)
#define MP_DEBUG_CONSTRUCTOR_EXIT  (1 << 15)
#define MP_DEBUG_DESTRUCTOR_ENTER  (1 << 16)
#define MP_DEBUG_DESTRUCTOR_EXIT   (1 << 17)

#define MP_MSG_LAST_TYPE MP_DEBUG_DESTRUCTOR_EXIT

#define MP_DEBUG_ALL  ULONG_MAX
#define MP_DEBUG_NONE 0


/*******************/
/* DEFAULT STREAMS */
/*******************/

/** \brief The default global output stream. */
#define MP_DEFAULT_MSG_STREAM stderr

/** \brief The default error stream. */
#define MP_DEFAULT_ERROR_STREAM MP_DEFAULT_MSG_STREAM

/** \brief The default warning stream. */
#define MP_DEFAULT_WARNING_STREAM MP_DEFAULT_MSG_STREAM

/** \brief The default info stream. */
#define MP_DEFAULT_INFO_STREAM MP_DEFAULT_MSG_STREAM

/** \brief The default debug stream. */
#define MP_DEFAULT_DEBUG_STREAM MP_DEFAULT_MSG_STREAM


/*******************/
/* OTHER CONSTANTS */
/*******************/

/** \brief The prefix printed before any message */
#define MP_LIB_STR_PREFIX "libmptk"


/***********************/
/* MESSAGING CLASS     */
/***********************/
/**
 * \brief The Messaging Server class holds and manages all the info necessary
 * to handle the redirection of error/warning/info/debug messages.
 */
class MP_Msg_Server_c {

  /********/
  /* DATA */
  /********/

public:

  /** \brief A default buffer, to store the current message string. */  
  char *stdBuff;
#define MP_DEFAULT_STDBUFF_SIZE 2048
  /** \brief The size of the default message-storing buffer. */  
  size_t stdBuffSize;
  /** \brief The type of the current message (see constants above). */  
  unsigned long int currentMsgType;

  /** \brief The handler function associated with the error messages. */  
  void (*errorHandler)( void );
  /** \brief The handler function associated with the warning messages. */  
  void (*warningHandler)( void );
  /** \brief The handler function associated with the info messages. */  
  void (*infoHandler)( void );
  /** \brief The handler function associated with the debug messages. */  
  void (*debugHandler)( void );

  /** \brief The output stream associated with error messages. */  
  FILE *errorStream;
  /** \brief The output stream associated with warning messages. */  
  FILE *warningStream;
  /** \brief The output stream associated with info messages. */  
  FILE *infoStream;
  /** \brief The output stream associated with debug messages. */  
  FILE *debugStream;

  /** \brief A binary mask to sort the DEBUG messages */
  unsigned long int debugMask;

  /** \brief A message stack **/
  char **msgStack;
  unsigned long int *msgTypeStack;
  unsigned long int stackSize;
  unsigned long int maxStackSize;
  /** \brief Granularity before a realloc of the msg stack occurs */
#define MP_MSG_STACK_GRANULARITY 128


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief A plain constructor **/
  MP_Msg_Server_c( void );
  /** \brief A plain destructor **/
  ~MP_Msg_Server_c( void );

  /***************************/
  /* OTHER METHODS           */
  /***************************/

};


/*****************************************************/
/* Global declaration of a well-known message server */
extern MP_Msg_Server_c MP_GLOBAL_MSG_SERVER;


/***************************************/
/* DEFINITION OF SOME MESSAGE HANDLERS */
/***************************************/

/** \brief A message handler which sends the incoming message
    to the appropriate pre-set stream. */
#define MP_FLUSH mp_msg_handler_flush
/** \brief Instanciation of the MP_FLUSH handler. */
void mp_msg_handler_flush( void );

/** \brief A message handler which ignores the incoming message. */
#define MP_IGNORE mp_msg_handler_ignore
/** \brief Instanciation of the MP_IGNORE handler. */
void mp_msg_handler_ignore( void );

/* TODO: MP_QUEUE, MP_RELEASE_QUEUE_AFTER_NEXT */

/**********************************/
/* MESSAGE HANDLER SETTING MACROS */
/**********************************/
/** \brief Set the error msg handler. */
#define set_error_handler( H )   ( MP_GLOBAL_MSG_SERVER.errorHandler = H )
/** \brief Set the warning msg handler. */
#define set_warning_handler( H ) ( MP_GLOBAL_MSG_SERVER.warningHandler = H )
/** \brief Set the info msg handler. */
#define set_info_handler( H )    ( MP_GLOBAL_MSG_SERVER.infoHandler = H )
/** \brief Set the debug msg handler. */
#define set_debug_handler( H )   ( MP_GLOBAL_MSG_SERVER.debugHandler = H )
/** \brief Set all the msg handlers (except debug). */
#define set_msg_handler( H )   set_error_handler( H );   \
                               set_warning_handler( H ); \
                               set_info_handler( H )

/** \brief Get the error msg handler. */
#define get_error_handler()   ( MP_GLOBAL_MSG_SERVER.errorHandler )
/** \brief Get the warning msg handler. */
#define get_warning_handler() ( MP_GLOBAL_MSG_SERVER.warningHandler )
/** \brief Get the info msg handler. */
#define get_info_handler()    ( MP_GLOBAL_MSG_SERVER.infoHandler )
/** \brief Get the debug msg handler. */
#define get_debug_handler()   ( MP_GLOBAL_MSG_SERVER.debugHandler )


/*************************/
/* STREAM SETTING MACROS */
/*************************/
/** \brief Redirect the error messages to (FILE*) F. */
#define set_error_stream( F )   ( MP_GLOBAL_MSG_SERVER.errorStream = F )
/** \brief Redirect the warning messages to (FILE*) F. */
#define set_warning_stream( F ) ( MP_GLOBAL_MSG_SERVER.warningStream = F )
/** \brief Redirect the info messages to (FILE*) F. */
#define set_info_stream( F )    ( MP_GLOBAL_MSG_SERVER.infoStream = F )
/** \brief Redirect the debug messages to (FILE*) F. */
#define set_debug_stream( F )   ( MP_GLOBAL_MSG_SERVER.debugStream = F )
/** \brief Redirect all the messages (except the debug messages) to (FILE*) F. */
#define set_msg_stream( F )   set_error_stream( F );   \
                              set_warning_stream( F ); \
                              set_info_stream( F )

/** \brief Get the current (FILE*) error stream. */
#define get_error_stream()   ( MP_GLOBAL_MSG_SERVER.errorStream )
/** \brief Get the current (FILE*) warning stream. */
#define get_warning_stream() ( MP_GLOBAL_MSG_SERVER.warningStream )
/** \brief Get the current (FILE*) info stream. */
#define get_info_stream()    ( MP_GLOBAL_MSG_SERVER.infoStream )
/** \brief Get the current (FILE*) debug stream. */
#define get_debug_stream()   ( MP_GLOBAL_MSG_SERVER.debugStream )


/***********************/
/* DEBUG MASK          */
/***********************/

/** \brief Set a mask to sort the printed debug messages.
 *
 *  \param M an unsigned long int made of an assembly of debug message types,
 *  e.g. ( MP_DEBUG_FUNC_ENTER | MP_DEBUG_FUNC_EXIT ). See mp_messaging.h for
 *  a list of available debug message types.
 */
#define set_debug_mask( M ) ( MP_GLOBAL_MSG_SERVER.debugMask = M )


/***********************/
/* MESSAGING FUNCTIONS */
/***********************/


/** \brief Pretty-printing of the libmptk error messages
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa set_error_stream(), set_error_handler().
 */
size_t mp_error_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk error messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 */
size_t mp_error_msg( FILE *fid, const char *funcName, const char *format, ... );



/** \brief Pretty-printing of the libmptk warning messages
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa set_warning_stream(), set_warning_handler().
 */
size_t mp_warning_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk warning messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 */
size_t mp_warning_msg( FILE *fid, const char *funcName, const char *format, ... );



/** \brief Pretty-printing of the libmptk info messages
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa set_info_stream(), set_info_handler().
 */
size_t mp_info_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk info messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 */
size_t mp_info_msg( FILE *fid, const char *funcName, const char *format, ... );



/** \brief Pretty-printing of the libmptk debug messages
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa set_debug_stream(), set_debug_handler(), set_debug_mask().
 */
size_t mp_debug_msg( const unsigned long int msgType, const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk debug messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 */
size_t mp_debug_msg( FILE *fid, const unsigned long int msgType, const char *funcName, const char *format, ... );



#endif /* __mp_messaging_h_ */
