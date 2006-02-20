#ifndef MPTKGUICONSOLEVIEW_H
#define MPTKGUICONSOLEVIEW_H

#include "wx/wx.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "MptkGuiUpDownPanel.h"

/**
 * \brief MptkGuiConsoleView is the unique console in the MptkGuiFrame
 * Shows messages to the user
 */

class MptkGuiConsoleView : public wxPanel {

public : 
  MptkGuiConsoleView(wxWindow *parent, int id, wxWindow * frame);

  virtual ~MptkGuiConsoleView();

  void appendText(wxString mess);
  void appendTextAndRefreshView(wxString mess);
  void showPosition(long line, long column);

  void OnBrowse(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnAppend(wxCommandEvent& event);
  void OnClear(wxCommandEvent& event);

  private :
  wxWindow * frame;

  wxBoxSizer * m_sizer;
  wxBoxSizer * menu_sizer;
  wxTextCtrl * consoleText;
  
  wxTextCtrl * fileTextCtrl;
  wxButton * buttonBrowse;
  wxButton * buttonSave;
  wxButton * buttonAppend;
  wxButton * buttonClear;

  MptkGuiUpDownPanel * panelUpDown;

  wxFFile *file;

  DECLARE_EVENT_TABLE()
};

enum {
  ID_Clear = 1300,
  ID_Browse,
  ID_Save,
  ID_Append
};

#endif
