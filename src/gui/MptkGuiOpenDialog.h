#ifndef MPTKGUIOPENDIALOG_H
#define MPTKGUIOPENDIALOG_H

#include "wx/wx.h"

/**
 * \brief Allows user to select the signal and the book to open
 */
class MptkGuiOpenDialog : public wxDialog 
{
public :

  MptkGuiOpenDialog(wxWindow *parent, wxWindowID id, wxString defaultOpenSignal, wxString defaultOpenBook);

  ~MptkGuiOpenDialog();
  
	wxString getSignalName();
	wxString getBookName();
	wxString getDefaultDirSignal();
	wxString getDefaultDirBook();

	void OnBrowseSignal(wxCommandEvent& WXUNUSED(event));
	void OnAutofillSignal(wxCommandEvent& WXUNUSED(event));
	void OnBrowseBook(wxCommandEvent& WXUNUSED(event));
	void OnAutofillBook(wxCommandEvent& WXUNUSED(event));
	void OnOpen(wxCommandEvent& WXUNUSED(event));
	void OnCancel(wxCommandEvent& WXUNUSED(event));

private :
	wxBoxSizer * sizer;
	wxPanel * panel;

	wxStaticBoxSizer * signalSizer;
	wxBoxSizer * signalButtonsSizer;
	wxTextCtrl * signalText;
	wxButton * signalButtonBrowse;
	wxButton * signalButtonAutofill;

	wxStaticBoxSizer * bookSizer;
	wxBoxSizer * bookButtonsSizer;
	wxTextCtrl * bookText;
	wxButton * bookButtonBrowse;
	wxButton * bookButtonAutofill;

	wxBoxSizer * buttonsSizer;
	wxButton * buttonOpen;
	wxButton * buttonCancel;

	wxString signalName;
	wxString bookName;
	wxString defaultDirSignal;
	wxString defaultDirBook;
	wxString autoFillSignal;
	wxString autoFillBook;

	void autoFill(int type, wxString fileName, wxString dirName);

DECLARE_EVENT_TABLE()
};

enum{
Ev_OpenDialog_Browse_Signal = 1,
Ev_OpenDialog_Autofill_Signal,
Ev_OpenDialog_Browse_Book,
Ev_OpenDialog_Autofill_Book
};

#endif
