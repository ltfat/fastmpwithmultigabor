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
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

/********************************************************/
/*                                                      */
/* PRETTY PRINTING OF ERROR/WARNING/INFO/DEBUG MESSAGES */
/*                                                      */
/********************************************************/

#include "mptk.h"
#include "mp_system.h"
#include <iostream>

using namespace std;

/*************************************/
/* DECLARATION OF A MESSAGING SERVER */
/* WITH GLOBAL SCOPE                 */
/*************************************/


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Msg_Server_c::MP_Msg_Server_c( void ) {
#ifndef NDEBUG
  cerr << MP_LIB_STR_PREFIX << " DEBUG -- MP_Msg_Server_c -- Entering the messaging server constructor.\n" << flush;
#endif

  /* Allocate the standard string buffer */
  if ( ( stdBuff = (char*) calloc( MP_DEFAULT_STDBUFF_SIZE, sizeof(char) ) ) == NULL ) {
  	cerr << MP_LIB_STR_PREFIX << "ERROR -- MP_Msg_Server() - Can't allocate the standard string buffer. Crashing the process !\n" << flush;
    exit( 0 );
  }
  /* Set the related values */
  stdBuffSize = MP_DEFAULT_STDBUFF_SIZE;
  currentMsgType = MP_MSG_NULL;

  /* Set the default handler */
  errorHandler = warningHandler = infoHandler = progressHandler = debugHandler = MP_FLUSH;

  /* Set the default output file values */
  errorStream = MP_DEFAULT_ERROR_STREAM;
  warningStream = MP_DEFAULT_WARNING_STREAM;
  infoStream = MP_DEFAULT_INFO_STREAM;
  progressStream = MP_DEFAULT_PROGRESS_STREAM;
  debugStream = MP_DEFAULT_DEBUG_STREAM;

  /* Initialize the debug mask to "all messages" */
  debugMask = MP_DEBUG_ALL;

  /* Initialize the stack */
  msgStack = NULL;
  msgTypeStack = NULL;
  stackSize = 0;
  maxStackSize = 0;

#ifndef NDEBUG
cerr << MP_LIB_STR_PREFIX << " DEBUG -- MP_Msg_Server_c -- Exiting the messaging server constructor.\n" << flush;
#endif
}

/**************/
/* destructor */
MP_Msg_Server_c::~MP_Msg_Server_c() {
#ifndef NDEBUG
cerr << MP_LIB_STR_PREFIX << " DEBUG -- ~MP_Msg_Server_c -- Entering the messaging server destructor.\n" << flush;
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
cerr << MP_LIB_STR_PREFIX << " DEBUG -- ~MP_Msg_Server_c -- Exiting the messaging server destructor.\n" << flush;
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

  switch ( MPTK_Server_c::get_msg_server()->currentMsgType ) {

  case MP_MSG_NULL:
   cerr << MP_LIB_STR_PREFIX << 
         " WARNING -- mp_msg_handler_flush() -" <<
	     " A NULL message type reached the mp_msg_handler_flush() handler." <<
	     " Ignoring this message.\n" 
    << flush;
    break;

  case MP_ERROR:
    if ( MPTK_Server_c::get_msg_server()->errorStream == NULL ) return;
    if ( MPTK_Server_c::get_msg_server()->errorStream == stderr ){
    fprintf( (FILE*)MPTK_Server_c::get_msg_server()->errorStream, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
    fflush( (FILE*)MPTK_Server_c::get_msg_server()->errorStream );}
    if ( MPTK_Server_c::get_msg_server()->errorStream == cerr )
    cerr << MPTK_Server_c::get_msg_server()->stdBuff << flush; 
    break;

  case MP_WARNING:
    if ( MPTK_Server_c::get_msg_server()->warningStream == NULL ) return;
    if ( MPTK_Server_c::get_msg_server()->warningStream == stderr ){
    fprintf( (FILE*)MPTK_Server_c::get_msg_server()->warningStream, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
    fflush( (FILE*)MPTK_Server_c::get_msg_server()->warningStream );}
    if ( MPTK_Server_c::get_msg_server()->warningStream == cerr )
    cerr << MPTK_Server_c::get_msg_server()->stdBuff << flush; 
    
    break;

  case MP_INFO:
    if ( MPTK_Server_c::get_msg_server()->infoStream == NULL ) return;
    if ( MPTK_Server_c::get_msg_server()->infoStream == stderr ){
    fprintf( (FILE*)MPTK_Server_c::get_msg_server()->infoStream, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
    fflush( (FILE*)MPTK_Server_c::get_msg_server()->infoStream );}
    if ( MPTK_Server_c::get_msg_server()->infoStream == cerr )
     cerr << MPTK_Server_c::get_msg_server()->stdBuff << flush; 
     
    break;

  case MP_PROGRESS:
    if ( MPTK_Server_c::get_msg_server()->progressStream == NULL ) return;
    fprintf( (FILE*)MPTK_Server_c::get_msg_server()->progressStream, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
    fflush( (FILE*)MPTK_Server_c::get_msg_server()->progressStream );
    break;

  default:
    if ( MPTK_Server_c::get_msg_server()->currentMsgType > MP_MSG_LAST_TYPE ) {
      fprintf( stderr, MP_LIB_STR_PREFIX  " ERROR -- mp_msg_handler_flush() -"
	       " Invalid message type handled by mp_msg_handler_flush()."
	       " Ignoring the message.\n" );
      fflush( stderr );
    }
    else {
      if ( !(MPTK_Server_c::get_msg_server()->currentMsgType & MPTK_Server_c::get_msg_server()->debugMask) )
	return; /* If the message type does not fit the mask,
		   stop here and do nothing. */
      if ( MPTK_Server_c::get_msg_server()->debugStream == NULL ) return;
      fprintf( (FILE*)MPTK_Server_c::get_msg_server()->debugStream, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
      fflush( (FILE*)MPTK_Server_c::get_msg_server()->debugStream );
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
  beginSize = snprintf( MPTK_Server_c::get_msg_server()->stdBuff, MPTK_Server_c::get_msg_server()->stdBuffSize,
			MP_LIB_STR_PREFIX " %s -- %s - ", strMsgType, funcName );

  /* Check if the string overflows the message buffer; if yes, just message */
  if ( beginSize >= MPTK_Server_c::get_msg_server()->stdBuffSize ) {
    fprintf( stderr, MP_LIB_STR_PREFIX " %s -- mp_msg() - Function name [%s] has been truncated.\n",
	     strMsgType, funcName );
    fflush( stderr );
  }

  /* Typeset the rest of the string, with the variable argument list */
  finalSize = beginSize + vsnprintf( MPTK_Server_c::get_msg_server()->stdBuff + beginSize,
				     MPTK_Server_c::get_msg_server()->stdBuffSize - beginSize,
				     format, arg );

  /* Check if the string overflows the message buffer; if yes, realloc the buffer and re-typeset */
  if ( finalSize >= MPTK_Server_c::get_msg_server()->stdBuffSize ) {
    char *tmp;
    if ( ( tmp = (char*)realloc( MPTK_Server_c::get_msg_server()->stdBuff, finalSize+1 ) ) == NULL ) {
      fprintf( stderr, MP_LIB_STR_PREFIX " ERROR -- mp_msg() - Can't realloc the message buffer."
	       " The current message will be truncated.\n" );
      fflush( stderr );
    }
    else {
      MPTK_Server_c::get_msg_server()->stdBuff = tmp;
      finalSize = beginSize + vsnprintf( MPTK_Server_c::get_msg_server()->stdBuff + beginSize,
					 MPTK_Server_c::get_msg_server()->stdBuffSize - beginSize,
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
  if ( MPTK_Server_c::get_msg_server()->errorHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = MP_ERROR;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "ERROR", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->errorHandler)();

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
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
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
  if ( MPTK_Server_c::get_msg_server()->warningHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = MP_WARNING;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "WARNING", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->warningHandler)();

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
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
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
  if ( MPTK_Server_c::get_msg_server()->infoHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = MP_INFO;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "INFO", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->infoHandler)();

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
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
  fflush( fid );

  return( done );
}


/*****************/
/* Progress messages */
/*****************/

/******************/
/* Using handler: */
size_t mp_progress_msg( const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MPTK_Server_c::get_msg_server()->progressHandler == MP_IGNORE ) return( 0 );
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = MP_PROGRESS;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "PROGRESS", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->progressHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_progress_msg( FILE *fid, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "PROGRESS", funcName, format, arg );
  va_end ( arg );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
  fflush( fid );

  return( done );
}


/******************/
/* Debug messages */
/******************/

/* ---- "Ghost" debug functions */
#ifndef NDEBUG

/******************/
/* Using handler: */
size_t mp_debug_msg( const unsigned long int msgType, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MPTK_Server_c::get_msg_server()->debugHandler == MP_IGNORE ) return( 0 );
 /* If the message type does not fit the mask, stop here and do nothing. */
  if ( !(msgType & MPTK_Server_c::get_msg_server()->debugMask) ) return( 0 );
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = msgType;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->debugHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_debug_msg( FILE *fid, const unsigned long int msgType, const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
 /* If the message type does not fit the mask, stop here and do nothing. */
  if ( !(msgType & MPTK_Server_c::get_msg_server()->debugMask) ) return( 0 );
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
  fflush( fid );

  return( done );
}

#endif /* #ifndef NDEBUG */

/*---- "Forced" debug functions */

/******************/
/* Using handler: */
size_t mp_debug_msg_forced( const unsigned long int msgType, const char *funcName,
			    const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* If the handler is MP_IGNORE, stop here and do nothing. */
  if ( MPTK_Server_c::get_msg_server()->debugHandler == MP_IGNORE ) return( 0 );
  /* NOTE: In this version, the mask is ignored, the message is output in any case. */
  /* Store the message type in the server */
  MPTK_Server_c::get_msg_server()->currentMsgType = msgType;
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
  /* Launch the message handler */
  (MPTK_Server_c::get_msg_server()->debugHandler)();

  return( done );
}

/**********************/
/* Bypassing handler: */
size_t mp_debug_msg_forced( FILE *fid, const unsigned long int /* msgType */,
			    const char *funcName, const char *format, ...  ) {

  va_list arg;
  size_t done;
  
  /* Make the message string */
  va_start ( arg, format );
  done = make_msg_str( "DEBUG", funcName, format, arg );
  va_end ( arg );
  /* NOTE: In this version, the mask is ignored, the message is output in any case. */
  /* Print the string */
  if ( fid == NULL ) return( 0 );
  fprintf( fid, "%s", MPTK_Server_c::get_msg_server()->stdBuff );
  fflush( fid );

  return( done );
}
