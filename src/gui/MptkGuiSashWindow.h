#ifndef MPTKGUI_SASHWINDOW_H
#define MPTKGUI_SASHWINDOW_H

#include "wx/wx.h"
#include "wx/sashwin.h"
#include "MptkGuiExtendedView.h"

/**
 * \brief Gives to the contained view resize propertie
 */
class MptkGuiSashWindow : public wxSashWindow {

public :

  MptkGuiSashWindow(wxWindow * parent, int id);
  
  ~MptkGuiSashWindow();

  void setView(MptkGuiExtendedView * mpView);
  MptkGuiExtendedView * getView();

  void OnDrag(wxSashEvent& event); 

private :
  MptkGuiExtendedView * view;

  DECLARE_EVENT_TABLE();
};

#endif
