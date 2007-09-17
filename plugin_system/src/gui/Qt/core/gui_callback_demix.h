#ifndef GUI_CALLBACK_DEMIX_H_
#define GUI_CALLBACK_DEMIX_H_

#include "gui_callback_abstract.h"

class MP_Gui_Callback_Demix_c:public MP_Gui_Callback_Abstract_c
{
/***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
public:
  MP_Gui_Callback_Demix_c();
  virtual ~MP_Gui_Callback_Demix_c();
  
protected:  
  std::vector<MP_Book_c*> *bookArray;
  std::vector<MP_Dict_c*> *dictArray;
  std::vector<MP_Signal_c*> *approxArray;
  std::vector<const char *> bookFileNameArray;
  int opArrayBook;
  

  
public:
bool openMixer(QString fileName);
void addDictToArray(QString fileName, int index);
bool coreInit();
bool initMpdDemixCore();
bool setDictArray();
bool plugApprox();
int setBookArray();
int getBookOpen();
void saveBook(QString fileName);
void saveApprox(QString fileName);
void setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName, QString sequenceFileName);
// Open a signal, returns true if success
virtual int openSignal(QString fileName);

//int initMpdCore(QString signalName, QString bookName);

MP_Mixer_c* mixer;


};


#endif /*GUI_CALLBACK_DEMIX_H_*/
