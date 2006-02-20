#include "MptkGuiDownEvent.h"

DEFINE_EVENT_TYPE(wxDOWNVIEW_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiDownEvent, wxEvent)

  MptkGuiDownEvent::MptkGuiDownEvent()
{
  SetEventType(wxDOWNVIEW_EVENT);
}

MptkGuiDownEvent::MptkGuiDownEvent(int ident)
{
  SetEventType(wxDOWNVIEW_EVENT);
  id = ident;
}
