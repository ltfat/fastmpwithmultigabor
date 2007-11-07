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
  public:
    /**
    * \brief a pointer on a Gabor Atom to select between short and long scale atoms in the book
    */
    MP_Gabor_Atom_Plugin_c* newAtom;
    /**
    * \brief a pointer on book to put the transient part of the signal
    */
    MP_Book_c * booktransient;
    /**
    * \brief a pointer on book to put the rest of the signal (tonal)
    */
    MP_Book_c * bookother;
    /**
    * \brief a pointer on the transient part of the signal
    */
    MP_Signal_c *transientSignal;
    /**
    * \brief a pointer on the tonal part of the signal
    */
    MP_Signal_c *otherSignal;

  protected:
    /**
     * \brief a pointer on an instance of MP_Gui_Callback_Demo_c for the singleton Design pattern
     */
    static  MP_Gui_Callback_Demo_c * guiCallbackDemo;
    
    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  private:
  /** \brief A private constructor */ 
  MP_Gui_Callback_Demo_c();
  
  public:
  /** \brief A public destructor */ 
  virtual ~MP_Gui_Callback_Demo_c();
  
  /** \brief A getter for singleton instance */ 
  static MP_Gui_Callback_Demo_c * get_gui_call_back();
  
  /***************************/
  /* MISC METHODS            */
  /***************************/
    
  public:
    /** \brief Method to play the transient signal */ 
    void playTransientSignal(std::vector<bool> * v, float startTime, float endTime);
    /** \brief Method to play the tonal signal */
    void playOtherSignal(std::vector<bool> * v, float startTime, float endTime);
    /** \brief a method to separate between to size of atoms
    *   \param length : the treshold for separation in samples unit
    */
    void separate(unsigned long int length);

    /** \brief a method to emit an info message by signal
    *   \param message : the text of message in char*
    */
    static void emitInfoMessage(char* message);
    /** \brief a method to emit a error message by signal
    *   \param message : the text of message in char*
    */
    static void emitErrorMessage(char* message);
    /** \brief a method to emit a warning message by signal
    *   \param message : the text of message in char*
    */
    static void emitWarningMessage(char* message);
    
  signals:
    /**  \brief A Qt signal to pass message from core to GUI
     *   \param message the text
     *   */
    void infoMessage(char* message);
    /**  \brief A Qt signal to pass message from core to GUI
    *   \param message the text
    *   */
    void errorMessage(char* message);
    /**  \brief A Qt signal to pass message from core to GUI
    *   \param message the text
    *   */
    void warningMessage(char* message);

  };

#endif /*MP_GUI_CALLBACK_DEMO_H_*/
