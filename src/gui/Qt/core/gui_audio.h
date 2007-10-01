/******************************************************************************/
/*                                                                            */
/*                              gui_audio.h                                   */
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

/*****************************************/
/*                                       */
/* DEFINITION OF THE GUI AUDIO H CLASS   */
/*                                       */
/*****************************************/
#ifndef GUI_AUDIO_H_
#define GUI_AUDIO_H_

#include "mptk.h"
#include "portaudio.h"
#include <cmath>
#include <vector>

#define FRAMES_PER_BUFFER  (64)
/**
 * \brief
 * MP_Gui_Audio reads MP_Signal_c and gives function to listen this MP_Signal_c
 * MP_Gui_Audio uses portaudio library that is include in portaudio_v18_1 directory
 */
class MP_Gui_Audio
  {

    /********/
    /* DATA */
    /********/

  private :
    /**  \brief pointer to MP_Signal_c object */
    MP_Signal_c * signal;

    /**  \brief Channel that will be read */
    int channel;

    /**  \brief Next sample to read when you use play */
    int sample;

    /**  \brief The last sample to read */
    int end;

    /**  \brief The gain when reading the signal */
    double gain;

    /**  \brief The maximum gain before reaching the limit of PortAudio */
    double maxGain;

    /**  \brief the state variable to know if we are at pause state or not */
    bool pauseVar;

    /**  \brief bit vector of selected channel */
    std::vector<bool> * selectedChannel;

    /**  \brief Stream that is use by portaudio */
    PortAudioStream * stream;


  public :

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
    /**  \brief Constructor */
    MP_Gui_Audio(MP_Signal_c * si);
    /**  \brief Destructor */
    virtual ~MP_Gui_Audio();

    /***************************/
    /* MISC METHODS            */
    /***************************/

    /**  \brief Play all the signal */
    void play();

    /**  \brief Play channel ch of the signal */
    void play(int ch);

    /**  \brief Play all the signal between begin and end */
    void play (int begin, int end);

    /**  \brief Play channel ch of the signal between begin and end */
    void play (int ch, int begin, int end);

    /**  \brief Play the channel in listchannel */
    void playSelected(std::vector<bool> * select);

    /**  \brief Play the channel in listchannel between begin and end */
    void playSelected(std::vector<bool> * select, int begin, int end);

    /**  \brief Pause the played sound */
    void pause();

    /**  \brief Restart the pause sound */
    void restart();

    /**  \brief Stop the played sound */
    void stop();

    /**  \brief To notify that the signal listen is finished */
    void listenIsFinished();

    /**  \brief The greatest value of signal between begin and end */
    double valSampleMax(int begin, int end);

    /**  \brief Function that initialize maxGain */
    void initMaxGain(int begin, int end);

    /**  \brief Access to signal*/
    MP_Signal_c * getSignal();

    /**  \brief Access to channel*/
    int getChannel();

    /**  \brief Access to sample*/
    int getSample();

    /**  \brief increment sample*/
    void incSample();

    /**  \brief Access to end*/
    int getEnd();

    /**  \brief Access to gain*/
    double getGain();

    /**  \brief Set gain to i*/
    void setGain(double i);

    /**  \brief Access to maxGain*/
    double getMaxGain();

    /**  \brief Access to selectedChannel*/
    std::vector<bool> * getSelectedChannel();

    /**  \brief Access to stream*/
    PortAudioStream * getStream();

  };

#endif /*GUI_AUDIO_H_*/
