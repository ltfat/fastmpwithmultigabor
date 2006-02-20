#ifndef MPTK_GUI_DOWN_EVENT_H_
#define MPTK_GUI_DOWN_EVENT_H_

#include "wx/wx.h"
#include "wx/event.h"

DECLARE_EVENT_TYPE(wxDOWNVIEW_EVENT,7777)
#define EVT_DOWN_VIEW(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxDOWNVIEW_EVENT, wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
    ),
/**
 * \brief Event generated when user click on the Down button in the MptkGuiUpDownPanel
 */
  class MptkGuiDownEvent : public wxCommandEvent {
  public : 
    MptkGuiDownEvent();
    MptkGuiDownEvent(int id);
    
    int getId() {return id;}

    DECLARE_DYNAMIC_CLASS(MptkGuiDownEvent);

  private :
    int id;
  };

#endif
