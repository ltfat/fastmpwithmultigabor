#ifndef MPTKGUISAVEDIALOG_H
#define MPTKGUISAVEDIALOG_H


#include "wx/filename.h"
#include "wx/wx.h"

/**This dialog box allows the user to save audio signals and book
   in specified file name.*/
class MptkGuiSaveDialog : public wxDialog 
{
public :
  
  /** \brief Constructor
      \param defaultDirSave Default path for saved files*/
  MptkGuiSaveDialog(wxWindow *parent, wxWindowID id, wxString defaultDirSave);

  ~MptkGuiSaveDialog();
  
  /** \brief Returns the current value in 'book name' field*/
  wxString getBookName();

  /** \brief Return the current value in 'book name' field*/
  wxString getResidualName();

  /** \brief Returns the current value in 'approx name' field*/
  wxString getApproxName();

  /** \brief Returns the current default path for saved files*/
  wxString getDefaultDir();

  /** \brief Function called when the user presses the 'browse' button for book file save*/
  void OnBrowseBook(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses the 'browse' button for residual signal file save*/
  void OnBrowseResidual(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses the 'browse' button for reconstructed signal file save*/
  void OnBrowseApprox(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses 'Save' button */
  void OnSave(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses 'autofill' button for book file save
      It will cause the reconstructed signal file name field to take the value "<bookname>_rebuilt.wav"
      and the residual signal file name field to take the value "<bookname>_residual.wav", where
      <bookname> is the string before the last '.' of the book file name field.*/ 
  void OnAutofillBook(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses 'autofill' button for residual signal file save.
      It will cause the reconstructed signal file name field to take the value "<residualname>_rebuilt.wav"
      and the book file name field to take the value "<residualname>.book", where <residualname> is the string 
      before the last '.' or before '_residual.wav' of the residual signal file name field.*/ 
  void OnAutofillResidual(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses 'autofill' button for reconstructed signal file save.
    It will cause the reconstructed signal file name field to take the value "<reconstname>_rebuilt.wav"
    and the book file name field to take the value "<reconstname>.book", where <reconstname> is the string 
    before the last '.' or before '_rebuilt.wav' of the reconstructed signal file name field.*/ 
  void OnAutofillApprox(wxCommandEvent& WXUNUSED(event));

  /** \brief Function called when the user presses 'Cancel' button. Closes the dialog box*/
  void OnCancel(wxCommandEvent& WXUNUSED(event));

private :
  wxString bookName;
  wxString residualName;
  wxString approxName;
  wxString defaultDir;
  
  // Main panel
  wxPanel * panel;
  // Main sizer
  wxBoxSizer * sizer;
  wxStaticBoxSizer * stBoxBooksSizer;
  wxBoxSizer * buttonsSizer;

  // Book
  wxStaticBoxSizer * bookSizer;
  wxTextCtrl * bookText;
  wxButton * buttonBrowseBook;
  wxButton * buttonAutoFillBook;

  // Residu
  wxStaticBoxSizer * residualSizer;
  wxTextCtrl * residualText;
  wxButton * buttonBrowseResidual;
  wxButton * buttonAutoFillResidual;

  //Approximation
  wxStaticBoxSizer * approxSizer;
  wxTextCtrl * approxText;
  wxButton * buttonBrowseApprox;
  wxButton * buttonAutoFillApprox;

  // Buttons
  wxButton * buttonSave;
  wxButton * buttonCancel;

  // Private procedure for open a File Dialog, returning selected file's name
  wxString saveFileDialog(wxString title);

  //Private procedure to evaluate if two wxStrings are equal
  bool compareStrings(wxString str1,wxString str2);

DECLARE_EVENT_TABLE()
};

enum{
Ev_SaveDialog_Browse_Book = 1,
Ev_SaveDialog_Browse_Residual,
Ev_SaveDialog_Browse_Approx,
Ev_SaveDialog_Autofill_Book,
Ev_SaveDialog_Autofill_Residual,
Ev_SaveDialog_Autofill_Approx,
Ev_SaveDialog_Save,
Ev_SaveDialog_Cancel
};

#endif

  


  
