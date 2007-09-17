#include "gui_callback.h"

MP_Gui_Callback_c::MP_Gui_Callback_c()
:MP_Gui_Callback_Abstract_c()
{
  approximant = NULL;
  book = NULL;
  opBook = NOTHING_OPENED;
}

MP_Gui_Callback_c::~MP_Gui_Callback_c()
{

}

// Open a book, returns true if success (default here)

int MP_Gui_Callback_c::openBook(QString fileName)
{
  if (book) delete book;
  FILE* fid = fopen(fileName.toStdString().c_str(),"rb");
  book = MP_Book_c::create(fid);
  fclose(fid);
  opBook = BOOK_OPENED;
  return BOOK_OPENED;
}

int MP_Gui_Callback_c::subAddBook(){
	
book->substract_add( signal, approximant, NULL );

}
// Initialize mpd_Core with given signal name and book name
int MP_Gui_Callback_c::initMpdCore(QString signalName, QString bookName)
{
  if (mpd_Core != NULL)
    {
      delete mpd_Core;
      mpd_Core = NULL;
    }

  if (signalName.size()>0)
    {
      opSig = openSignal(signalName);
    }

  if (bookName.size()>0)
    {
      opBook = openBook(bookName);
    }

  if (opSig == SIGNAL_OPENED && opBook == NOTHING_OPENED)
    {

      book = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );

    }
  if (opSig == NOTHING_OPENED && opBook == BOOK_OPENED)
    {
      signal = MP_Signal_c::init( book->numChans, book->numSamples, book->sampleRate);
      baseSignal = new MP_Signal_c(*signal);
    }

  if (opSig == SIGNAL_OPENED || opBook == BOOK_OPENED)
    {
      approximant = MP_Signal_c::init( book->numChans, book->numSamples, book->sampleRate);
      mpd_Core = MP_Mpd_Core_c::create(signal,book,approximant);


      if (opSig == SIGNAL_OPENED && opBook == BOOK_OPENED) return SIGNAL_AND_BOOK_OPENED;
      if (opSig == SIGNAL_OPENED && opBook == NOTHING_OPENED) return SIGNAL_OPENED;
      if (opSig == NOTHING_OPENED && opBook == BOOK_OPENED) return BOOK_OPENED;
    }

  return NOTHING_OPENED;
}

void MP_Gui_Callback_c::setDictionary(QString fileName)
{
  if (dicoName!=fileName)
    {

      MP_Dict_c *oldDict = NULL;
      if (MP_Dict_c::init(fileName.toStdString().c_str())&& (mpd_Core !=NULL)) oldDict = mpd_Core->change_dict(MP_Dict_c::init(fileName.toStdString().c_str()));
      if ( oldDict ) delete( oldDict );

      dicoName=fileName;
    }
}


void MP_Gui_Callback_c::setSave(const unsigned long int setSaveHit,QString setBookFileName,QString setResFileName,QString setDecayFileName )
{
  mpd_Core->set_save_hit(setSaveHit,setBookFileName.toStdString().c_str(),setResFileName.toStdString().c_str(),setDecayFileName.toStdString().c_str());
}



// Save book
void MP_Gui_Callback_c::saveBook(QString fileName)
{
  if (book) book->print(fileName.toStdString().c_str(), MP_TEXT);
}


// Save approximant
void MP_Gui_Callback_c::saveApproximant(QString fileName)
{
  if (approximant)approximant->wavwrite(fileName.toStdString().c_str());
}

bool MP_Gui_Callback_c::coreInit()
{
  if (mpd_Core) return true;
  else return false;
}

int MP_Gui_Callback_c::getBookOpen(){
return opBook;
}
