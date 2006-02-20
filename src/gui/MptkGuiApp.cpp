#include "wx/wx.h"
#include "MptkGuiApp.h"

IMPLEMENT_APP(MptkGuiApp)

// Creation of the main frame
  bool MptkGuiApp::OnInit()
{
  frame = new MptkGuiFrame(_T("MptkGui"));

  frame->SetSize(1024, 768);

  frame->Show(true);
  SetTopWindow(frame);
  return true;
}

