#include "MptkGuiFrame.h"
#include "gpl.h"

#include "bitmaps/open.xpm"
#include "bitmaps/open_signal.xpm"
#include "bitmaps/open_book.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/stop.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/iterate_once.xpm"
#include "bitmaps/iterate_all.xpm"
#include "bitmaps/iterate_stop.xpm"
#include "bitmaps/Nb.xpm"
#include "bitmaps/SNR.xpm"

BEGIN_EVENT_TABLE(MptkGuiFrame, wxFrame)
  EVT_MENU(Ev_File_Open, MptkGuiFrame::OnFileOpen)
  EVT_MENU(Ev_File_OpenSignal, MptkGuiFrame::OnFileOpenSignal)
  EVT_MENU(Ev_File_OpenBook, MptkGuiFrame::OnFileOpenBook)
  EVT_MENU(Ev_File_Save, MptkGuiFrame::OnFileSave)
  EVT_MENU(Ev_File_Close, MptkGuiFrame::OnFileClose)
  EVT_MENU(Ev_File_Quit, MptkGuiFrame::OnFileQuit)
  
  EVT_MENU(Ev_View_NewSignal, MptkGuiFrame::OnViewNewSignal)
  EVT_MENU(Ev_View_NewAtom, MptkGuiFrame::OnViewNewAtom)
  EVT_MENU(Ev_View_NewSpectrogram, MptkGuiFrame::OnViewNewSpectrogram)
  EVT_MENU(Ev_View_Console, MptkGuiFrame::OnViewConsole)
  EVT_MENU(Ev_View_Mask, MptkGuiFrame::OnViewMask)
  EVT_MENU(Ev_View_Settings, MptkGuiFrame::OnViewSettings)
  
  EVT_MENU(Ev_Listen_BaseSignal, MptkGuiFrame::OnListenBaseSignal)
  EVT_MENU(Ev_Listen_ApproximantSignal, MptkGuiFrame::OnListenApproximantSignal)
  EVT_MENU(Ev_Listen_ResidualSignal, MptkGuiFrame::OnListenResidualSignal)
  EVT_MENU(Ev_Listen_Play, MptkGuiFrame::OnListenPlay)
  EVT_MENU(Ev_Listen_Pause, MptkGuiFrame::OnListenPause)
  EVT_MENU(Ev_Listen_Stop, MptkGuiFrame::OnListenStop)
  
  EVT_MENU(Ev_Mask_LoadExistingMask, MptkGuiFrame::OnMaskLoadExistingMask)
  EVT_MENU(Ev_Mask_SelectAllAtoms, MptkGuiFrame::OnMaskSelectAllAtoms)
  EVT_MENU(Ev_Mask_DeselectAllAtoms, MptkGuiFrame::OnMaskDeselectAllAtoms)
  EVT_MENU(Ev_Mask_InvertCurrentMask, MptkGuiFrame::OnMaskInvertCurrentMask)
  EVT_MENU(Ev_Mask_AddCurrentSelectionToMask, MptkGuiFrame::OnMaskAddCurrentSelectionToMask)
  EVT_MENU(Ev_Mask_SaveMask, MptkGuiFrame::OnMaskSaveMask)
  
  EVT_MENU(Ev_Decomposition_IterateOnce, MptkGuiFrame::OnDecompositionIterateOnce)
  EVT_MENU(Ev_Decomposition_IterateAll, MptkGuiFrame::OnDecompositionIterateAll)
  EVT_MENU(Ev_Decomposition_Stop_Iteration, MptkGuiFrame::OnDecompositionStopIteration)
  EVT_MENU(Ev_Decomposition_Settings, MptkGuiFrame::OnDecompositionSettings)

  EVT_MENU(Ev_Check_Nb, MptkGuiFrame::OnCheckNb)
  EVT_MENU(Ev_Check_SNR, MptkGuiFrame::OnCheckSNR)
  EVT_COMBOBOX(Ev_Combo_Nb, MptkGuiFrame::OnComboNb)
  EVT_COMBOBOX(Ev_Combo_SNR, MptkGuiFrame::OnComboSNR)
  
  EVT_MENU(Ev_Help_License, MptkGuiFrame::OnHelpLicense)
  EVT_MENU(Ev_Help_About, MptkGuiFrame::OnHelpAbout)
  
  EVT_ZOOM(MptkGuiFrame::OnZoom)
  EVT_DELETE_VIEW(MptkGuiFrame::OnDeleteView)
  EVT_UP_VIEW(MptkGuiFrame::OnUpView)
  EVT_DOWN_VIEW(MptkGuiFrame::OnDownView)
  EVT_SASH_DRAGGED(wxID_ANY, MptkGuiFrame::OnDrag)
  EVT_LISTEN_FINISH(MptkGuiFrame::OnFinishedListen)
  EVT_SETTING(MptkGuiFrame::OnMPSettingDone)
  EVT_UPDATESETTING(MptkGuiFrame::OnMPSettingApply)
  END_EVENT_TABLE()

// A unique console
  MptkGuiConsoleView * MPTK_GUI_CONSOLE;

// The global status bar
  wxStatusBar * MPTK_GUI_STATUSBAR;

// Creator
  MptkGuiFrame::MptkGuiFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
			     long style)
    : wxFrame((wxWindow *) NULL, wxID_ANY, title, pos, size, style)
{
  //Initialization
  listenSignalType = BASE_SIGNAL;
  control = new MptkGuiCallback(this);
  subMenuChannel = NULL;
  nbView = 0;
  vectViews = new std::vector<MptkGuiSashWindow *>();
  initializeProperties();
  
  mpSettingsDialog = new MptkGuiMPSettingsDialog(this, wxID_ANY, mpSetting,control);
  limitsSettingsDialog = new MptkGuiLimitsSettingsDialog(this, wxID_ANY,limitsSetting);

  // Build various menus
  buildMenus();

  // Main panel
  m_panel = new wxScrolledWindow(this, wxID_ANY);
  m_panel->SetScrollbars(20, 20, 50, 50);
  m_sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_panel->SetSizer(m_sizerFrame);

  m_sizerFrame->Fit(m_panel);
  m_sizerFrame->SetSizeHints(this);
	
  // Create the toolBar
  createToolBar();

  // Create the status bar
  createStatusBar();
  MPTK_GUI_STATUSBAR = GetStatusBar();	

  // Build the console
  buildConsole();

  // Disable some menus etc
  disableActions();
}

// Destructor
MptkGuiFrame::~MptkGuiFrame()
{
  saveProperties();
}

void MptkGuiFrame::buildConsole()
{
  extern const char* gplText;

// Create the console
  sashConsole = new MptkGuiSashWindow(m_panel, nbView);
  MPTK_GUI_CONSOLE = new MptkGuiConsoleView(sashConsole, nbView, this);
  nbView++;
  // Add the console to the frame
  m_sizerFrame->Add(sashConsole, 0, wxEXPAND | wxALL, 1);
  // Show the license
  MPTK_GUI_CONSOLE->appendText(wxT(gplText));
  MPTK_GUI_CONSOLE->appendText(wxT("---- Console: End of the GNU Public License.\n"));
  MPTK_GUI_CONSOLE->showPosition( 345, 0 );
}


// Initialize program variables from "mptkGuiProperties" file if it exists
void MptkGuiFrame::initializeProperties()
{

  config = new wxFileConfig(wxEmptyString, wxEmptyString, wxT("mptkGuiConfig"), wxT("mptkGuiConfig"), wxCONFIG_USE_RELATIVE_PATH); 

  config->Read( wxT("defaultDirOpenSignal"),  &(defaultDirOpenSignal), wxT("") );
  config->Read(wxT("defaultDirOpenBook"), &defaultDirOpenBook, wxT(""));
  config->Read(wxT("defaultDirSave"), &defaultDirSave, wxT(""));
  
  config->Read(wxT("defaultDicoDir"), &(mpSetting.defaultDicoDir), wxT(""));
  config->Read(wxT("defaultDicoName"), &(mpSetting.defaultDicoName), wxT(""));
  config->Read(wxT("defaultNbIterCheck"), &(mpSetting.defaultNbIterCheck), true);
  config->Read(wxT("defaultNbIterValue"), &(mpSetting.defaultNbIterValue), 10);
  config->Read(wxT("defaultSNRCheck"), &(mpSetting.defaultSNRCheck), true);
  config->Read(wxT("defaultSNRValue"), &(mpSetting.defaultSNRValue), 20);
  config->Read(wxT("defautNbIterSaveValue"), &(mpSetting.defautNbIterSaveValue), 0);
  config->Read(wxT("defaultBookDir"), &(mpSetting.defaultBookDir), wxT(""));
  config->Read(wxT("defaultBookName"), &(mpSetting.defaultBookName), wxT(""));
  config->Read(wxT("defaultResidualDir"), &(mpSetting.defaultResidualDir), wxT(""));
  config->Read(wxT("defaultResidualName"), &(mpSetting.defaultResidualName), wxT(""));
  config->Read(wxT("defaultDecayDir"), &(mpSetting.defaultDecayDir), wxT(""));
  config->Read(wxT("defaultDecayName"), &(mpSetting.defaultDecayName), wxT(""));
  config->Read(wxT("defaultNbIterOutputCheck"), &(mpSetting.defaultNbIterOutputCheck), false);
  config->Read(wxT("defaultNbIterRefreshCheck"), &(mpSetting.defaultNbIterRefreshCheck), false);
  config->Read(wxT("defaultNbIterOutputValue"), &(mpSetting.defaultNbIterOutputValue), 0);
  config->Read(wxT("defaultVerbose"), &(mpSetting.defaultVerbose), false);
  config->Read(wxT("defaultQuiet"), &(mpSetting.defaultQuiet), false);
  config->Read(wxT("defaultNormal"), &(mpSetting.defaultNormal), true);
}

// Save the properties in the file "mptkGuiProperties"
void MptkGuiFrame::saveProperties()
{
  initMPSettingStruct result=mpSettingsDialog->save();

  config->Write(wxT("defaultDirOpenSignal"), defaultDirOpenSignal);
  config->Write(wxT("defaultDirOpenBook"), defaultDirOpenBook);
  config->Write(wxT("defaultDirSave"), defaultDirSave);

  config->Write(wxT("defaultDicoDir"), result.defaultDicoDir);
  config->Write(wxT("defaultDicoName"), result.defaultDicoName);
  config->Write(wxT("defaultNbIterCheck"), result.defaultNbIterCheck);
  config->Write(wxT("defaultNbIterValue"), result.defaultNbIterValue);
  config->Write(wxT("defaultSNRCheck"), result.defaultSNRCheck);
  config->Write(wxT("defaultSNRValue"), result.defaultSNRValue);
  config->Write(wxT("defautNbIterSaveValue"), result.defautNbIterSaveValue);
  config->Write(wxT("defaultBookDir"), result.defaultBookDir);
  config->Write(wxT("defaultBookName"),result.defaultBookName);
  config->Write(wxT("defaultResidualDir"), result.defaultResidualDir);
  config->Write(wxT("defaultResidualName"), result.defaultResidualName);
  config->Write(wxT("defaultDecayDir"), result.defaultDecayDir);
  config->Write(wxT("defaultDecayName"), result.defaultDecayName);
  config->Write(wxT("defaultNbIterOutputCheck"), result.defaultNbIterOutputCheck);
  config->Write(wxT("defaultNbIterRefreshCheck"), result.defaultNbIterRefreshCheck);
  config->Write(wxT("defaultNbIterOutputValue"), result.defaultNbIterOutputValue);
  config->Write(wxT("defaultVerbose"), result.defaultVerbose);
  config->Write(wxT("defaultQuiet"), result.defaultQuiet);
  config->Write(wxT("defaultNormal"), result.defaultNormal);

  config->Flush();
}
void MptkGuiFrame::disableActions()
{
  menuFile->Enable(Ev_File_Save,false);
  menuFile->Enable(Ev_File_Close, false);
 
  menuView->Enable(Ev_View_NewSignal,false);
  menuView->Enable(Ev_View_NewAtom,false);
  menuView->Enable(Ev_View_NewSpectrogram, false);
  menuView->Enable(Ev_View_Mask,false);
  
  
  menuListen->Enable(Ev_Listen_BaseSignal,false);
  menuListen->Enable(Ev_Listen_ApproximantSignal,false);
  menuListen->Enable(Ev_Listen_ResidualSignal,false);
  menuListen->Enable(Ev_Listen_Stop,false);
  menuListen->Enable(Ev_Listen_Pause,false);
  menuListen->Enable(Ev_Listen_Play,false);
  
  
  menuDecomposition->Enable(Ev_Decomposition_IterateOnce,false);
  menuDecomposition->Enable(Ev_Decomposition_IterateAll,false);
  menuDecomposition->Enable(Ev_Decomposition_Settings,false);

  toolBar->EnableTool(Ev_File_Save,false);
  toolBar->EnableTool(Ev_Decomposition_IterateOnce,false);
  toolBar->EnableTool(Ev_Decomposition_IterateAll,false);
  toolBar->EnableTool(Ev_Decomposition_Stop_Iteration,false);
  toolBar->EnableTool(Ev_Listen_Play,false); // disable the buttons
  toolBar->EnableTool(Ev_Listen_Pause,false);
  toolBar->EnableTool(Ev_Listen_Stop,false); 
  toolBar->EnableTool(Ev_Check_Nb,false); 
  toolBar->EnableTool(Ev_Check_SNR,false); 
}

void MptkGuiFrame::refresh()
{
  m_panel->SetSize(m_panel->GetSize());
  m_sizerFrame->Layout();
}

void MptkGuiFrame::createToolBar()
{ 
  toolBar = CreateToolBar(/*wxTB_TEXT*//*wxTB_HORZ_TEXT*/);
  toolBar->AddTool(Ev_File_Open,_T(" Open"),wxBitmap(open_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Open"),_T("Open a file"),NULL);
  toolBar->AddTool(Ev_File_OpenSignal,_T(" Open a signal"),wxBitmap(open_signal_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Open a signal"),_T("Open a signal directly"),NULL);
  toolBar->AddTool(Ev_File_OpenBook,_T(" Open a book"),wxBitmap(open_book_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Open a book"),_T("Open a book directly"),NULL);
  toolBar->AddTool(Ev_File_Save,_T(" Save books"),wxBitmap(save_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Save"),_T("Save a file"),NULL);
  toolBar->AddSeparator();
  toolBar->AddTool(Ev_Listen_Play,_T(" Play"),wxBitmap(play_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Play"),_T("Play a sound"),NULL);
  toolBar->AddTool(Ev_Listen_Pause,_T(" Pause"),wxBitmap(pause_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Pause"),_T("Press ths button to make a pause"),NULL);
  toolBar->AddTool(Ev_Listen_Stop,_T(" Stop"),wxBitmap(stop_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Stop"),_T("Stop the sound"),NULL);
  toolBar->AddSeparator();
  toolBar->AddTool(Ev_Decomposition_IterateOnce,_T(" Iterate Once"),wxBitmap(iterate_once_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Iterate Once"),_T("Press this button for a step by step decomposition"),NULL);
  toolBar->AddTool(Ev_Decomposition_IterateAll,_T(" Iterate All"),wxBitmap(iterate_all_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Iterate all"),_T("Performs a full decomposition"),NULL);
  toolBar->AddTool(Ev_Decomposition_Stop_Iteration,_T(" Stop iteration"),wxBitmap(iterate_stop_xpm),wxNullBitmap,wxITEM_NORMAL,_T("Stop Iteration"),_T("Stop the Iteration"),NULL);
  toolBar->AddSeparator();
  /**/
  toolBar->AddCheckTool(Ev_Check_Nb, _T("Atoms"), Nb_xpm, wxNullBitmap, _T("Number of atoms"),_T("Check to active this option. The displayed options are NOT representative of the true numbers if the Decomposition>Settings window has been modified"),NULL);
  comboNB = new wxComboBox(toolBar, Ev_Combo_Nb, wxEmptyString, wxDefaultPosition, wxSize(75,wxDefaultCoord) );
  comboNB->Append(_T("5"));
  comboNB->Append(_T("10"));
  comboNB->Append(_T("50"));
  comboNB->Append(_T("100"));
  comboNB->Append(_T("500"));
  comboNB->Append(_T("1000"));
  comboNB->Append(_T("5000"));
  comboNB->Append(_T("10000"));
  comboNB->Append(_T("50000"));
  comboNB->Append(_T("100000"));
  comboNB->Append(_T("500000"));
  comboNB->Append(_T("1000000"));
  comboNB->Append(_T(" "));
  toolBar->AddControl(comboNB);
  toolBar->AddSeparator();

  toolBar->AddCheckTool(Ev_Check_SNR, _T("SNR"), SNR_xpm, wxNullBitmap, _T("SNR"),_T("Check to active this option. The displayed options are NOT representative of the true numbers if the Decomposition>Settings window has been modified"),NULL);
  comboSNR = new wxComboBox(toolBar, Ev_Combo_SNR, wxEmptyString, wxDefaultPosition, wxSize(75,wxDefaultCoord) );
  comboSNR->Append(_T("5"));
  comboSNR->Append(_T("10"));
  comboSNR->Append(_T("15"));
  comboSNR->Append(_T("20"));
  comboSNR->Append(_T("25"));
  comboSNR->Append(_T("30"));
  comboSNR->Append(_T("35"));
  comboSNR->Append(_T("40"));
  comboSNR->Append(_T("45"));
  comboSNR->Append(_T("50"));
  comboSNR->Append(_T("55"));
  comboSNR->Append(_T("60"));
  comboSNR->Append(_T("65"));
  comboSNR->Append(_T("70"));
  comboSNR->Append(_T("75"));
  comboSNR->Append(_T("80"));
  comboSNR->Append(_T("85"));
  comboSNR->Append(_T("90"));
  comboSNR->Append(_T("95"));
  comboSNR->Append(_T("100"));
  comboSNR->Append(_T(" "));
  toolBar->AddControl(comboSNR);
  toolBar->AddSeparator();
  /**/
  toolBar->Realize();
  updateToolBarBoxes();
}

// Create the statusBar
void MptkGuiFrame::createStatusBar()
{
	statusBar = CreateStatusBar();
}

// Returns the channels selected in the submenu channels
std::vector<bool> * MptkGuiFrame::getSelectedChannels()
{
  int numChans = control->getNumChans();
  std::vector<bool> * v =  new std::vector<bool>(numChans, false);
  for(int i = 0 ; i<numChans ; i++){
    if (subMenuChannel->FindItemByPosition(i)->IsChecked()){
      (*v)[i] = true;
    }
  }
  return v;
}

// Builds the various menus
void MptkGuiFrame::buildMenus()
{
  buildFileMenu();
  buildViewMenu();
  buildListenMenu();
  //buildMaskMenu();
  buildDecompositionMenu();
  buildHelpMenu();

  menuBar = new wxMenuBar();
  menuBar->Append(menuFile, _T("&File"));
  menuBar->Append(menuView, _T("&View"));
  menuBar->Append(menuListen, _T("&Listen"));
  //menuBar->Append(menuMask, _T("&Mask"));
  menuBar->Append(menuDecomposition, _T("&Decomposition"));
  menuBar->Append(menuHelp, _T("&Help"));

  SetMenuBar(menuBar);
}

// Builds menu File
void MptkGuiFrame::buildFileMenu()
{
  menuFile = new wxMenu();
  menuFile->Append(Ev_File_Open, _T("Open..."), _T("Open a signal and/or a book"));
  menuFile->Append(Ev_File_OpenSignal, _T("Open signal only..."), _T("Open a signal"));
  menuFile->Append(Ev_File_OpenBook, _T("Open book only..."), _T("Open a book"));
  menuFile->Append(Ev_File_Save, _T("Save..."), _T("Save book, residual signal, rebuilt (approximant) signal"));
  menuFile->AppendSeparator();
  menuFile->Append(Ev_File_Close,_T("Close"), _T("Close all views, reset the environment"));
  menuFile->AppendSeparator();
  menuFile->Append(Ev_File_Quit,_T("Quit"), _T("Exit the MptkGui"));
}

// Builds menu View
void MptkGuiFrame::buildViewMenu()
{
  // The submenu allowing to select the signal to play is created here
  menuView = new wxMenu;
  menuView->Append(Ev_View_NewSignal, _T("New signal view"), _T("Add a new signal view"));
  menuView->Append(Ev_View_NewAtom, _T("New atom view"), _T("Add a new atom view"));
  menuView->Append(Ev_View_NewSpectrogram, _T("New spectrogram view"), _T("Add a spectrogram view"));
  menuView->AppendSeparator();
  menuView->Append(Ev_View_Console, _T("Hide Console"), _T("Hide/Show the console for info/warning/error messages"));
  menuView->Append(Ev_View_Mask, _T("Hide/Display mask"));
  menuView->AppendSeparator();
  menuView->Append(Ev_View_Settings, _T("Settings"), _T("Set the settings : colormap, range limits"));
}

// Builds menu Listen
void MptkGuiFrame::buildListenMenu()
{
  // The submenu allowing to select the signal to play is created here
  subMenuSignal = new wxMenu;
  subMenuSignal->AppendRadioItem(Ev_Listen_BaseSignal, _T("Base signal"), _T("Listen original signal"));  
  subMenuSignal->AppendRadioItem(Ev_Listen_ApproximantSignal, _T("Approximant signal"), _T("Listen the rebuilt signal (approximant)"));
  subMenuSignal->AppendRadioItem(Ev_Listen_ResidualSignal, _T("Residual signal"), _T("Listen the residual signal"));
  // Then turn to the Listen menu
  menuListen = new wxMenu;
  menuListen->Append(0,_T("Signal..."), subMenuSignal);
  menuListen->AppendSeparator();
  menuListen->Append(Ev_Listen_Play, _T("Play"), _T("Play the selected signal with the selected channels"));
  menuListen->Append(Ev_Listen_Pause, _T("Pause"));
  menuListen->Append(Ev_Listen_Stop, _T("Stop"));
}

// Builds submenu Channels (with the right number of channels)
void MptkGuiFrame::buildChannelSubMenu(int numChans)
{
  if (subMenuChannel != NULL) {
    menuListen->Delete(menuListen->FindItemByPosition(2));
  }
  subMenuChannel = new wxMenu;
  for (int i=0 ; i<numChans ; i++){
    subMenuChannel->AppendCheckItem(Ev_Listen_CheckChannel, wxString::Format(_T("Channel %i"),i));
    subMenuChannel->FindItemByPosition(i)->Check(true);
  }
  menuListen->Insert(2, wxID_ANY, _T("Listen channels..."), subMenuChannel);
}

// Builds menu Mask
void MptkGuiFrame::buildMaskMenu()
{
  menuMask = new wxMenu;
  menuMask->Append(Ev_Mask_LoadExistingMask, _T("Load existing mask"));
  menuMask->Append(Ev_Mask_SelectAllAtoms, _T("Select all atoms"));
  menuMask->Append(Ev_Mask_DeselectAllAtoms, _T("Deselect all atoms"));
  menuMask->Append(Ev_Mask_InvertCurrentMask, _T("Invert current mask"));
  menuMask->Append(Ev_Mask_AddCurrentSelectionToMask, _T("Add current selection to mask"));
  menuMask->Append(Ev_Mask_SaveMask, _T("Save mask"));
}

// Builds menu Decomposition
void MptkGuiFrame::buildDecompositionMenu()
{
  menuDecomposition = new wxMenu;
  menuDecomposition->Append(Ev_Decomposition_IterateOnce, _T("Iterate once"));
  menuDecomposition->Append(Ev_Decomposition_IterateAll, _T("Iterate all"));
  menuDecomposition->AppendSeparator();
  menuDecomposition->Append(Ev_Decomposition_Settings, _T("Settings"), _T("Settings for matching pursuit decomposition"));
}

// Builds menu Help
void MptkGuiFrame::buildHelpMenu()
{
  menuHelp = new wxMenu;
  menuHelp->Append(Ev_Help_About, _T("About"));
  menuHelp->Append(Ev_Help_License, _T("License"));
}


/**********************************************************/
/* Various event procedures		               	  */
/**********************************************************/

void MptkGuiFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
  openDialog = new MptkGuiOpenDialog(this, -1, defaultDirOpenSignal, defaultDirOpenBook);
  
  if (openDialog->ShowModal() == wxID_OK) {
  	wxString signalName = openDialog->getSignalName();
  	wxString bookName = openDialog->getBookName();
  	defaultDirOpenSignal = openDialog->getDefaultDirSignal();
  	defaultDirOpenBook = openDialog->getDefaultDirBook();

	if (signalName != _T("") || bookName != _T("")){
		wxCommandEvent * ev = new wxCommandEvent();
		OnFileClose(*ev);
		delete ev;	
		openSignalAndBook(signalName, bookName);
	}
  }
  delete openDialog;
}

void MptkGuiFrame::OnFileOpenSignal(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,
						_T("Open a signal"),
						defaultDirOpenSignal,
						"",
						"*",
						wxOPEN,
						wxDefaultPosition);
	
	if (openFileDialog->ShowModal()== wxID_OK) {
	  wxCommandEvent * ev = new wxCommandEvent();
	  OnFileClose(*ev);
	  delete ev;
	  
	  defaultDirOpenSignal = openFileDialog->GetDirectory();
	  openSignal(openFileDialog->GetPath());
	}
	delete openFileDialog;
}

void MptkGuiFrame::OnFileOpenBook(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog * openFileDialog = new wxFileDialog(this,
						   _T("Open a book"),
						   defaultDirOpenBook,
						   _T(""),
						   _T("*"),
						   wxOPEN,
						   wxDefaultPosition);
   if (openFileDialog->ShowModal()== wxID_OK) {
   	  wxCommandEvent * ev = new wxCommandEvent();
	  OnFileClose(*ev);
	  delete ev;
	
	  defaultDirOpenBook = openFileDialog->GetDirectory();
	  openBook( openFileDialog->GetPath() );
   }
   delete openFileDialog;
}

void MptkGuiFrame::OnFileSave(wxCommandEvent& WXUNUSED(ev))
{
  saveDialog = new MptkGuiSaveDialog(this, -1, defaultDirSave);
  saveDialog->ShowModal();
	
  wxString bookName = saveDialog->getBookName();
  wxString approxName = saveDialog->getApproxName();
  wxString residualName = saveDialog->getResidualName();
  defaultDirSave = saveDialog->getDefaultDir();

  if( bookName != _T("") ){
    control->saveBook(bookName);
  }
  if( approxName != _T("") ){
    control->saveApproximant(approxName);
  } 
 if( residualName != _T("") ){
    control->saveResidual(residualName);
  }

}

void MptkGuiFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	if (vectViews->size()>0) {
		for (int i = 0 ; i < (int) vectViews->size() ; i++ ){
			MptkGuiSashWindow * s = vectViews->at(i);
			delete s;	
		}
		delete vectViews;
		vectViews = new std::vector<MptkGuiSashWindow *>();
		m_sizerFrame->Layout();
		delete control;
		control = new MptkGuiCallback( this );
		disableActions();
	}
}

void MptkGuiFrame::OnFileQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

void MptkGuiFrame::OnViewNewSignal(wxCommandEvent& event)
{ 
  MptkGuiSashWindow * sashWin = new MptkGuiSashWindow(m_panel, nbView);
  MptkGuiExtendedSignalView * sigExtView;
  switch (event.GetId()){
  case APPROXIMANT_SIGNAL : sigExtView = new MptkGuiExtendedSignalView(sashWin, nbView, control, APPROXIMANT_SIGNAL);break;
  case RESIDUAL_SIGNAL : sigExtView = new MptkGuiExtendedSignalView(sashWin, nbView, control, RESIDUAL_SIGNAL);break;
  default : sigExtView = new MptkGuiExtendedSignalView(sashWin, nbView, control, BASE_SIGNAL);break;
  }
  sashWin->setView(sigExtView);
  nbView ++;
  vectViews->push_back(sashWin);
  
  m_sizerFrame->Prepend(sashWin, 0, wxEXPAND | wxALL, 3);
  m_panel->SetSize(m_panel->GetSize());	
}

void MptkGuiFrame::OnViewNewAtom(wxCommandEvent& WXUNUSED(event))
{
  MptkGuiSashWindow * sashWin = new MptkGuiSashWindow(m_panel, nbView);
  MptkGuiExtendedTFView * tfView = new MptkGuiExtendedTFView(sashWin, nbView, control->getBook());
  sashWin->setView(tfView);
  nbView ++;
  vectViews->push_back(sashWin);
  m_sizerFrame->Prepend(sashWin, 0, wxEXPAND | wxALL, 3);
  m_panel->SetSize(m_panel->GetSize());
}

void MptkGuiFrame::OnViewNewSpectrogram(wxCommandEvent& WXUNUSED(event)){}

void MptkGuiFrame::OnViewConsole(wxCommandEvent& WXUNUSED(event))
{
  if (sashConsole->IsShown()) {
     sashConsole->Hide();
     menuView->SetLabel(Ev_View_Console,_T("Show console"));
     m_panel->SetSize(m_panel->GetSize());
  }
  else{
    sashConsole->Show(true);
    menuView->SetLabel(Ev_View_Console,_T("Hide console"));
    m_panel->SetSize(m_panel->GetSize());
  }
}

void MptkGuiFrame::OnViewMask(wxCommandEvent& WXUNUSED(event)){}

void MptkGuiFrame::OnViewSettings(wxCommandEvent& WXUNUSED(event))
{
  bool got_amp_params=false;
  bool got_freq_params=false;
  bool got_time_params=false;
  bool got_all_params=false;
  struct initLimitsSettingsStruct current_params;

  //On va recuperer les parametres des vues courantes pour remplir les
  //champs editables de la boite de dialogue avec des valeurs par defaut
  for (int i = 0 ; (i < (int) vectViews->size())&&(!got_all_params) ; i++) 
    {
      MptkGuiExtendedView * view = vectViews->at(i)->getView();

      if(view->isSignalView())
	{
	  MptkGuiExtendedSignalView * signalview=(MptkGuiExtendedSignalView *) view;
	  if(!got_time_params)
	    {
	      current_params.time_min=signalview->getStartTime();
	      current_params.time_max=signalview->getEndTime();
	      got_time_params=true;
	    }
	  
	  if(!got_amp_params)
	    {
	      current_params.amp_min=signalview->getMinAmp();
	      current_params.amp_max=signalview->getMaxAmp();
	      got_amp_params=true;
	    }
	}
      else
	{
	  MptkGuiExtendedTFView * tfview=(MptkGuiExtendedTFView *) view;
	  if(!got_time_params)
	    {
	      current_params.time_min=tfview->getTempDebut();
	      current_params.time_max=tfview->getTempFin();
	      got_time_params=true;
	    }

	  if(!got_freq_params)
	    {
	      current_params.freq_min=tfview->getFrequenceMin();
	      current_params.freq_max=tfview->getFrequenceMax();
	      current_params.dB_min=tfview->getDBMin();
	      current_params.dB_max=tfview->getDBMax();
	      current_params.colormap_type=tfview->getSelectedCMapType();
	      got_freq_params=true;
	    }
	}
      
      if(got_time_params&&got_freq_params&&got_amp_params) got_all_params=true;
    }
  
  limitsSettingsDialog->setEnabledAmpFields(got_amp_params);
  limitsSettingsDialog->setEnabledFreqFields(got_freq_params);
  limitsSettingsDialog->setAllParams(&current_params);
  int res=limitsSettingsDialog->ShowModal();
  if(res==wxID_OK)
    {
      //L'utilisateur a clique OK et les parametres donnes sont coherents.
      //On les applique donc
      struct initLimitsSettingsStruct * saved_params=limitsSettingsDialog->getSavedValues();
      for (int i = 0 ; (i < (int) vectViews->size()); i++)
	{
	  MptkGuiExtendedView * view = vectViews->at(i)->getView();
	  if(view->isSignalView())
	    {
	      MptkGuiExtendedSignalView * signalview=(MptkGuiExtendedSignalView *) view;
	      if((saved_params->time_min!=current_params.time_min)
		 ||(saved_params->time_max!=current_params.time_max)
		 ||(saved_params->amp_min!=current_params.amp_min)
		 ||(saved_params->amp_max!=current_params.amp_max))
		signalview->zoom(saved_params->time_min,saved_params->time_max,saved_params->amp_min,saved_params->amp_max);
	  
	    }
	  else
	    {
	      MptkGuiExtendedTFView * tfview=(MptkGuiExtendedTFView *) view;
	      if(current_params.colormap_type!=saved_params->colormap_type)
		tfview->setSelectedCMapType(saved_params->colormap_type);

	      if((saved_params->time_min!=current_params.time_min)
		 ||(saved_params->time_max!=current_params.time_max)
		 ||(saved_params->freq_min!=current_params.amp_min)
		 ||(saved_params->freq_max!=current_params.amp_max))
		{
		  tfview->zoom(saved_params->time_min,saved_params->time_max,saved_params->freq_min,saved_params->freq_max);
		  tfview->setDBMin(saved_params->dB_min);
		  tfview->setDBMax(saved_params->dB_max);
		}
	    }
	}
    }
}

void MptkGuiFrame::OnListenBaseSignal(wxCommandEvent& WXUNUSED(event))
{
  listenSignalType = BASE_SIGNAL;
}


void MptkGuiFrame::OnListenApproximantSignal(wxCommandEvent& WXUNUSED(event))
{
  listenSignalType = APPROXIMANT_SIGNAL;
}

void MptkGuiFrame::OnListenResidualSignal(wxCommandEvent& WXUNUSED(event))
{
  listenSignalType = RESIDUAL_SIGNAL;
}

void MptkGuiFrame::OnListenPlay(wxCommandEvent& WXUNUSED(event))
{	
  menuListen->Enable(Ev_Listen_Play,false);  
  menuListen->Enable(Ev_Listen_Stop,true);
  menuListen->Enable(Ev_Listen_Pause,true);
  toolBar->EnableTool(Ev_Listen_Play,false);
  toolBar->EnableTool(Ev_Listen_Pause,true);
  toolBar->EnableTool(Ev_Listen_Stop,true);
  std::vector<bool> * selectedChans = getSelectedChannels();
  switch (listenSignalType){
  	case BASE_SIGNAL : control->playBaseSignal(selectedChans);break;
	case APPROXIMANT_SIGNAL : control->playApproximantSignal(selectedChans);break;
	case RESIDUAL_SIGNAL : control->playResidualSignal(selectedChans);break;
  }
}

void MptkGuiFrame::OnListenPause(wxCommandEvent& WXUNUSED(event))
{
  if (menuListen->GetLabel(Ev_Listen_Pause)==_T("Pause"))
    {
      control->pauseListen();
      menuListen->SetLabel(Ev_Listen_Pause,_T("Restart"));
    }
  else
    {
      control->restartListen();
      menuListen->SetLabel(Ev_Listen_Pause,_T("Pause"));
    }
}

void MptkGuiFrame::OnListenStop(wxCommandEvent& WXUNUSED(event))
{
  control->stopListen();
  menuListen->Enable(Ev_Listen_Pause,false);
  menuListen->Enable(Ev_Listen_Play,true);  
  toolBar->EnableTool(Ev_Listen_Play,true);
  toolBar->EnableTool(Ev_Listen_Pause,false);
  if (menuListen->GetLabel(Ev_Listen_Pause)==_T("Restart")) {menuListen->SetLabel(Ev_Listen_Pause,_T("Pause"));}
}


void MptkGuiFrame::OnMaskLoadExistingMask(wxCommandEvent& WXUNUSED(event)){}
void MptkGuiFrame::OnMaskSelectAllAtoms(wxCommandEvent& WXUNUSED(event)){}
void MptkGuiFrame::OnMaskDeselectAllAtoms(wxCommandEvent& WXUNUSED(event)){}
void MptkGuiFrame::OnMaskInvertCurrentMask(wxCommandEvent& WXUNUSED(event)){}
void MptkGuiFrame::OnMaskAddCurrentSelectionToMask(wxCommandEvent& WXUNUSED(event)){}
void MptkGuiFrame::OnMaskSaveMask(wxCommandEvent& WXUNUSED(event)){}

void MptkGuiFrame::OnDecompositionIterateOnce(wxCommandEvent& WXUNUSED(event))
{
  control->iterateOnce();
  decompositionFinished();
}

void MptkGuiFrame::OnDecompositionIterateAll(wxCommandEvent& WXUNUSED(event))
{
  wxBusyInfo wait( _T("Please wait, decomposition in progress...") );
  toolBar->EnableTool(Ev_Decomposition_Stop_Iteration,true);
  control->iterateAll();
  decompositionFinished();
}

void MptkGuiFrame::OnDecompositionStopIteration(wxCommandEvent& WXUNUSED(event))
{
  control->stopIteration();
  decompositionFinished();
  toolBar->EnableTool(Ev_Decomposition_Stop_Iteration,false);
}

void MptkGuiFrame::OnDecompositionSettings(wxCommandEvent& WXUNUSED(event))
{
  mpSettingsDialog->Show();
}

void MptkGuiFrame::OnCheckNb(wxCommandEvent& WXUNUSED(event))
{
 bool nb = toolBar->GetToolState(Ev_Check_Nb);
 long int nbValue;
 comboNB->GetValue().ToLong(&nbValue);
 if(nb) control->setIterationNumber(nbValue);
 else control->unsetIterationNumber();
 mpSettingsDialog->update();
}

void MptkGuiFrame::OnCheckSNR(wxCommandEvent& WXUNUSED(event))
{
 bool snr = toolBar->GetToolState(Ev_Check_SNR);
 double snrValue;
 comboSNR->GetValue().ToDouble(&snrValue);
 if(snr) control->setSNR(snrValue);
 else control->unsetSNR();
 mpSettingsDialog->update();
}

void MptkGuiFrame::OnComboNb(wxCommandEvent& WXUNUSED(event))
{
 bool nb = toolBar->GetToolState(Ev_Check_Nb);
 long int nbValue;
 comboNB->GetValue().ToLong(&nbValue);
 if(nb) {control->setIterationNumber(nbValue);
   mpSettingsDialog->update();}
}

void MptkGuiFrame::OnComboSNR(wxCommandEvent& WXUNUSED(event))
{
 bool snr = toolBar->GetToolState(Ev_Check_SNR);
 double snrValue;
 comboSNR->GetValue().ToDouble(&snrValue);
 if(snr) {control->setSNR(snrValue);
   mpSettingsDialog->update();}
}


void MptkGuiFrame::OnMPSettingApply(MptkGuiSettingUpdateEvent& WXUNUSED(event)) {
	updateToolBarBoxes();
 }

void MptkGuiFrame::updateToolBarBoxes(void) {
  if(mpSettingsDialog->nbIterCheckBox->IsChecked())
	{
		toolBar->ToggleTool(Ev_Check_Nb,true);
		comboNB->SetValue(mpSettingsDialog->nbIterValueText->GetValue());
	}
  else	
	{
		toolBar->ToggleTool(Ev_Check_Nb,false);
		comboNB->SetValue(_T(" "));
	}
  if(mpSettingsDialog->snrCheckBox->IsChecked())
	{
		toolBar->ToggleTool(Ev_Check_SNR,true);
		comboSNR->SetValue(mpSettingsDialog->snrValueText->GetValue());
	}
  else	
	{
		toolBar->ToggleTool(Ev_Check_SNR,false);
		comboSNR->SetValue(_T(" "));
	}
}

void MptkGuiFrame::OnHelpLicense(wxCommandEvent& WXUNUSED(event)){
	MptkGuiLicenseDialog * licenseDialog = new MptkGuiLicenseDialog(this);
	licenseDialog->ShowModal();
	delete licenseDialog;
}

void MptkGuiFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
  const char* aboutText = "Graphical user interface realized by :\n"
    "\tNicolas Bonnet\n"
    "\tBenjamin Boutier\n"
    "\tVincent Chapon\n"
    "\tSylvestre Cozic\n"
    "\n"
    "Based on the Matching Pursuit ToolKit (MPTK),\n"
    "realized by:\n"
    "\tRémi Gribonval\t\tremi@irisa.fr\n"
    "\tSacha Krstulovic\tsacha@irisa.fr\n"
    "\tSylvain Lesage\t\tslesage@irisa.fr\n"
    "\n"
    "For more info, visit MPTK's web page at:\n"
    "\thttp://mptk.gforge.inria.fr/\n"
    "\n"
    "Copyright 2005 IRISA/INRIA, Rennes, France.\n"
    "Research work led by the METISS Team:\n"
    "http://www.irisa.fr/metiss/\n"
    "\n"
    "This software is distributed under the GPL.\n"
    "(See the \"License\" option under the \"Help\" menu.)\n"
    "\n"
    "Enjoy Matching Pursuit !";

  wxMessageBox(wxT(aboutText),
	       wxT("About"), 
	       wxICON_INFORMATION | wxOK);
}

// Propagates the zoom to the other views
void MptkGuiFrame::OnZoom(MptkGuiZoomEvent& event){
  int zoomedViewId = event.getId();
  MptkGuiExtendedView * zoomedView = getView(zoomedViewId);
  
  for (int i = 0 ; i < (int) vectViews->size() ; i++) {
    MptkGuiExtendedView * view = vectViews->at(i)->getView();
    if (view->getId() != zoomedViewId) {
      if (zoomedView->isSignalView()){
		if (view->isSignalView()){
	  	view->zoom(event.getFirstTime(), event.getLastTime(),event.getMinAmp(), event.getMaxAmp());
		}
		else {
	  	view->zoom(event.getFirstTime(), event.getLastTime());
		}
      }
      else{
		if(view->isTFView()){
	 	 view->zoom(event.getFirstTime(), event.getLastTime(), event.getFrequence_bas(), event.getFrequence_haut());
		}
		else{
		  view->zoom(event.getFirstTime(), event.getLastTime());
		}
      }
    }
  }
}


// When a view is deleted, removes it in the vectViews
void MptkGuiFrame::OnDeleteView(MptkGuiDeleteViewEvent& event){
  for(int i = 0; i < (int)vectViews->size() ; i++) {
    MptkGuiExtendedView * view = vectViews->at(i)->getView();
    if (view->getId() == event.getId()) {
      MptkGuiSashWindow * s = vectViews->at(i);
      vectViews->erase(vectViews->begin()+i);
      delete s;
      m_sizerFrame->Layout();
      break;
    }
  }
}

// Move a view up
void MptkGuiFrame::OnUpView(MptkGuiUpEvent& event){
  int index = getIndex(event.getId());

  if(index>=0) {
    wxWindow * view = m_sizerFrame->GetItem(index)->GetWindow();
    m_sizerFrame->Detach(index);
    m_sizerFrame->Insert(index-1, view, 0, wxEXPAND | wxALL, 3);
    m_panel->SetSize(m_panel->GetSize());
  }
}

// Move a view down
void MptkGuiFrame::OnDownView(MptkGuiDownEvent& event){
  int index = getIndex(event.getId());

  if(index <(int) vectViews->size()) {
    wxWindow * view = m_sizerFrame->GetItem(index)->GetWindow();
    m_sizerFrame->Detach(index);
    m_sizerFrame->Insert(index+1, view, 0, wxEXPAND | wxALL, 3);
    m_panel->SetSize(m_panel->GetSize());
  }
}

// Refresh the sizer when a sashEvent occurs
void MptkGuiFrame::OnDrag(wxSashEvent& event){ 
  if (event.GetDragStatus() == wxSASH_STATUS_OK){
     m_panel->SetSize(m_panel->GetSize());
  }
}

// Reactivate the right buttons in the toolBar and in the menu when
// you have finished to listen the signal
void MptkGuiFrame::OnFinishedListen(MptkGuiListenFinishedEvent& WXUNUSED(event))
{
  menuListen->Enable(Ev_Listen_Play,true);
  menuListen->Enable(Ev_Listen_Pause,false);
  toolBar->EnableTool(Ev_Listen_Play,true);
  toolBar->EnableTool(Ev_Listen_Pause,false);
}

void MptkGuiFrame::OnMPSettingDone(MptkGuiSettingEvent& WXUNUSED(event))
{
    menuDecomposition->Enable(Ev_Decomposition_IterateOnce,true);
    menuDecomposition->Enable(Ev_Decomposition_IterateAll,true);
    toolBar->EnableTool(Ev_Decomposition_IterateOnce,true);
    toolBar->EnableTool(Ev_Decomposition_IterateAll,true);
}
		
// Call the control to open the signal if the file exists
void MptkGuiFrame::openSignal( wxString fileName )
{
  if ( fileName != _T("")) {
    if( wxFileName::FileExists( fileName ) ){
      if (control->initMpdCore(fileName, "") == SIGNAL_OPENED){
	addSignalViews();	
      }
    }
    else {
      wxMessageBox(wxT("File " + fileName +" doesn't exist or is not a valid signal (libsndfile compliant types)"), 
		   wxT("Error"), 
		   wxICON_ERROR | wxOK);
    }
  }
}

// Call the control to open book if the file exists
void MptkGuiFrame::openBook( wxString fileName )
{
	if ( fileName != _T("")) {
		if( wxFileName::FileExists( fileName ) ){
		  if (control->initMpdCore("",fileName) == BOOK_OPENED){
		    if (control->getApproximant() !=NULL){
		      addBookViews();}
		  }
		}
		else {
		  wxMessageBox(wxT("File " + fileName +" doesn't exist or is not a valid book"), 
			       wxT("Error"), 
			       wxICON_ERROR | wxOK);
      		}
	}
}

// Call the control to open a signal and a book
void MptkGuiFrame::openSignalAndBook(wxString signalName, wxString bookName)
{
  if (signalName != _T("") && bookName == _T("")) openSignal(signalName);
  if (signalName == _T("") && bookName != _T("")) openBook(bookName);

  if ( signalName != _T("") && bookName != _T("")) {
		if( wxFileName::FileExists( signalName ) ){
		  if (wxFileName::FileExists( bookName )){
		    switch(control->initMpdCore(signalName, bookName)){
		    case SIGNAL_OPENED : addSignalViews();break;
		    case BOOK_OPENED : addBookViews();break;
		    case SIGNAL_AND_BOOK_OPENED : addBookViews();addSignalViews();break;
		    case NOTHING_OPENED : wxMessageBox(wxT("Problem when loading signal and book in mpd_Core"), 
						       wxT("Error"), 
						       wxICON_ERROR | wxOK);break;
		    }
		  }
		  else{
		    wxMessageBox(wxT("File " + bookName +" doesn't exist"), 
				 wxT("Error"), 
				 wxICON_ERROR | wxOK);
		    openSignal(signalName);
		  }
		}
		else {
		  wxMessageBox(wxT("File " + signalName +" doesn't exist"), 
			       wxT("Error"), 
			       wxICON_ERROR | wxOK);
		  openBook(bookName);
		}
  }
} 

// Add views when you open a signal
void MptkGuiFrame::addSignalViews()
{
  wxCommandEvent * evt = new wxCommandEvent();
  evt->SetId(RESIDUAL_SIGNAL);
  OnViewNewSignal(*evt);
  delete evt;
		
  evt = new wxCommandEvent();
  evt->SetId(APPROXIMANT_SIGNAL);
  OnViewNewSignal(*evt);
  delete evt;

  evt = new wxCommandEvent();
  evt->SetId(BASE_SIGNAL);
  OnViewNewSignal(*evt);
  delete evt; 

  activeSignalControls();
}

// Add  Views when you open a book
void MptkGuiFrame::addBookViews()
{
  
  wxCommandEvent * evt = new wxCommandEvent();
  evt->SetId(APPROXIMANT_SIGNAL);
  OnViewNewSignal(*evt);
  delete evt;
  
  evt = new wxCommandEvent();
  OnViewNewAtom(*evt);
  delete evt;
  
  activeSignalControls();
  activeBookControls();
}

// Active some valid controls when a signal is opened
void MptkGuiFrame::activeSignalControls()
{
  // build the menu to listen channels
  buildChannelSubMenu(control->getNumChans());
  
  menuFile->Enable(Ev_File_Save,true);
  menuFile->Enable(Ev_File_Close, true);
  
  menuView->Enable(Ev_View_NewSignal,true);
  menuListen->Enable(Ev_Listen_BaseSignal,true);
  menuListen->Enable(Ev_Listen_ApproximantSignal,true);
  menuListen->Enable(Ev_Listen_ResidualSignal,true);
  menuListen->Enable(Ev_Listen_Play,true);				
  menuListen->Enable(Ev_Listen_Stop,true);
  
  menuDecomposition->Enable(Ev_Decomposition_Settings,true);

  toolBar->EnableTool(Ev_File_Save,true);
  toolBar->EnableTool(Ev_Listen_Play,true);
  toolBar->EnableTool(Ev_Listen_Stop,true);
  toolBar->EnableTool(Ev_Check_Nb,true); 
  toolBar->EnableTool(Ev_Check_SNR,true);
}

// Active some valid controls when a book is opened
void MptkGuiFrame::activeBookControls()
{
  menuView->Enable(Ev_View_NewAtom, true);
  menuFile->Enable(Ev_File_Save,true);
  menuFile->Enable(Ev_File_Close, true);
}
		
// In order to reactivate some control in the views if correct decomposition
void MptkGuiFrame::decompositionFinished()
{ 
	if (control->getBook()->numAtoms > 0){
	        bool containsTFView = false;

		for(int i = 0; i < (int)vectViews->size() ; i++) {
		  MptkGuiExtendedView * view = vectViews->at(i)->getView();
		  if (!view->isSignalView()) {
		    containsTFView = true;break;
		  }
		}
		
		// If no TFView present, add one
		if (!containsTFView) {
		  wxCommandEvent * evt = new wxCommandEvent();
		  OnViewNewAtom(*evt);
		  delete evt;
		}

 		menuListen->Enable(Ev_Listen_ResidualSignal,true);
		menuListen->Enable(Ev_Listen_ApproximantSignal,true);
 		menuView->Enable(Ev_View_NewAtom, true);
		MPTK_GUI_CONSOLE->appendText(wxT("Decomposition successful\n"));
 		refresh();
	}
}

// Returns the view identified by id in vectViews
MptkGuiExtendedView * MptkGuiFrame::getView(int id){
  for(int i = 0; i < (int)vectViews->size() ; i++) {
    MptkGuiExtendedView * view = vectViews->at(i)->getView();
    if (view->getId() == id) {
      return view;
    }
  }
  return NULL;
}

// Returns the index of the idView in the m_sizerFrame
int MptkGuiFrame::getIndex(int idView)
{
  int index = 0;
  while(m_sizerFrame->GetItem(index)->GetWindow()->GetId() != idView){
    index++;
  }
  return index;
}
