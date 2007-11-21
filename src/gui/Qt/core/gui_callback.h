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
 * \brief MP_Gui_Callback_c is a concrete class that provides the link between main_window (graphical side)
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 * \note inherit from QTrhread in order to have threading abilities for decomposition
 */

class MP_Gui_Callback_c:public MP_Gui_Callback_Abstract_c
  {
    Q_OBJECT
    /***********/
    /* DATA    */
    /***********/
  public:
    /**  \brief A vector containing the filter length size of each blocks in dictionary */
    vector<unsigned long int> * dictFilterLengthsVector;
    /**  \brief A integer with the open status of the book */
    int opBook;

  private :
    /**  \brief A Qstring with the name of the dict to open */
    QString dicoName;
    /**  \brief A pointer on a MP_Gui_Callback_c instance for singleton design pattern */
    static  MP_Gui_Callback_c * guiCallback;

  protected :
    /**  \brief A pointer on a book to store the atoms */
    MP_Book_c *book;
    /**  \brief A Pointer on MP_Signal_c base signal for approxime the decomposition */
    MP_Signal_c *approximant;

    /***********/
    /* METHODS */
    /***********/
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  protected:
    /** \brief Private constructor  */
    MP_Gui_Callback_c();
  public:
    /** \brief Public destructor  */
    virtual ~MP_Gui_Callback_c();
    /** \brief a getter on MP_Gui_Callback_c instance  */
    static MP_Gui_Callback_c * get_gui_call_back();
    /** \brief Method to init the mpd core  */
    int initMpdCore(QString signalName, QString bookName);

    /***************************/
    /* MISC METHODS            */
    /***************************/

    /** \brief Method to open a book
     *  \param fileName name of the book to open
     */
    int openBook(QString fileName);
    /** \brief Method to save a book
    *  \param fileName name of the book to save
    */
    void saveBook(QString fileName);
    /** \brief Method to save the approximant
    *  \param fileName name of the approximant to save
    */
    void saveApproximant(QString fileName);
    /** \brief Method to save the dictionary
     *  \param dictName name of the dictionary to save
     */
    void saveDictionary(QString dictName);
    /** \brief Method to get the signal  */
    MP_Signal_c * getSignal();
    /** \brief Method to get the approximant  */
    MP_Signal_c * getApproximant();
    /** \brief Method to get the residual  */
    MP_Signal_c * getResidual();
    /** \brief Method to get the book  */
    MP_Book_c * getBook();
    /** \brief Method to get the number of channel from signal  */
    int getNumChans();
    /** \brief Method to set the dictionary
     *  \param dictName name of the dictionary to set
     */
    void setDictionary(QString dictName);
    /** \brief Method to init dictionary  */
    void initDictionary();
    /** \brief Method to add a default block to the dictionary
    *  \param blockName name of the dictionary to set
    */
    int addDefaultBlockToDictionary(QString blockName);
    /** \brief Method to set the save setting
    *   \param setSaveHit the set save hit
    *   \param bookFileName name of the book to save
    *   \param resFileName name of the residual file to save
    *   \param decayFileName name of the decay file to save
    * 
    */
    void setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName);
    /** \brief Method to unset the save setting */
    void unsetSave();
    /** \brief Method to set the report hit in term of number of iteration
    *  \param setReportHit number of iter for reporting status
    */
    void setReport(const unsigned long int setReportHit );
    /** \brief Method to substract/add atom from the book to residual */
    void subAddBook();
    /** \brief Method to get the state of initialisation from the core */
    bool coreInit();
    /** \brief Method to get the number of iteration */
    unsigned long int get_num_iter(void);
    /** \brief Method to get the status of the book*/
    int getBookOpen();
    /** \brief Method to get fill a vector containing the filter length of the block of the dictionnary
    *  \param blocksNumber the number of blocks in dictionary
    */
    void getDictFilterlengths(int blocksNumber);
    /** \brief Method to add a custom block in the dictionnary
    *  \param setPropertyMap the parameter map defining the block
    */
    void addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap);

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
    
     /** \brief Method to play the approximant signal */
    void playApproximantSignal(std::vector<bool> * v, float startTime, float endTime);
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

#endif /*GUI_CALLBACK_H_*/
