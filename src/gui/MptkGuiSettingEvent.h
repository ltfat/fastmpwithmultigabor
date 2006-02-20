#ifndef MPTKGUISETTINGEVENT_H_
#define MPTKGUISETTINGEVENT_H_

#include <wx/wx.h>
#include <wx/event.h>

DECLARE_EVENT_TYPE(wxSETTING_EVENT,7777)
#define EVT_SETTING(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxSETTING_EVENT, wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
    ),
/**
 * \brief Event generated when parameters in the MptkGuiMPSettings dialog
 * are corrects (test when user click on OK or Apply button)
 */
class MptkGuiSettingEvent : public wxCommandEvent {
 public:
  
  MptkGuiSettingEvent();

  DECLARE_DYNAMIC_CLASS(MptkGuiSettingEvent);
};

#endif /*MPTKGUISETTINGEVENT_H_*/
