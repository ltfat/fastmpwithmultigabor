#include "gui_callback_demo.h"

MP_Gui_Callback_Demo_c::MP_Gui_Callback_Demo_c():
    MP_Gui_Callback_c()
{
  newAtom = NULL;

}

MP_Gui_Callback_Demo_c::~MP_Gui_Callback_Demo_c()
{}

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