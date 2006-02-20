#include "MptkGuiZoomEvent.h"


DEFINE_EVENT_TYPE(wxZOOM_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiZoomEvent, wxEvent)
  MptkGuiZoomEvent::MptkGuiZoomEvent() 
{
  SetEventType(wxZOOM_EVENT);
}

MptkGuiZoomEvent::MptkGuiZoomEvent(int ident, float tFirst, float tLast)
{
  SetEventType(wxZOOM_EVENT);
  id = ident;
  this->tFirst = tFirst;
  this->tLast = tLast;
}

MptkGuiZoomEvent::MptkGuiZoomEvent(int ident, float tFirst, float tLast, float min, float max)
{
  SetEventType(wxZOOM_EVENT);
  id = ident;
  this->tFirst = tFirst;
  this->tLast = tLast;
  this->min = min;
  this->max = max;
}
