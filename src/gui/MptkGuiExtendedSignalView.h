#ifndef MPTKGUIEXTENDEDSIGNALVIEW_H
#define MPTKGUIEXTENDEDSIGNALVIEW_H

#include "wx/wx.h"
#include "wx/sashwin.h"

#include "MptkGuiCallback.h"
#include "MptkGuiExtendedView.h"
#include "MptkGuiSignalView.h"
#include "MptkGuiGlobalConstants.h"

/** \brief Adds the toolbar to the Signal view */
class MptkGuiExtendedSignalView : public wxPanel, public MptkGuiExtendedView {

 public :
   
  MptkGuiExtendedSignalView(wxWindow* parent, int id, MptkGuiCallback * callback, int type);

  virtual ~MptkGuiExtendedSignalView();

  int getId();
  void zoom(float tFirst, float tLast);
  void zoom(float tFirst,float tLast,float min_amp,float max_amp);

  void OnMenu(wxCommandEvent& event);
  void OnChannels(wxCommandEvent& event);
  
  void OnBaseSignal(wxCommandEvent& event);
  void OnApproximantSignal(wxCommandEvent& event);
  void OnResidualSignal(wxCommandEvent& event);
  
  void OnPlay(wxCommandEvent& event);
  void OnPlayAll(wxCommandEvent& event);
  void OnSelectChan(wxCommandEvent& event);
  
  void OnResetZoom(wxCommandEvent& event);
  void OnSignalInfo(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);

  float getStartTime(){return sigView->getStartTime();}
  float getEndTime(){return sigView->getEndTime();}
  float getMinAmp(){return sigView->getMinAmp();}
  float getMaxAmp(){return sigView->getMaxAmp();}

   // Associated signalView
  MptkGuiSignalView * sigView;

 private :
  // Boolean if listen in pause state
  bool pause;
  
  // Type of signal present in the view
  int type;
  
  // Sizers
  wxBoxSizer *m_sizer;
  wxBoxSizer *buttonsSizer;

  // Buttons
  wxButton * menuButton;
  wxButton * chanButton;
  wxBitmapButton * playButton;
  wxBitmapButton * playAllButton;
  wxBitmapButton * signalInfoButton;
  wxButton * closeButton;
  
  // Radio Button for signal selection
  wxRadioButton * baseSigRadioButton;
  wxRadioButton * approxSigRadioButton;
  wxRadioButton * resSigRadioButton;

  // Menus
  wxMenu * menu;
  wxMenu * menuChannel;
  wxMenu * subMenuSignal;
  
  MptkGuiCallback * control;

  void changeSignal(int t, MP_Signal_c * sig);
  void buildMenu();
  void buildMenuChannel(int numChans);

  DECLARE_EVENT_TABLE()

};

enum {
  ID_Menu = 1000,
  ID_Channels,
  ID_BaseSignal,
  ID_ApproximantSignal,
  ID_ResidualSignal,
  ID_Play,
  ID_PlayAll,
  ID_Zoom,
  ID_Info,
  ID_Close,
  ID_Chan,
  ID_ChanMax = 1099
};

#endif

