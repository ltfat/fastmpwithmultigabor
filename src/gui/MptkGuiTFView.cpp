#include "MptkGuiTFView.h"
#include "MptkGuiFrame.h"

BEGIN_EVENT_TABLE(MptkGuiTFView, wxPanel)
  EVT_PAINT(MptkGuiTFView::OnPaint)
  EVT_SIZE(MptkGuiTFView::OnResize)
  EVT_LEFT_DOWN(MptkGuiTFView::OnDown)
  EVT_LEFT_UP(MptkGuiTFView::OnUp)
  EVT_MOTION(MptkGuiTFView::OnMotion)
  EVT_MIDDLE_UP(MptkGuiTFView::OnCenterClick)
END_EVENT_TABLE()

  MptkGuiTFView::MptkGuiTFView(wxWindow* parent, int id, MptkGuiColormaps * couleur, int rate)
    :wxPanel(parent,id)
{
  colormap=couleur;
  zooming=false;
  selectedChannel=0;
  sampleRate = rate;
}

MptkGuiTFView::~MptkGuiTFView(){}


void MptkGuiTFView::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
  wxPaintDC * dc;
  dc=new wxPaintDC(this);
  PrepareDC(*dc);
  // copie de dessin dans dc
  dc->Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight(),dessin,0,0);
  delete dc;
}

void MptkGuiTFView::OnDown(wxMouseEvent& evt)
{
  // on met les coordonnées du click dans des variables temporaires
  ttemp=(int)(((float)tdeb)+evt.GetX()*((float)(tfin-tdeb))/((float) (GetSize().GetWidth())));
  ftemp=fdeb+ (GetSize().GetHeight()-evt.GetY())*((ffin-fdeb)/GetSize().GetHeight());
  coordtempX=evt.GetX();
  coordtempY=evt.GetY();
  zooming=true;
}


void MptkGuiTFView::OnUp(wxMouseEvent& evt)
{ 
  int tf,tmpt;
  double ff, tmpf;
  // on prend les coordonnées du point 
  tf=(int)(((float)tdeb)+evt.GetX()*((float)(tfin-tdeb))/((float) (GetSize().GetWidth())));
  ff=fdeb+ (GetSize().GetHeight()-evt.GetY())*((ffin-fdeb)/GetSize().GetHeight());
  // On réagence les coordonnées des 2 points pour qu'elles est un sens
  if (ttemp>tf) {tmpt=tf;tf=ttemp;ttemp=tmpt;}
  if (ftemp>ff) {tmpf=ff;ff=ftemp;ftemp=tmpf;}
  // on zoom en utilisant les coordonnées
  zoom(ttemp,tf,ftemp,ff);
  // envoi de l'évenement zoom
  sendZoom();
  ttemp=0;
  ftemp=0;
  zooming=false;
}

void MptkGuiTFView::OnMotion(wxMouseEvent& evt)
{
  int x,y,width,height;
  wxPaintDC * dc;
  if (zooming) 
    {
      // on dessine le rectangle dans lequel on va zoomer
      dc=new wxPaintDC(this);
      PrepareDC(*dc);
      dc->Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight(),dessin,0,0);
      if (coordtempX>evt.GetX()){x=evt.GetX();width=coordtempX-evt.GetX();}
      else {x=coordtempX;width=evt.GetX()-coordtempX;}
      if (coordtempY>evt.GetY()){y=evt.GetY();height=coordtempY-evt.GetY();}

      else {y=coordtempY;height=evt.GetY()-coordtempY;}
      dc->SetPen(* wxBLACK_PEN);
      dc->SetBrush(* wxTRANSPARENT_BRUSH);
      dc->DrawRectangle(x,y,width,height);
      delete dc;
    }
  // on met dans la status bar la coordonnée temp/fréquence de la souris
  float t = ((float)tdeb)+evt.GetX()*((float)(tfin-tdeb))/((float) (GetSize().GetWidth()));
  float f = fdeb+ (GetSize().GetHeight()-evt.GetY())*((ffin-fdeb)/GetSize().GetHeight());
  MPTK_GUI_STATUSBAR->SetStatusText(wxString::Format("x : %f sec (%lu samples)., y : %f Hz",t/(float)sampleRate, (unsigned long int)t, f*(float)sampleRate));
  
}

void MptkGuiTFView::OnCenterClick(wxMouseEvent& WXUNUSED(evt))
{
  resetZoom();
}

void MptkGuiTFView::zoom(int td,int tf,float fd,float ff)
{
  if (tf-td!=0 && fd-ff!=0) {
  tdeb=td;
  tfin=tf;
  fdeb=fd;
  ffin=ff;
  }
  wxSizeEvent * sizeEv=new wxSizeEvent(); 
  OnResize(* sizeEv);
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(*paintEv);
  delete sizeEv;
  delete paintEv;
}


void MptkGuiTFView::zoom(float td,float tf,float fd,float ff)
{
  tdeb=(int)((float)td*(float)sampleRate);
  tfin=(int)((float)tf*(float)sampleRate);
  fdeb=fd/sampleRate;
  ffin=ff/sampleRate;
  wxSizeEvent * sizeEv=new wxSizeEvent(); 
  OnResize(* sizeEv);
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(*paintEv);
  delete sizeEv;
  delete paintEv;
}

 void MptkGuiTFView::zoom(float td,float tf)
{
  
  tdeb=(int)((float)td*(float)sampleRate);
  tfin=(int)((float)tf*(float)sampleRate);
  wxSizeEvent * sizeEv=new wxSizeEvent(); 
  OnResize(* sizeEv);
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(*paintEv);
  delete sizeEv;
  delete paintEv;
}

void MptkGuiTFView::sendZoom()
{
  MptkGuiZoomEvent * event=new MptkGuiZoomEvent(GetId(), (float) tdeb/(float) sampleRate, (float) tfin/(float)sampleRate, fdeb*(float)sampleRate, ffin*(float)sampleRate);
  ProcessEvent(*event);
  delete event;
}

void MptkGuiTFView::setSelectedChannel(int chan)
{
  dessin->Clear();
  dessin->setSelectedChannel(chan);
  dessin->dessine_TF_map();
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(*paintEv);
  delete paintEv;
}


void MptkGuiTFView::refreshColor()
{
  dessin->Clear();
  dessin->dessine_TF_map();
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(*paintEv);
  delete paintEv;
}
