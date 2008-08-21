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

#ifndef GUI_CALLBACK_ABSTRACT_H_
#define GUI_CALLBACK_ABSTRACT_H_

#include "mptk.h"
#include "../core/gui_audio.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QString>

#include <sstream>
#include <string>

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

class MP_Gui_Callback_Abstract_c: public QThread
  {
    Q_OBJECT


    /***********/
    /* DATA    */
    /***********/
  protected :
    /**  \brief A Pointer on MP_Mpd_Core_c */
    MP_Mpd_Core_c *mpd_Core;
    /**  \brief A Pointer on MP_Mpd_demix_Core_c */
    MP_Mpd_demix_Core_c *mpd_Demix_Core;
    /**  \brief A Pointer on MP_Signal_c base signal for playing original signal */
    MP_Signal_c *baseSignal;
    /**  \brief A Pointer on MP_Gui_Audio class for playing signals */
    MP_Gui_Audio* audio;
    /**  \brief A integer with the open status of the signal */
    int opSig;
    /**  \brief A boolean indicated if the callback is active (for the tab) */
    bool activated;
    /**  \brief A QWaitCondition to manage teh threading in decomposition */
    QWaitCondition iterateCond;
    /**  \brief A boolean indicated if the decomposition is running or not */
    bool mpRunning;
    /**  \brief A QMutex to protect access to mpRunning */
    QMutex mutex;

  public:
    /**  \brief A Pointer on MP_Signal_c base signal for working decomposition */
    MP_Signal_c *signal;

  signals:
    /**  \brief A Qt signal to indicate the status of iteration: running or not
     *   \param status A boolean (true if iteration is running, false else) 
     *   */
    void runningIteration(bool status);
    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief Public constructor  */
    MP_Gui_Callback_Abstract_c()
    {
      MPTK_Env_c::get_env()->load_environment_if_needed(NULL);
      mpd_Core = NULL;
      mpd_Demix_Core = NULL;
      signal = NULL;
      baseSignal = NULL;
      audio = NULL;
      mpRunning = false;
      QThread::start();
      opSig = NOTHING_OPENED;
      activated = false;
    };

    /** \brief Public destructor  */
    virtual ~MP_Gui_Callback_Abstract_c()
    {
      if (mpd_Core) delete mpd_Core;
      if (mpd_Demix_Core) delete mpd_Demix_Core;
      if (signal) delete signal;
      if (baseSignal) delete baseSignal;
      if ( audio) delete audio;
    };
    /***************************/
    /* MISC METHODS            */
    /***************************/

    /** \brief Method to activate the core */
    void setActivated()
    {
      activated =  true;
    }

    /** \brief Method to desactivate the core */
    void setDesactivated()
    {
      activated =  false;
    }

    /** \brief Method to get if the core is activated */
    bool getActivated()
    {
      return activated;
    }

    /** \brief Method to stop the audio stream */
    void stopPortAudioStream()
    {
      if (audio != NULL)
        {
          if (audio->getStream() != NULL) audio->stop();
        }
    }
    /** \brief Method to unset Num Iter */

    void unsetIter()
    {
      if (mpd_Core && getActivated())mpd_Core->reset_iter_condition();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_iter_condition();
    }

    /** \brief Method to unset the SNR */
    void unsetSNR()
    {
      if (mpd_Core && getActivated())mpd_Core->reset_snr_condition();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_snr_condition();
    }

    /** \brief Method to set the SNR */
    void setSNR(double snr)
    {
      if (mpd_Core && getActivated())mpd_Core->set_snr_condition(snr);
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->set_snr_condition(snr);
    }

    /** \brief Method to unset the Verbose mode */
    void unSetVerbose()
    {
      if (mpd_Core && getActivated())mpd_Core->reset_verbose();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_verbose();
    }

    /** \brief Method to set the Verbose mode */
    void setVerbose()
    {
      if (mpd_Core && getActivated())mpd_Core->set_verbose();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->set_verbose();
    }

    /** \brief Method to play the base signal */
    void playBaseSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(baseSignal, v, startTime, endTime);
    }
    /** \brief Method to play the residual signal */
    void playResidualSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(signal, v, startTime, endTime);
    }
    /** \brief Method to play a signal */
    void play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime)
    {

      stopPortAudioStream();
      if (sig != NULL)
        {
          audio = new MP_Gui_Audio(sig);
          if (startTime<endTime)
            {
              int deb = 0;
              int end = (int)(signal->sampleRate);
              audio->playSelected(v, deb, end);
            }
          else audio->playSelected(v);
        }
    }

    /** \brief Method to set the number of max. iterations */
    void setIterationNumber(long int numberIt)
    {
      if (mpd_Core && getActivated())mpd_Core->set_iter_condition(numberIt);
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->set_iter_condition(numberIt);
    }
    /** \brief Method to iterate one time */
    void iterateOnce()
    {
      if (mpd_Core && getActivated() && mpd_Core->can_step()) mpd_Core->step();
      if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())mpd_Demix_Core->step();
    }

    /** \brief Method to iterate */
    void iterateAll()
    {
      if (mpd_Core && getActivated()&& mpd_Core->can_step() ) //
        {
          /* display conditions */
          mpd_Core->info_conditions();
          /* lauch the run method (inherited from QThread) */
          mutex.lock();
          if (!mpRunning)iterateCond.wakeAll();
          mutex.unlock();

        }
      else if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
        {
          /* display conditions */
          mpd_Demix_Core->info_conditions();
          /* lauch the run method (inherited from QThread) */
          mutex.lock();
          if (!mpRunning)iterateCond.wakeAll();
          mutex.unlock();

        }
    }

    /** \brief Method run inherited from QThread */
    void run()
    {


      while (true)
        { /* take mutex and pass the boolean to true */
          mutex.lock();
          /*   if (!mpRunning) error à afficher*/
          /* Wait condition */
          iterateCond.wait(&mutex);
          mpRunning = true;
          mutex.unlock();
          /* Test if can step and the run */
          if (mpd_Core && getActivated() && mpd_Core->can_step())
            { /* emit a signal to indicate that decomposition is started */
              emit runningIteration(true);
              mpd_Core->run();
              /* display results */
              mpd_Core->info_result();
              /* emit a signal to indicate that decomposition is over */
              emit runningIteration(false);
              /* take mutex and pass the boolean to false */
              mutex.lock();
              mpRunning = false;
              mutex.unlock();
            }
          /* Test if can step and the run */
          if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
            { /* emit a signal to indicate that decomposition is started */
              emit runningIteration(true);
              mpd_Demix_Core->run();
              /* display results */
              mpd_Demix_Core->info_result();
              /* emit a signal to indicate that decomposition is over */
              emit runningIteration(false);
              /* take mutex and pass the boolean to false */
              mutex.lock();
              mpRunning = false;
              mutex.unlock();
            }

        }
    }

    /** \brief Method to save the residual signal
    *  \param fileName: name of the file to save
    */
    void saveResidual(QString fileName)
    {
      if (signal) signal->wavwrite(fileName.toAscii().constData());
    }
    /** \brief Method to get the number of iteration done */
    unsigned long int getNumIter(void)
    {
      if (mpd_Core && getActivated()) return mpd_Core->get_num_iter();
      if (mpd_Demix_Core && getActivated())return mpd_Demix_Core->get_num_iter();
    }
    /** \brief Method to save the residual decay data in a text file
    *  \param fileName: name of the text file to save
    */
    void saveDecay(QString fileName)
    {
      if (mpd_Core && getActivated()) mpd_Core->save_decay( fileName.toAscii().constData());
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->save_decay( fileName.toAscii().constData());
    }

    /** \brief Method to open a signal
     *  \param fileName : name of the signal to open
     */
    virtual int openSignal(QString fileName)
    {
      if (signal != NULL) delete signal;
      if (baseSignal != NULL) delete baseSignal;
      signal = MP_Signal_c::init( fileName.toAscii().constData() );
      if (signal != NULL) baseSignal = new MP_Signal_c ( *signal );
      if (signal != NULL)
        {
          opSig = SIGNAL_OPENED;
          return SIGNAL_OPENED;
        }

      else return NOTHING_OPENED;
    }
    /** \brief Method to know if a signal is open
     *  \return an int that indicate the state of signal
     */
    int getSignalOpen()
    {
      return opSig;
    }
    /** \brief Method to return the number of iter set in the core
    *  \return an unsigned long int that indicate the number of iteration
    */
    unsigned long int get_num_iter(void)
    {
      if (mpd_Core && getActivated()) return(mpd_Core->get_num_iter());
      else if (mpd_Demix_Core && getActivated()) return(mpd_Demix_Core->get_num_iter());
      else return 0;
    }

    /** \brief Method to get the sample rate of the signal plugged in the core
      *  \return an int that indicate the state of signal
      */
    int getSignalSampleRate(void)
    {
      if (baseSignal!=NULL) return baseSignal->sampleRate;
      else return 0;
    }

  private slots:
    /** \brief Slot to stop iteration if requested
      */
    void stopIteration()
    {
      if (mpd_Core && getActivated())mpd_Core->force_stop();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->force_stop();
    }
  };

#endif /*GUI_CALLBACK_ABSTRACT_H_*/

