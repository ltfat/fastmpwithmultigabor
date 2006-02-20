#include "MptkGuiUpEvent.h"

DEFINE_EVENT_TYPE(wxUPVIEW_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiUpEvent, wxEvent)

  MptkGuiUpEvent::MptkGuiUpEvent()
{
  SetEventType(wxUPVIEW_EVENT);
}

MptkGuiUpEvent::MptkGuiUpEvent(int ident)
{
  SetEventType(wxUPVIEW_EVENT);
  id = ident;
}
