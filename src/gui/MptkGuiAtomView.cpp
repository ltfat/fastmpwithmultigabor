#include  "MptkGuiAtomView.h"
#include  "MptkGuiFrame.h"

  MptkGuiAtomView::MptkGuiAtomView(wxWindow* parent, int id, MP_Book_c * book,  MptkGuiColormaps * couleur)
    :MptkGuiTFView(parent,id,couleur, book->sampleRate)
{
  dessin = NULL;
  this->book=book;
  tdeb=0;
  tfin=book->numSamples;
  fdeb=0;
  ffin=0.5;
}

MptkGuiAtomView::~MptkGuiAtomView()
{
  delete dessin;
}

void MptkGuiAtomView::OnResize(wxSizeEvent& WXUNUSED(evt))
{
if (book->numAtoms > 0){
  if (dessin!=NULL) {delete dessin;} 
  dessin = new MptkGuiAtomDessin(this,book,colormap);
  dessin->dessine(tdeb,tfin,fdeb,ffin);
  maxTotal=dessin->maxTotal;
  MptkGuiResizeTFMapEvent * Ev=new MptkGuiResizeTFMapEvent();
  ProcessEvent(* Ev);
  delete Ev;
 }
}


void  MptkGuiAtomView::resetZoom()
{
  tdeb=0;
  tfin=book->numSamples-1;
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


void MptkGuiAtomView::OnMotion(wxMouseEvent& evt)
{
  MP_Atom_c *atomClosest = NULL;
  unsigned long int nClosest;

  MptkGuiTFView::OnMotion(evt);
  float t = ((float)tdeb)+evt.GetX()*((float)(tfin-tdeb))/((float) (GetSize().GetWidth()));
  float f = fdeb+ (GetSize().GetHeight()-evt.GetY())*((ffin-fdeb)/GetSize().GetHeight());
  /* Find closest atom in book */
  atomClosest = book->get_closest_atom(t,f,selectedChannel,NULL,&nClosest);

    //// if (NULL != atomClosest) {
    ////    ??? atomClosest->getSupportPolygon(????);
    ///     drawSupportPolygon
    ///  }
//     /* Draw a rectangle around it */
//     if (distClosest != -1.0) {
//       float t1,t2,df,f1l,f1h,f2l,f2h,xscale,yscale;
//       int x1,x2,y1l,y1h,y2l,y2h;
//       wxPaintDC * dc = new wxPaintDC(this);
//       PrepareDC(*dc);
//       dc->Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight(),dessin,0,0);
//       /* Compute the coordinates of the rectangle in real coordinates */
//       t1 = (float)gatom->support[selectedChannel].pos;
//       t2 = t1+duration;
//       df = 40 / duration;
//       f1l = (float)gatom->freq-df/2;
//       f1h = (float)gatom->freq+df/2;
//       f2l = (float)gatom->freq+slope*duration-df/2;
//       f2h = (float)gatom->freq+slope*duration+df/2;
//       /* Convert into local coordinates */
//       xscale = (float)(GetSize().GetWidth())/((float)(tfin-tdeb));
//       yscale = (float)(GetSize().GetHeight())/((float)(ffin-fdeb));
//       x1  = (int)(xscale*(t1 -(float)tdeb));
//       x2  = (int)(xscale*(t2 -(float)tdeb));
//       y1l = GetSize().GetHeight()-(int)(yscale*(f1l-(float)fdeb));
//       y1h = GetSize().GetHeight()-(int)(yscale*(f1h-(float)fdeb));
//       y2l = GetSize().GetHeight()-(int)(yscale*(f2l-(float)fdeb));
//       y2h = GetSize().GetHeight()-(int)(yscale*(f2h-(float)fdeb));

//       /* The drawing itself */
//       dc->SetPen(* wxBLACK_PEN);
//       dc->SetBrush(* wxTRANSPARENT_BRUSH);
//       dc->DrawLine(x1,y1l,x2,y2l);
//       dc->DrawLine(x2,y2l,x2,y2h);
//       dc->DrawLine(x2,y2h,x1,y1h);
//       dc->DrawLine(x1,y1h,x1,y1l);
//       delete dc;
//     

  if (NULL == atomClosest)
    MPTK_GUI_STATUSBAR->SetStatusText(wxString::Format("x : %f sec (%lu samples)., y : %f Hz [no atom]",t/(float)sampleRate, (unsigned long int)t, f*(float)sampleRate));
  else
    MPTK_GUI_STATUSBAR->SetStatusText(wxString::Format("x : %f sec (%lu samples)., y : %f Hz [atom : %lu/%lu '%s']",t/(float)sampleRate, (unsigned long int)t, f*(float)sampleRate, nClosest, book->numAtoms, atomClosest->type_name()));
}				   
