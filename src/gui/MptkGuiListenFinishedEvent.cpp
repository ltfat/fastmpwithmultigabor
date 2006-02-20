#include "MptkGuiListenFinishedEvent.h"

DEFINE_EVENT_TYPE(wxLISTENFINISHED_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiListenFinishedEvent, wxEvent)

  MptkGuiListenFinishedEvent::MptkGuiListenFinishedEvent()
{
  SetEventType(wxLISTENFINISHED_EVENT);
}

