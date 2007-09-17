#include "gui_callback_demix.h"
#include <sstream>
#include <string>

MP_Gui_Callback_Demix_c::MP_Gui_Callback_Demix_c():
    MP_Gui_Callback_Abstract_c()
{
  mixer = NULL;
}

MP_Gui_Callback_Demix_c::~MP_Gui_Callback_Demix_c()
{}

bool MP_Gui_Callback_Demix_c::openMixer(QString fileName)
{
  string suffix;
  const char* suffixeConstChar;
  FILE * mixerFile = fopen (fileName.toStdString().c_str(),"rt");
  istringstream iss( fileName.toStdString().c_str() );
  std::getline( iss , suffix , '.' );
  std::getline( iss , suffix , '.' );
  suffixeConstChar = suffix.c_str();
  if ( !strcmp( suffixeConstChar ,"txt" ) )
    {
      if (mixerFile)
        mixer = MP_Mixer_c::creator_from_txt_file(mixerFile);
    }
  if (mixer == NULL) return false;
  else
    {
      dictArray = new  std::vector<MP_Dict_c*>(mixer->numSources);
      return true;
    }
}

void MP_Gui_Callback_Demix_c::addDictToArray(QString fileName, int index)
{
  MP_Dict_c* dict = MP_Dict_c::init( fileName.toStdString().c_str());
  dictArray->at(index) = dict;
}

bool MP_Gui_Callback_Demix_c::setDictArray()
{
  if (mpd_Demix_Core)
    {
      mpd_Demix_Core->change_dict( dictArray );
      return true;
    }
  else return false;
}

int MP_Gui_Callback_Demix_c::setBookArray()
{
  if (signal && mixer)
    {
      bookArray = new  std::vector<MP_Book_c*>(mixer->numSources);
      for (unsigned int j =0; j <mixer->numSources; j++) bookArray->at(j) = MP_Book_c::create(1, signal->numSamples, signal->sampleRate );
      opArrayBook = BOOK_OPENED;
      return BOOK_OPENED;
    }
  else return NOTHING_OPENED;
}
bool MP_Gui_Callback_Demix_c::coreInit()
{
  if (mpd_Demix_Core) return true;
  else return false;
}

int MP_Gui_Callback_Demix_c::openSignal(QString fileName)
{
  if (MP_Gui_Callback_Abstract_c::openSignal(fileName)== SIGNAL_OPENED)
    {
      approxArray = new std::vector<MP_Signal_c*>(mixer->numSources);
      for (unsigned int j =0; j <mixer->numSources; j++) approxArray->at(j) = MP_Signal_c::init( signal->numChans, signal->numSamples, signal->sampleRate);

      return SIGNAL_OPENED;
        }

      else return NOTHING_OPENED;

}

bool MP_Gui_Callback_Demix_c::plugApprox()
{
  mpd_Demix_Core->plug_approximant( approxArray );

}

bool MP_Gui_Callback_Demix_c::initMpdDemixCore()
{
  mpd_Demix_Core = MP_Mpd_demix_Core_c::create( signal, mixer, bookArray );
  if (mpd_Demix_Core)return true;
  else return false;
}

int MP_Gui_Callback_Demix_c::getBookOpen()
{
  return opArrayBook;
}

void MP_Gui_Callback_Demix_c::saveBook(QString fileName)
{
  char line[1024];
  for (unsigned int j = 0; j < mixer->numSources; j++ )
    {
      sprintf( line, "%s_%02u.bin", fileName.toStdString().c_str(), j );
      bookArray->at(j)->print( line, MP_BINARY);
    }
}

void MP_Gui_Callback_Demix_c::saveApprox(QString fileName)
{
  char line[1024];
  if ((approxArray && approxArray->size()==mixer->numSources))
  for (unsigned int j = 0; j < mixer->numSources; j++ )
    {
      sprintf( line, "%s_%02u.wav", fileName.toStdString().c_str(), j );
      approxArray->at(j)->wavwrite( line );
      
    }
}

void MP_Gui_Callback_Demix_c::setSave(const unsigned long int setSaveHit,QString bookFileName, QString resFileName,QString decayFileName, QString sequenceFileName){
mpd_Demix_Core->set_save_hit(setSaveHit,bookFileName.toStdString().c_str(),resFileName.toStdString().c_str(),decayFileName.toStdString().c_str(),sequenceFileName.toStdString().c_str());
}