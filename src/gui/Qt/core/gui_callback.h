/******************************************************************************/
/*                                                                            */
/*                            gui_callback.h                                  */
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

/****************************************/
/*                                      */
/* DEFINITION OF THE GUI CALLBACK CLASS */
/*                                      */
/****************************************/

#ifndef GUI_CALLBACK_H_
#define GUI_CALLBACK_H_

#include "gui_callback_abstract.h"
#include "tinyxml.h"

/***********************/
/* CONSTANTS           */
/***********************/



/**
 * \brief MptkGuiCallback provides the link between MptkGuiFrame (graphical side)
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 */

class MP_Gui_Callback_c:public MP_Gui_Callback_Abstract_c
  {
  	/***********/
    /* DATA    */
    /***********/
    
    /**  \brief A integer with the open status of the book */
    int opBook;
     /**  \brief A Qstring with the name of the dict to open */
    private :
    QString dicoName;
    /**  \brief A pointer on a book to store the atoms */
    protected :
    MP_Book_c *book;
    
    
    /***********/
    /* METHODS */
    /***********/
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
    
  public:
    /** \brief Public constructor  */
    MP_Gui_Callback_c();
    /** \brief Public destructor  */
    virtual ~MP_Gui_Callback_c();

    /***************************/
    /* MISC METHODS            */
    /***************************/

    int openBook(QString fileName);
    int initMpdCore(QString signalName, QString bookName);
    void saveBook(QString fileName);
    void saveApproximant(QString fileName);


    MP_Signal_c * getSignal();
    MP_Signal_c * getApproximant();
    MP_Signal_c * getResidual();
    MP_Book_c * getBook();

    int getNumChans();
    void pauseListen();
    void restartListen();
    void stopListen();
    void setDictionary(QString dicoName);
    void initDictionary();
    int addDefaultBlockToDictionary(QString blockName);
    void setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName);
    void unsetSave();
    void setReport(const unsigned long int setReportHit );
    void unsetReport();
    void verbose();
    void quiet();
    void normal();
    void setAllHandler();
    void setProgressHandler();
    bool canIterate();
    int getIterationValue();
    int subAddBook();
    bool getIterCheck();
    bool coreInit();
    float getSNRValue();
    bool getSNRCheck();
    unsigned long int get_num_iter(void);
    int getBookOpen();
    void addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap);
    bool saveDictionary(QString dictName);
    void play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime);


  };

#endif /*GUI_CALLBACK_H_*/
