/******************************************************************************/
/*                                                                            */
/*                           gui_callback.cpp                                 */
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
/* gui_callback.cpp : methods for class MainWindow        */
/*                                                        */
/**********************************************************/
#include "gui_callback.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

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
      opBook = NOTHING_OPENED;
      opSig = NOTHING_OPENED;
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

// Set a dictionnary to the core
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

void MP_Gui_Callback_c::initDictionary(){
mpd_Core->init_dict();
}

void MP_Gui_Callback_c::setSave(const unsigned long int setSaveHit,QString setBookFileName,QString setResFileName,QString setDecayFileName )
{
  mpd_Core->set_save_hit(setSaveHit,setBookFileName.toStdString().c_str(),setResFileName.toStdString().c_str(),setDecayFileName.toStdString().c_str());
}
// Save Custom dictionary
bool MP_Gui_Callback_c::saveDictionary(QString dictName){
if (mpd_Core) mpd_Core->save_dict(dictName.toStdString().c_str());
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

int MP_Gui_Callback_c::addDefaultBlockToDictionary(QString blockName){
if (mpd_Core) { mpd_Core->add_default_block_to_dict(blockName.toStdString().c_str());
 mpd_Core->plug_dict_to_signal();
 return 1;
}
else return 0;
}

void MP_Gui_Callback_c::addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap){
mpd_Core->addCustomBlockToDictionary(setPropertyMap);
mpd_Core->plug_dict_to_signal();
}
