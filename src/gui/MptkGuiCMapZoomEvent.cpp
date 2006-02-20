#include "MptkGuiCMapZoomEvent.h"


DEFINE_EVENT_TYPE(wxCMAP_ZOOM_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiCMapZoomEvent, wxEvent)
  MptkGuiCMapZoomEvent::MptkGuiCMapZoomEvent() 
{
  SetEventType(wxCMAP_ZOOM_EVENT);
}

MptkGuiCMapZoomEvent::MptkGuiCMapZoomEvent(int ident, float dBmin,float dBmax)
{
  SetEventType(wxCMAP_ZOOM_EVENT);
  id = ident;
  this->dBmin=dBmin;
  this->dBmax=dBmax;
}

