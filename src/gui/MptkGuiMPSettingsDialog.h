#ifndef MPTKGUIMPSETTINGSDIALOG_H
#define MPTKGUIMPSETTINGSDIALOG_H

#include "wx/wx.h"
#include "wx/filename.h"
#include "mptk.h"
#include "MptkGuiCallback.h"
#include "MptkGuiSettingEvent.h"
#include "MptkGuiSettingUpdateEvent.h"

struct initMPSettingStruct{
  wxString defaultDicoDir; 
  wxString defaultDicoName;
  bool defaultNbIterCheck; 
  bool defaultSNRCheck;
  long int defaultNbIterValue;
  double defaultSNRValue;
  long int defautNbIterSaveValue;
  wxString defaultBookDir; 
  wxString defaultBookName;
  wxString defaultResidualDir;
  wxString defaultResidualName;
  wxString defaultDecayDir;
  wxString defaultDecayName;
  bool defaultNbIterOutputCheck;
  bool defaultNbIterRefreshCheck;
  long int defaultNbIterOutputValue;
  bool defaultVerbose;
  bool defaultQuiet;
  bool defaultNormal;
};

/**
 * \brief Allows user to set the parameters of the decomposition
 */

class MptkGuiMPSettingsDialog : public wxDialog 
{
public :

  MptkGuiMPSettingsDialog(wxWindow *parent, wxWindowID id,struct initMPSettingStruct init, MptkGuiCallback * control);

  void OnBrowseDico(wxCommandEvent& WXUNUSED(event));
  void OnBrowseBook(wxCommandEvent& WXUNUSED(event));
  void OnBrowseResidual(wxCommandEvent& WXUNUSED(event));
  void OnBrowseDecay(wxCommandEvent& WXUNUSED(event));
  void OnCheckBoxSnrClick(wxCommandEvent& WXUNUSED(event));
  void OnCheckBoxNbIterClick(wxCommandEvent& WXUNUSED(event));
  void OnCheckBoxOutputClick(wxCommandEvent& WXUNUSED(event));
  void OnOK(wxCommandEvent& WXUNUSED(event));
  void OnApply(wxCommandEvent& WXUNUSED(event));
  void OnCancel(wxCommandEvent& WXUNUSED(event));
  void update();
  bool apply();
  initMPSettingStruct save();

	wxCheckBox * nbIterCheckBox;
	wxCheckBox * snrCheckBox;
	wxTextCtrl * nbIterValueText;
	wxTextCtrl * snrValueText;

private :

        MptkGuiCallback * control;

	wxBoxSizer * sizer;
	wxPanel * panel;

	wxStaticBoxSizer * dicoSizer;
	wxTextCtrl * dicoText;
	wxButton * dicoButtonBrowse;

	wxStaticBoxSizer * stopCondSizer;
	wxBoxSizer * checkBoxesSizer;
	wxBoxSizer * valuesSizer;

	wxStaticBoxSizer * saveSizer;
	wxBoxSizer * textSizer;
	wxStaticText * debutText;
	wxTextCtrl * nbIterSave;
	wxStaticText * finText;
	wxFlexGridSizer * allSaveSizer;
	wxTextCtrl * bookSave;
	wxTextCtrl * residualSave;
	wxTextCtrl * decaySave;
	wxButton * bookButtonBrowse;
	wxButton * residualButtonBrowse;
	wxButton * decayButtonBrowse;
	wxStaticText * book;
	wxStaticText * residual;
	wxStaticText * decay;

	wxStaticBoxSizer * outputSizer;
	wxBoxSizer * outputProgressSizer;
	wxCheckBox * reportprogress;
        wxCheckBox * refreshCheck;
	wxTextCtrl * nbIterOutput;
	wxStaticText * IterationText;
	wxBoxSizer * outputVerboseSizer;
	wxRadioButton * verbose;
	wxRadioButton * quiet;
	wxRadioButton * normal;

	wxBoxSizer * buttonsSizer;
	wxButton * buttonOK;
        wxButton * buttonApply;
	wxButton * buttonCancel;

	wxString defaultDicoDir;
        wxString defaultBookDir;
        wxString defaultResidualDir;
        wxString defaultDecayDir;
  

DECLARE_EVENT_TABLE()
};

enum{
  Ev_MPSettings_Browse_dico = 1,
    Ev_MPSettings_Browse_book ,
    Ev_MPSettings_Browse_residual,
    Ev_MPSettings_Browse_decay,
    Ev_MPSettings_check_Snr,
    Ev_MPSettings_check_nbIter,
    Ev_MPSettings_check_Output
};

#endif
