#ifndef MP_GUI_CALLBACK_DEMO_H_
#define MP_GUI_CALLBACK_DEMO_H_

#include "gui_callback.h"
#include "../plugin/base/gabor_atom_plugin.h"

class MP_Gui_Callback_Demo_c:public MP_Gui_Callback_c
{
public:
    void playTransientSignal(std::vector<bool> * v, float startTime, float endTime);
    void playOtherSignal(std::vector<bool> * v, float startTime, float endTime);
	MP_Gui_Callback_Demo_c();
	virtual ~MP_Gui_Callback_Demo_c();
	void separate(unsigned long int length);
	MP_Gabor_Atom_Plugin_c* newAtom;
	MP_Book_c * booktransient;
	MP_Book_c * bookother;
	MP_Signal_c *transientSignal;
	MP_Signal_c *otherSignal;
	
};

#endif /*MP_GUI_CALLBACK_DEMO_H_*/
