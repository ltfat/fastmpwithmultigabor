#ifndef MPTKGUIEXTENDEDTFVIEW_H
#define MPTKGUIEXTENDEDTFVIEW_H

#include "wx/wx.h"
#include "mptk.h"
#include "MptkGuiAtomView.h"
#include "MptkGuiColormaps.h"
#include "MptkGuiColorMapView.h"
#include "MptkGuiSpectrogramView.h"
#include "MptkGuiExtendedView.h"
#include "MptkGuiDeleteViewEvent.h"
#include "MptkGuiResizeTFMapEvent.h"

/** \brief Adds the toolbar to the TF view */
class MptkGuiExtendedTFView : public wxPanel, public MptkGuiExtendedView
{
public :

  MptkGuiExtendedTFView(wxWindow* parent, int id, MP_Book_c * book);
  MptkGuiExtendedTFView(wxWindow* parent,int id, MP_Signal_c * signal);
  ~MptkGuiExtendedTFView();

  void buildView(wxWindow* parent);
  void buildMenu();
  void buildSubMenuChannel(int numChans);
  
  void zoom(float tdeb,float tfin, float fdeb,float ffin);
  void zoom(float tdeb,float tfin);
  int  getId();
  float getTempDebut();
  float getTempFin();
  float getFrequenceMin();
  float getFrequenceMax();
  float getDBMin();
  float getDBMax();
  int getSelectedCMapType();
  void setDBMin(float new_dBmin);
  void setDBMax(float new_dBmax);
  void setSelectedCMapType(int new_cmap_type);
  
  void OnMenu(wxCommandEvent& event);
  void OnResetZoom(wxCommandEvent& event);
  void OnBookInfo(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);
  void OnSelectChan(wxCommandEvent& event);
  void OnRefreshColor(MptkGuiCMapZoomEvent& event);
  void OnResizeTFMap(MptkGuiResizeTFMapEvent& event);

private:
  MptkGuiAtomView * atomview;
  MP_Book_c * book;
  MptkGuiSpectrogramView * spectro;
  MP_Signal_c * signal;
  MptkGuiColormaps *  colormap;
  MptkGuiColorMapView * colorview;


  // Sizers
  wxBoxSizer * m_sizer;
  wxBoxSizer * buttonsSizer;

  // Menus
  wxMenu * menu;
  wxMenu * subMenuChannel;

  // Buttons
  wxButton * menuButton;
  wxBitmapButton * closeButton;
  wxBitmapButton * bookInfoButton;
  
  DECLARE_EVENT_TABLE()

};

enum {
  ID_AtomView_Menu = 1100,
  ID_AtomView_Zoom,
  ID_AtomView_Close,
  ID_AtomView_Info,
  ID_Chans,
  ID_ChansMax=1199
};

#endif


