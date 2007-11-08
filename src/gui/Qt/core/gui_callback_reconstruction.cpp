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
#include "gui_callback_reconstruction.h"

MP_Gui_Callback_Reconstruct_c * MP_Gui_Callback_Reconstruct_c::guiCallbackRecons = NULL;
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

MP_Gui_Callback_Reconstruct_c::MP_Gui_Callback_Reconstruct_c()
{
  if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
  reconstruct = NULL;
  audio = NULL;
  approximant = NULL;
  mpRunning = false;
  /* Make the book */
  book = MP_Book_c::create();
  QThread::start();
  opSig = NOTHING_OPENED;
  opBook = NOTHING_OPENED;
  activated = false;
}

MP_Gui_Callback_Reconstruct_c::~MP_Gui_Callback_Reconstruct_c()
{
  if (reconstruct) delete reconstruct;
  if ( approximant) delete approximant;
  if (book) delete book;
  if ( audio) delete audio;
}

MP_Gui_Callback_Reconstruct_c* MP_Gui_Callback_Reconstruct_c::get_gui_call_back()
{
  if (!guiCallbackRecons)
    {
      guiCallbackRecons= new MP_Gui_Callback_Reconstruct_c();
    }
  return guiCallbackRecons;
}

/***********/
/* METHODS */
/***********/

/***************************/
/* MISC METHODS            */
/***************************/

void MP_Gui_Callback_Reconstruct_c::setActivated()
{
  activated =  true;
}

void MP_Gui_Callback_Reconstruct_c::setDesactivated()
{
  activated =  false;
}

bool MP_Gui_Callback_Reconstruct_c::getActivated()
{
  return activated;
}

void MP_Gui_Callback_Reconstruct_c::play(MP_Signal_c * sig, std::vector<bool> * v, float startTime, float endTime)
{

  stopPortAudioStream();
  if (sig != NULL)
    {
      audio = new MP_Gui_Audio(sig);
      if (startTime<endTime)
        {
          int deb = 0;
          int end = (int)(reconstruct->sampleRate);
          audio->playSelected(v, deb, end);
        }
      else audio->playSelected(v);
    }
}

void MP_Gui_Callback_Reconstruct_c::playApproximantSignal(std::vector<bool> * v, float startTime, float endTime)
{
  play(approximant, v, startTime, endTime);
}

void MP_Gui_Callback_Reconstruct_c::playReconstructSignal(std::vector<bool> * v, float startTime, float endTime)
{
  play(reconstruct, v, startTime, endTime);
}

void MP_Gui_Callback_Reconstruct_c::stopPortAudioStream()
{
  if (audio != NULL)
    {
      if (audio->getStream() != NULL) audio->stop();
    }
}


    /** \brief Method to reconstruct */
    void MP_Gui_Callback_Reconstruct_c::reconstructSignals()
    {
      if (getActivated()) 
        {
          /* display conditions */

          /* lauch the run method (inherited from QThread) */
          mutex.lock();
          if (!mpRunning)iterateCond.wakeAll();
          mutex.unlock();
        }
    }

void MP_Gui_Callback_Reconstruct_c::run()
{


  while (true)
    {
      /* take mutex and pass the boolean to true */
      mutex.lock();
      //if (!mpRunning) error à afficher*/
      /* Wait condition */
      iterateCond.wait(&mutex);
      mpRunning = true;
      mutex.unlock();
      /* Test if can step and the run */
      if (opBook == BOOK_OPENED)
        { /* emit a signal to indicate that decomposition is started */
          emit runningReconstruction(true);
          book->substract_add( NULL, reconstruct, NULL );
          book->substract_add( NULL, approximant, NULL );
          emit runningReconstruction(false);
          /* take mutex and pass the boolean to false */
          mutex.lock();
          mpRunning = false;
          mutex.unlock();
        }
    }
}

int MP_Gui_Callback_Reconstruct_c::openBook(QString fileName)
{
  if (book) delete book;
  FILE* fid = fopen(fileName.toStdString().c_str(),"rb");
  book = MP_Book_c::create(fid);
  fclose(fid);
  opBook = BOOK_OPENED;
  return BOOK_OPENED;
}

void MP_Gui_Callback_Reconstruct_c::saveReconstruct(QString fileName)
{
  if (reconstruct) reconstruct->wavwrite(fileName.toStdString().c_str());
}

void MP_Gui_Callback_Reconstruct_c::saveApproximant(QString fileName)
{
  if (approximant) approximant->wavwrite(fileName.toStdString().c_str());
}

int MP_Gui_Callback_Reconstruct_c::getSignalOpen()
{
  return opSig;
}

int MP_Gui_Callback_Reconstruct_c::openSignal(QString fileName)
    {
      if (reconstruct != NULL) delete reconstruct;
      if (approximant != NULL) delete approximant;
      reconstruct = MP_Signal_c::init( fileName.toStdString().c_str() );
      if (reconstruct != NULL) approximant = MP_Signal_c::init( reconstruct->numChans, reconstruct->numSamples, reconstruct->sampleRate );
      if (reconstruct != NULL)
        {
          opSig = SIGNAL_OPENED;
          return SIGNAL_OPENED;
        }

      else return NOTHING_OPENED;
    }

int MP_Gui_Callback_Reconstruct_c::initSignals(){
	
      if (reconstruct != NULL) delete reconstruct;
      if (approximant != NULL) delete approximant;
      if (book!= NULL) reconstruct = MP_Signal_c::init( book->numChans, book->numSamples, book ->sampleRate );
      if (book!= NULL) approximant = MP_Signal_c::init( book->numChans, book->numSamples, book ->sampleRate );
      if (reconstruct != NULL)
        {
          opSig = SIGNAL_OPENED;
          return SIGNAL_OPENED;
        }
}    

 bool MP_Gui_Callback_Reconstruct_c::coreInit(){
 	if (book!= NULL && reconstruct != NULL && approximant != NULL) return true;
 	else return false;
 }
 
 void MP_Gui_Callback_Reconstruct_c::emitInfoMessage(char* message){
emit MP_Gui_Callback_Reconstruct_c::get_gui_call_back()->infoMessage(message);
}

void MP_Gui_Callback_Reconstruct_c::emitErrorMessage(char* message){
emit MP_Gui_Callback_Reconstruct_c::get_gui_call_back()->errorMessage(message);
}

void MP_Gui_Callback_Reconstruct_c::emitWarningMessage(char* message){
emit MP_Gui_Callback_Reconstruct_c::get_gui_call_back()->warningMessage(message);
}