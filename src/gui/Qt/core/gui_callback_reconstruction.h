/******************************************************************************/
/*                                                                            */
/*                         gui_callback_abstract.h                            */
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

/***************************************************/
/*                                                 */
/* DEFINITION OF THE GUI CALLBACK ABSTRACT CLASS   */
/*                                                 */
/***************************************************/

#ifndef GUI_CALLBACK_RECONSTRUCTION_H_
#define GUI_CALLBACK_RECONSTRUCTION_H_

#include "mptk.h"
#include "../core/gui_audio.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

/***********************/
/* CONSTANTS           */
/***********************/

#define NOTHING_OPENED -1
#define SIGNAL_OPENED 0
#define BOOK_OPENED 1
#define SIGNAL_AND_BOOK_OPENED 2

/**
 * \brief MP_Gui_Callback_Abstract_c is an abstract class that provides the link between main_window (graphical side)
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 * \note inherit from QTrhread in order to have threading abilities for decomposition
 */

class MP_Gui_Callback_Reconstruct_c: public QThread
  {
    Q_OBJECT

    /***********/
    /* DATA    */
    /***********/
  protected :
    /**  \brief A pointer on the MP_Gui_Callback_Reconstruct_c instance  */
    static MP_Gui_Callback_Reconstruct_c * guiCallbackRecons;
    /**  \brief A Pointer on MP_Signal_c base signal for approxime the decomposition */
    MP_Signal_c *approximant;
    /**  \brief A Pointer on MP_Gui_Audio class for playing signals */
    MP_Gui_Audio* audio;
    /**  \brief A integer with the open status of the signal */
    int opSig;
    /**  \brief A boolean indicated if the callback is active (for the tab) */
    bool activated;
    /**  \brief A integer with the open status of the book */
    int opBook;
    /**  \brief A QWaitCondition to manage the threading in decomposition */
    QWaitCondition iterateCond;
    /**  \brief A boolean indicated if the decomposition is running or not */
    bool mpRunning;
    /**  \brief A QMutex to protect access to mpRunning */
    QMutex mutex;
    /**  \brief A Pointer on a book */
    MP_Book_c *book;

  public:

    /**  \brief A Pointer on MP_Signal_c base signal for playing original signal */
    MP_Signal_c *reconstruct;
  signals:
    /**  \brief A Qt signal to indicate the status of reconstruction: running or not
     *   \param status A boolean (true if iteration is running, false else) 
     *   */
    void runningReconstruction(bool status);

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  private:
    /** \brief Private constructor  */
    MP_Gui_Callback_Reconstruct_c();

  public:

    /** \brief Public destructor  */
    virtual ~MP_Gui_Callback_Reconstruct_c();
    /** \brief A getter on singleton instance  */
    static MP_Gui_Callback_Reconstruct_c* get_gui_call_back();

    /***************************/
    /* MISC METHODS            */
    /***************************/
  public:
    int openBook(QString fileName);
    /** \brief Method to activate the core */
    void setActivated();
    /** \brief Method to desactivate the core */
    void setDesactivated();
    /** \brief Method to get if the core is activated */
    bool getActivated();
    /** \brief Method to stop the audio stream */
    void stopPortAudioStream();
    /** \brief Method to play the base signal */
    void playReconstructSignal(std::vector<bool> * v, float startTime, float endTime);
    /** \brief Method to play the approximant signal */
    void playApproximantSignal(std::vector<bool> * v, float startTime, float endTime);
    /** \brief Method to play a signal */
    void play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime);
    /** \brief Method to reconstruct */
    void reconstructSignals();
    /** \brief Method to initialise the signal with book parameters */
    int initSignals();
    /** \brief Method run inherited from QThread */
    void run();
    /** \brief Method to save the reconstruct signal
    *  \param fileName: name of the file to save
    */
    void saveReconstruct(QString fileName);
    /** \brief Method to save the approximant signal
    *  \param fileName: name of the file to save
    */
    void saveApproximant(QString fileName);
    /** \brief Method to get the status of the callback
    */
    bool coreInit();
    /** \brief Method to know if a signal is open
     *  \return an int that indicate the state of signal
     */
    int getSignalOpen();
    /** \brief Method to open a signal
     *  \param fileName : name of the signal to open
     */
    int openSignal(QString fileName);
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

#endif /*GUI_CALLBACK_RECONSTRUCT_H_*/

