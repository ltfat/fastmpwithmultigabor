/******************************************************************************/
/*                                                                            */
/*                               mp_messaging.cpp                             */
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


/*************************************/
/* DECLARATION OF A MESSAGING SERVER */
/* WITH GLOBAL SCOPE                 */
/*************************************/
/**
 * \brief Declaration of a unique messaging server with global scope over the
 * whole library
 */
MP_Msg_Server_c MP_GLOBAL_MSG_SERVER;


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Msg_Server_c::MP_Msg_Server_c( void ) {
#ifndef NDEBUG
  fprintf( stderr, MP_LIB_STR_PREFIX " DEBUG -- ~MP_Msg_Server_c -- Entering the messaging server constructor.\n" );
#endif

  /* Allocate the standard string buffer */
  if ( ( stdBuff = (char*) calloc( MP_DEFAULT_STDBUFF_SIZE, sizeof(char) ) ) == NULL ) {
    fprintf( stderr, MP_LIB_STR_PREFIX " ERROR -- MP_Msg_Server() - Can't allocate the standard string buffer."
	     " Crashing the process !\n" );
    fflush( stderr );
    exit( 0 );
  }
  /* Set the related values */
  stdBuffSize = MP_DEFAULT_STDBUFF_SIZE;
  currentMsgType = MP_MSG_NULL;

  /* Set the default handler */
  errorHandler = warningHandler = infoHandler = MP_FLUSH;
  debugHandler = MP_IGNORE;

  /* Set the default output file values */
  errorStream = MP_DEFAULT_ERROR_STREAM;
  warningStream = MP_DEFAULT_WARNING_STREAM;
  infoStream = MP_DEFAULT_INFO_STREAM;
  debugStream = MP_DEFAULT_DEBUG_STREAM;

  /* Initialize the debug mask to "all messages" */
  debugMask = MP_DEBUG_ALL;

  /* Initialize the stack */
  msgStack = NULL;
  msgTypeStack = NULL;
  stackSize = 0;
  maxStackSize = 0;

#ifndef NDEBUG
  fprintf( stderr, MP_LIB_STR_PREFIX " DEBUG -- ~MP_Msg_Server_c -- Exiting the messaging server destructor.\n" );
#endif
}

/**************/
/* destructor */
MP_Msg_Server_c::~MP_Msg_Server_c() {
#ifndef NDEBUG
  fprintf( stderr, MP_LIB_STR_PREFIX " DEBUG -- ~MP_Msg_Server_c -- Entering the messaging server destructor.\n" );
  fflush( stderr );
#endif

  /* Free the standard string buffer */
  if ( stdBuff ) free( stdBuff );

  /* Free the msg stack */
  if ( stackSize != 0 ) {
    unsigned long int i;
    for ( i = 0; i < stackSize; i++ ) {
      if ( msgStack[i] ) free ( msgStack[i] );
    }
  }
  if ( msgStack ) free( msgStack );
  if ( msgTypeStack ) free( msgTypeStack );


#ifndef NDEBUG
  fprintf( stderr, MP_LIB_STR_PREFIX " DEBUG -- ~MP_Msg_Server_c -- Exiting the messaging server destructor.\n" );
  fflush( stderr );
#endif
}


/***********/
/* METHODS */
/***********/

/* TODO: push/pop messages to/from the stack. */


/***********************/
/* MESSAGE HANDLERS    */
/***********************/

/***********************************************************/
/* Handler which flushes the output to the relevant stream */
void mp_msg_handler_flush( void ) {

  switch ( MP_GLOBAL_MSG_SERVER.currentMsgType ) {

  case MP_MSG_NULL:
    fprintf( stderr, MP_LIB_STR_PREFIX  " WARNING -- mp_msg_handler_flush() -"
	     " A NULL message type reached the mp_msg_handler_flush() handler."
	     " Ignoring this message.\n" );
    fflush( stderr );
    break;

  case MP_ERROR:
    if ( MP_GLOBAL_MSG_SERVER.errorStream == NULL ) return;
    fprintf( MP_GLOBAL_MSG_SERVER.errorStream, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
    fflush( MP_GLOBAL_MSG_SERVER.errorStream );
    break;

  case MP_WARNING:
    if ( MP_GLOBAL_MSG_SERVER.warningStream == NULL ) return;
    fprintf( MP_GLOBAL_MSG_SERVER.warningStream, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
    fflush( MP_GLOBAL_MSG_SERVER.warningStream );
    break;

  case MP_INFO:
    if ( MP_GLOBAL_MSG_SERVER.infoStream == NULL ) return;
    fprintf( MP_GLOBAL_MSG_SERVER.infoStream, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
    fflush( MP_GLOBAL_MSG_SERVER.infoStream );
    break;

  default:
    if ( MP_GLOBAL_MSG_SERVER.currentMsgType > MP_MSG_LAST_TYPE ) {
      fprintf( stderr, MP_LIB_STR_PREFIX  " ERROR -- mp_msg_handler_flush() -"
	       " Invalid message type handled by mp_msg_handler_flush()."
	       " Ignoring the message.\n" );
      fflush( stderr );
    }
    else {
      if ( !(MP_GLOBAL_MSG_SERVER.currentMsgType & MP_GLOBAL_MSG_SERVER.debugMask) )
	return; /* If the message type does not fit the mask,
		   stop here and do nothing. */
      if ( MP_GLOBAL_MSG_SERVER.debugStream == NULL ) return;
      fprintf( MP_GLOBAL_MSG_SERVER.debugStream, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
      fflush( MP_GLOBAL_MSG_SERVER.debugStream );
    }
    break;

  }

  return;
}

/*********************************************/
/* Handler which ignores the current message */
void mp_msg_handler_ignore( void ) {
  return;
}


/***********************/
/* MESSAGING FUNCTIONS */
/***********************/

/***************************************************************/
/* Generic pretty-printing and "parking" of the message string */
/* 
 * This function formats the passed message and
 * stores it in stdBuff in the global messaging server.
 */
size_t make_msg_str( const char *strMsgType, const char *funcName, const char *format, va_list arg ) {

  size_t finalSize;
  size_t beginSize;

  /* Pretty-print the beginning of the string */
  beginSize = snprintf( MP_GLOBAL_MSG_SERVER.stdBuff, MP_GLOBAL_MSG_SERVER.stdBuffSize,
			MP_LIB_STR_PREFIX " %s -- %s - ", strMsgType, funcName );

  /* Check if the string overflows the message buffer; if yes, just message */
  if ( beginSize >= MP_GLOBAL_MSG_SERVER.stdBuffSize ) {
    fprintf( stderr, MP_LIB_STR_PREFIX " %s -- mp_msg() - Function name [%s] has been truncated.\n",
	     strMsgType, funcName );
    fflush( stderr );
  }

  /* Typeset the rest of the string, with the variable argument list */
  finalSize = beginSize + vsnprintf( MP_GLOBAL_MSG_SERVER.stdBuff + beginSize,
				     MP_GLOBAL_MSG_SERVER.stdBuffSize - beginSize,
				     format, arg );

  /* Check if the string overflows the message buffer; if yes, realloc the buffer and re-typeset */
  if ( finalSize >= MP_GLOBAL_MSG_SERVER.stdBuffSize ) {
    char *tmp;
    if ( ( tmp = (char*)realloc( MP_GLOBAL_MSG_SERVER.stdBuff, finalSize+1 ) ) == NULL ) {
      fprintf( stderr, MP_LIB_STR_PREFIX " ERROR -- mp_msg() - Can't realloc the message buffer."
	       " The current message will be truncated.\n" );
      fflush( stderr );
    }
    else {
      MP_GLOBAL_MSG_SERVER.stdBuff = tmp;
      finalSize = beginSize + vsnprintf( MP_GLOBAL_MSG_SERVER.stdBuff + beginSize,
					 MP_GLOBAL_MSG_SERVER.stdBuffSize - beginSize,
					 format, arg );
    }
  }

  /* Return the final message size */
  return( finalSize );
}


/******************/
/* Error messages */
/******************/

/******************/
/* Using handler: */
size_t mp_error_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MP_GLOBAL_MSG_SERVER.errorHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MP_GLOBAL_MSG_SERVER.currentMsgType = MP_ERROR;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "ERROR", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MP_GLOBAL_MSG_SERVER.errorHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_error_msg( FILE *fid, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "ERROR", funcName, format, arg );
  va_end ( arg );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
  fflush( fid );

  return( done );
}


/********************/
/* Warning messages */
/********************/

/******************/
/* Using handler: */
size_t mp_warning_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MP_GLOBAL_MSG_SERVER.warningHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MP_GLOBAL_MSG_SERVER.currentMsgType = MP_WARNING;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "WARNING", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MP_GLOBAL_MSG_SERVER.warningHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_warning_msg( FILE *fid, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "WARNING", funcName, format, arg );
  va_end ( arg );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
  fflush( fid );

  return( done );
}


/*****************/
/* Info messages */
/*****************/

/******************/
/* Using handler: */
size_t mp_info_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MP_GLOBAL_MSG_SERVER.infoHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MP_GLOBAL_MSG_SERVER.currentMsgType = MP_INFO;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "INFO", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MP_GLOBAL_MSG_SERVER.infoHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_info_msg( FILE *fid, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "INFO", funcName, format, arg );
  va_end ( arg );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
  fflush( fid );

  return( done );
}


/******************/
/* Debug messages */
/******************/

#ifndef NDEBUG

/******************/
/* Using handler: */
size_t mp_debug_msg( const unsigned long int msgType, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MP_GLOBAL_MSG_SERVER.debugHandler == MP_IGNORE ) return( 0 );
 /* If the message type does not fit the mask, stop here and do nothing. */
  if ( !(msgType & MP_GLOBAL_MSG_SERVER.debugMask) ) return( 0 );
  /* Store the message type in the server */
  MP_GLOBAL_MSG_SERVER.currentMsgType = msgType;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MP_GLOBAL_MSG_SERVER.debugHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_debug_msg( FILE *fid, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MP_GLOBAL_MSG_SERVER.stdBuff );
  fflush( fid );

  return( done );
}

#endif /* #ifndef NDEBUG */
