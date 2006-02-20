/******************************************************************************/
/*                                                                            */
/*                              MptkGuiFrame.h                                */
/*                                                                            */
/*                           Matching Pursuit GUI                             */
/******************************************************************************/

/******************************************************************************/
/*                                       				      */
/*                         CREATION OF THE MAIN WINDOW	 		      */
/*                                                                            */
/******************************************************************************/

/** \brief This class provides the main window */

#ifndef MPTKGUIFRAME_H
#define MPTKGUIFRAME_H

#include "wx/wx.h"
#include "wx/fileconf.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/busyinfo.h"
#include "mptk.h"

#include "MptkGuiOpenDialog.h"
#include "MptkGuiSaveDialog.h"
#include "MptkGuiLicenseDialog.h"
#include "MptkGuiLimitsSettingsDialog.h"
#include "MptkGuiMPSettingsDialog.h"

#include "MptkGuiAudio.h"
#include "MptkGuiCallback.h"

#include "MptkGuiSashWindow.h"
#include "MptkGuiExtendedSignalView.h"
#include "MptkGuiExtendedTFView.h"
#include "MptkGuiConsoleView.h"

#include "MptkGuiGlobalConstants.h"

class MptkGuiFrame : public wxFrame
{

  /*******************/
 /*  PUBLIC METHODS */
/*******************/

public :

  /** \brief Constructor that creates and initializes the main window */
  MptkGuiFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition,
	       const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	
  /** \brief Destructor */
  virtual ~MptkGuiFrame();
  
  /** \brief Initializes program variables from "mptkGuiProperties" file if it exists */
  void initializeProperties();
  /** \brief Save the properties in the file "mptkGuiProperties" */
  void saveProperties();

  /** \brief Refreshes the inner window */
  void refresh();
	
  /** \brief
   * Each of these methods react to a particuliar event
   */	
  void OnFileOpen(wxCommandEvent& event);
  void OnFileOpenSignal(wxCommandEvent& event);
  void OnFileOpenBook(wxCommandEvent& event);
  void OnFileSave(wxCommandEvent& event);
  void OnFileClose(wxCommandEvent& event);
  void OnFileQuit(wxCommandEvent& event);

  void OnViewNewSignal(wxCommandEvent& event);
  void OnViewNewAtom(wxCommandEvent& event);
  void OnViewNewSpectrogram(wxCommandEvent& event);
  void OnViewConsole(wxCommandEvent& event);
  void OnViewMask(wxCommandEvent& event);
  void OnViewSettings(wxCommandEvent& event);

  void OnListenBaseSignal(wxCommandEvent& event);
  void OnListenApproximantSignal(wxCommandEvent& event);
  void OnListenResidualSignal(wxCommandEvent& event);
  void OnListenPlay(wxCommandEvent& event);
  void OnListenPause(wxCommandEvent& event);
  void OnListenStop(wxCommandEvent& event);

  void OnMaskLoadExistingMask(wxCommandEvent& event);
  void OnMaskSelectAllAtoms(wxCommandEvent& event);
  void OnMaskDeselectAllAtoms(wxCommandEvent& event);
  void OnMaskInvertCurrentMask(wxCommandEvent& event);
  void OnMaskAddCurrentSelectionToMask(wxCommandEvent& event);
  void OnMaskSaveMask(wxCommandEvent& event);

  void OnDecompositionIterateOnce(wxCommandEvent& event);
  void OnDecompositionIterateAll(wxCommandEvent& event);
  void OnDecompositionStopIteration(wxCommandEvent& event);
  void OnDecompositionSettings(wxCommandEvent& event);

  void OnCheckNb(wxCommandEvent& event);
  void OnCheckSNR(wxCommandEvent& event);
  void OnComboNb(wxCommandEvent& event);
  void OnComboSNR(wxCommandEvent& event);

  void OnHelpLicense(wxCommandEvent& event);
  void OnHelpAbout(wxCommandEvent& event);
 
  void OnZoom(MptkGuiZoomEvent& event);
  void OnDeleteView(MptkGuiDeleteViewEvent& event);
  
  void OnUpView(MptkGuiUpEvent& event);
  void OnDownView(MptkGuiDownEvent& event);
  
  void OnDrag(wxSashEvent& event);
  
  void OnFinishedListen(MptkGuiListenFinishedEvent& event);
  void OnMPSettingDone(MptkGuiSettingEvent& event);
  void OnMPSettingApply(MptkGuiSettingUpdateEvent& event);
  
  /****************/
 /* PRIVATE DATA */
/****************/

private :
  /** \brief Number of views added */
  int nbView;

  /** \brief The sahsWindow of the console */
  MptkGuiSashWindow * sashConsole;

  /** \brief Type of signal selected */
  short int listenSignalType;

  /** \brief Vector to contain added views (contained in a MptkGuiSashWindow) */
  std::vector<MptkGuiSashWindow *> * vectViews;

  /** \brief Control */
  MptkGuiCallback * control;

  /** \brief Differents menus of the application */
  wxMenu *menuFile;
  wxMenu *menuView;
  wxMenu *subMenuView;
  wxMenu *menuListen;
  wxMenu *subMenuSignal;

  wxMenu *subMenuChannel;
  wxMenu *menuCanal;
  wxMenu *menuSettings;
  wxMenu *menuMask;
  wxMenu *menuDecomposition;
  wxMenu *menuHelp;
  wxMenuBar *menuBar;
  
  wxStatusBar * statusBar;
  wxComboBox *comboNB,*comboSNR;
  wxToolBar * toolBar;

  /** \brief Dialog window */
  MptkGuiOpenDialog * openDialog;
  MptkGuiSaveDialog * saveDialog;
  MptkGuiLimitsSettingsDialog * limitsSettingsDialog;
  MptkGuiMPSettingsDialog * mpSettingsDialog;

  wxScrolledWindow *m_panel;
  wxBoxSizer *m_sizerFrame; 

  wxBoxSizer *m_sizer;

  /** \brief configuration File */
  wxFileConfig *config;

  /** \brief Default properties struture */
  struct initMPSettingStruct mpSetting;
  struct initLimitsSettingsStruct limitsSetting;
  wxString defaultDirOpenSignal;
  wxString defaultDirOpenBook;
  wxString defaultDirSave;

  /*******************/
 /* PRIVATE METHODS */
/*******************/
  
  /** \brief Disables some icons/menus when their content are unactive */
  void disableActions();

  /** \brief Updates the boxes of the toolbar */
  void updateToolBarBoxes();

  /** \brief Opens a signal */
  void openSignal(wxString fileName);
  /** \brief Opens a book */
  void openBook(wxString fileName);
  /** \brief Opens both signal and book*/
  void openSignalAndBook(wxString signalName, wxString bookName);

  /** \brief Add views when you open a signal */
  void addSignalViews();
  /** \brief Add views when you open a book */
  void addBookViews();

  /** \brief Active some valid controls when a signal is opened */  
  void activeSignalControls();
  /** \brief Active some valid controls when a book is opened */  
  void activeBookControls();

  /** \brief In order to reactivate some control in the views if correct decomposition */
  void decompositionFinished();
  /** \brief Returns the channels selected in the submenu channels */
  std::vector<bool> * getSelectedChannels();
  /** \brief Returns the view identified by id in vectViews */
  MptkGuiExtendedView * getView(int id);
  /** \brief Returns the index of the idView in the m_sizerFrame */
  int getIndex(int idView);

  /** \brief Builds the tool bar */
  void createToolBar();
  /** \brief Builds the status bar */
  void createStatusBar();

  /** \brief Create the console	*/
  void buildConsole();

  /** \brief
   * Each of these methods builds a specific menu
   */
  void buildMenus();
  void buildFileMenu();
  void buildViewMenu();
  void buildListenMenu();
  void buildChannelSubMenu(int numChans);
  void buildMaskMenu();
  void buildDecompositionMenu();
  void buildHelpMenu(); 

  DECLARE_EVENT_TABLE()
    };

  /***************/
 /* EVENT TABLE */
/***************/

/** \brief table of the events generated by this class */
enum
  {
    Ev_File_Open = 1,
    Ev_File_OpenSignal,
    Ev_File_OpenBook,
    Ev_File_Close,
    Ev_File_Save,
    Ev_File_Quit,
    Ev_View_NewSignal,
    Ev_View_NewAtom,
    Ev_View_NewSpectrogram,
    Ev_View_Console,
    Ev_View_Mask,
    Ev_View_Settings,
    Ev_Listen_BaseSignal,
    Ev_Listen_ApproximantSignal,
    Ev_Listen_ResidualSignal,
    Ev_Listen_CheckChannel,
    Ev_Listen_Play,
    Ev_Listen_Pause,
    Ev_Listen_Stop,
    Ev_Mask_LoadExistingMask,
    Ev_Mask_SelectAllAtoms,
    Ev_Mask_DeselectAllAtoms,
    Ev_Mask_InvertCurrentMask,
    Ev_Mask_AddCurrentSelectionToMask,
    Ev_Mask_SaveMask,
    Ev_Decomposition_IterateOnce,
    Ev_Decomposition_IterateAll,
    Ev_Decomposition_Stop_Iteration,
    Ev_Decomposition_Settings,
    Ev_Help_License,
    Ev_Help_About,
    Ev_Check_Nb,
    Ev_Check_SNR,
    Ev_Combo_Nb,
    Ev_Combo_SNR
  };

#endif
