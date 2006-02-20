#include "MptkGuiAtomDessin.h"

MptkGuiAtomDessin::MptkGuiAtomDessin(wxWindow* parent, MP_Book_c * book, MptkGuiColormaps * couleur)
  :MptkGuiDessin(parent,couleur)
{
  this->book=book;
}

 MptkGuiAtomDessin::~MptkGuiAtomDessin(){}


void MptkGuiAtomDessin::remplir_TF_map(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin)
{
  if (map!=NULL) {delete (map);}
  map= new MP_TF_Map_c (taille.GetWidth(), taille.GetHeight(), book->numChans, tdeb, tfin, fdeb, ffin);
  book->add_to_tfmap(map,MP_TFMAP_PSEUDO_WIGNER,NULL);
  maxTotal=map->ampMax;
}
