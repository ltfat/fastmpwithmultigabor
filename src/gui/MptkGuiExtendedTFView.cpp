#include "MptkGuiExtendedTFView.h"
#include <iostream>

#include "bitmaps/quit.xpm"
#include "bitmaps/book_info.xpm"

BEGIN_EVENT_TABLE(MptkGuiExtendedTFView, wxPanel)
  EVT_BUTTON(ID_AtomView_Menu, MptkGuiExtendedTFView::OnMenu)
  EVT_MENU(ID_AtomView_Zoom, MptkGuiExtendedTFView::OnResetZoom)
  EVT_BUTTON(ID_AtomView_Info, MptkGuiExtendedTFView::OnBookInfo)
  EVT_BUTTON(ID_AtomView_Close, MptkGuiExtendedTFView::OnClose)
  EVT_MENU_RANGE(ID_Chans, ID_ChansMax, MptkGuiExtendedTFView::OnSelectChan)
  EVT_CMAP_ZOOM(MptkGuiExtendedTFView::OnRefreshColor)
  EVT_RESIZE_TF_MAP(MptkGuiExtendedTFView::OnResizeTFMap)
END_EVENT_TABLE()


  MptkGuiExtendedTFView::MptkGuiExtendedTFView(wxWindow* parent, int id, MP_Book_c * book)
  :wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxHSCROLL | wxVSCROLL | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
  signalView=false;
  buildView(parent);
  buildSubMenuChannel(book->numChans);

  this->signal=NULL;
  spectro=NULL;
  this->book=book;
  atomview=new MptkGuiAtomView(this, id ,book,colormap);
  
  //Add the view to the atomView
  m_sizer->Add(atomview, 1 , wxEXPAND|wxALL, 1);
}



MptkGuiExtendedTFView::MptkGuiExtendedTFView(wxWindow* parent, int id, MP_Signal_c * signal)
  :wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxHSCROLL | wxVSCROLL | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
  signalView=false;
  buildView(parent);
  buildSubMenuChannel(signal->numChans);

  this->book=NULL;
  atomview=NULL; 
  this->signal=signal;
  spectro=new MptkGuiSpectrogramView(this,id ,signal,colormap);
  bookInfoButton->Enable(false);
  // Add the view to the atomView
  m_sizer->Add(spectro, 1 , wxEXPAND | wxALL, 1);
}


MptkGuiExtendedTFView::~MptkGuiExtendedTFView()
{
  if (atomview!=NULL) {delete atomview;}
  if (spectro!=NULL) {delete spectro;}
  if (colormap!=NULL) {delete colormap;}
}


void MptkGuiExtendedTFView::buildView(wxWindow* parent)
{
  colormap=new MptkGuiColormaps(255,JET,0.00001,20,LOGARITHMIC);
  // Main sizer for adding buttons (top) and the signal view (bottom
  m_sizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(m_sizer);

  // Buttons
  buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
  
  menuButton = new wxButton(this, ID_AtomView_Menu, _T("Menu"), wxDefaultPosition, wxSize(50,30));
  closeButton = new wxBitmapButton(this, ID_AtomView_Close, wxBitmap(quit_xpm), wxDefaultPosition, wxSize(50,30));
  colorview=new MptkGuiColorMapView(this,colormap,0.01,20);
  bookInfoButton = new wxBitmapButton(this, ID_AtomView_Info, wxBitmap(book_info_xpm), wxDefaultPosition, wxSize(50,30));

  panelUpDown = new MptkGuiUpDownPanel(this, GetId());
 
  buttonsSizer->Add(menuButton, 0, wxALL, 1);
  buttonsSizer->Add(colorview,1,wxEXPAND|wxALL,1);
  buttonsSizer->Add(bookInfoButton, 0, wxALL, 1);
  buttonsSizer->Add(panelUpDown, 0, wxALL, 1);
  buttonsSizer->Add(closeButton, 0, wxALL, 1);
  // Menu
  buildMenu();

  // Force the TFView to have the same size than its parent
  int width, height;
  parent->GetClientSize(&width, &height);
  this->SetSize(width, height);

  // Add the panel containing buttons to the sizer
  m_sizer->Add(buttonsSizer, 0, wxEXPAND|wxALL, 2);
}


void MptkGuiExtendedTFView::buildMenu()
{
  menu = new wxMenu;
  menu->Append(ID_AtomView_Zoom, _T("Reset zoom"));
}



void MptkGuiExtendedTFView::buildSubMenuChannel(int numChans)
{
  subMenuChannel = new wxMenu;
  for (int i=0 ; i<numChans ; i++){
    subMenuChannel->AppendRadioItem(ID_Chans+i, wxString::Format("Channel %i",i)); 
  }
  menu->Append(wxID_ANY, "Channels...", subMenuChannel);
}

int MptkGuiExtendedTFView::getId()
{
  return GetId();
}

void MptkGuiExtendedTFView::zoom(float tdeb,float tfin, float fdeb,float ffin)
{
  if (atomview!=NULL) {atomview->zoom(tdeb,tfin,fdeb,ffin);}
  else {spectro->zoom(tdeb,tfin,fdeb,ffin);}
}

void MptkGuiExtendedTFView::zoom(float tdeb,float tfin)
{
  if (atomview!=NULL) {atomview->zoom(tdeb,tfin);}
  else {spectro->zoom(tdeb,tfin);}
}

float MptkGuiExtendedTFView::getTempDebut()
{
  if (atomview!=NULL) {return (float)atomview->tdeb/((float)book->sampleRate);}
  else {return (float)spectro->tdeb/((float)signal->sampleRate);}
}

float MptkGuiExtendedTFView::getTempFin()
{
  if (atomview!=NULL) {return (float)atomview->tfin/((float)book->sampleRate);}
  else {return (float)spectro->tfin/((float)signal->sampleRate);}
}

float MptkGuiExtendedTFView::getFrequenceMin()
{
  if (atomview!=NULL) {return book->sampleRate*atomview->fdeb;}
  else {return signal->sampleRate*spectro->fdeb;}
}

float MptkGuiExtendedTFView::getFrequenceMax()
{
  if (atomview!=NULL) {return book->sampleRate*atomview->ffin;
}
  else {return signal->sampleRate*spectro->ffin;}
  
}

void MptkGuiExtendedTFView::OnMenu(wxCommandEvent& WXUNUSED(event))
{
  PopupMenu(menu, wxPoint(menuButton->GetPosition().x,menuButton->GetPosition().y + 25) );
}


void MptkGuiExtendedTFView::OnResetZoom(wxCommandEvent& WXUNUSED(event))
{
  if (atomview==NULL) {spectro->resetZoom();} else {atomview->resetZoom();}
}

void MptkGuiExtendedTFView::OnBookInfo(wxCommandEvent& WXUNUSED(event))
{
  book->short_info();
}

void MptkGuiExtendedTFView::OnClose(wxCommandEvent& WXUNUSED(event))
{
  MptkGuiDeleteViewEvent * evt = new MptkGuiDeleteViewEvent(GetId());
  ProcessEvent(*evt);
  delete evt;
}

void MptkGuiExtendedTFView::OnSelectChan(wxCommandEvent& event)
{
  if (atomview!=NULL) {atomview->setSelectedChannel(event.GetId()-ID_Chans);}
  else {spectro->setSelectedChannel(event.GetId()-ID_Chans);}
}

void MptkGuiExtendedTFView::OnRefreshColor(MptkGuiCMapZoomEvent& WXUNUSED(event))
{
  if (atomview!=NULL) {atomview->refreshColor();}
  else {spectro->refreshColor();}
}

void MptkGuiExtendedTFView::OnResizeTFMap(MptkGuiResizeTFMapEvent& WXUNUSED(event))
{
 if (atomview!=NULL) {colorview->setMaxBound(atomview->maxTotal);}
 else {colorview->setMaxBound(spectro->maxTotal);}
 colorview->SetSize(colorview->GetSize());
}

float MptkGuiExtendedTFView::getDBMin()
{
  if(colorview!=NULL) return colorview->getMinBound();
  else return -1;
}

float MptkGuiExtendedTFView::getDBMax()
{
  if(colorview!=NULL) return colorview->getMaxBound();
  else return -1;
}

int MptkGuiExtendedTFView::getSelectedCMapType()
{
  if(colormap!=NULL) return colormap->getColormapType();
  else return JET;
}

void MptkGuiExtendedTFView::setDBMin(float new_dBmin)
{
  if(colorview!=NULL) colorview->setMinBound(new_dBmin);
}

void MptkGuiExtendedTFView::setDBMax(float new_dBmax)
{
  if(colorview!=NULL) colorview->setMaxBound(new_dBmax);
}

 void MptkGuiExtendedTFView::setSelectedCMapType(int new_cmap_type)
{
  colormap->setColormapType(new_cmap_type);
}
