#include "MptkGuiOpenDialog.h"

BEGIN_EVENT_TABLE(MptkGuiOpenDialog, wxDialog)
	EVT_BUTTON(Ev_OpenDialog_Browse_Signal, MptkGuiOpenDialog::OnBrowseSignal)
	EVT_BUTTON(Ev_OpenDialog_Autofill_Signal, MptkGuiOpenDialog::OnAutofillSignal)
	EVT_BUTTON(Ev_OpenDialog_Browse_Book, MptkGuiOpenDialog::OnBrowseBook)
	EVT_BUTTON(Ev_OpenDialog_Autofill_Book, MptkGuiOpenDialog::OnAutofillBook)
	EVT_BUTTON(wxID_OPEN, MptkGuiOpenDialog::OnOpen)
END_EVENT_TABLE ()

// Creator
  MptkGuiOpenDialog::MptkGuiOpenDialog( wxWindow *parent, wxWindowID id, wxString defaultOpenSignal, wxString defaultOpenBook) : wxDialog( parent, id, _T("Open..."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("dialogBox") )
{
	// Interface
	panel = new wxPanel(this, wxID_ANY);
	sizer = new wxBoxSizer(wxVERTICAL);

	// Signal
	signalSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Signal"));
	signalText = new wxTextCtrl(panel, wxID_ANY);
	
	signalButtonsSizer = new wxBoxSizer(wxVERTICAL);
	signalButtonBrowse = new wxButton(panel, Ev_OpenDialog_Browse_Signal, _T("Browse..."));
	signalButtonAutofill = new wxButton(panel, Ev_OpenDialog_Autofill_Signal, _T("Autofill"));
	signalButtonsSizer->Add(signalButtonBrowse, 1, wxALL, 3);
	signalButtonsSizer->Add(signalButtonAutofill, 1, wxALL, 3);
	
	signalSizer->Add(signalText, 6, wxCENTER | wxALL, 3);
	signalSizer->Add(signalButtonsSizer, 2, wxEXPAND | wxALL, 3);
	

	// Book
	bookSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Book"));
	bookText = new wxTextCtrl(panel, wxID_ANY);
	
	bookButtonsSizer = new wxBoxSizer(wxVERTICAL);
	bookButtonBrowse = new wxButton(panel, Ev_OpenDialog_Browse_Book, _T("Browse..."));
	bookButtonAutofill = new wxButton(panel, Ev_OpenDialog_Autofill_Book, _T("Autofill"));
	bookButtonsSizer->Add(bookButtonBrowse, 1, wxEXPAND | wxALL, 3);
	bookButtonsSizer->Add(bookButtonAutofill, 1, wxEXPAND | wxALL, 3);

	bookSizer->Add(bookText, 6, wxCENTER | wxALL, 3);
	bookSizer->Add(bookButtonsSizer, 2, wxEXPAND | wxALL, 3);

	// Button Open & Cancel
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonOpen = new wxButton(panel, wxID_OPEN, _T("Open"));
	buttonCancel = new wxButton(panel, wxID_CANCEL, _T("Cancel"));

	buttonsSizer->Add(buttonOpen, 1, wxCENTER | wxALL, 10);
	buttonsSizer->Add(buttonCancel, 1, wxCENTER | wxALL, 10);
	
	sizer->Add(signalSizer, 2, wxEXPAND | wxALL, 3);
	sizer->Add(bookSizer, 2, wxEXPAND | wxALL, 3);
	sizer->Add(buttonsSizer, 1, wxCENTER | wxALL, 3);

	panel->SetAutoLayout( TRUE );
	panel->SetSizer(sizer);
    sizer->Fit(panel);
    sizer->SetSizeHints(panel);
	SetSizeHints(panel->GetSize());

	// Initialisation
	signalName = "";
	bookName = "";
	defaultDirSignal = defaultOpenSignal;
	defaultDirBook = defaultOpenBook;
	autoFillSignal = "";
	autoFillBook = "";
}

MptkGuiOpenDialog::~MptkGuiOpenDialog()
{
	delete panel;
}

wxString MptkGuiOpenDialog::getSignalName()
{
	return signalName;
}

wxString MptkGuiOpenDialog::getBookName()
{
	return bookName;
}

wxString MptkGuiOpenDialog::getDefaultDirSignal()
{
  return defaultDirSignal;
}

wxString MptkGuiOpenDialog::getDefaultDirBook()
{
  return defaultDirBook;
}

// Autofill procedure
// If type == 1 : generate the autoFillBook
// If type == 2 : generate the autoFillSignal
void MptkGuiOpenDialog::autoFill(int type, wxString fileName, wxString dirName)
{
	// Substract length of dirName to fileName
	wxString sub = fileName.Mid(dirName.Length());
	if (sub.Length() != 0){
		if(sub.Matches("*.*")){
		// Search char "." from the end
		size_t index = sub.Find('.',true);
		//Substract extension
		sub = sub.Mid((size_t) 0, index);
		}
		if(type == 1) {// Generate autoFillBook
		autoFillBook = dirName + sub + "Book.bin";
		}
		if(type == 2) {// Generate autoFillSignal
		autoFillSignal = dirName + sub + "Signal.wav";
		}	
	}
}

// Event procedures

void MptkGuiOpenDialog::OnBrowseSignal(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a signal"),
						defaultDirSignal,
						"",
						"*",
						wxOPEN,
						wxDefaultPosition);
	if (openFileDialog->ShowModal()== wxID_OK) {
	  wxString fileName = openFileDialog->GetPath();

	  if (fileName != ""){
		defaultDirSignal = openFileDialog->GetDirectory();
		signalText->SetValue(fileName);
		signalText->SetInsertionPointEnd();
		autoFill(1, fileName, defaultDirSignal);
	  }
	}
}

void MptkGuiOpenDialog::OnBrowseBook(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a book"),
						defaultDirBook,
						"",
						"*",
						wxOPEN,
						wxDefaultPosition);
	if (openFileDialog->ShowModal()== wxID_OK){
	  wxString fileName = openFileDialog->GetPath();

	  if (fileName != ""){
		defaultDirBook = openFileDialog->GetDirectory();
		bookText->SetValue(fileName);
		bookText->SetInsertionPointEnd();
		autoFill(2, fileName, defaultDirBook);
	  }
	}
}

void MptkGuiOpenDialog::OnAutofillSignal(wxCommandEvent& WXUNUSED(event))
{
	if (autoFillSignal !="") {
		signalText->SetValue(autoFillSignal);
		signalText->SetInsertionPointEnd();
	}
}

void MptkGuiOpenDialog::OnAutofillBook(wxCommandEvent& WXUNUSED(event))
{
	if (autoFillBook !="") {
		bookText->SetValue(autoFillBook);
		bookText->SetInsertionPointEnd();
	}
}

void MptkGuiOpenDialog::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	signalName = signalText->GetValue();
	bookName = bookText->GetValue();
	EndModal(wxID_OK);
}
void MptkGuiOpenDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_CANCEL);
}
