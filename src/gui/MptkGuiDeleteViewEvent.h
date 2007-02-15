#ifndef MPTK_DELETE_VIEW_EVENT_H_
#define MPTK_DELETE_VIEW_EVENT_H_

#include <wx/wx.h>
#include <wx/event.h>

DECLARE_EVENT_TYPE(wxDELETEVIEW_EVENT,7777)
#define EVT_DELETE_VIEW(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxDELETEVIEW_EVENT, wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
    ),

/**
 * \brief Event generated when user click on close button in a MptkGuiExtendedView
 * Allows the MptkGuiFrame to update the screen
 */

class MptkGuiDeleteViewEvent : public wxCommandEvent {
 public:
  
  MptkGuiDeleteViewEvent();
  MptkGuiDeleteViewEvent(int id);
  
  int getId(){return id;};

  DECLARE_DYNAMIC_CLASS(MptkGuiDeleteViewEvent);

private :
  int id;
};

#endif
