#ifndef MPTKGUIAUDIO_H
#define MPTKGUIAUDIO_H

#include "mptk.h"
#include "portaudio.h"
#include "MptkGuiListenFinishedEvent.h"
#include "wx/wx.h"
#include <cmath>
#include <vector>

#define FRAMES_PER_BUFFER  (64)
/** 
 * \brief
 * MptkGuiAudio reads MP_Signal_c and gives function to listen this MP_Signal_c
 * MptkGuiAudio uses portaudio library that is include in portaudio_v18_1 directory
 */
class MptkGuiAudio
{

private :
// pointer to MP_Signal_c object
MP_Signal_c * signal;

// Channel that will be read
int channel;

// Next sample to read when you use play
int sample;

// The last sample to read
int end;

// The gain when reading the signal
double gain;

// The maximum gain before reaching the limit of PortAudio
double maxGain;

// the state variable to know if we are at pause state or not
bool pauseVar;

// bit vector of selected channel
std::vector<bool> * selectedChannel;

// Stream that is use by portaudio
PortAudioStream * stream;

// the parent in the gui
wxWindow * parent;

public :

// Constructor
MptkGuiAudio(wxWindow * parent, MP_Signal_c * si);

// Play all the signal
void play();

// Play channel ch of the signal
void play(int ch);

// Play all the signal between begin and end
void play (int begin, int end);

// Play channel ch of the signal between begin and end
void play (int ch, int begin, int end);

// Play the channel in listchannel
void playSelected(std::vector<bool> * select);

// Play the channel in listchannel between begin and end
void playSelected(std::vector<bool> * select, int begin, int end);

// Pause the played sound
void pause();

// Restart the pause sound
void restart();

// Stop the played sound
void stop();

// To notify that the signal listen is finished
void listenIsFinished();

// The greatest value of signal between begin and end
double valSampleMax(int begin, int end);

// Function that initialize maxGain
void initMaxGain(int begin, int end);

// Access to signal
MP_Signal_c * getSignal();

// Access to channel
int getChannel();

// Access to sample
int getSample();

// increment sample
void incSample();

// Access to end
int getEnd();

//Access to gain
double getGain();

// Set gain to i
void setGain(double i);

// Access to maxGain
double getMaxGain();

// Access to selectedChannel
std::vector<bool> * getSelectedChannel();

// Access to stream
PortAudioStream * getStream();

};

#endif
