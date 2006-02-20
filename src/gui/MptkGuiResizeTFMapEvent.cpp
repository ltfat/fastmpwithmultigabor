#include "MptkGuiResizeTFMapEvent.h"


DEFINE_EVENT_TYPE(RESIZE_TF_MAP_EVENT);
IMPLEMENT_DYNAMIC_CLASS(MptkGuiResizeTFMapEvent, wxEvent)
  MptkGuiResizeTFMapEvent::MptkGuiResizeTFMapEvent() 
{
  SetEventType(RESIZE_TF_MAP_EVENT);
}
