#ifndef MPTK_ZOOM_EVENT_H_
#define MPTK_ZOOM_EVENT_H_

#include <wx/wx.h>
#include <wx/event.h>

DECLARE_EVENT_TYPE(wxZOOM_EVENT,7777)
#define EVT_ZOOM(fn)	       \
    DECLARE_EVENT_TABLE_ENTRY( \
     wxZOOM_EVENT,wxID_ANY, wxID_ANY,	\
	(wxObjectEventFunction)(wxEventFunction)&fn, \
	(wxObject *) NULL \
     ),
class MptkGuiZoomEvent : public wxCommandEvent {
 public:
  
  MptkGuiZoomEvent();
  MptkGuiZoomEvent(int id, float tFirst, float tLast);
  MptkGuiZoomEvent(int id, float tFirst, float tLast , float min, float max);
  
  int getId(){return id;}
  float getFirstTime(){return tFirst;};
  float getLastTime(){return tLast;};
  float getMinAmp(){return min;}
  float getMaxAmp(){return max;};
  float getFrequence_bas(){return min;}
  float getFrequence_haut(){return max;}
  DECLARE_DYNAMIC_CLASS(MptkGuiZoomEvent);

private:
  int id;
  float tFirst;
  float tLast;
  float min;
  float max;
};

#endif /*MPTK_ZOOM_EVENT_H_*/
