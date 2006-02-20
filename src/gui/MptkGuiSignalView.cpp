#include "MptkGuiSignalView.h"
#include "MptkGuiFrame.h"
#include <iostream>

#define SIGNAL_ZOOM_DEADZONE 10

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MptkGuiSignalView, wxPanel)
    EVT_PAINT  ( MptkGuiSignalView::OnPaint)
    EVT_LEFT_UP ( MptkGuiSignalView::reacLeftUp)
    EVT_SIZE  ( MptkGuiSignalView::onResize)
    EVT_LEFT_DOWN( MptkGuiSignalView::reacLeftDown )
    EVT_RIGHT_DOWN(MptkGuiSignalView::reacRightDown)
    EVT_RIGHT_UP(MptkGuiSignalView::reacRightUp)
    EVT_MIDDLE_DOWN(MptkGuiSignalView::reacMiddleDown)
    EVT_MIDDLE_UP(MptkGuiSignalView::reacMiddleUp)
    EVT_MOTION( MptkGuiSignalView::onMouseMove)
END_EVENT_TABLE() 

  MptkGuiSignalView::MptkGuiSignalView(wxWindow *parent, int id)
        : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    WIDTH=(*parent).GetSize().x;
    HEIGHT=(*parent).GetSize().y;
    bufferedimage=new wxBufferedPaintDC(this);
    (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
    cornerNW=wxPoint(0,0);
    origine= wxPoint(cornerNW.x,cornerNW.y+HEIGHT/2);
    zooming_left=false;
    zooming_right=false;
    setSignal(NULL);
    lastclick=wxPoint(-1,-1);
    selected_channel=0;
}

/**
 *Applique un nouveau signal a dessiner sur la zone a partir d'une 
 *structure definie dans la librairie MPTK
 *Si le parametre vaut NULL alors sur l'axe des abscisses est dessine sur la zone.
 */
void  MptkGuiSignalView::setSignal(MP_Signal_c * newsignal)
{
   WIDTH=GetSize().x;
   HEIGHT=GetSize().y;
   wxPoint previous_point;
   signal=newsignal;
   
   (*bufferedimage).SetPen(*wxBLACK_PEN);
   //On efface le canevas avant de dessiner dessus
   (*bufferedimage).DrawRectangle(cornerNW.x,cornerNW.y,cornerNW.x+WIDTH,cornerNW.y+HEIGHT);

    
    //On dessine a present le signal
    if(signal!=NULL)
      {
	 //On calcule l'amplitude min et max du signal
 	double ampmin=-1,ampmax=1;
 	for(int i=0;i<(int) (*signal).numSamples;i++)
 	  {
 	    double val=(double) (*signal).channel[selected_channel][i];
            if(val<ampmin) ampmin=val;
 	    else if(val>ampmax) ampmax=val;
 	  }
       
        if(-ampmin>ampmax) ampmax=-ampmin;
	else ampmin=-ampmax;//Pour que l'axe des abscisses (amp=0) soit bien au milieu de l'oscillogramme
	current_scale=((float)(HEIGHT))/((float)(ampmax-ampmin));
	current_start_x=0;
	current_start_y=-current_scale*ampmin;
	if (signal->numSamples >0) current_step=((float)(WIDTH))/((*signal).numSamples);
	else current_step = 1;
      }
    updateScreen();
}


/**
 *Fonction servant a rafraichir le dessin
 */
void   MptkGuiSignalView::OnPaint(wxPaintEvent & WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackgroundMode( wxSOLID );
    dc.Blit(0,0,WIDTH,HEIGHT,bufferedimage,0,0);
}

/**
*Fonction reagissant aux evenements 'relachement du bouton gauche de la souris
* alors que le curseur survole le canevas'. On va lancer un zoom sur le rectangle
* defini par le point ou le bouton gauche a ete enfonce et sa position courante.
*/
void MptkGuiSignalView::reacLeftUp(wxMouseEvent & event)
{
  zooming_left=false;
  (*bufferedimage).Blit(0,0,WIDTH,HEIGHT,backupbuffer,0,0);
  delete backupbuffer;
  wxPoint NWCorner;
  wxPoint SECorner;

  //On calcule le coin superieur gauche et inferieur droit de la selection
  if(event.GetPosition().x>lastclick.x)
    {
      NWCorner=wxPoint(lastclick.x,0);
      SECorner=wxPoint(event.GetPosition().x,0);
    }
  else
    {
      SECorner=wxPoint(lastclick.x,0);
      NWCorner=wxPoint(event.GetPosition().x,0);
    }

  if(event.GetPosition().y>lastclick.y)  
    {
      NWCorner.y=lastclick.y;
      SECorner.y=event.GetPosition().y;
    }
  else
    {
      SECorner.y=lastclick.y;
      NWCorner.y=event.GetPosition().y;
    }
  
  if(((SECorner.x-NWCorner.x)>SIGNAL_ZOOM_DEADZONE)&&((SECorner.y-NWCorner.y)>SIGNAL_ZOOM_DEADZONE))
    {
      //Les calculs suivants serviront a recalculer l'echelle (espace entre 2 echantillons,
      //correspondance entre valeur d'un echantillon et son ordonnee sur le canevas  
      float A=(NWCorner.x-current_start_x)/current_step;
      float B=(SECorner.x-current_start_x)/current_step;
      current_step=WIDTH/(B-A);
      current_start_x=-A*current_step;
      A=(NWCorner.y-current_start_y)/current_scale;
      B=(SECorner.y-current_start_y)/current_scale;
      current_scale=HEIGHT/(B-A);
      current_start_y=-A*current_scale;
      float ampmin=-current_start_y/current_scale;
      float ampmax=(HEIGHT-current_start_y)/current_scale;
      updateScreen();

      // Generation of a zoom event
   
      MptkGuiZoomEvent * evt=new MptkGuiZoomEvent(GetId(), (-current_start_x/current_step)/signal->sampleRate, ((WIDTH-current_start_x)/current_step)/signal->sampleRate,ampmin,ampmax);
      ProcessEvent(*evt);
      delete evt;
    }
}

/**
*Fonction reagissant aux evenements 'enfoncement du bouton gauche de la souris
* alors que le curseur survole le canevas'
*/
void MptkGuiSignalView::reacLeftDown(wxMouseEvent & event)
{
   zooming_left=true;
   backupbuffer=new wxBufferedPaintDC(this);
   (*backupbuffer).Blit(0,0,WIDTH,HEIGHT,bufferedimage,0,0);
   lastclick=event.GetPosition();
}


/**
*Fonction reagissant aux evenements 'enfoncement du bouton droit de la souris
* alors que le curseur survole le canevas'
*/
void MptkGuiSignalView::reacRightDown(wxMouseEvent & event )
{
   zooming_right=true;
   backupbuffer=new wxBufferedPaintDC(this);
   (*backupbuffer).Blit(0,0,WIDTH,HEIGHT,bufferedimage,0,0);
   lastclick=event.GetPosition();
}

/**
*Fonction reagissant aux evenements 'relachement du bouton droit de la souris
* alors que le curseur survole le canevas'. On va lancer un zoom sur le rectangle
* de hauteur la hauteur totale du canevas et de largeur celle du rectangle defini
* par le point ou le bouton droit a ete enfonce et sa position courante.
*/
void MptkGuiSignalView::reacRightUp(wxMouseEvent & event)
{
  zooming_right=false;
  (*bufferedimage).Blit(0,0,WIDTH,HEIGHT,backupbuffer,0,0);
  delete backupbuffer;
  wxPoint NWCorner;
  wxPoint SECorner;

  //On calcule le coin superieur gauche et inferieur droit de la selection
  if(event.GetPosition().x>lastclick.x)
    {
      NWCorner=wxPoint(lastclick.x,0);
      SECorner=wxPoint(event.GetPosition().x,0);
    }
  else
    {
      SECorner=wxPoint(lastclick.x,0);
      NWCorner=wxPoint(event.GetPosition().x,0);
    }

  NWCorner.y=0;
  SECorner.y=HEIGHT;

   if((SECorner.x-NWCorner.x>SIGNAL_ZOOM_DEADZONE)&&(SECorner.y-NWCorner.y>SIGNAL_ZOOM_DEADZONE))
     {
       //Les calculs suivants serviront a recalculer l'echelle (espace entre 2 echantillons,
       //correspondance entre valeur d'un echantillon et son ordonnee sur le canevas  
       float A=(NWCorner.x-current_start_x)/current_step;
       float B=(SECorner.x-current_start_x)/current_step;
       current_step=WIDTH/(B-A);
       current_start_x=-A*current_step;
       A=(NWCorner.y-current_start_y)/current_scale;
       B=(SECorner.y-current_start_y)/current_scale;
       current_scale=HEIGHT/(B-A);
       current_start_y=-A*current_scale;
       float ampmin=-current_start_y/current_scale;
       float ampmax=(HEIGHT-current_start_y)/current_scale;
       updateScreen();

       //On signale a la fenetre parente contenant les differentes vues le zoom
       //On passe pour cela les valeurs des indices des echantillons min et max a l'ecran
       MptkGuiZoomEvent * evt=new MptkGuiZoomEvent(GetId(), (-current_start_x/current_step)/signal->sampleRate, ((WIDTH-current_start_x)/current_step)/signal->sampleRate,ampmin,ampmax);
       ProcessEvent(*evt);
       delete evt;
     }
}

void MptkGuiSignalView::reacMiddleDown(wxMouseEvent & WXUNUSED(event))
{
}

void MptkGuiSignalView::reacMiddleUp(wxMouseEvent & WXUNUSED(event))
{
  resetZoom();
}

/**
 *Donne la position du dernier clic souris sur le canevas
 */
wxPoint  MptkGuiSignalView::getLastClick()
{
  return lastclick;
}

/**
 *Rend le signal courant 
 */
MP_Signal_c *  MptkGuiSignalView::getSignal()
{
  return signal;
}

/**
 *Effectue un zoom sur la zone rectangulaire definie par les 4 nouvelles valeurs d'echelle.
 *Le premier et le deuxieme parametre correspondent respectivement aux temps et aux valeurs
 *  d'amplitudes aux bornes.
 */
void  MptkGuiSignalView::zoom(float tdeb, float tfin, float min_amp, float max_amp)
{
  float firstSample=tdeb*signal->sampleRate;
  float lastSample=tfin*signal->sampleRate;
  current_step=WIDTH/((float)(lastSample-firstSample));
  current_start_x=-((float)firstSample)*current_step;
  current_scale=HEIGHT/(max_amp-min_amp);
  current_start_y=-current_scale*min_amp;
  updateScreen();
}

void  MptkGuiSignalView::zoom(float tdeb, float tfin)
{
  float firstSample=tdeb*signal->sampleRate;
  float lastSample=tfin*signal->sampleRate;
  current_step=WIDTH/((float)(lastSample-firstSample));
  current_start_x=-((float)firstSample)*current_step;
  updateScreen();
}

/**
 *.Fonction servant a rafraichir entierement l'image, en redessinant tout le signal.
 * Elle sera appelee en cas de redimensionnement de la fenetre, chargement d'un nouveau
 * signal, zoom, ou changement du canal du signal a  afficher.
 */  
void  MptkGuiSignalView::updateScreen()
{
  (*bufferedimage).SetPen(*wxWHITE_PEN);
   //On efface le canevas avant de dessiner dessus
   (*bufferedimage).DrawRectangle(cornerNW.x,cornerNW.y,cornerNW.x+WIDTH,cornerNW.y+HEIGHT);
   //(*bufferedimage).SetPen(*wxGREEN_PEN);

    //On dessine l'axe x si besoin
  int pos_axe=(int) current_start_y;
  if(pos_axe>0&&pos_axe<HEIGHT)
    {
       (*bufferedimage).SetPen(*wxRED_PEN);
       (*bufferedimage).DrawLine(0,pos_axe,WIDTH,pos_axe); 
    }

  (*bufferedimage).SetPen(*new wxPen(*new wxColour(0,0,172)));
  if(signal!=NULL && signal->numSamples>0)
    {
      //Il y a 2 facons utilisees pour dessiner l'oscillogramme du signal.
      //La premiere dite 'antialiasing' est utilisee lorsqu'il y a plus d'echantillons que de 
      //colonnes de pixels sur la zone d'affichage. La seconde est utilisee dans le cas contraire
      int indexmin=(int) (-current_start_x/current_step);
      int indexmax=(int) ((WIDTH-current_start_x)/current_step);
      float nsamp=((float)(indexmax-indexmin))/WIDTH;;
      if(nsamp>=2)
	{
	  //On emploie ici la methode 'antialiasing'. Cela consiste a tracer dans chaque colonne une droite
	  // entre le min et le max des echantillons tombant dans cette colonne.
	  //printf("updateScreen avec antialiasing\n");
	  
	  int i;
	  float minsamplevalue,maxsamplevalue,maxprec,minprec;
	  minsamplevalue=0;
	  maxsamplevalue=0;
	  //nsamp=(*signal).numSamples/WIDTH;
	  for(i=0;i<WIDTH;i++)
	    {
	      //calculons le min et max des echantillons sur cette colonne
	      
	      if(i==0)
		{
		  for(int j=indexmin;j<((int) (nsamp+indexmin));j++)
		    {
		      if(j==indexmin)
			{
			  minsamplevalue=(*signal).channel[selected_channel][j];
			  maxsamplevalue=(*signal).channel[selected_channel][j];
			}
		      else
			{           
			  if((*signal).channel[selected_channel][j]>maxsamplevalue)
			    maxsamplevalue=(*signal).channel[selected_channel][j];
                        
			  if((*signal).channel[selected_channel][j]<minsamplevalue)
			    minsamplevalue=(*signal).channel[selected_channel][j];
			}              
		    }
		}
	      else if(i==WIDTH-1)
		{
		  //Ici on cherche dans le 'reste' d'échantillons de la division
		  //du nombre total d'échantillons par le nombre de pixels par ligne
		  for(int j=(int)(nsamp*i+indexmin);j<indexmax;j++)
		    {
		      if(j==(int)(nsamp*i+indexmin))
			{
			  minsamplevalue=(*signal).channel[selected_channel][j];
			  maxsamplevalue=(*signal).channel[selected_channel][j];
			}
		      else
			{           
			  if((*signal).channel[selected_channel][j]>maxsamplevalue)
			    maxsamplevalue=(*signal).channel[selected_channel][j];
                        
			  if((*signal).channel[selected_channel][j]<minsamplevalue)
			    minsamplevalue=(*signal).channel[selected_channel][j];
			}   
		    }
		}
	      else
		{
		  for(int j=(int)(nsamp*i+indexmin);j<(int)(nsamp*(i+1)+indexmin);j++)
		    {
		      if(j==(int)(nsamp*i+indexmin))
			{
			  minsamplevalue=(*signal).channel[selected_channel][j];
			  maxsamplevalue=(*signal).channel[selected_channel][j];
			}
		      else
			{           
			  if((*signal).channel[selected_channel][j]>maxsamplevalue)
			    maxsamplevalue=(*signal).channel[selected_channel][j];
                        
			  if((*signal).channel[selected_channel][j]<minsamplevalue)
			    minsamplevalue=(*signal).channel[selected_channel][j];
			}
		    }
		}
		  
	      if(i>0)
		{
		  //Si le max de la colonne precedente est inferieur au min de la
		  //colonne i ou bien l'inverse alors on trace un trait joignant 
		  //les deux colonnes
		 //  (*bufferedimage).SetPen(*wxGREEN_PEN);
		  if(minsamplevalue>maxprec)
		    {
		      (*bufferedimage).DrawLine((i-1),(int) (current_scale*maxprec+current_start_y),i,(int) (current_scale*minsamplevalue+current_start_y));                       
		    }
		  else if(maxsamplevalue<minprec)
		    {
		      (*bufferedimage).DrawLine((i-1),(int)(current_scale*minprec+current_start_y),i,(int) (current_scale*maxsamplevalue+current_start_y));    
		    }
		 // (*bufferedimage).SetPen(*new wxPen(*new wxColour(0,0,128)));
		}
		
	        minprec=minsamplevalue;
	        maxprec=maxsamplevalue;
          
		//On dessine un trait allant de la valeur minimum des echantillons de 
		//cette colonne au max.
		(*bufferedimage).DrawLine(i ,(int)  (current_scale*minsamplevalue+current_start_y),i,(int) (current_scale*maxsamplevalue+current_start_y-1));
	    }
	  
	}
      else 
	{
	  //La seconde methode, pour le cas ou les echantillons peuvent etre moins nombreux
	  //que les colonnes de pixels, consiste a les relier par des droites.
	  int i,x1,x2,y1,y2;

	  x1=(int) (current_start_x);
	  y1=(int) (((*signal).channel[selected_channel][0])*current_scale+current_start_y);
	  for(i=1;i<(int)(*signal).numSamples;i++)
	    {
	      x2=(int) (i*current_step+current_start_x);
	      y2=(int) (((*signal).channel[selected_channel][i])*current_scale+current_start_y);
	      if((x1<WIDTH)&&(x2>0)&&(!((y1<0&&y2<0)||(y1>=HEIGHT&&y2>=HEIGHT))))
		{
		  (*bufferedimage).DrawLine(x1,y1,x2,y2);
		}	
	      x1=x2;
	      y1=y2;
	    }
	}
    }

  this->OnPaint(*(new wxPaintEvent(0)));
}

/**
 * Reinitialisation du zoom (affiche le signal comme 
 * s'il venait d'etre charge)
 */
void MptkGuiSignalView::resetZoom()
{
  setSignal(signal);

   float ampmin=-current_start_y/current_scale;
   float ampmax=(HEIGHT-current_start_y)/current_scale;
   MptkGuiZoomEvent * evt=new MptkGuiZoomEvent(GetId(), (-current_start_x/current_step)/signal->sampleRate, ((WIDTH-current_start_x)/current_step)/signal->sampleRate,ampmin,ampmax);
   ProcessEvent(*evt);
   delete evt;
}

/**
 * Fonction appelee lors d'un redimensionnement de cette vue signal
 */
void  MptkGuiSignalView::onResize(wxSizeEvent & event )
{
  current_step=current_step*event.GetSize().GetWidth()/WIDTH;
  current_scale=current_scale*event.GetSize().GetHeight()/HEIGHT;
  current_start_x=current_start_x*event.GetSize().GetWidth()/WIDTH;
  current_start_y=current_start_y*event.GetSize().GetHeight()/HEIGHT;
  WIDTH=event.GetSize().GetWidth();
  HEIGHT=event.GetSize().GetHeight();
  
  if (bufferedimage != NULL) delete bufferedimage;
  bufferedimage=new wxBufferedPaintDC(this);
  (*bufferedimage).SetBrush(*wxWHITE_BRUSH);
  updateScreen();
}

/**
 *Selectionne le canal du signal a afficher. Si le signal est NULL,
 * ou le canal demande inexistant, le canal selectionne sera remis 
 * a 0.
 */ 
void  MptkGuiSignalView::setSelectedChannel(int numchan)
{
  if(signal==NULL)
    {
      selected_channel=0;
    }
  else
    {
      if((numchan>=0)&&(numchan<(*signal).numChans))
	{
	  selected_channel=numchan;
	}
      else
	{
	  selected_channel=0;
	}
      updateScreen();
    }
	    
}

/**
 * Retourne le numero (indice) du canal selectionne
 */
int MptkGuiSignalView::getSelectedChannel()
{
	return selected_channel;
}

/**
 *Reaction au deplacement de la souris sur la zone d'affichage (canevas)
 */
void MptkGuiSignalView::onMouseMove(wxMouseEvent &event)
{
  if(zooming_left||zooming_right)
    {
      (*bufferedimage).Blit(0,0,WIDTH,HEIGHT,backupbuffer,0,0);
      if(((event.GetPosition().x-lastclick.x>SIGNAL_ZOOM_DEADZONE)||(event.GetPosition().x-lastclick.x<-SIGNAL_ZOOM_DEADZONE))
	 &&((event.GetPosition().y-lastclick.y>SIGNAL_ZOOM_DEADZONE)||(event.GetPosition().y-lastclick.y<-SIGNAL_ZOOM_DEADZONE)))
	{
	  (*bufferedimage).SetPen(*wxBLACK_PEN);
	  if(zooming_left)
	    {
	      (*bufferedimage).DrawLine(lastclick.x,lastclick.y,event.GetPosition().x,lastclick.y);
	      (*bufferedimage).DrawLine(lastclick.x,lastclick.y,lastclick.x,event.GetPosition().y);
	      (*bufferedimage).DrawLine(lastclick.x,event.GetPosition().y,event.GetPosition().x,event.GetPosition().y);
	      (*bufferedimage).DrawLine(event.GetPosition().x,lastclick.y,event.GetPosition().x,event.GetPosition().y);
	    }
	  else if(zooming_right)
	    {
	      (*bufferedimage).DrawLine(lastclick.x,0,event.GetPosition().x,0);
	      (*bufferedimage).DrawLine(lastclick.x,0,lastclick.x,HEIGHT-1);
	      (*bufferedimage).DrawLine(lastclick.x,HEIGHT-1,event.GetPosition().x,HEIGHT-1);
	      (*bufferedimage).DrawLine(event.GetPosition().x,0,event.GetPosition().x,HEIGHT-1);
	    }
	}
      this->OnPaint(*(new wxPaintEvent(0)));
    }
  float t = getStartTime() + (getEndTime()-getStartTime())*event.GetX()/WIDTH;
  float a = getMinAmp() + (getMaxAmp()-getMinAmp())*event.GetY()/HEIGHT;
  MPTK_GUI_STATUSBAR->SetStatusText(wxString::Format("x : %f sec. (%lu samples), y : %f",t, (unsigned long int)(t*signal->sampleRate), a));
}

/**
 *Donne le temps correspondant au debut du signal affiche
 */
float MptkGuiSignalView::getStartTime()
{
  if(signal!=NULL)
    {
      return -current_start_x/(current_step*signal->sampleRate);
    }
  else return -1;
}

/**
 *Donne le temps correspondant a la fin du signal affiche
 */
float MptkGuiSignalView::getEndTime()
{
   if(signal!=NULL)
    {
     // printf("getEndTime:current_start_x:%f,current_step:%f,WIDTH:%d,samplerate:%f\n",current_start_x,current_step,WIDTH,signal->sampleRate);
      return (WIDTH-current_start_x)/(current_step*signal->sampleRate);
    }
  else return -1;
}

/**
 *Donne l'amplitude min du zoom courant
 */
float MptkGuiSignalView::getMinAmp()
{
  if(signal!=NULL)
    {
      return (-current_start_y)/current_scale;
    }
  else return -1;
}

/**
 *Donne l'amplitude max du zoom courant
 */
float MptkGuiSignalView::getMaxAmp()
{
  if(signal!=NULL)
    {
      return (HEIGHT-current_start_y)/current_scale;
    }
  else return -1;
}

