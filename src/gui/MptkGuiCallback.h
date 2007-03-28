#ifndef MPTKGUICALLBACK_H
#define MPTKGUICALLBACK_H

#include "mptk.h"
#include "wx/wx.h"
#include "MptkGuiAudio.h"
#include "MptkGuiHandlers.h"

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

class MptkGuiCallback
{
public :

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

  MptkGuiCallback(wxWindow * par);
  ~MptkGuiCallback();

  /***********/
  /* METHODS */
  /***********/

  int openSignal(wxString fileName);
  int openBook(wxString fileName);
  int initMpdCore(wxString signalName, wxString bookName);
  void saveBook(wxString fileName);
  void saveApproximant(wxString fileName);
  void saveResidual(wxString fileName);

  MP_Signal_c * getSignal();
  MP_Signal_c * getApproximant();
  MP_Signal_c * getResidual();
  MP_Book_c * getBook();

  int getNumChans(); 
  
  /**
   *
   */
  void playBaseSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  void playApproximantSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  void playResidualSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  void pauseListen();
  void restartListen();
  void stopListen();
  
  void setDictionary(wxString dicoName);
  void unsetIterationNumber();
  void setIterationNumber(long int numberIt);
  void unsetSNR();
  void setSNR(double snr);
  void setSave(const unsigned long int setSaveHit,const char* bookFileName,const  char* resFileName,const  char* decayFileName);
  void unsetSave();
  void setReport(const unsigned long int setReportHit );
  void unsetReport();
  void iterateOnce();
  void iterateAll();
  void stopIteration();
  void verbose();
  void quiet();
  void normal();
  void setAllHandler();
  void setProgressHandler();
  bool canIterate();
  int getIterationValue();
  bool getIterCheck();
  float getSNRValue();
  bool getSNRCheck();

private :
  MP_Mpd_Core_c * mpd_Core;
  MP_Signal_c *signal;
  MP_Signal_c *baseSignal;
  MP_Signal_c *approximant;
  MP_Book_c *book;

  wxString dicoName;

  wxWindow * parent;
  
  MptkGuiAudio * audio;

  void play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime);
  void stopPortAudioStream();
};

#endif
