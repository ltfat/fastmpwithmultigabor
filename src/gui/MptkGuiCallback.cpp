#include "MptkGuiCallback.h"
#include <iostream>


// Creator
MptkGuiCallback::MptkGuiCallback(wxWindow * par)
{
	mpd_Core = NULL;
	audio = NULL;
	parent = par;
	
	set_msg_handler(mp_gui_handler);
	set_error_handler(mp_gui_error_handler);
}

// Destructor
MptkGuiCallback::~MptkGuiCallback()
{
	if (mpd_Core != NULL) delete mpd_Core;
	stopPortAudioStream();	 
  if ( signal ) delete signal;
  if ( baseSignal ) delete baseSignal;
  if ( approximant ) delete approximant;
  if ( book ) delete book;
	
	
}

// Open a signal, returns true if success
int MptkGuiCallback::openSignal(wxString fileName)
{
  signal = MP_Signal_c::init( fileName );
  baseSignal = new MP_Signal_c ( *signal );
  if (signal != NULL) return SIGNAL_OPENED;
  else return NOTHING_OPENED;
 
}

// Open a book, returns true if success (default here)

int MptkGuiCallback::openBook(wxString fileName)
{ FILE* fid = fopen(fileName,"r");
  book = MP_Book_c::create(fid);
  fclose(fid);
  return BOOK_OPENED;
}

// Initialize mpd_Core with given signal name and book name
int MptkGuiCallback::initMpdCore(wxString signalName, wxString bookName){
  int opSig = NOTHING_OPENED;
  int opBook = NOTHING_OPENED;

  if (mpd_Core != NULL) {
    delete mpd_Core;
    mpd_Core = NULL;
  }
  
  if (signalName != _T("")){
    opSig = openSignal(signalName);
  }

  if (bookName != _T("")){
    opBook = openBook(bookName);
  }

  if (opSig == SIGNAL_OPENED && opBook == NOTHING_OPENED){    

    book = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );

  }
  if (opSig == NOTHING_OPENED && opBook == BOOK_OPENED){
    signal = MP_Signal_c::init( book->numChans, book->numSamples, book->sampleRate);
    baseSignal = new MP_Signal_c(*signal);
  }

  if (opSig == SIGNAL_OPENED || opBook == BOOK_OPENED) {
    approximant = MP_Signal_c::init( book->numChans, book->numSamples, book->sampleRate);
    mpd_Core = MP_Mpd_Core_c::create(signal,book,approximant);
    

    if (opSig == SIGNAL_OPENED && opBook == BOOK_OPENED) return SIGNAL_AND_BOOK_OPENED;
    if (opSig == SIGNAL_OPENED && opBook == NOTHING_OPENED) return SIGNAL_OPENED;
    if (opSig == NOTHING_OPENED && opBook == BOOK_OPENED) return BOOK_OPENED;
  }

  return NOTHING_OPENED;
}

// Save book
void MptkGuiCallback::saveBook(wxString fileName)
{
    book->print(fileName, MP_TEXT);
}

// Save approximant
void MptkGuiCallback::saveApproximant(wxString fileName)
{
    approximant->wavwrite(fileName);
}

// Save residual
void MptkGuiCallback::saveResidual(wxString fileName)
{
  signal->wavwrite(fileName);
}

// Returns the base signal
MP_Signal_c * MptkGuiCallback::getSignal()
{
  return baseSignal;
}

// Returns the approximant signal
MP_Signal_c * MptkGuiCallback::getApproximant()
{ 
  return approximant;
}

// Returns the residual signal
MP_Signal_c * MptkGuiCallback::getResidual()
{
  return signal;
}

// Returns the current signal
MP_Book_c * MptkGuiCallback::getBook()
{
  return book;
}

int MptkGuiCallback::getNumChans()
{
	if (mpd_Core != NULL){
		if(signal != NULL){
			return 	signal->numChans;
		}

		else return book->numChans;

	}
	else return 0;
}

// Stop the port audio stream for further play, if it is not stoppped before
void MptkGuiCallback::stopPortAudioStream()
{
	if (audio != NULL) {if (audio->getStream() != NULL) audio->stop();}
}


// Play the signal
void MptkGuiCallback::playBaseSignal(std::vector<bool> * v, float startTime, float endTime)
{
  play(baseSignal, v, startTime, endTime);
}

// Play the rebuilt signal
void MptkGuiCallback::playApproximantSignal(std::vector<bool> * v, float startTime, float endTime)
{
  play(approximant, v, startTime, endTime);
}

// Play the signal
void MptkGuiCallback::playResidualSignal(std::vector<bool> * v, float startTime, float endTime)
{
  play(signal, v, startTime, endTime);
}

// Play the given signal
void MptkGuiCallback:: play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime)
{
        stopPortAudioStream();
	if (sig != NULL){
		audio = new MptkGuiAudio(parent, sig);
		if (startTime<endTime){
		  int deb = (int)(startTime*sig->sampleRate);
		  int end = (int)(endTime*sig->sampleRate);
		  audio->playSelected(v, deb, end);
		}
		else audio->playSelected(v);
	}
}
void MptkGuiCallback::pauseListen()
{
	audio->pause();
}

void MptkGuiCallback::restartListen()
{
	audio->restart();
}

void MptkGuiCallback::stopListen()
{
	stopPortAudioStream();
}

// Set the dictonary
void MptkGuiCallback::setDictionary(wxString fileName)
{ FILE* dicFid;
  if (dicoName!=fileName){

  	MP_Dict_c *oldDict;
  	oldDict = mpd_Core->change_dict(MP_Dict_c::init(fileName));
  	if ( oldDict ) delete( oldDict );

  	dicoName=fileName;
  	}
}

// Unset the number of max. iteration
void MptkGuiCallback::unsetIterationNumber()
{
  mpd_Core->reset_iter_condition();
}

// Set the number of max. iterations
void MptkGuiCallback::setIterationNumber(long int numberIt)
{
	mpd_Core->set_iter_condition(numberIt);	
}

// Unset the SNR
void MptkGuiCallback::unsetSNR()
{
  mpd_Core->reset_snr_condition();
}

// Set the SNR
void MptkGuiCallback::setSNR(double snr)
{
	mpd_Core->set_snr_condition(snr);
}

void MptkGuiCallback::setSave(const unsigned long int setSaveHit,const char* setBookFileName,const  char* setResFileName,const char* setDecayFileName )
{
  mpd_Core->set_save_hit(setSaveHit,setBookFileName,setResFileName,setDecayFileName );
}

void MptkGuiCallback::unsetSave()
{ 
  mpd_Core->reset_save_hit();
}

void MptkGuiCallback::setReport( const unsigned long int setReportHit )
{
  mpd_Core->set_report_hit(setReportHit);
}

void MptkGuiCallback::unsetReport()
{
  mpd_Core->reset_report_hit();
}

void MptkGuiCallback::iterateOnce()
{
  if (mpd_Core->can_step()) mpd_Core->step();
}	

void MptkGuiCallback::iterateAll()
{
  mpd_Core->info_conditions();
  if (mpd_Core->can_step()) {mpd_Core->run();}
}

void MptkGuiCallback::stopIteration()
{
  mpd_Core->force_stop();	
}

void MptkGuiCallback::verbose()
{
  mpd_Core->set_verbose();
}
void MptkGuiCallback:: quiet()
{
 mpd_Core->reset_verbose();
}

void  MptkGuiCallback::normal()
{
 mpd_Core->reset_verbose();
}

void MptkGuiCallback::setAllHandler()
{
set_msg_handler(mp_gui_all_handler);
}

void MptkGuiCallback::setProgressHandler()
{
set_msg_handler(mp_gui_handler);
}

 bool MptkGuiCallback::canIterate()
{
  return mpd_Core->can_step();
}

int MptkGuiCallback::getIterationValue()
{
  return mpd_Core->get_current_stop_after_iter();
}

bool MptkGuiCallback::getIterCheck()
{ 
return mpd_Core->get_use_stop_after_iter_state();
}

float MptkGuiCallback::getSNRValue()
{
  return mpd_Core->get_current_stop_after_snr();
}

bool MptkGuiCallback::getSNRCheck()
{ 
return mpd_Core->get_use_stop_after_snr_state();
}
