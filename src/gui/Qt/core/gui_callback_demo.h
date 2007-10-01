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

class MP_Gui_Callback_Demo_c:public MP_Gui_Callback_c
{
public:
    void playTransientSignal(std::vector<bool> * v, float startTime, float endTime);
    void playOtherSignal(std::vector<bool> * v, float startTime, float endTime);
	MP_Gui_Callback_Demo_c();
	virtual ~MP_Gui_Callback_Demo_c();
	void separate(unsigned long int length);
	MP_Gabor_Atom_Plugin_c* newAtom;
	MP_Book_c * booktransient;
	MP_Book_c * bookother;
	MP_Signal_c *transientSignal;
	MP_Signal_c *otherSignal;
	
};

#endif /*MP_GUI_CALLBACK_DEMO_H_*/
