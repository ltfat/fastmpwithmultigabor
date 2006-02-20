#include  "MptkGuiSpectrogramView.h"

  MptkGuiSpectrogramView::MptkGuiSpectrogramView(wxWindow* parent, int id, MP_Signal_c * signal,  MptkGuiColormaps * couleur)
    :MptkGuiTFView(parent,id,couleur, signal->sampleRate)
{
  this->signal=signal;
  tdeb=0;
  tfin=signal->numSamples;
  fdeb=0;
  ffin=0.5;
  dessin = NULL;
}

MptkGuiSpectrogramView::~MptkGuiSpectrogramView()
{
  delete dessin;
}

void MptkGuiSpectrogramView::OnResize(wxSizeEvent& WXUNUSED(evt))
{
  if (dessin!=NULL) {delete dessin;} 
  dessin = new MptkGuiSpectrogramDessin(this,signal,colormap);
  dessin->dessine(tdeb,tfin,fdeb,ffin);
  maxTotal=dessin->maxTotal;
  MptkGuiResizeTFMapEvent * Ev=new MptkGuiResizeTFMapEvent();
  ProcessEvent(* Ev);
  delete Ev;
}


void  MptkGuiSpectrogramView::resetZoom()
{
  tdeb=0;
  tfin=signal->numSamples-1;
  fdeb=0;
  ffin=0.5;
  sendZoom();
  wxSizeEvent * sizeEv=new wxSizeEvent(); 
  OnResize(* sizeEv);
  wxPaintEvent * paintEv=new wxPaintEvent();
  OnPaint(* paintEv);
  delete sizeEv;
  delete paintEv;
}
