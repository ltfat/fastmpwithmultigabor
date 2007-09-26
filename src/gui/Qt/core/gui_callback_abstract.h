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
 * \brief MptkGuiCallback provides the link between MptkGuiFrame (graphical side)
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 */

class MP_Gui_Callback_Abstract_c: public QThread
  {
    Q_OBJECT
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
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
    virtual ~MP_Gui_Callback_Abstract_c()
    {
      if (mpd_Core) delete mpd_Core;
      if (mpd_Demix_Core) delete mpd_Demix_Core;
      if (signal) delete signal;
      if (baseSignal) delete baseSignal;
      if ( approximant) delete approximant;
      if ( audio) delete audio;



    };

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
    void setActivated()
    {
      activated =  true;
    }
    void setDesactivated()
    {
      activated =  false;
    }

    bool getActivated()
    {
      return activated;
    }


    void stopPortAudioStream()
    {
      if (audio != NULL)
        {
          if (audio->getStream() != NULL) audio->stop();
        }
    }
//Unset Num Iter
void unsetIter()
   {
      if (mpd_Core && getActivated())mpd_Core->reset_iter_condition();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_iter_condition();
    }

// Unset the SNR
    void unsetSNR()
    {
      if (mpd_Core && getActivated())mpd_Core->reset_snr_condition();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_snr_condition();
    }

// Set the SNR
    void setSNR(double snr)
    {
      if (mpd_Core && getActivated())mpd_Core->set_snr_condition(snr);
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->set_snr_condition(snr);
    }

// unSet the Verbose mode
    void unSetVerbose()
    {
      if (mpd_Core && getActivated())mpd_Core->reset_verbose();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->reset_verbose();
    }

// Set the Verbose mode
    void setVerbose()
    {
      if (mpd_Core && getActivated())mpd_Core->set_verbose();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->set_verbose();
    }
// Play the signal
    void playBaseSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(baseSignal, v, startTime, endTime);
    }

    void playApproximantSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(approximant, v, startTime, endTime);
    }

// Play the signal
    void playResidualSignal(std::vector<bool> * v, float startTime, float endTime)
    {
      play(signal, v, startTime, endTime);
    }

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

// Set the number of max. iterations
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

    void iterateAll()
    {
      if (mpd_Core && getActivated() && mpd_Core->can_step())
        {
          mpd_Core->info_conditions();
          run();
          QThread::wait ( ULONG_MAX );
          mpd_Core->info_result();
        }
      else if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
        {
          mpd_Demix_Core->info_conditions();
          run();
          QThread::wait ( ULONG_MAX );
          mpd_Demix_Core->info_result();
        }
    }

    void run()
    {
      if (mpd_Core && getActivated() && mpd_Core->can_step())
        {
          mpd_Core->run();
        }

      if (mpd_Demix_Core && getActivated() && mpd_Demix_Core->can_step())
        {
          mpd_Demix_Core->run();
        }
    }



    void saveResidual(QString fileName)
    {
      if (signal) signal->wavwrite(fileName.toStdString().c_str());
    }

    void saveDecay(QString fileName)
    {
      if (mpd_Core && getActivated()) mpd_Core->save_decay( fileName.toStdString().c_str());
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->save_decay( fileName.toStdString().c_str());
    }

// Open a signal, returns true if success
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
    
int getSignalSampleRate(void){
	return baseSignal->sampleRate;
}

private slots:
    void stopIteration()
    {
      if (mpd_Core && getActivated())mpd_Core->force_stop();
      if (mpd_Demix_Core && getActivated())mpd_Demix_Core->force_stop();
    }



  };






#endif /*GUI_CALLBACK_ABSTRACT_H_*/
