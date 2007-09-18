#ifndef GUI_CALLBACK_H_
#define GUI_CALLBACK_H_

#include "gui_callback_abstract.h"

/***********************/
/* CONSTANTS           */
/***********************/



/**
 * \brief MptkGuiCallback provides the link between MptkGuiFrame (graphical side) 
 * and MP_Mpd_Core_c (toolkit side, libmptk)
 */

class MP_Gui_Callback_c:public MP_Gui_Callback_Abstract_c
{
/***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
public:
  MP_Gui_Callback_c();
  virtual ~MP_Gui_Callback_c();

  /***********/
  /* METHODS */
  /***********/

  //int openSignal(QString fileName);
  int openBook(QString fileName);
  int initMpdCore(QString signalName, QString bookName);
  void saveBook(QString fileName);
  void saveApproximant(QString fileName);
  //void saveResidual(QString fileName);

  MP_Signal_c * getSignal();
  MP_Signal_c * getApproximant();
  MP_Signal_c * getResidual();
  MP_Book_c * getBook();

  int getNumChans(); 
  int opBook;
  
  /**
   *
   */
  //void playBaseSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  //void playApproximantSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  //void playResidualSignal(std::vector<bool> * v, float startTime = -1, float endTime = -1);
  void pauseListen();
  void restartListen();
  void stopListen();
  
  void setDictionary(QString dicoName);
  //void unsetIterationNumber();
//  void setIterationNumber(long int numberIt);
  //void unsetSNR();
  //void setSNR(double snr);
  void setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName);
  void unsetSave();
  void setReport(const unsigned long int setReportHit );
  //void saveDecay(QString fileName);
  void unsetReport();
  //void iterateOnce();
  //void iterateAll();
  //void stopIteration();
  void verbose();
  void quiet();
  void normal();
  void setAllHandler();
  void setProgressHandler();
  bool canIterate();
  int getIterationValue();
  int subAddBook();
  bool getIterCheck();
  bool coreInit();
  float getSNRValue();
  bool getSNRCheck();
  unsigned long int get_num_iter(void);
  int getBookOpen();
  
private :
  QString dicoName;
  
protected :
  MP_Book_c *book;

  void play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime);


};

#endif /*GUI_CALLBACK_H_*/
