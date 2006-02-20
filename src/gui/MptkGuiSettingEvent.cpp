#include "MptkGuiSettingEvent.h"

DEFINE_EVENT_TYPE(wxSETTING_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiSettingEvent, wxEvent)

  MptkGuiSettingEvent::MptkGuiSettingEvent()
{
  SetEventType(wxSETTING_EVENT);
}
