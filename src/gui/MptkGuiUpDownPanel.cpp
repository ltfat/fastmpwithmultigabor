#include "MptkGuiUpDownPanel.h"

#include "bitmaps/up.xpm"
#include "bitmaps/down.xpm"

BEGIN_EVENT_TABLE(MptkGuiUpDownPanel, wxPanel)
  EVT_BUTTON(ID_Up, MptkGuiUpDownPanel::OnUp)
  EVT_BUTTON(ID_Down, MptkGuiUpDownPanel::OnDown)
END_EVENT_TABLE()

  MptkGuiUpDownPanel::MptkGuiUpDownPanel(wxWindow *parent, int id) : wxPanel(parent, id)
{
  // Create the main sizer
  m_sizer = new wxBoxSizer(wxHORIZONTAL);
  SetSizer(m_sizer);
  
  // Create the buttons
  buttonUp = new wxBitmapButton(this, ID_Up, wxBitmap(up_xpm), wxDefaultPosition, wxSize(50,30));
  buttonDown = new wxBitmapButton(this, ID_Down, wxBitmap(down_xpm), wxDefaultPosition, wxSize(50,30));

  // Add buttons to the sizer
  m_sizer->Add(buttonUp, 0, wxALL, 0);
  m_sizer->Add(buttonDown, 0, wxALL, 0);
}

void MptkGuiUpDownPanel::OnUp(wxCommandEvent& WXUNUSED(event))
{
  MptkGuiUpEvent * evt = new MptkGuiUpEvent(GetId());
  ProcessEvent(*evt);
  delete evt;
}

void MptkGuiUpDownPanel::OnDown(wxCommandEvent& WXUNUSED(event))
{
  MptkGuiDownEvent * evt = new MptkGuiDownEvent(GetId());
  ProcessEvent(*evt);
  delete evt;
}

