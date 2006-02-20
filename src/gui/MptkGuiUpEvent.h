#ifndef MPTK_GUI_UP_EVENT_H_
#define MPTK_GUI_UP_EVENT_H_

#include "wx/wx.h"
#include "wx/event.h"

DECLARE_EVENT_TYPE(wxUPVIEW_EVENT,7777)
#define EVT_UP_VIEW(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxUPVIEW_EVENT, wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
    ),
/**
 * \brief Event generated when user click on the Up button in the MptkGuiUpDownPanel
 */
  class MptkGuiUpEvent : public wxCommandEvent {
  public : 
    MptkGuiUpEvent();
    MptkGuiUpEvent(int id);
    
    int getId() {return id;}

    DECLARE_DYNAMIC_CLASS(MptkGuiUpEvent);

  private :
    int id;
  };

#endif
