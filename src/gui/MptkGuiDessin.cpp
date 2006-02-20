#include "MptkGuiDessin.h"


MptkGuiDessin::MptkGuiDessin(wxWindow* parent, MptkGuiColormaps * couleur)
  :wxBufferedPaintDC(parent)
{
  this->parent=parent;
  this->couleur = couleur;
  selectedChannel=0;
  map=NULL;
}

 MptkGuiDessin::~MptkGuiDessin()
{
  delete (map);
}

void MptkGuiDessin::dessine(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin)
{
  taille=parent->GetSize();
  remplir_TF_map(tdeb,tfin,fdeb,ffin);
  dessine_TF_map();
}


void MptkGuiDessin::dessine_TF_map()
{
  float t;
  for(int x=0; x<taille.GetHeight(); x++) {
    for (int y=0; y<taille.GetWidth(); y++) {
      MP_Colormap_t * index=couleur->give_color(map->channel[selectedChannel][y*taille.GetHeight()+x]);
      wxColour coul((int) (index[0]*255),(int) (index[1]*255),(int) (index[2]*255));
      wxPen pen(coul);
      SetPen(pen);
      DrawPoint(y,taille.GetHeight()-x);
    }
  }
}


void MptkGuiDessin::setSelectedChannel(int chan)
{
  selectedChannel=chan;
}
