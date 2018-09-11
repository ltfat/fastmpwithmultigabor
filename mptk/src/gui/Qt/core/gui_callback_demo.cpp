/******************************************************************************/
/*                                                                            */
/*                         gui_callback_demo.cpp                              */
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
/* gui_callback_demo.cpp : methods for class MainWindow   */
/*                                                        */
/**********************************************************/

#include "gui_callback_demo.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
MP_Gui_Callback_Demo_c * MP_Gui_Callback_Demo_c::guiCallbackDemo = NULL;

MP_Gui_Callback_Demo_c::MP_Gui_Callback_Demo_c():
    MP_Gui_Callback_c()
{
  newAtom = NULL;
  transientSignal= NULL;
  otherSignal = NULL;
  
}

MP_Gui_Callback_Demo_c::~MP_Gui_Callback_Demo_c()
{
if (newAtom) delete newAtom;
}

MP_Gui_Callback_Demo_c * MP_Gui_Callback_Demo_c::get_gui_call_back(){
 	  if (!guiCallbackDemo)
    {
      guiCallbackDemo = new MP_Gui_Callback_Demo_c();
    }
return guiCallbackDemo;
}

void MP_Gui_Callback_Demo_c::separate(unsigned long int length)
{
  booktransient = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );
  bookother = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );
  transientSignal = MP_Signal_c::init(signal->numChans, signal->numSamples, signal->sampleRate );
  otherSignal = MP_Signal_c::init(signal->numChans, signal->numSamples, signal->sampleRate );
	
  for (unsigned int nAtom =0 ; nAtom < book->numAtoms; nAtom++)
    {
      if ( strcmp(book->atom[nAtom]->type_name(), "gabor")==0)
        {
          if (book->atom[nAtom]->support[0].len < length) booktransient->append( book->atom[nAtom] );
          else bookother->append( book->atom[nAtom] );
        }
    }
  if ( booktransient->substract_add( NULL, transientSignal, NULL ) == 0 ) {
  }
  if ( bookother->substract_add( NULL, otherSignal, NULL ) == 0 ) {
  }
  

}


void MP_Gui_Callback_Demo_c::playTransientSignal(std::vector<bool> * v, float startTime, float endTime)
{
  MP_Gui_Callback_Abstract_c::play(transientSignal, v, startTime, endTime);
}

void MP_Gui_Callback_Demo_c::playOtherSignal(std::vector<bool> * v, float startTime, float endTime)
{
  MP_Gui_Callback_Abstract_c::play(otherSignal, v, startTime, endTime);
}

void MP_Gui_Callback_Demo_c::emitInfoMessage(char* message){
emit MP_Gui_Callback_Demo_c::get_gui_call_back()->infoMessage(message);
}

void MP_Gui_Callback_Demo_c::emitErrorMessage(char* message){
emit MP_Gui_Callback_Demo_c::get_gui_call_back()->errorMessage(message);
}

void MP_Gui_Callback_Demo_c::emitWarningMessage(char* message){
emit MP_Gui_Callback_Demo_c::get_gui_call_back()->warningMessage(message);
}
