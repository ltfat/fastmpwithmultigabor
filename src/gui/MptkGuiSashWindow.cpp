#include "MptkGuiSashWindow.h"

BEGIN_EVENT_TABLE(MptkGuiSashWindow, wxSashWindow)
  EVT_SASH_DRAGGED(wxID_ANY, MptkGuiSashWindow::OnDrag)
  END_EVENT_TABLE()

  MptkGuiSashWindow::MptkGuiSashWindow(wxWindow * parent, int id):
  wxSashWindow(parent, id)
{
  view = NULL;

  SetSashVisible(wxSASH_BOTTOM, true);

  SetSashBorder(wxSASH_BOTTOM, true);

  SetSizeHints(200,200);
}

MptkGuiSashWindow::~MptkGuiSashWindow()
{
  if (view != NULL)  delete view;
}

void MptkGuiSashWindow::setView(MptkGuiExtendedView * mpView)
{
  view = mpView;
}

MptkGuiExtendedView * MptkGuiSashWindow::getView()
{
  return view;
}

void MptkGuiSashWindow::OnDrag(wxSashEvent& event){ 
  if (event.GetDragStatus() == wxSASH_STATUS_OK){
    SetSizeHints(0, event.GetDragRect().GetSize().GetHeight());
  }
  event.Skip();
}
