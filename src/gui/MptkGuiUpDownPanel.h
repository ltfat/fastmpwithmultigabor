#ifndef MPTKGUIUPDOWNPANEL_H
#define MPTKGUIUPDOWNPANEL_H

#include "wx/wx.h"
#include "MptkGuiUpEvent.h"
#include "MptkGuiDownEvent.h"

/** 
  * Used in the views (MptkGuiExtendedView and MptkGuiConsoleView) to
  * move the view by clicking on the button Up or Down
  */
class MptkGuiUpDownPanel : public wxPanel {

public :
  MptkGuiUpDownPanel(wxWindow* parent, int id);

  void OnUp(wxCommandEvent& event);
  void OnDown(wxCommandEvent& event);

private :
  wxBoxSizer *m_sizer;
  wxBitmapButton *buttonUp;
  wxBitmapButton *buttonDown;

  DECLARE_EVENT_TABLE()
};

enum {
  ID_Up = 1400,
  ID_Down
};

#endif

