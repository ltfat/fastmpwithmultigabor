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
 */

class MP_Gui_Callback_Abstract_c: public QThread
  {
    Q_OBJECT


    /***********/
    /* DATA    */
    /***********/
  protected :
    MP_Mpd_Core_c *mpd_Core;
    MP_Mpd_demix_Core_c *mpd_Demix_Core;
    MP_Signal_c *baseSignal;
    MP_Signal_c *approximant;
    QString dicoName;
    MP_Gui_Audio* audio;
    int opSig;
    bool activated;

  public:
    MP_Signal_c *signal;


    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief Public Constructor  */
    MP_Gui_Callback_Abstract_c()
    {
      if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
      mpd_Core = NULL;
      mpd_Demix_Core = NULL;
      signal = NULL;
      baseSignal = NULL;
      audio = NULL;
      approximant = NULL;
      QThread::start(LowestPriority);
      QThread::wait ( ULONG_MAX );
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
      if ( approximant) delete approximant;
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
  /** \brief Method to play the approximant signal */
    void playApproximantSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(approximant, v, startTime, endTime);
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

    void iterateOnce()
    {
      if (mpd_Core && getActivated() && mpd_Core->can_step()) mpd_Core->step();
      if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())mpd_Demix_Core->step();
    }
    
   /** \brief Method to iterate */
    void iterateAll()
    {
      if (mpd_Core && getActivated() && mpd_Core->can_step())
        {
          /* display conditions */
          mpd_Core->info_conditions();
          /* lauch the run method (inherited from QThread) */
          run();
          /* say the QThread to wait */
          QThread::wait ( ULONG_MAX );
          /* display results */
          mpd_Core->info_result();
        }
      else if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
        {
          /* display conditions */
          mpd_Demix_Core->info_conditions();
          /* lauch the run method (inherited from QThread) */
          run();
          /* say the QThread to wait */
          QThread::wait ( ULONG_MAX );
          /* display results */
          mpd_Demix_Core->info_result();
        }
    }
    
   /** \brief Method run inherited from QThread */
    void run()
    {
      /* Test if can step and the run */	
      if (mpd_Core && getActivated() && mpd_Core->can_step())
        {
          mpd_Core->run();
        }
      /* Test if can step and the run */
      if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
        {
          mpd_Demix_Core->run();
        }
    }

    /** \brief Method ti save the residual signal
    *  \param fileName: name of the panel to display
    */
    void saveResidual(QString fileName)
    {
      if (signal) signal->wavwrite(fileName.toStdString().c_str());
    }

    void saveDecay(QString fileName)
    {
      if (mpd_Core && getActivated()) mpd_Core->save_decay( fileName.toStdString().c_str());
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->save_decay( fileName.toStdString().c_str());
    }

    /** \brief Method to open a signal
        *   \param fileName : name of the signal to open
        * */
    virtual int openSignal(QString fileName)
    {
      if (signal != NULL) delete signal;
      if (baseSignal != NULL) delete baseSignal;
      signal = MP_Signal_c::init( fileName.toStdString().c_str() );
      if (signal != NULL) baseSignal = new MP_Signal_c ( *signal );
      if (signal != NULL)
        {
          opSig = SIGNAL_OPENED;
          return SIGNAL_OPENED;
        }

      else return NOTHING_OPENED;
    }

    int getSignalOpen()
    {
      return opSig;
    }

    unsigned long int get_num_iter(void)
    {
      if (mpd_Core && getActivated()) return(mpd_Core->get_num_iter());
      else if (mpd_Demix_Core && getActivated()) return(mpd_Demix_Core->get_num_iter());
      else return 0;
    }

    int getSignalSampleRate(void)
    {
      if (baseSignal!=NULL) return baseSignal->sampleRate;
      else return 0;
    }

  private slots:
    void stopIteration()
    {
      if (mpd_Core && getActivated())mpd_Core->force_stop();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->force_stop();
    }

  };

#endif /*GUI_CALLBACK_ABSTRACT_H_*/

