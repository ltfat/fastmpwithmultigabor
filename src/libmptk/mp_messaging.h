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
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

#ifndef __mp_messaging_h_
#define __mp_messaging_h_

#include "mp_system.h"

#include "mp_hash_container_header.h"


#if defined(_MSC_VER)
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

/*********************/
/* DEFAULT MSG TYPES */
/*********************/
/* - NULL type: */
#define MP_MSG_NULL   0
/* - General types: */
#define MP_ERROR      1
#define MP_WARNING    (1 << 1)
#define MP_INFO       (1 << 2)
#define MP_PROGRESS   (1 << 3)
/* - Reserved for future use: */
#define MP_RESERVED_1 (1 << 4)
#define MP_RESERVED_2 (1 << 5)
#define MP_RESERVED_3 (1 << 6)
/* - Debugging types (up to 24 types): */
/* these types can be used to fine tune which debug messages should
   or should not be printed at runtime. See the set_debug_mask() macro. */
/* -- general purpose: */
#define MP_DEBUG         (1 << 7)
#define MP_DEBUG_GENERAL MP_DEBUG
/* -- when entering/exiting functions: */
#define MP_DEBUG_FUNC_ENTER (1 << 8)
#define MP_DEBUG_FUNC_EXIT  (1 << 9)
#define MP_DEBUG_FUNC_BOUNDARIES ( MP_DEBUG_FUNC_ENTER | MP_DEBUG_FUNC_EXIT )
/* -- information emitted during loops: */
#define MP_DEBUG_ABUNDANT  (1 << 10)  /* for intensive loops (lots of output, e.g. in blocks) */
#define MP_DEBUG_MEDIUM    (1 << 11) /* for medium frequency loops */
#define MP_DEBUG_SPARSE    (1 << 12) /* loops with sparse output (e.g., dictionary browsing) */
/* -- information related to file I/O: */
#define MP_DEBUG_FILE_IO   (1 << 13)
/* -- construction/deletion of objects: */
#define MP_DEBUG_CONSTRUCTION (1 << 14)
#define MP_DEBUG_DESTRUCTION  (1 << 15)
#define MP_DEBUG_OBJ_LIFE ( MP_DEBUG_CONSTRUCTION | MP_DEBUG_DESTRUCTION )
/* -- Matching Pursuit iterations: */
#define MP_DEBUG_MP_ITERATIONS (1 << 16)
/* -- Specific for function create_atom(): */
#define MP_DEBUG_CREATE_ATOM   (1 << 17)
/* -- Specific for function create_atom()array bounds check: */
#define MP_DEBUG_ARRAY_BOUNDS  (1 << 18)
/* -- Argument parsing in utils: */
#define MP_DEBUG_PARSE_ARGS    (1 << 19)
/* -- MPD loop in utils: */
#define MP_DEBUG_MPD_LOOP      (1 << 20)
/* -- Specific for internal atom operations: */
#define MP_DEBUG_ATOM          (1 << 21)
/* -- Specific for addressing issues */
#define MP_DEBUG_ADDR          (1 << 22)

#define MP_MSG_LAST_TYPE MP_DEBUG_ADDR

#define MP_DEBUG_ALL  ULONG_MAX
#define MP_DEBUG_NONE 0

/*******************/
/* OTHER CONSTANTS */
/*******************/

/** \brief The prefix printed before any message */
#define MP_LIB_STR_PREFIX "mptk"


/***********************/
/* MESSAGING CLASS     */
/***********************/
/**
 * \brief The Messaging Server class holds and manages all the info necessary
 * to handle the redirection of error/warning/info/debug messages.
 */
class MP_Msg_Server_c
  {

    /********/
    /* DATA */
    /********/
      protected:
    /** \brief Protected pointer on MP_Atom_Factory_c*/
    static MP_Msg_Server_c * myMsgServer;
     public:
     
STL_EXT_NM::hash_map<const char*,void(*)(char * message),mp_hash_fun, mp_eqstr> displayFunction;

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
    /** \brief The handler function associated with the progress messages. */
    void (*progressHandler)( void );
    /** \brief The handler function associated with the debug messages. */
    void (*debugHandler)( void );

    /** \brief The output stream associated with error messages. */
    void * errorStream;
    /** \brief The output stream associated with warning messages. */
    void * warningStream;
    /** \brief The output stream associated with info messages. */
    void *infoStream;
    /** \brief The output stream associated with progress messages. */
    void *progressStream;
    /** \brief The output stream associated with debug messages. */
    void *debugStream;

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
    static void default_display_error_function(char* message);
    /** \brief Method to get the MP_Atom_Factory_c */
    static MP_Msg_Server_c * get_msg_server();
    /** \brief A plain constructor **/

    /** \brief A plain destructor **/
    virtual ~MP_Msg_Server_c( void );
    void register_display_function(const char* functionType, void(*displayFunctionPointer)(char * message));
    void (*get_display_function( const char* functionType))(char * message);

#if defined(_MSC_VER)

int mp_vsnprintf( char *str, size_t size, const char *format, va_list ap )
{
    if( str != NULL )
    {
        //Version "secure" microsoft (VS2005 ou supérieur requis)
        //_vsnprintf_s( str, size, _TRUNCATE, format, ap );
 
        //Version "ancienne"
        _vsnprintf( str, size, format, ap );
        str[size-1] = '\0';
    }
    return _vscprintf( format, ap );
}
#else

int mp_vsnprintf( char *str, size_t size, const char *format, va_list ap )
{
  return vsnprintf(str,size,format,ap);
}

#endif


  private:
    MP_Msg_Server_c( void );

  };

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
#define set_error_handler( H )   ( MPTK_Server_c::get_msg_server()->errorHandler = H )
/** \brief Set the warning msg handler. */
#define set_warning_handler( H ) ( MPTK_Server_c::get_msg_server()->warningHandler = H )
/** \brief Set the info msg handler. */
#define set_info_handler( H )    ( MPTK_Server_c::get_msg_server()->infoHandler = H )
/** \brief Set the progress msg handler. */
#define set_progress_handler( H )    ( MPTK_Server_c::get_msg_server()->progressHandler = H )
/** \brief Set the debug msg handler. */
#define set_debug_handler( H )   ( MPTK_Server_c::get_msg_server()->debugHandler = H )
/** \brief Set all the msg handlers (except debug). */
#define set_msg_handler( H )   set_error_handler( H );   \
  set_warning_handler( H );				 \
  set_info_handler( H );				 \
  set_progress_handler( H );

/** \brief Get the error msg handler. */
#define get_error_handler()   ( MPTK_Server_c::get_msg_server()->errorHandler )
/** \brief Get the warning msg handler. */
#define get_warning_handler() ( MPTK_Server_c::get_msg_server()->warningHandler )
/** \brief Get the info msg handler. */
#define get_info_handler()    ( MPTK_Server_c::get_msg_server()->infoHandler )
/** \brief Get the progress msg handler. */
#define get_progress_handler()    ( MPTK_Server_c::get_msg_server()->progressHandler )
/** \brief Get the debug msg handler. */
#define get_debug_handler()   ( MPTK_Server_c::get_msg_server()->debugHandler )


/*************************/
/* STREAM SETTING MACROS */
/*************************/
/** \brief Redirect the error messages to (FILE*) F. */
#define set_error_stream( F )   ( MPTK_Server_c::get_msg_server()->errorStream = F )
/** \brief Redirect the warning messages to (FILE*) F. */
#define set_warning_stream( F ) ( MPTK_Server_c::get_msg_server()->warningStream = F )
/** \brief Redirect the info messages to (FILE*) F. */
#define set_info_stream( F )    ( MPTK_Server_c::get_msg_server()->infoStream = F )
/** \brief Redirect the progress messages to (FILE*) F. */
#define set_progress_stream( F )    ( MPTK_Server_c::get_msg_server()->progressStream = F )
/** \brief Redirect the debug messages to (FILE*) F. */
#define set_debug_stream( F )   ( MPTK_Server_c::get_msg_server()->debugStream = F )
/** \brief Redirect all the messages (except the debug messages) to (FILE*) F. */
#define set_msg_stream( F )   set_error_stream( F );   \
                              set_warning_stream( F ); \
                              set_info_stream( F )

/** \brief Get the current (FILE*) error stream. */
#define get_error_stream()   ( MPTK_Server_c::get_msg_server()->errorStream )
/** \brief Get the current (FILE*) warning stream. */
#define get_warning_stream() ( MPTK_Server_c::get_msg_server()->warningStream )
/** \brief Get the current (FILE*) info stream. */
#define get_info_stream()    ( (FILE*)MPTK_Server_c::get_msg_server()->infoStream )
/** \brief Get the current (FILE*) progress stream. */
#define get_progress_stream()    ( MPTK_Server_c::get_msg_server()->progressStream )
/** \brief Get the current (FILE*) debug stream. */
#define get_debug_stream()   ( MPTK_Server_c::get_msg_server()->debugStream )


/***********************/
/* DEBUG MASK          */
/***********************/

/** \brief Set a mask to sort the printed debug messages.
 *
 *  \param M an unsigned long int made of an assembly of debug message types,
 *  e.g. ( MP_DEBUG_FUNC_ENTER | MP_DEBUG_FUNC_EXIT ). See mp_messaging.h for
 *  a list of available debug message types.
 */
#define set_debug_mask( M ) ( MPTK_Server_c::get_msg_server()->debugMask = M )


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



/** \brief Pretty-printing of the libmptk progress messages
 *
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \sa set_progress_stream(), set_progress_handler().
 */
size_t mp_progress_msg( const char *funcName, const char *format, ... );

/** \brief Pretty-printing of the libmptk progress messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 */
size_t mp_progress_msg( FILE *fid, const char *funcName, const char *format, ... );



/** \brief Pretty-printing of the libmptk debug messages
 *
 * \param msgType a message type, defined in mp_messaging.h, which allows to
 * sort the debug output according to the mask set with set_debug_mask()
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \note The mp_debug_msg functions are not compiled when the flag -DNDEBUG is used.
 *
 * \sa mp_debug_msg_forced(), set_debug_stream(), set_debug_handler(), set_debug_mask().
 */
#ifndef NDEBUG
size_t mp_debug_msg( const unsigned long int msgType, const char *funcName, const char *format, ... );
#else
#define mp_debug_msg
//( A, B, C, ... ) (void)(0)
#endif

/** \brief Pretty-printing of the libmptk debug messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param msgType a message type, defined in mp_messaging.h, which allows to
 * sort the debug output according to the mask set with set_debug_mask()
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \note The mp_debug_msg functions are not compiled when the flag -DNDEBUG is used.
 *
 * \sa mp_debug_msg_forced(), set_debug_stream(), set_debug_handler(), set_debug_mask().
 */
#ifndef NDEBUG
size_t mp_debug_msg( FILE *fid, const unsigned long int msgType, const char *funcName, const char *format, ... );
#endif


/** \brief Forced pretty-printing of the libmptk debug messages
 *
 * \param msgType a message type, defined in mp_messaging.h. For this function,
 * the debug messages mask is ignored.
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \note This function is always compiled in (no interaction with -DNDEBUG). This "alias"
 * can be used to force the output of a particular debug function.
 *
 * \sa mp_msg_debug(), set_debug_stream(), set_debug_handler(), set_debug_mask().
 */
size_t mp_debug_msg_forced( const unsigned long int msgType, const char *funcName, const char *format, ... );

/** \brief Forced pretty-printing of the libmptk debug messages to a specific stream
 *
 * \param fid the (FILE*) stream to write to
 * \param msgType a message type, defined in mp_messaging.h, which allows to
 * sort the debug output according to the mask set with set_debug_mask()
 * \param funcName the name of the calling function
 * \param format a format string similar to the printf formats
 * \param ... a variable list of arguments to be printed according to the format
 *
 * \note This function is always compiled in (no interaction with -DNDEBUG). This "alias"
 * can be used to force the output of a particular debug function.
 *
 * \sa mp_msg_debug(), set_debug_stream(), set_debug_handler(), set_debug_mask().
 */
size_t mp_debug_msg_forced( FILE *fid, const unsigned long int msgType, const char *funcName, const char *format, ... );


#endif /* __mp_messaging_h_ */
