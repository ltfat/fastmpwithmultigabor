/******************************************************************************/
/*                                                                            */
/*                            gui_audio.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
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

/**********************************************************/
/*                                                        */
/* gui_audio.cpp : methods for class MainWindow           */
/*                                                        */
/**********************************************************/

// Implement the MptkGuiAudio class
#include "gui_audio.h"


static int portAudioCallbackall (   void *inputBuffer, void *outputBuffer,
				    unsigned long framesPerBuffer,
				    PaTimestamp outTime,void *userData);

static int portAudioCallbackSelectedChannel (   void *inputBuffer, void *outputBuffer,
						unsigned long framesPerBuffer,
						PaTimestamp outTime,void *userData);

// Constructor
MP_Gui_Audio::MP_Gui_Audio(MP_Signal_c * sig) 
{
	signal = sig;
	channel = 0;
	end=sig->numSamples;
	gain=1;
	pauseVar=false;
	stream = NULL;
	initMaxGain(0,end);
}

MP_Gui_Audio::~MP_Gui_Audio(){

}
// the play function that read all the signal
void MP_Gui_Audio::play() {
// Initialize the variable before reading
PaError err;
sample=0;
end=signal->numSamples;
if (stream != NULL) {stop();}

// PortAudio function call for the reading
err = Pa_Initialize();
err = Pa_OpenStream(
              &stream,                       // Stream to initialize
              paNoDevice,                    // default input device 
              0,                             // no input 
              paFloat32,                     // 32 bit floating point input
              NULL,
              Pa_GetDefaultOutputDeviceID(), // id of the output device, portaudio will find it for us
              signal->numChans,              
              paFloat32,                     // 32 bit floating point output 
              NULL,
              signal->sampleRate,            // Sample rate of the signal in hertz
              FRAMES_PER_BUFFER,             // lenght of the buffer
              0,                             // number of buffers, if zero then use default minimum 
              paClipOff,                     // we won't output out of range samples so don't bother clipping them 
              portAudioCallbackall,              // function that will be call by portaudio
              this);                         // object that will be read, here it's this
err=Pa_StartStream(stream);
}

// the play function that read all the signal
void MP_Gui_Audio::play(int begin, int end) {
// Initialize the variable before reading
PaError err;
sample=begin;
end=end;
if (stream != NULL) {stop();}

// PortAudio function call for the reading
err = Pa_Initialize();
err = Pa_OpenStream(
              &stream,                       // Stream to initialize
              paNoDevice,                    // default input device 
              0,                             // no input 
              paFloat32,                     // 32 bit floating point input
              NULL,
              Pa_GetDefaultOutputDeviceID(), // id of the output device, portaudio will find it for us
              signal->numChans,              
              paFloat32,                     // 32 bit floating point output 
              NULL,
              signal->sampleRate,            // Sample rate of the signal in hertz
              FRAMES_PER_BUFFER,             // lenght of the buffer
              0,                             // number of buffers, if zero then use default minimum 
              paClipOff,                     // we won't output out of range samples so don't bother clipping them 
              portAudioCallbackall,          // function that will be call by portaudio
              this);                         // object that will be read, here it's this
err=Pa_StartStream(stream);
}


// Play the channel in listchannel
void MP_Gui_Audio::playSelected(std::vector<bool> * select) {
// Initialize the variable before reading
PaError err;
selectedChannel=select;
sample=0;
end=signal->numSamples;
if (stream != NULL) {stop();}

// PortAudio function call for the reading
err = Pa_Initialize();
err = Pa_OpenStream(
              &stream,                       // Stream to initialize
              paNoDevice,                    // default input device 
              0,                             // no input 
              paFloat32,                     // 32 bit floating point input
              NULL,
              Pa_GetDefaultOutputDeviceID(), // id of the output device, portaudio will find it for us
              selectedChannel->size(),
              paFloat32,                     // 32 bit floating point output 
              NULL,
              signal->sampleRate,            // Sample rate of the signal in hertz
              FRAMES_PER_BUFFER,             // lenght of the buffer
              0,                             // number of buffers, if zero then use default minimum 
              paClipOff,                     // we won't output out of range samples so don't bother clipping them 
              portAudioCallbackSelectedChannel,   // function that will be call by portaudio
              this);                         // object that will be read, here it's this
err=Pa_StartStream(stream);
}


// Play the channel in listchannel between begin and end
void MP_Gui_Audio::playSelected(std::vector<bool> * select, int begin, int end) {
// Initialize the variable before reading
PaError err;
selectedChannel=select;
this->sample=begin;
this->end=end;
if (stream != NULL) {stop();}

// PortAudio function call for the reading
err = Pa_Initialize();
err = Pa_OpenStream(
              &stream,                       // Stream to initialize
              paNoDevice,                    // default input device 
              0,                             // no input 
              paFloat32,                     // 32 bit floating point input
              NULL,
              Pa_GetDefaultOutputDeviceID(), // id of the output device, portaudio will find it for us
              selectedChannel->size(),              
              paFloat32,                     // 32 bit floating point output 
              NULL,
              signal->sampleRate,            // Sample rate of the signal in hertz
              FRAMES_PER_BUFFER,             // lenght of the buffer
              0,                             // number of buffers, if zero then use default minimum 
              paClipOff,                     // we won't output out of range samples so don't bother clipping them 
              portAudioCallbackSelectedChannel,              // function that will be call by portaudio
              this);                         // object that will be read, here it's this
err=Pa_StartStream(stream);
}


// the pause function
void MP_Gui_Audio::pause(){
PaError err;
err = Pa_StopStream(stream);
pauseVar=true;
}

void MP_Gui_Audio::restart(){
if (pauseVar) {Pa_StartStream(stream);}
}


// the stop function
void MP_Gui_Audio::stop() {
PaError err;
err = Pa_StopStream(stream);
err = Pa_CloseStream(stream);
Pa_Terminate();
stream = NULL;
pauseVar=false;
}

void MP_Gui_Audio::listenIsFinished()
{
	//MptkGuiListenFinishedEvent * evt = 	new MptkGuiListenFinishedEvent();
	//parent->ProcessEvent(*evt);
	///delete evt;
}

// Function that give the maximum value of the sample between begin and end
double MP_Gui_Audio::valSampleMax(int begin, int end){
double max =0;
for (int i=0; i<this->signal->numChans; i++) {
	for (int j=begin; j<=end; j++) {
		double tmp = std::abs(this->signal->channel[i][j]);
		if (tmp > max) {max=tmp;}
	} 
}
return max;
}

// PortAudio does not accept value of sample if they are not beween -1 and 1 so the gain is limit
// by the greatest value of sample 
void MP_Gui_Audio::initMaxGain(int begin, int end){
maxGain=1/valSampleMax(begin,end);
 if (maxGain<1) gain=maxGain;
}


// Accessor

MP_Signal_c * MP_Gui_Audio::getSignal() {return signal;}

int MP_Gui_Audio::getChannel() {return channel;}

int MP_Gui_Audio::getSample() {return sample;}

void MP_Gui_Audio::incSample() {sample++;}

int MP_Gui_Audio::getEnd(){return end;}

double MP_Gui_Audio::getGain(){return gain;}

void MP_Gui_Audio::setGain(double i) {
if (i>maxGain) {gain=maxGain;}
else {gain=i;}
}

double MP_Gui_Audio::getMaxGain(){return maxGain;}

std::vector<bool> * MP_Gui_Audio::getSelectedChannel(){return selectedChannel;};

PortAudioStream * MP_Gui_Audio::getStream() {return stream;}


// Callback fonctions are function that read sample of the signal
// and put them into a buffer of length framesPerBuffer. they return 0
// as long as the the signal is not finish and 1 when it is finish.
// These functions are require by portaudio

static int portAudioCallbackall (   void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             PaTimestamp outTime,void *userData)
{
	MP_Gui_Audio * sig=(MP_Gui_Audio *) userData;
	float *out = (float*)outputBuffer;
	(void) outTime;
	(void) inputBuffer;
	int fini=0;
	for (unsigned long i=0; i<framesPerBuffer; i++)
	{
		for (int j=0;j<sig->getSignal()->numChans;j++)
		{
		*out++=sig->getSignal()->channel[j][sig->getSample()]*sig->getGain();
		}
		sig->incSample();
		if (sig->getSample()>sig->getEnd())
		{
			fini=1;
			break;
		}
	}
	return fini;
}	

static int portAudioCallbackSelectedChannel (   void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             PaTimestamp outTime,void *userData)
{
	MP_Gui_Audio * sig=(MP_Gui_Audio *) userData;
	std::vector<bool> * selectedChannel=sig->getSelectedChannel();
	float *out = (float*)outputBuffer;
	(void) outTime;
	(void) inputBuffer;
	int fini=0;
	for (unsigned long i=0; i<framesPerBuffer; i++)
	{	
		int k=0;
		for (std::vector<bool>::iterator j=selectedChannel->begin(); j!=selectedChannel->end();j++)
		{
		if (*j) {*out++=sig->getSignal()->channel[k][sig->getSample()]*sig->getGain();}
		else {*out++=0;}
		}
		sig->incSample();
		if (sig->getSample()>sig->getEnd())
		{
			fini=1;
			break;
		}
		k++;
	}
	if (fini) {sig->listenIsFinished();}
	return fini;
}	

