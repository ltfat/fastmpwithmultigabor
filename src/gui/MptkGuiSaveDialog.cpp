#include "MptkGuiSaveDialog.h"
#include <iostream>

BEGIN_EVENT_TABLE(MptkGuiSaveDialog, wxDialog)
	EVT_BUTTON(Ev_SaveDialog_Browse_Book, MptkGuiSaveDialog::OnBrowseBook)
	EVT_BUTTON(Ev_SaveDialog_Browse_Residual, MptkGuiSaveDialog::OnBrowseResidual)
	EVT_BUTTON(Ev_SaveDialog_Browse_Approx, MptkGuiSaveDialog::OnBrowseApprox)
	EVT_BUTTON(Ev_SaveDialog_Autofill_Book, MptkGuiSaveDialog::OnAutofillBook)
	EVT_BUTTON(Ev_SaveDialog_Autofill_Residual, MptkGuiSaveDialog::OnAutofillResidual)
	EVT_BUTTON(Ev_SaveDialog_Autofill_Approx, MptkGuiSaveDialog::OnAutofillApprox)
	EVT_BUTTON(wxID_SAVE, MptkGuiSaveDialog::OnSave)
END_EVENT_TABLE ()

// Creator
  MptkGuiSaveDialog::MptkGuiSaveDialog( wxWindow *parent, wxWindowID id, wxString defaultDirSave) : wxDialog( parent, id, wxString())
{
	panel = new wxPanel(this, wxID_ANY);
	sizer = new wxBoxSizer(wxVERTICAL);
	
	// Main sizer books' names and buttons (Save and Cancel)
	stBoxBooksSizer = new wxStaticBoxSizer(wxVERTICAL, panel, _T("Save"));
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Books
	bookSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Save the book"));
	bookText = new wxTextCtrl(panel, wxID_ANY);
	buttonBrowseBook = new wxButton(panel, Ev_SaveDialog_Browse_Book, _T("Browse..."));
	buttonAutoFillBook = new wxButton(panel, Ev_SaveDialog_Autofill_Book, _T("auto fill"));
	bookSizer->Add(bookText, 6, wxALL, 5);
	bookSizer->Add(buttonBrowseBook, 2, wxALL, 5);
	bookSizer->Add(buttonAutoFillBook, 2, wxALL, 5);

	residualSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Save the residual signal"));
	residualText = new wxTextCtrl(panel, wxID_ANY);
	buttonBrowseResidual = new wxButton(panel, Ev_SaveDialog_Browse_Residual, _T("Browse..."));
	buttonAutoFillResidual = new wxButton(panel, Ev_SaveDialog_Autofill_Residual, _T("auto fill"));
	residualSizer->Add(residualText, 6, wxALL, 5);
	residualSizer->Add(buttonBrowseResidual, 2, wxALL, 5);
	residualSizer->Add(buttonAutoFillResidual, 2, wxALL, 5);

	approxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Save the rebuilt (approx) signal"));
	approxText = new wxTextCtrl(panel, wxID_ANY);
	buttonBrowseApprox = new wxButton(panel, Ev_SaveDialog_Browse_Approx, _T("Browse..."));
	buttonAutoFillApprox = new wxButton(panel, Ev_SaveDialog_Autofill_Approx, _T("auto fill"));
	approxSizer->Add(approxText, 6, wxALL, 5);
	approxSizer->Add(buttonBrowseApprox, 2, wxALL, 5);
	approxSizer->Add(buttonAutoFillApprox, 2, wxALL, 5);

	stBoxBooksSizer->Add(bookSizer, 1, wxEXPAND | wxALL, 3);
	stBoxBooksSizer->Add(residualSizer, 1, wxEXPAND |wxALL, 3);
	stBoxBooksSizer->Add(approxSizer, 1, wxEXPAND |wxALL, 3);

	// Buttons Save and Cancel
	buttonSave = new wxButton(panel, wxID_SAVE, _T("Save"));
	buttonCancel = new wxButton(panel, wxID_CANCEL, _T("Cancel"));

	buttonsSizer->Add(buttonSave, 1, wxCENTER | wxALL, 10);
	buttonsSizer->Add(buttonCancel, 1, wxCENTER | wxALL, 10);	

	// Add the sizers to the main sizer
	sizer->Add(stBoxBooksSizer, 6, wxEXPAND | wxALL, 3);
	sizer->Add(buttonsSizer, 2, wxCENTER |wxALL, 2);
	
	panel->SetAutoLayout( TRUE );
	panel->SetSizer(sizer);
    	sizer->Fit(panel);
    	sizer->SetSizeHints(panel);
	SetSizeHints(panel->GetSize());

	// Initialization
	bookName = "";
	residualName = "";
	approxName = "";
	defaultDir = defaultDirSave;
}

MptkGuiSaveDialog::~MptkGuiSaveDialog()
{
	delete panel;
}
	
wxString MptkGuiSaveDialog::getBookName()
{
	return bookName;
}

wxString MptkGuiSaveDialog::getResidualName()
{
	return residualName;
}

wxString MptkGuiSaveDialog::getApproxName()
{
	return approxName;
}

wxString MptkGuiSaveDialog::getDefaultDir()
{
  return defaultDir;
}

// Open a file dialog menu and retruns the selected file
wxString MptkGuiSaveDialog::saveFileDialog(wxString title)
{
	wxFileDialog * saveFileDialog = new wxFileDialog(this,
						title,
						defaultDir,
						"",
						"*",
						wxSAVE,
						wxDefaultPosition);
	if (saveFileDialog->ShowModal()== wxID_OK) {
	  wxString file = saveFileDialog->GetPath();
	  if (file != _T("")){
	    if (wxFileName::DirExists( (wxFileName::FileName(file)).GetPath() )){
	      defaultDir = saveFileDialog->GetDirectory();
	      return file;
	    }
	    else wxMessageBox(wxT("Directory of " + file +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
	  }
	}
	return "";
}

// Events procedures

void MptkGuiSaveDialog::OnBrowseBook(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName = saveFileDialog(_T("Save book"));
	if (fileName != "") {
		bookText->SetValue(fileName);
		bookText->SetInsertionPointEnd();
	}
}

void MptkGuiSaveDialog::OnBrowseResidual(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName = saveFileDialog(_T("Save residual"));
	if (fileName != "") {
		residualText->SetValue(fileName);
		residualText->SetInsertionPointEnd();
	}
}

void MptkGuiSaveDialog::OnBrowseApprox(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName = saveFileDialog(_T("Save approximant"));
	if (fileName != "") {
		approxText->SetValue(fileName);
		approxText->SetInsertionPointEnd();
	}
}

void MptkGuiSaveDialog::OnSave(wxCommandEvent& WXUNUSED(event))
{
	bookName = bookText->GetValue();
	residualName = residualText->GetValue();
	approxName = approxText->GetValue();
	Close(true);
}
void MptkGuiSaveDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MptkGuiSaveDialog::OnAutofillBook(wxCommandEvent& WXUNUSED(event))
{
  wxString rootpath=bookText->GetValue();
  wxString path;
  
   if(rootpath.Find('.',true)==-1)
    {
      path=wxString(rootpath);
      rootpath.Append(".book");
      bookText->SetValue(rootpath);
    }
  else
    {
      path=rootpath.BeforeLast('.');
    }

  path.Append("_residual.wav");
  residualText->SetValue(path);
  path=rootpath.BeforeLast('.');
  path.Append("_rebuilt.wav");
  approxText->SetValue(path);
  
}

void MptkGuiSaveDialog::OnAutofillResidual(wxCommandEvent& WXUNUSED(event))
{
  wxString rootpath=residualText->GetValue();
  wxString _residual="_residual.wav";
  wxString path1;
  wxString path2;
  if(rootpath.Find('.',true)==-1)
    {
      path1=wxString(rootpath);
      path2=wxString(rootpath);
      rootpath.Append(".wav");
      residualText->SetValue(rootpath);
    }
  else
    {
      path1=rootpath.BeforeLast('.');
      path2=rootpath.BeforeLast('.');
    }

  if(_residual.Length()<rootpath.Length())
    {
      if(compareStrings(_residual,(rootpath.Mid(rootpath.Length()-_residual.Length()))))
	{
	  path1=rootpath.BeforeLast('_');
	  path2=rootpath.BeforeLast('_');
	}
    }
  path1.Append(".book");
  bookText->SetValue(path1);
  path2.Append("_rebuilt.wav");
  approxText->SetValue(path2);
}

void MptkGuiSaveDialog::OnAutofillApprox(wxCommandEvent& WXUNUSED(event))
{
  wxString rootpath=approxText->GetValue();
  wxString path1;
  wxString path2;
  if(rootpath.Find('.',true)==-1)
    {
      path1=wxString(rootpath);
      path2=wxString(rootpath);
      rootpath.Append(".wav");
      approxText->SetValue(rootpath);
    }
  else
    {
      path1=rootpath.BeforeLast('.');
      path2=rootpath.BeforeLast('.');
    }

  wxString _approx="_rebuilt.wav";
  if(_approx.Length()<rootpath.Length())
    {
      if(compareStrings(_approx,(rootpath.Mid(rootpath.Length()-_approx.Length()))))
	{
	  path1=rootpath.BeforeLast('_');
	  path2=rootpath.BeforeLast('_');
	}
    }
  path1.Append(".book");
  bookText->SetValue(path1);
  path2.Append("_residual.wav");
  residualText->SetValue(path2);
}

bool MptkGuiSaveDialog::compareStrings(wxString str1,wxString str2)
{
  uint i;
  if(str1.Length()!=str2.Length()) return false;
  else
    {
      for(i=0;(i<str1.Length())&&(i<str2.Length());i++)
	{
	  if(!(str1.GetChar(i)==str2.GetChar(i))) return false;
	}
    }

  return true;
}

