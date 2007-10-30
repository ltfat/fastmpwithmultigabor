/******************************************************************************/
/*                                                                            */
/*                            gui_callback_demo.h                                  */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Roy Benjamin                                               Mon Feb 21 2007 */
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

/*********************************************/
/*                                           */
/* DEFINITION OF THE GUI CALLBACK DEMO CLASS */
/*                                           */
/*********************************************/

#ifndef MP_GUI_CALLBACK_DEMO_H_
#define MP_GUI_CALLBACK_DEMO_H_

#include "gui_callback.h"
#include "../plugin/base/gabor_atom_plugin.h"

/**
 * \brief MP_Gui_Callback_Demo_c is a concrete class that provides the link between main_window (graphical side)
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 * \note inherit from QTrhread in order to have threading abilities for decomposition
 */

class MP_Gui_Callback_Demo_c:public MP_Gui_Callback_c
{
	Q_OBJECT
    /***********/
    /* DATA    */
    /***********/
  protected:
  static  MP_Gui_Callback_Demo_c * guiCallbackDemo;
public:
    void playTransientSignal(std::vector<bool> * v, float startTime, float endTime);
    void playOtherSignal(std::vector<bool> * v, float startTime, float endTime);
	MP_Gui_Callback_Demo_c();
	virtual ~MP_Gui_Callback_Demo_c();
	static MP_Gui_Callback_Demo_c * get_gui_call_back();
	void separate(unsigned long int length);
	MP_Gabor_Atom_Plugin_c* newAtom;
	MP_Book_c * booktransient;
	MP_Book_c * bookother;
	MP_Signal_c *transientSignal;
	MP_Signal_c *otherSignal;
	
	static void emitInfoMessage(char* message);
    static void emitErrorMessage(char* message);
    static void emitWarningMessage(char* message);
    signals:
  /**  \brief A Qt signal to
   *   \param status A boolean (true if iteration is running, false else) 
   *   */
    void infoMessage(char* message);
    void errorMessage(char* message);
    void warningMessage(char* message);
	
};

#endif /*MP_GUI_CALLBACK_DEMO_H_*/
