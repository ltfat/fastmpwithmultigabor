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
 * \brief
 * MptkGuiAudio reads MP_Signal_c and gives function to listen this MP_Signal_c
 * MptkGuiAudio uses portaudio library that is include in portaudio_v18_1 directory
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
    MP_Gui_Callback_Demix_c();
    virtual ~MP_Gui_Callback_Demix_c();

    /***************************/
    /* MISC METHODS            */
    /***************************/
  public:
    bool openMixer(QString fileName);
    void addDictToArray(QString fileName, int index);
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
