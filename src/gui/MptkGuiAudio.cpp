// Implement the MptkGuiAudio class
#include "MptkGuiAudio.h"

static int portAudioCallbackall (   void *inputBuffer, void *outputBuffer,
				    unsigned long framesPerBuffer,
				    PaTimestamp outTime,void *userData);

static int portAudioCallbackSelectedChannel (   void *inputBuffer, void *outputBuffer,
						unsigned long framesPerBuffer,
						PaTimestamp outTime,void *userData);

// Constructor
MptkGuiAudio::MptkGuiAudio(wxWindow * par, MP_Signal_c * sig) 
{
	parent = par;
	signal = sig;
	channel = 0;
	end=signal->numSamples;
	gain=1;
	pauseVar=false;
	stream = NULL;
	initMaxGain(0,end);
}


// the play function that read all the signal
void MptkGuiAudio::play() {
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
void MptkGuiAudio::play(int begin, int end) {
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
void MptkGuiAudio::playSelected(std::vector<bool> * select) {
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
void MptkGuiAudio::playSelected(std::vector<bool> * select, int begin, int end) {
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
void MptkGuiAudio::pause(){
PaError err;
err = Pa_StopStream(stream);
pauseVar=true;
}

void MptkGuiAudio::restart(){
if (pauseVar) {Pa_StartStream(stream);}
}


// the stop function
void MptkGuiAudio::stop() {
PaError err;
err = Pa_StopStream(stream);
err = Pa_CloseStream(stream);
Pa_Terminate();
stream = NULL;
pauseVar=false;
}

void MptkGuiAudio::listenIsFinished()
{
	MptkGuiListenFinishedEvent * evt = 	new MptkGuiListenFinishedEvent();
	parent->ProcessEvent(*evt);
	delete evt;
}

// Function that give the maximum value of the sample between begin and end
double MptkGuiAudio::valSampleMax(int begin, int end){
double max =0;
for (int i=0; i<signal->numChans; i++) {
	for (int j=begin; j<=end; j++) {
		double tmp = std::abs(signal->channel[i][j]);
		if (tmp > max) {max=tmp;}
	} 
}
return max;
}

// PortAudio does not accept value of sample if they are not beween -1 and 1 so the gain is limit
// by the greatest value of sample 
void MptkGuiAudio::initMaxGain(int begin, int end){
maxGain=1/valSampleMax(begin,end);
 if (maxGain<1) gain=maxGain;
}


// Accessor

MP_Signal_c * MptkGuiAudio::getSignal() {return signal;}

int MptkGuiAudio::getChannel() {return channel;}

int MptkGuiAudio::getSample() {return sample;}

void MptkGuiAudio::incSample() {sample++;}

int MptkGuiAudio::getEnd(){return end;}

double MptkGuiAudio::getGain(){return gain;}

void MptkGuiAudio::setGain(double i) {
if (i>maxGain) {gain=maxGain;}
else {gain=i;}
}

double MptkGuiAudio::getMaxGain(){return maxGain;}

std::vector<bool> * MptkGuiAudio::getSelectedChannel(){return selectedChannel;};

PortAudioStream * MptkGuiAudio::getStream() {return stream;}


// Callback fonctions are function that read sample of the signal
// and put them into a buffer of length framesPerBuffer. they return 0
// as long as the the signal is not finish and 1 when it is finish.
// These functions are require by portaudio

static int portAudioCallbackall (   void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             PaTimestamp outTime,void *userData)
{
	MptkGuiAudio * sig=(MptkGuiAudio *) userData;
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
	MptkGuiAudio * sig=(MptkGuiAudio *) userData;
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

