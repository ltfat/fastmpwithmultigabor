#include "MptkGuiColorMapView.h"
#include <math.h>
#include <iostream>
#define LATERAL_MARGIN 50  //Marge a gauche et a droite de la barre en degrade representant la colormap
#define VERTICAL_MARGIN 5 //Marge verticale au dessus de la barre de colormap
#define SIZER_SIZE 10  //largeur et hauteur des triangles sur lesquels on clique 
#define TEXTFIELD_WIDTH 45  //largeur du champ texte affichant les valeurs en dB
#define TEXTFIELD_HEIGHT 15 //hauteur du champ texte affichant les valeurs en dB
#define CMAP_HEIGHT 8  //hauteur de la barre en degrade representant la colormap
#define RED 0
#define GREEN 1
#define BLUE 2


BEGIN_EVENT_TABLE(MptkGuiColorMapView, wxPanel)
    EVT_PAINT(MptkGuiColorMapView::OnPaint)
    EVT_LEFT_DOWN(MptkGuiColorMapView::reacLeftDown)
    EVT_LEFT_UP(MptkGuiColorMapView::reacLeftUp)
  EVT_MIDDLE_UP(MptkGuiColorMapView::reacMiddleUp)
    EVT_SIZE(MptkGuiColorMapView::onResize)
    EVT_MOTION(MptkGuiColorMapView::reacMouseMotion)
END_EVENT_TABLE()  


  MptkGuiColorMapView::MptkGuiColorMapView(wxWindow *parent,MptkGuiColormaps *cmap,float min_bound,float max_bound)
        : wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    WIDTH=GetSize().x;
    HEIGHT=GetSize().y;
    bufferedimage=new wxBufferedPaintDC(this);
    (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
    colormap=cmap;
    minsizerpos=LATERAL_MARGIN;
    maxsizerpos=WIDTH-LATERAL_MARGIN-1;
    this->bornemin=min_bound;
    this->bornemax=max_bound;  
    prio=false;
    minsizermoving=false;
    maxsizermoving=false;
}

/**
 *Change l'objet colormap 
 */
void MptkGuiColorMapView::setColorMap(MptkGuiColormaps * new_cmap)
{
  colormap=new_cmap;
  drawColorMap();
}

/**
 *Fonction servant a redessiner le canevas
 */
void   MptkGuiColorMapView::OnPaint(wxPaintEvent & WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackgroundMode( wxSOLID );
    dc.Blit(0,0,WIDTH,HEIGHT,bufferedimage,0,0);
}


/**
 *Fonction appelee lors d'un redimensonnement
 */
void  MptkGuiColorMapView::onResize(wxSizeEvent & event )
{
  float rescale=((float) event.GetSize().GetWidth()-2*LATERAL_MARGIN)/(WIDTH-2*LATERAL_MARGIN);
  WIDTH=event.GetSize().GetWidth();
  HEIGHT=event.GetSize().GetHeight();
  delete bufferedimage;
  bufferedimage=new wxBufferedPaintDC(this);
  (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
  (*bufferedimage).SetPen(*wxWHITE_PEN); 
  (*bufferedimage).DrawRectangle(0,0,WIDTH,HEIGHT);
  minsizerpos=(minsizerpos-LATERAL_MARGIN)*rescale+LATERAL_MARGIN;
  if(minsizerpos<LATERAL_MARGIN) minsizerpos=LATERAL_MARGIN;
  maxsizerpos=(maxsizerpos-LATERAL_MARGIN)*rescale+LATERAL_MARGIN;
  if(maxsizerpos>=WIDTH-LATERAL_MARGIN) maxsizerpos=WIDTH-LATERAL_MARGIN-1;
  if(minsizerpos>maxsizerpos) minsizerpos=maxsizerpos;
  updateScreen();
}

/**
 * Rend la valeur min selectionnee  (correspondant a la valeur selectionnee par le curseur min)
 */
float MptkGuiColorMapView::getCurrentMin()
{
  return (minsizerpos-LATERAL_MARGIN)*((bornemax-bornemin)/(WIDTH-2*LATERAL_MARGIN))+bornemin;
} 

/**
 * Rend la valeur max selectionnee 
 */
float MptkGuiColorMapView::getCurrentMax()
{
  return (maxsizerpos-LATERAL_MARGIN)*((bornemax-bornemin)/(WIDTH-2*LATERAL_MARGIN))+bornemin;
} 

/**
 * Fonction appelee lorsqu'on enfonce le bouton gauche de la souris
 */
void MptkGuiColorMapView::reacLeftDown(wxMouseEvent & event)
{
  if((event.GetPosition().y>=VERTICAL_MARGIN+CMAP_HEIGHT) && (event.GetPosition().y<VERTICAL_MARGIN+CMAP_HEIGHT+SIZER_SIZE))
    {
      if(prio) //On considere que la fleche 'min' est sur le dessus en cas de chevauchement des fleches
	{
	  if((event.GetPosition().x>=minsizerpos-SIZER_SIZE/2)&&(event.GetPosition().x<minsizerpos+SIZER_SIZE/2))
	    {
	      minsizermoving=true;
	    }
	  else if((event.GetPosition().x>=maxsizerpos-SIZER_SIZE/2)&&(event.GetPosition().x<maxsizerpos+SIZER_SIZE/2))
	    {
	      maxsizermoving=true;prio=false;
	    }
	}
      else //On considere que la fleche 'max' est sur le dessus
	{
	  
	  if((event.GetPosition().x>=maxsizerpos-SIZER_SIZE/2)&&(event.GetPosition().x<maxsizerpos+SIZER_SIZE/2))
	    {
	      maxsizermoving=true;
	    }
	  else if((event.GetPosition().x>=minsizerpos-SIZER_SIZE/2)&&(event.GetPosition().x<minsizerpos+SIZER_SIZE/2))
	    {
	      minsizermoving=true;prio=true;
	    }
	}
    }
}

/**
 * Fonction appelee lors du relachement du bouton gauche de la souris
 */
void MptkGuiColorMapView::reacLeftUp(wxMouseEvent & WXUNUSED(event))
{
  if(minsizermoving) minsizermoving=false;
  else if(maxsizermoving) maxsizermoving=false;
  //MaJ de la colormap
  (*colormap).setMin(getCurrentMin());
  (*colormap).setMax(getCurrentMax());
  //Generation d'un evenement pour maj des vues correspondantes.
  MptkGuiCMapZoomEvent * evt=new MptkGuiCMapZoomEvent(GetId(),(int) getCurrentMin(),(int) getCurrentMax());
  ProcessEvent(*evt);
  delete evt;
  
}

void MptkGuiColorMapView::reacMiddleUp(wxMouseEvent & WXUNUSED(event))
{
  minsizerpos=LATERAL_MARGIN;
  maxsizerpos=WIDTH-LATERAL_MARGIN-1;
 (*colormap).setMin(getCurrentMin());
 (*colormap).setMax(getCurrentMax());
  MptkGuiCMapZoomEvent * evt=new MptkGuiCMapZoomEvent(GetId(),(int) getCurrentMin(),(int) getCurrentMax());
  updateScreen();
  ProcessEvent(*evt);
  delete evt;
}

/**
 * Change la borne min en dB
 */
void MptkGuiColorMapView::setMinBound(float new_minbound) 
{
  new_minbound=pow(10,new_minbound/20);
  if(new_minbound>bornemin) new_minbound=bornemin;
  float x=(bornemax-bornemin)/((float) (WIDTH-2*LATERAL_MARGIN));
  float y=(bornemax-new_minbound)/((float) (WIDTH-2*LATERAL_MARGIN));
  float minsizerval=(minsizerpos-LATERAL_MARGIN)*x;//minsizerval=valeur en dB pointee par le curseur min
  float maxsizerval=(maxsizerpos-LATERAL_MARGIN)*x;
  bornemin=new_minbound;
  minsizerpos=minsizerval/y+LATERAL_MARGIN;
  maxsizerpos=maxsizerval/y+LATERAL_MARGIN;
  updateScreen();
  MptkGuiCMapZoomEvent * evt=new MptkGuiCMapZoomEvent(GetId(),(int) getCurrentMin(),(int) getCurrentMax());
  ProcessEvent(*evt);
  delete evt;
}

/**
 * Change la borne max en dB
 */
void MptkGuiColorMapView::setMaxBound(float new_maxbound) 
{
  new_maxbound=pow(10,new_maxbound/20);
  if(new_maxbound<bornemin) new_maxbound=bornemin;
  float x=(bornemax-bornemin)/((float) (WIDTH-2*LATERAL_MARGIN));
  float y=(new_maxbound-bornemin)/((float) (WIDTH-2*LATERAL_MARGIN));
  float minsizerval=(minsizerpos-LATERAL_MARGIN)*x;//minsizerval=valeur en dB pointee par le curseur min
  float maxsizerval=(maxsizerpos-LATERAL_MARGIN)*x;
  bornemax=new_maxbound;
  minsizerpos=minsizerval/y+LATERAL_MARGIN;
  maxsizerpos=maxsizerval/y+LATERAL_MARGIN;
  updateScreen();
  MptkGuiCMapZoomEvent * evt=new MptkGuiCMapZoomEvent(GetId(),(int) getCurrentMin(),(int) getCurrentMax());
  ProcessEvent(*evt);
  delete evt;
}

/**
 * Donne la valeur de la borne min en dB
 */
float MptkGuiColorMapView::getMinBound()
{
  return 20*log10(bornemin);
}

/**
 * Donne la valeur de la borne max en dB
 */
float MptkGuiColorMapView::getMaxBound()
{
  return 20*log10(bornemax);
}

/**
 * Reaction a un deplacement de la souris
 */
void MptkGuiColorMapView::reacMouseMotion(wxMouseEvent & event)
{
  if(minsizermoving)
    {
      minsizerpos=event.GetPosition().x;
      if(minsizerpos<LATERAL_MARGIN) minsizerpos=LATERAL_MARGIN;
      if(minsizerpos>maxsizerpos) minsizerpos=maxsizerpos;
      updateScreen();
      this->OnPaint(*(new wxPaintEvent(0)));
      
    }
  else if(maxsizermoving) 
    {
      maxsizerpos=event.GetPosition().x;
      if(maxsizerpos>=WIDTH-LATERAL_MARGIN) maxsizerpos=WIDTH-LATERAL_MARGIN-1;
      if(maxsizerpos<minsizerpos) maxsizerpos=minsizerpos;
      updateScreen();
      this->OnPaint(*(new wxPaintEvent(0)));
    }

}

/**
 * Rafraichissement de la vue
 */
void MptkGuiColorMapView::updateScreen()
{
  drawColorMap();
  drawSizers();
  this->OnPaint(*(new wxPaintEvent(0)));
}

/**
 * Rafraichissemnt de la partie 'colormap' (degradé de couleurs) de la vue
 */
void MptkGuiColorMapView::drawColorMap()
{
  //On efface la partie du canevas correspondant a la representation de la colormap
  // en degrade
  int CMAP_WIDTH=WIDTH-2*LATERAL_MARGIN;
  (*bufferedimage).SetPen(*wxBLACK_PEN);
  (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
  (*bufferedimage).DrawRectangle(LATERAL_MARGIN,VERTICAL_MARGIN,CMAP_WIDTH+1,CMAP_HEIGHT+1);
  
   if(colormap!=NULL)
    {
      
      MP_Colormap_t * colours=(*colormap).getRow(1);
      wxColour myColour1((char) (255*colours[RED]),(char) (255*colours[GREEN]),(char) (255*colours[BLUE]));
      (*bufferedimage).SetPen(*new wxPen(myColour1));
      (*bufferedimage).SetBrush(*new wxBrush(myColour1));
      (*bufferedimage).DrawRectangle(LATERAL_MARGIN+1,VERTICAL_MARGIN+1,(int) minsizerpos-LATERAL_MARGIN,CMAP_HEIGHT-2);

      for(float i=minsizerpos;i<maxsizerpos;i++)
	{
	  float intensite=( ((i-minsizerpos)*((float)(*colormap).getColorNumber())/(maxsizerpos-minsizerpos)));
	  if(intensite>=(*colormap).getColorNumber()) intensite=(*colormap).getColorNumber()-1;
	  colours=(*colormap).getRow((int) intensite);
	  if(colours==NULL) break;
	  wxColour myColour2((char) (255*colours[RED]),(char) (255*colours[GREEN]),(char) (255*colours[BLUE]));
	  (*bufferedimage).SetPen(*new wxPen(myColour2));
	  (*bufferedimage).DrawLine((int) i,VERTICAL_MARGIN+1,(int) i,VERTICAL_MARGIN+CMAP_HEIGHT-1);
	}
      
      if((*colormap).getColorNumber()-1>0) colours=(*colormap).getRow((*colormap).getColorNumber()-1);
      else colours=(*colormap).getRow(1);
       wxColour myColour3((char) (255*colours[RED]),(char) (255*colours[GREEN]),(char) (255*colours[BLUE]));
      (*bufferedimage).SetPen(*new wxPen(myColour3));
      (*bufferedimage).SetBrush(*new wxBrush(myColour3));
      (*bufferedimage).DrawRectangle((int) maxsizerpos,VERTICAL_MARGIN+1,(int) (WIDTH-LATERAL_MARGIN-maxsizerpos),CMAP_HEIGHT-2);
    }
  
}

/**
 * Rafraichissemnt de la partie 'sizers' (fleches mobiles) de la vue
 */
void MptkGuiColorMapView::drawSizers()
{
  int pos_fleches=VERTICAL_MARGIN+CMAP_HEIGHT;
  //float current_db_scale=(dBmax-dBmin)/(HEIGHT-2*VERTICAL_MARGIN);
  (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
  (*bufferedimage).SetPen(*wxWHITE_PEN);
  (*bufferedimage).DrawRectangle(0,VERTICAL_MARGIN+CMAP_HEIGHT,WIDTH ,HEIGHT-pos_fleches);
  (*bufferedimage).SetPen(*wxBLACK_PEN);

  //On dessine les fleches des sizers
  (*bufferedimage).DrawLine((int) minsizerpos-SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE,(int) minsizerpos+SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE);
  (*bufferedimage).DrawLine((int) maxsizerpos-SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE,(int) maxsizerpos+SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE);
  (*bufferedimage).DrawLine((int) minsizerpos-SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE,(int) minsizerpos,(int) pos_fleches);
  (*bufferedimage).DrawLine((int) maxsizerpos-SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE,(int) maxsizerpos,(int) pos_fleches);
  (*bufferedimage).DrawLine((int) minsizerpos,(int) pos_fleches,(int) minsizerpos+SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE);
  (*bufferedimage).DrawLine((int) maxsizerpos,(int) pos_fleches,(int) maxsizerpos+SIZER_SIZE/2,(int) pos_fleches+SIZER_SIZE);
  //On dessine le texte du sizer a gauche de la fleche indiquant la valeur correspondant en dB a sa position
  (*bufferedimage).SetFont(*wxSMALL_FONT);
  wxString sizerstring="";
  sizerstring+=wxString::Format("%.1f",20*log10(getCurrentMin()));
  sizerstring+="dB";
  (*bufferedimage).DrawText(sizerstring,(int) minsizerpos-TEXTFIELD_WIDTH,pos_fleches);
  sizerstring="";
  sizerstring+=wxString::Format("%.1f",20*log10(getCurrentMax()));
  sizerstring+="dB";
  (*bufferedimage).DrawText(sizerstring,(int) maxsizerpos+SIZER_SIZE/2,(int) pos_fleches);
}

void setColorMapType(int new_colormap_type);

