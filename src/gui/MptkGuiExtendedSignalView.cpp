#include "MptkGuiExtendedSignalView.h"

#include "bitmaps/play_one.xpm"
#include "bitmaps/play_all.xpm"
#include "bitmaps/signal_info.xpm"
#include "bitmaps/quit.xpm"

BEGIN_EVENT_TABLE(MptkGuiExtendedSignalView, wxPanel)
  EVT_BUTTON(ID_Menu, MptkGuiExtendedSignalView::OnMenu)
  EVT_BUTTON(ID_Channels, MptkGuiExtendedSignalView::OnChannels)
  EVT_BUTTON(ID_Play, MptkGuiExtendedSignalView::OnPlay)
  EVT_BUTTON(ID_PlayAll, MptkGuiExtendedSignalView::OnPlayAll)
  EVT_BUTTON(ID_Info, MptkGuiExtendedSignalView::OnSignalInfo)
  EVT_BUTTON(ID_Close, MptkGuiExtendedSignalView::OnClose)
  
  EVT_RADIOBUTTON(ID_BaseSignal, MptkGuiExtendedSignalView::OnBaseSignal)
  EVT_RADIOBUTTON(ID_ApproximantSignal, MptkGuiExtendedSignalView::OnApproximantSignal)
  EVT_RADIOBUTTON(ID_ResidualSignal, MptkGuiExtendedSignalView::OnResidualSignal)
  
  EVT_MENU(ID_Zoom, MptkGuiExtendedSignalView::OnResetZoom)

  EVT_MENU_RANGE(ID_Chan, ID_ChanMax, MptkGuiExtendedSignalView::OnSelectChan)
END_EVENT_TABLE()

  MptkGuiExtendedSignalView::MptkGuiExtendedSignalView(wxWindow * parent, int id, MptkGuiCallback * callback, int type) : 
    wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  signalView = true;
  pause = false;
  control = callback;
  
  // Main sizer for adding buttons (top) and the signal view (bottom)
  m_sizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(m_sizer);

  // Set view type
  this->type = type;
  
  // Buttons sizer
  buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
  
  // Buttons
  menuButton = new wxButton(this, ID_Menu, _T("Menu"), wxDefaultPosition, wxSize(50,30));
  chanButton = new wxButton(this, ID_Channels, _T("Channels"), wxDefaultPosition, wxSize(65,30));
  
  baseSigRadioButton = new wxRadioButton(this, ID_BaseSignal, wxT("Base Signal"));
  approxSigRadioButton = new wxRadioButton(this, ID_ApproximantSignal, wxT("Approximant"));
  resSigRadioButton = new wxRadioButton(this, ID_ResidualSignal, wxT("Residual"));
  
  playButton = new wxBitmapButton(this, ID_Play, wxBitmap(play_one_xpm) , wxDefaultPosition, wxSize(50,30));
  playAllButton = new wxBitmapButton(this, ID_PlayAll, wxBitmap(play_all_xpm) , wxDefaultPosition, wxSize(50,30));
  signalInfoButton = new wxBitmapButton(this, ID_Info, wxBitmap(signal_info_xpm) , wxDefaultPosition, wxSize(50,30));
  
  panelUpDown = new MptkGuiUpDownPanel(this, id);
  closeButton = new wxBitmapButton(this, ID_Close, wxBitmap(quit_xpm),wxDefaultPosition, wxSize(50,30));
  
  buttonsSizer->Add(menuButton, 0, wxALL, 2);
  buttonsSizer->Add(chanButton, 0, wxALL, 2);
  buttonsSizer->Add(baseSigRadioButton, 1, wxALL, 3);
  buttonsSizer->Add(approxSigRadioButton, 1, wxALL, 3);
  buttonsSizer->Add(resSigRadioButton, 1, wxALL, 3);
  buttonsSizer->Add(playButton, 0, wxALL, 2);
  buttonsSizer->Add(playAllButton, 0, wxALL, 2);
  buttonsSizer->Add(signalInfoButton, 0, wxALL, 2);
  buttonsSizer->Add(panelUpDown, 0, wxALIGN_RIGHT | wxALL, 2);
  buttonsSizer->Add(closeButton, 0, wxALL, 2);
 
  // Create the sigView, add the signal and the menu for select channels
  sigView = new MptkGuiSignalView(this, id);
  switch(type){
  case BASE_SIGNAL : baseSigRadioButton->SetValue(true);sigView->setSignal(control->getSignal());break;
  case APPROXIMANT_SIGNAL : approxSigRadioButton->SetValue(true);sigView->setSignal(control->getApproximant());break;
  case RESIDUAL_SIGNAL : resSigRadioButton->SetValue(true);sigView->setSignal(control->getResidual());break; 
  }

  // Menus
  buildMenu();
  buildMenuChannel(control->getSignal()->numChans);
  
  // Force the ExtendedSignalView to have the same size than its parent
  int width, height;
  parent->GetClientSize(&width, &height);
  this->SetSize(width, height);

  // Add the panel containing buttons to the sizer
  m_sizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, 2);

  // Add the view to the sizer
  m_sizer->Add(sigView, 7 , wxEXPAND | wxALL, 2);  
}

MptkGuiExtendedSignalView::~MptkGuiExtendedSignalView()
{
  delete sigView;
}

int MptkGuiExtendedSignalView::getId()
{
  return GetId();
}

void MptkGuiExtendedSignalView::zoom(float tFirst, float tLast)
{
  sigView->zoom(tFirst, tLast);
}

void MptkGuiExtendedSignalView::zoom(float tFirst, float tLast,float min_amp,float max_amp)
{
  sigView->zoom(tFirst, tLast, min_amp, max_amp);
}

void MptkGuiExtendedSignalView::buildMenu()
{
  menu = new wxMenu;
  menu->Append(ID_Zoom, _T("Reset zoom"));
}

void MptkGuiExtendedSignalView::buildMenuChannel(int numChans)
{
  menuChannel = new wxMenu;
  for (int i=0 ; i<numChans ; i++){
    menuChannel->AppendRadioItem(ID_Chan+i, wxString::Format("Channel %i",i)); 
  }
}

// Event procedures

void MptkGuiExtendedSignalView::OnMenu(wxCommandEvent& WXUNUSED(event))
{
  PopupMenu(menu, wxPoint(menuButton->GetPosition().x,menuButton->GetPosition().y + 25) );
}

void MptkGuiExtendedSignalView::OnChannels(wxCommandEvent& WXUNUSED(event))
{
  PopupMenu(menuChannel, wxPoint(chanButton->GetPosition().x,menuButton->GetPosition().y + 25) );
}

void MptkGuiExtendedSignalView::OnBaseSignal(wxCommandEvent& WXUNUSED(event))
{
	if(control->getSignal() != NULL) {
	  changeSignal(BASE_SIGNAL,control->getSignal());
	}
}

void MptkGuiExtendedSignalView::OnApproximantSignal(wxCommandEvent& WXUNUSED(event))
{
	if(control->getApproximant() != NULL) {
	  changeSignal(APPROXIMANT_SIGNAL,control->getApproximant());
	}
}

void MptkGuiExtendedSignalView::OnResidualSignal(wxCommandEvent& WXUNUSED(event))
{
	if(control->getResidual() != NULL) {
	  changeSignal(RESIDUAL_SIGNAL,control->getResidual());
	}
}

// Change the signal in the sigView, keeps zoom parameters.
void MptkGuiExtendedSignalView::changeSignal(int t, MP_Signal_c * sig)
{
          float start = sigView->getStartTime();
	  float end = sigView->getEndTime();
	  float minAmp = sigView->getMinAmp();
	  float maxAmp = sigView->getMaxAmp();
	  sigView->setSignal(sig);
	  sigView->zoom(start, end, minAmp, maxAmp);
	  type = t;
}

void MptkGuiExtendedSignalView::OnPlay(wxCommandEvent& WXUNUSED(event))
{
	std::vector<bool> * v =  new std::vector<bool>(control->getSignal()->numChans, false);
	(*v)[sigView->getSelectedChannel()] = true;
	if ( type == BASE_SIGNAL ) control->playBaseSignal(v, sigView->getStartTime(), sigView->getEndTime());
	else if ( type == APPROXIMANT_SIGNAL ) control->playApproximantSignal(v, sigView->getStartTime(), sigView->getEndTime());
	else if ( type == RESIDUAL_SIGNAL ) control->playResidualSignal(v, sigView->getStartTime(), sigView->getEndTime());
	//delete v;
}

void MptkGuiExtendedSignalView::OnPlayAll(wxCommandEvent& WXUNUSED(event))
{
	std::vector<bool> * v =  new std::vector<bool>(control->getSignal()->numChans, true);

	if ( type == BASE_SIGNAL ) control->playBaseSignal(v, sigView->getStartTime(), sigView->getEndTime());
	else if ( type == APPROXIMANT_SIGNAL ) control->playApproximantSignal(v, sigView->getStartTime(), sigView->getEndTime());
	else if ( type == RESIDUAL_SIGNAL ) control->playResidualSignal(v, sigView->getStartTime(), sigView->getEndTime());
}

void MptkGuiExtendedSignalView::OnSelectChan(wxCommandEvent& event)
{
  sigView->setSelectedChannel(event.GetId()-ID_Chan);
}

void MptkGuiExtendedSignalView::OnResetZoom(wxCommandEvent& WXUNUSED(event))
{
  sigView->resetZoom();
}

void MptkGuiExtendedSignalView::OnSignalInfo(wxCommandEvent& WXUNUSED(event))
{
  sigView->getSignal()->info();
}

void MptkGuiExtendedSignalView::OnClose(wxCommandEvent& WXUNUSED(event))
{
  MptkGuiDeleteViewEvent * evt = new MptkGuiDeleteViewEvent(GetId());
  ProcessEvent(*evt);
  delete evt;
}

