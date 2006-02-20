#include "MptkGuiConsoleView.h"
#include "MptkGuiFrame.h"

BEGIN_EVENT_TABLE(MptkGuiConsoleView, wxPanel)
  EVT_BUTTON(ID_Browse, MptkGuiConsoleView::OnBrowse)
  EVT_BUTTON(ID_Save, MptkGuiConsoleView::OnSave)
  EVT_BUTTON(ID_Append, MptkGuiConsoleView::OnAppend)
  EVT_BUTTON(ID_Clear, MptkGuiConsoleView::OnClear)
END_EVENT_TABLE()

  MptkGuiConsoleView::MptkGuiConsoleView(wxWindow *parent, int id, wxWindow * fr) : 
    wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  // For refresh, the console needs the pointer to the frame
  frame = fr;
  // Create the main sizer
  m_sizer = new wxBoxSizer(wxVERTICAL);

  // Name of the console
  wxStaticText * text = new wxStaticText(this, wxID_ANY, wxT("Console"));
  
  // Create the sizer, the buttons and the panel for buttons up & down 
  menu_sizer = new wxBoxSizer(wxHORIZONTAL);

  buttonSave = new wxButton(this, ID_Save, _T("Save"), wxDefaultPosition, wxSize(50,30));
  buttonAppend = new wxButton(this, ID_Append, _T("Append"), wxDefaultPosition, wxSize(65,30));
  fileTextCtrl = new wxTextCtrl(this, wxID_ANY);
  buttonBrowse = new wxButton(this, ID_Browse, _T("Browse..."), wxDefaultPosition, wxSize(75,30)); 
  panelUpDown = new MptkGuiUpDownPanel(this, id);
  buttonClear = new wxButton(this, ID_Clear, _T("Clear"), wxDefaultPosition, wxSize(50,30));
  
  // Add the button & the panel to the menu_sizer
  menu_sizer->Add(buttonSave, 0, wxALL, 2);
  menu_sizer->Add(buttonAppend, 0, wxALL, 2);
  menu_sizer->Add(fileTextCtrl, 1, wxEXPAND | wxALL, 2);
  menu_sizer->Add(buttonBrowse, 0, wxALL, 2);
  menu_sizer->Add(panelUpDown, 0, wxALL, 2);
  menu_sizer->Add(buttonClear, 0, wxALL, 2);
  
  // Create the consoleText
  consoleText = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE );
  consoleText->SetEditable(false);

  // Add elements in their sizer
  SetSizer(m_sizer);
  m_sizer->Add(text, 0, wxCENTER , 2);
  m_sizer->Add(menu_sizer, 0, wxEXPAND, 1);
  m_sizer->Add(consoleText, 1, wxEXPAND | wxALL, 1);
}

MptkGuiConsoleView::~MptkGuiConsoleView()
{
}

void MptkGuiConsoleView::appendText(wxString mess)
{
  consoleText->AppendText(mess);
  wxTheApp->Yield();
}


void MptkGuiConsoleView::appendTextAndRefreshView(wxString mess)
{
  consoleText->AppendText(mess);
  ((MptkGuiFrame *) frame)->refresh();
  wxTheApp->Yield();
}

void MptkGuiConsoleView::showPosition( long line, long column )
{
  consoleText->ShowPosition( consoleText->XYToPosition(line,column) );
}


void MptkGuiConsoleView::OnClear(wxCommandEvent& WXUNUSED(event))
{
  consoleText->Clear();
}

void MptkGuiConsoleView::OnBrowse(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog * saveDialog = new wxFileDialog(this,
						wxT("Select the file to save console text"),
						"",
						"",
						"*",
						wxSAVE,
						wxDefaultPosition);
  if ( saveDialog->ShowModal()== wxID_OK ) {
   fileTextCtrl->SetValue(saveDialog->GetPath());
   fileTextCtrl->SetInsertionPointEnd();
  }
}

void MptkGuiConsoleView::OnSave(wxCommandEvent& WXUNUSED(event))
{
  wxString   fileString = fileTextCtrl->GetValue();

  /* Check if the file name is empty */
  if ( fileString == "" ) wxMessageBox(wxT("Please give a file name in the editable field"
					 " (or use the \"Browse\" button to select a file)."),
					   wxT("Error"), wxICON_ERROR | wxOK);
  /* Open the file and write (wxWidgets does all the other checks, e.g. valid pathname etc.) */
  else { 
    file = new wxFFile(fileString, "w");
    if (file->IsOpened()){
      file->Write(consoleText->GetValue());
      file->Close();
      MPTK_GUI_CONSOLE->appendText("The console text has been saved to file [" + fileString + "].\n");
    }
    delete file;
  }
}

void MptkGuiConsoleView::OnAppend(wxCommandEvent& WXUNUSED(event))
{
  wxString fileString = fileTextCtrl->GetValue();

  /* Check if the file name is empty */
  if ( fileString == "" ) wxMessageBox(wxT("Please give a file name in the editable field"
					 " (or use the \"Browse\" button to select a file)."),
					   wxT("Error"), wxICON_ERROR | wxOK);
  /* Open the file and write (wxWidgets does all the other checks, e.g. valid pathname etc.) */
  else { 
    file = new wxFFile(fileString, "aw");
    if (file->IsOpened()){
      file->Write(consoleText->GetValue());
      file->Close();
      MPTK_GUI_CONSOLE->appendText("The console text has been appended to file [" + fileString + "].\n");
    }
    delete file;
  }
}
