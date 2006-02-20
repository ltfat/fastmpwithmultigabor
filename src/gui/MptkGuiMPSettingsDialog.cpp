#include "MptkGuiMPSettingsDialog.h"
#include "MptkGuiFrame.h"

BEGIN_EVENT_TABLE(MptkGuiMPSettingsDialog, wxDialog)
  EVT_BUTTON(Ev_MPSettings_Browse_dico, MptkGuiMPSettingsDialog::OnBrowseDico)
  EVT_BUTTON(Ev_MPSettings_Browse_book, MptkGuiMPSettingsDialog::OnBrowseBook)
  EVT_BUTTON(Ev_MPSettings_Browse_residual, MptkGuiMPSettingsDialog::OnBrowseResidual)
  EVT_BUTTON(Ev_MPSettings_Browse_decay, MptkGuiMPSettingsDialog::OnBrowseDecay)
  EVT_CHECKBOX(Ev_MPSettings_check_nbIter,MptkGuiMPSettingsDialog::OnCheckBoxNbIterClick)
  EVT_CHECKBOX(Ev_MPSettings_check_Snr,MptkGuiMPSettingsDialog::OnCheckBoxSnrClick)
  EVT_CHECKBOX(Ev_MPSettings_check_Output,MptkGuiMPSettingsDialog::OnCheckBoxOutputClick)
  EVT_BUTTON(wxID_APPLY,MptkGuiMPSettingsDialog::OnApply)
  EVT_BUTTON(wxID_OK,MptkGuiMPSettingsDialog::OnOK)
  EVT_BUTTON(wxID_CANCEL,MptkGuiMPSettingsDialog::OnCancel)
END_EVENT_TABLE ()

// Creator
  MptkGuiMPSettingsDialog::MptkGuiMPSettingsDialog(wxWindow *parent, wxWindowID id,struct initMPSettingStruct init,MptkGuiCallback * control)
    : wxDialog(parent,id,_T("Matching Pursuit settings"),wxDefaultPosition,wxDefaultSize,wxCLOSE_BOX | wxSYSTEM_MENU)
{
	
	// Interface
	panel = new wxPanel(this, wxID_ANY);
	sizer = new wxBoxSizer(wxVERTICAL);

	// Dictionary
	dicoSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Dictionary"));
	dicoText = new wxTextCtrl(panel, wxID_ANY);
	dicoButtonBrowse = new wxButton(panel, Ev_MPSettings_Browse_dico, _T("Browse..."));
	dicoSizer->Add(dicoText, 1, wxCENTER | wxALL, 3);
	dicoSizer->Add(dicoButtonBrowse, 0, wxCENTER | wxALL, 3);
	

	// Stop Condition
	stopCondSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, _T("Stop conditions"));
	// check boxes
	checkBoxesSizer = new wxBoxSizer(wxVERTICAL);
	nbIterCheckBox = new wxCheckBox(panel,Ev_MPSettings_check_nbIter , _T("After a iteration number"));
	snrCheckBox = new wxCheckBox(panel, Ev_MPSettings_check_Snr, _T("SNR (DB of reached energy)"));
	checkBoxesSizer->Add(nbIterCheckBox, 0, wxALL, 3);
	checkBoxesSizer->Add(snrCheckBox, 0,  wxALL, 3);
	// Values of conditions
	valuesSizer = new wxBoxSizer(wxVERTICAL);
	nbIterValueText = new wxTextCtrl(panel, wxID_ANY);
	snrValueText = new wxTextCtrl(panel, wxID_ANY);
	valuesSizer->Add(nbIterValueText, 0,  wxALL, 3);
	valuesSizer->Add(snrValueText, 0,  wxALL, 3);
	// Add sizers
	stopCondSizer->Add(checkBoxesSizer, 0, wxCENTER | wxALL, 3);
	stopCondSizer->Add(valuesSizer, 0, wxCENTER | wxALL, 3);

	//Intermediate save
	saveSizer=new wxStaticBoxSizer(wxVERTICAL, panel, _T("Intermediate saves"));
	//text
	debutText=new wxStaticText(panel, wxID_ANY, _T("Save intermediate results every :"));
	textSizer= new wxBoxSizer(wxHORIZONTAL);
	nbIterSave =  new wxTextCtrl(panel, wxID_ANY);
	finText = new wxStaticText(panel, wxID_ANY, _T("iterations"));
	textSizer->Add(nbIterSave,0, wxALL, 3);
	textSizer->Add(finText,0, wxALL, 3);

	allSaveSizer =new wxFlexGridSizer(3,3,0,0);
	//book
	book= new wxStaticText(panel, wxID_ANY, _T("Book :"));
	bookSave= new wxTextCtrl(panel, wxID_ANY);
	bookButtonBrowse= new wxButton(panel, Ev_MPSettings_Browse_book, _T("Browse..."));
	allSaveSizer->Add(book, 0, wxCENTER | wxALL, 3);
	allSaveSizer->Add(bookSave, 1, wxEXPAND| wxALL, 6);
	allSaveSizer->Add(bookButtonBrowse, 0, wxCENTER | wxALL, 3);

	//residual
	residual = new wxStaticText(panel, wxID_ANY, _T("residual :"));
	residualSave= new wxTextCtrl(panel, wxID_ANY);
	residualButtonBrowse = new wxButton(panel, Ev_MPSettings_Browse_residual, _T("Browse..."));
	allSaveSizer->Add(residual, 0, wxCENTER | wxALL, 3);
	allSaveSizer->Add(residualSave, 1, wxEXPAND| wxALL, 6);
	allSaveSizer->Add(residualButtonBrowse, 0, wxCENTER |  wxALL, 3);

	//decay
	decay= new wxStaticText(panel, wxID_ANY, _T("Energy decay :"));
	decaySave= new wxTextCtrl(panel, wxID_ANY);
	decayButtonBrowse= new wxButton(panel, Ev_MPSettings_Browse_decay, _T("Browse..."));
	allSaveSizer->Add(decay, 0, wxCENTER | wxALL, 3);
	allSaveSizer->Add(decaySave, 1, wxEXPAND| wxALL, 6);
	allSaveSizer->Add(decayButtonBrowse, 0, wxCENTER | wxALL, 3);
	allSaveSizer->AddGrowableCol(1);
	//add all in saveSizer
	saveSizer->Add(debutText,0, wxEXPAND | wxALL, 3);
	saveSizer->Add(textSizer,0, wxEXPAND | wxALL, 3);
	saveSizer->Add(allSaveSizer,1, wxEXPAND | wxALL, 3);

	//output setting
	outputSizer = new wxStaticBoxSizer(wxVERTICAL, panel, _T("Console output"));
	outputProgressSizer = new wxBoxSizer(wxHORIZONTAL);
	reportprogress =  new wxCheckBox(panel, Ev_MPSettings_check_Output, _T("Report progress every "));
	nbIterOutput =  new wxTextCtrl(panel, wxID_ANY);
	IterationText = new wxStaticText(panel, wxID_ANY, _T("iterations"));
	refreshCheck = new wxCheckBox(panel, -1, _T("refresh view "));
	outputProgressSizer->Add(reportprogress,0,wxEXPAND | wxALL, 3);
	outputProgressSizer->Add(nbIterOutput,0,wxEXPAND | wxALL, 3);
	outputProgressSizer->Add(IterationText,0,wxEXPAND | wxALL, 3);
	outputProgressSizer->Add(refreshCheck,0,wxEXPAND|wxALL,3);
	outputVerboseSizer = new wxBoxSizer(wxHORIZONTAL);
	verbose = new wxRadioButton(panel, -1, _T("Verbose "));
	quiet = new wxRadioButton(panel, -1, _T("Quiet "));
	normal = new wxRadioButton(panel, -1, _T("Normal "));
	outputVerboseSizer->Add(normal,0,wxEXPAND | wxALL, 3);
	outputVerboseSizer->Add(verbose,0,wxEXPAND | wxALL, 3);
	outputVerboseSizer->Add(quiet,0,wxEXPAND | wxALL, 3);
	outputSizer->Add(outputProgressSizer,0,wxEXPAND | wxALL, 3);
	outputSizer->Add(outputVerboseSizer,0,wxEXPAND | wxALL, 3);

	// Button OK & Cancel
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonOK = new wxButton(panel, wxID_OK, _T("OK"));
	buttonApply= new wxButton(panel, wxID_APPLY, _T("Apply"));
	buttonCancel = new wxButton(panel, wxID_CANCEL, _T("Cancel"));

	buttonsSizer->Add(buttonOK, 0, wxCENTER | wxALL, 10);
	buttonsSizer->Add(buttonApply, 0, wxCENTER | wxALL, 10);
	buttonsSizer->Add(buttonCancel, 0, wxCENTER | wxALL, 10);
	
	sizer->Add(dicoSizer, 0, wxEXPAND | wxALL, 3);
	sizer->Add(stopCondSizer, 0, wxEXPAND | wxALL, 3);
	sizer->Add(saveSizer, 0, wxEXPAND | wxALL, 3);
	sizer->Add(outputSizer, 0, wxEXPAND | wxALL, 3);
	sizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, 3);

	panel->SetAutoLayout( TRUE );
	panel->SetSizer(sizer);
    	sizer->Fit(panel);
    	sizer->SetSizeHints(panel);
	SetSizeHints(panel->GetSize());

	// Initialization
	defaultDicoDir = init.defaultDicoDir;
	dicoText->SetValue(init.defaultDicoName);
	nbIterCheckBox->SetValue(init.defaultNbIterCheck);
	snrCheckBox->SetValue(init.defaultSNRCheck);
	nbIterValueText->SetValue(wxString::Format("%li",init.defaultNbIterValue));
	snrValueText->SetValue(wxString::Format("%lf",init.defaultSNRValue));
	nbIterSave->SetValue(wxString::Format("%li",init.defautNbIterSaveValue));
	bookSave->SetValue(init.defaultBookName);
	residualSave->SetValue(init.defaultResidualName);
	decaySave->SetValue(init.defaultDecayName);
	defaultBookDir=init.defaultBookDir;if (!reportprogress->IsChecked()) {refreshCheck->Enable(false);nbIterOutput->Enable(false);}
	defaultResidualDir=init.defaultResidualDir;
	defaultDecayDir=init.defaultDecayDir;
	reportprogress->SetValue(init.defaultNbIterOutputCheck);
	refreshCheck->SetValue(init.defaultNbIterRefreshCheck);
	nbIterOutput->SetValue(wxString::Format("%li",init.defaultNbIterOutputValue));
	verbose->SetValue(init.defaultVerbose);
	quiet->SetValue(init.defaultQuiet);
	if (!reportprogress->IsChecked()) {refreshCheck->Enable(false);nbIterOutput->Enable(false);}

	this->control=control;
}


// Event procedures

void MptkGuiMPSettingsDialog::OnBrowseDico(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a dictionary"),
						defaultDicoDir,
						"",
						"*",
						wxOPEN,
						wxDefaultPosition);
	if (openFileDialog->ShowModal()== wxID_OK) {
	  wxString file = openFileDialog->GetPath();
	  if (file != _T("")){
	    if ( wxFileName::FileExists( file ) ){
	      defaultDicoDir = openFileDialog->GetDirectory();
	      dicoText->SetValue(file);
	      dicoText->SetInsertionPointEnd();
	    }
	    else wxMessageBox(wxT("File " + file +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
	  }
	}
}

void MptkGuiMPSettingsDialog::OnBrowseBook(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a dictionary"),
						defaultBookDir,
						"",
						"*",
						wxSAVE,
						wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK){
	  wxString fileName = openFileDialog->GetPath();

	  if (fileName != ""){
		bookSave->SetValue(fileName);
		bookSave->SetInsertionPointEnd();
	  }
	}
}

void MptkGuiMPSettingsDialog::OnBrowseResidual(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a dictionary"),
						defaultResidualDir,
						"",
						"*",
						wxSAVE,
						wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK){
	  wxString fileName = openFileDialog->GetPath();

	  if (fileName != ""){
		residualSave->SetValue(fileName);
		residualSave->SetInsertionPointEnd();
	  }
	}
}

void MptkGuiMPSettingsDialog::OnBrowseDecay(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a dictionary"),
						defaultDecayDir,
						"",
						"*",
						wxSAVE,
						wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK){
	  wxString fileName = openFileDialog->GetPath();

	  if (fileName != ""){
		decaySave->SetValue(fileName);
		decaySave->SetInsertionPointEnd();
	  }
	}
}


void MptkGuiMPSettingsDialog::OnCheckBoxSnrClick(wxCommandEvent& WXUNUSED(event))
{
  if (!snrCheckBox->IsChecked()) {nbIterCheckBox->SetValue(true);}
}

void MptkGuiMPSettingsDialog::OnCheckBoxNbIterClick(wxCommandEvent& WXUNUSED(event))
{
  if (!nbIterCheckBox->IsChecked()) {snrCheckBox->SetValue(true);}
}

void MptkGuiMPSettingsDialog::OnCheckBoxOutputClick(wxCommandEvent& WXUNUSED(event))
{
  if (!reportprogress->IsChecked()) {refreshCheck->Enable(false);nbIterOutput->Enable(false);}
  else {refreshCheck->Enable(true);nbIterOutput->Enable(true);}
}

void MptkGuiMPSettingsDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  if (apply()) Hide();
}

void MptkGuiMPSettingsDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
  apply();
}

void MptkGuiMPSettingsDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  Hide();
}

void MptkGuiMPSettingsDialog::update()
{
  nbIterValueText->SetValue(wxString::Format("%i",control->getIterationValue()));
  snrValueText->SetValue(wxString::Format("%f",10*log10(control->getSNRValue())));
  nbIterCheckBox->SetValue(control->getIterCheck());
  snrCheckBox->SetValue(control->getSNRCheck());
}


// Returns true if valid parameters
bool MptkGuiMPSettingsDialog::apply()
{
  long int nbIter,nbIterS,nbIterO;
  double snr;
  
  if (wxFileName::FileExists(dicoText->GetValue())){ control->setDictionary(dicoText->GetValue());}
  else {
    wxMessageBox(wxT("Dictionary " + dicoText->GetValue() +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
    return false;
  }

  if (nbIterCheckBox->IsChecked()) {nbIterValueText->GetValue().ToLong(&nbIter);control->setIterationNumber(nbIter);} 
  else {control->unsetIterationNumber();}
  
  if (snrCheckBox->IsChecked()) {snrValueText->GetValue().ToDouble(&snr);control->setSNR(snr);} 
  else {control->unsetSNR();}
  
  nbIterSave->GetValue().ToLong(&nbIterS);
  if (nbIterS!=0) {
    if (bookSave->GetValue() != _T("") &&
	!wxFileName::DirExists( (wxFileName::FileName( bookSave->GetValue())).GetPath())){
      wxMessageBox(wxT("Directory of " + bookSave->GetValue() +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
      return false;
    }
    else if (residualSave->GetValue() != _T("") &&
	     !wxFileName::DirExists( (wxFileName::FileName( residualSave->GetValue())).GetPath())){
     wxMessageBox(wxT("Directory of " + residualSave->GetValue() +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
     return false;
    }
    else if (decaySave->GetValue() != _T("") &&
	     !wxFileName::DirExists( (wxFileName::FileName( decaySave->GetValue())).GetPath())){
     wxMessageBox(wxT("Directory of " + decaySave->GetValue() +" doesn't exist"), wxT("Error"), wxICON_ERROR | wxOK);
     return false;
    }
    control->setSave(nbIterS,bookSave->GetValue(),residualSave->GetValue(),decaySave->GetValue());} 
  else {control->unsetSave();}
  
  if (reportprogress->IsChecked()) 
    {
      nbIterOutput->GetValue().ToLong(&nbIterO);
      control->setReport(nbIterO);
      if (refreshCheck->IsChecked()) {control->setAllHandler();}
      else  {control->setProgressHandler();}
    } 
  else  
    {control->unsetReport();}
  
  if (verbose->GetValue()) {control->verbose();} else {if (quiet->GetValue()) {control->quiet();} 
  else {control->normal();}}
  
  if (control->canIterate()) {
  MptkGuiSettingEvent * evt = 	new MptkGuiSettingEvent();
  GetParent()->ProcessEvent(*evt);
  delete evt;
  }
  MptkGuiSettingUpdateEvent * evt = new MptkGuiSettingUpdateEvent();
  GetParent()->ProcessEvent(*evt);
  delete evt;
  return true;
}

initMPSettingStruct MptkGuiMPSettingsDialog::save()
{
  long int nbIter,nbIterS,nbIterO;
  double snr;
  initMPSettingStruct result;
  result.defaultDicoDir=defaultDicoDir;
  result.defaultDicoName=dicoText->GetValue();
  result.defaultNbIterCheck=nbIterCheckBox->IsChecked();
  result.defaultSNRCheck=snrCheckBox->IsChecked();
  nbIterValueText->GetValue().ToLong(&nbIter);
  result.defaultNbIterValue=nbIter;
  snrValueText->GetValue().ToDouble(&snr);
  result.defaultSNRValue=snr;
  nbIterSave->GetValue().ToLong(&nbIterS);
  result.defautNbIterSaveValue=nbIterS;
  result.defaultBookDir=defaultBookDir;
  result.defaultBookName=bookSave->GetValue();
  result.defaultResidualDir=defaultResidualDir;
  result.defaultResidualName=residualSave->GetValue();
  result.defaultDecayDir=defaultDecayDir;
  result.defaultDecayName=decaySave->GetValue();
  result.defaultNbIterOutputCheck=reportprogress->IsChecked();
  result.defaultNbIterRefreshCheck=refreshCheck->IsChecked();
  nbIterOutput->GetValue().ToLong(&nbIterO);
  result.defaultNbIterOutputValue=nbIterO;
  result.defaultVerbose=verbose->GetValue();
  result.defaultQuiet=quiet->GetValue();
  result.defaultNormal=normal->GetValue();
  return result;
}
