#include "MptkGuiLicenseDialog.h"
#include "gpl.h"

BEGIN_EVENT_TABLE(MptkGuiLicenseDialog, wxDialog)
	EVT_BUTTON(wxID_OK, MptkGuiLicenseDialog::OnOK)
END_EVENT_TABLE ()

MptkGuiLicenseDialog::MptkGuiLicenseDialog(wxWindow * parent) : 
	wxDialog( parent, wxID_ANY, _T("License"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("dialogBox") )
{
        extern const char* gplText;

	// Creation 
	panel = new wxPanel(this, wxID_ANY);
	sizer = new wxBoxSizer(wxVERTICAL);
	
	textCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE);
	textCtrl->SetValue(wxT(gplText));
	
	buttonOK = new wxButton(panel, wxID_OK, wxT("OK"));
	
	// Add controls
	panel->SetSizer(sizer);
	sizer->Add(textCtrl, 1, wxEXPAND | wxALL, 4);
	sizer->Add(buttonOK, 0, wxCENTER, 4);
	
	SetSize(600,600);
}

MptkGuiLicenseDialog::~MptkGuiLicenseDialog()
{
	delete panel;
}
void MptkGuiLicenseDialog::OnOK(wxCommandEvent&  WXUNUSED(event))
{
	EndModal(wxID_OK);
}
