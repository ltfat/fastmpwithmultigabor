#ifndef MPTK_CMAP_ZOOM_EVENT_H_
#define MPTK_CMAP_ZOOM_EVENT_H_

#include <wx/wx.h>
#include <wx/event.h>

DECLARE_EVENT_TYPE(wxCMAP_ZOOM_EVENT,7777)
#define EVT_CMAP_ZOOM(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxCMAP_ZOOM_EVENT,wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
     ),

/**
 * \brief MptkGuiCMapEvent is the event genrated when user moves the bounds
 * of the MptkGuiColorMapView
 */

class MptkGuiCMapZoomEvent : public wxCommandEvent {
 public:
  
  MptkGuiCMapZoomEvent();
  MptkGuiCMapZoomEvent(int id, float dBmin,float dBmax);
  
  int getId(){return id;}
  float getDBmin(){return dBmin;};
  float getDBmax(){return dBmax;};
  DECLARE_DYNAMIC_CLASS(MptkGuiZoomEvent);

private:
  int id;
  float dBmin;
  float dBmax;  
};

#endif /*MPTK_ZOOM_EVENT_H_*/
