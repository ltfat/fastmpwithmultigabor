#include "MptkGuiDeleteViewEvent.h"

DEFINE_EVENT_TYPE(wxDELETEVIEW_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiDeleteViewEvent, wxEvent)

  MptkGuiDeleteViewEvent::MptkGuiDeleteViewEvent()
{
  SetEventType(wxDELETEVIEW_EVENT);
}

  MptkGuiDeleteViewEvent::MptkGuiDeleteViewEvent(int ident)
{
  SetEventType(wxDELETEVIEW_EVENT);
  id = ident;
}

