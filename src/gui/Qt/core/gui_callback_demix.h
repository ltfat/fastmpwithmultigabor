/******************************************************************************/
/*                                                                            */
/*                          gui_callback_demix.h                              */
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

/**********************************************/
/*                                            */
/* DEFINITION OF THE GUI CALLBACK DEMIX CLASS */
/*                                            */
/**********************************************/

#ifndef GUI_CALLBACK_DEMIX_H_
#define GUI_CALLBACK_DEMIX_H_

#include "gui_callback_abstract.h"

/**
 * \brief MP_Gui_Callback_Demix_c is a concrete class that provides the link between main_window (graphical side)
 * and MP_Mpd_Core_Demix_c (toolkit side, libmptk)
 * \note inherit from QTrhread in order to have threading abilities for decomposition
 */
class MP_Gui_Callback_Demix_c:public MP_Gui_Callback_Abstract_c
  {
    /***********/
    /* DATA    */
    /***********/
  protected:
  /**
   * \brief a vector to stock the book for each sources
   */
    std::vector<MP_Book_c*> *bookArray;
   /**
   * \brief a vector to stock the dictionary for each sources
   */
    std::vector<MP_Dict_c*> *dictArray;
   /**
   * \brief a vector to stock the dictionary for each sources
   */
    std::vector<MP_Signal_c*> *approxArray;
      /**
   * \brief a vector to stock the dictionary for each sources
   */
    std::vector<const char *> bookFileNameArray;
    int opArrayBook;
  public:
    MP_Mixer_c* mixer;

    /***********/
    /* METHODS */
    /***********/
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
     /** \brief a public constructor */
    MP_Gui_Callback_Demix_c();
    /** \brief a public destructor */
    virtual ~MP_Gui_Callback_Demix_c();

    /***************************/
    /* MISC METHODS            */
    /***************************/
  public:
     /** \brief a method to open the Mixer file
      *  \param fileName the name of the mixer
      *  \return a bool to indicate success or not
      *  */
      
    bool openMixer(QString fileName);
      /** \brief a method to add a dictionary to the array 
      *  \param fileName the name of the dictionary
      *  \param index an int to indicate the position of the dictionary
      *  */
    void addDictToArray(QString fileName, int index);
      /** \brief a method to add a dictionary to the array 
      *  \param fileName the name of the dictionary
      *  \param index an int to indicate the position of the dictionary
      *  */
    bool coreInit();
    bool initMpdDemixCore();
    bool setDictArray();
    bool plugApprox();
    int setBookArray();
    int getBookOpen();
    void saveBook(QString fileName);
    void saveApprox(QString fileName);
    void setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName, QString sequenceFileName);
    virtual int openSignal(QString fileName);
  };


#endif /*GUI_CALLBACK_DEMIX_H_*/
