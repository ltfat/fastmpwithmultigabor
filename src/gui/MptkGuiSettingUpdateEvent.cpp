#include "MptkGuiSettingUpdateEvent.h"

DEFINE_EVENT_TYPE(wxSETTING_UPDATEEVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiSettingUpdateEvent, wxEvent)

  MptkGuiSettingUpdateEvent::MptkGuiSettingUpdateEvent()
{
  SetEventType(wxSETTING_UPDATEEVENT);
}
