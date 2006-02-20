#ifndef MPTKGUILICENSE_H
#define MPTKGUILICENSE_H

#include "wx/wx.h"

/**
 * \brief Shows the GPL in a dialob box
 */
class MptkGuiLicenseDialog : public wxDialog 
{
public :
	MptkGuiLicenseDialog(wxWindow *parent);
	
	~MptkGuiLicenseDialog();
	
	void OnOK(wxCommandEvent& event);
	
private :
	wxBoxSizer * sizer;
	wxPanel * panel;
	
	wxTextCtrl * textCtrl;
	
	wxButton * buttonOK;

DECLARE_EVENT_TABLE()
};

#endif
