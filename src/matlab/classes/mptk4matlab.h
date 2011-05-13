/******************************************************************************/
/*                                                                            */
/*                  	      mptk4matlab.h                                   */
/*                                                                            */
/*          				mptk4matlab toolbox                   */
/*                                                                            */
/* Remi Gribonval                                           	 July 13 2008 */
/* -------------------------------------------------------------------------- */
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
 * $Version 0.5.3$
 * $Date 05/22/2007$
 */

#include "mex.h"
#include "mptk.h"

/** \brief The function to be called at the beginning of each mex file 
 * \param functionName the name of the calling MEX-function. Typically, call it as InitMPTK4Matlab(mexFunctionName())
 */
MPTK_LIB_EXPORT extern void InitMPTK4Matlab(const char *functionName);

/** \brief Converts a MP_Dict_c object to a Matlab structure 
 * \param dict the MPTK object
 * \return the created Matlab structure, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *mp_create_mxDict_from_dict(MP_Dict_c *dict);

/** \brief Converts a Matlab structure to a MP_Dict_c object
 * \param mxDict the Matlab structre
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern MP_Dict_c *mp_create_dict_from_mxDict(const mxArray *mxDict);

/** \brief Converts a Matlab structure to a MP_Dict_c object
 * \param mxDict the Matlab structre
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *anywaveTableRead(map<string, string, mp_ltstring> *paramMap, char *szFileName);
MPTK_LIB_EXPORT extern char *anywaveDataRead(mxArray *mxBlock);

/** \brief Converts a MP_Signal_c object to a Matlab structure 
 * \param signal the MPTK object
 * \return the created Matlab structure, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *mp_create_mxSignal_from_signal(MP_Signal_c *signal);

/** \brief Converts a Matlab structure to a MP_Signal_c object
 * \param mxSignal the Matlab structre
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern MP_Signal_c *mp_create_signal_from_mxSignal(const mxArray *mxSignal);

/** \brief Converts a MP_Book_c object to a Matlab structure 
 * \param book the MPTK object
 * \return the created Matlab structure, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *mp_create_mxBook_from_book(MP_Book_c *book);

/** \brief Converts a Matlab structure to a MP_Book_c object
 * \param mxBook the Matlab structre
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern MP_Book_c *mp_create_book_from_mxBook(const mxArray *mxBook);

/** \brief Converts a MP_Anywave_Table_c object to a Matlab structure 
 * \param AnyTable the MPTK object
 * \return the created Matlab structure, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *mp_create_mxAnywaveTable_from_anywave_table(const MP_Anywave_Table_c *AnyTable);

/** \brief Converts a Matlab structure to a MP_Anywave_Table_c object
 * \param mxTable the Matlab structre
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern double *mp_get_anywave_datas_from_mxAnywaveTable(const mxArray *mxTable);


