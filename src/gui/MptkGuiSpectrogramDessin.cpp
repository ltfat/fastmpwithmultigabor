#include "MptkGuiSpectrogramDessin.h"

MptkGuiSpectrogramDessin::MptkGuiSpectrogramDessin(wxWindow* parent, MP_Signal_c * signal, MptkGuiColormaps * couleur)
  :MptkGuiDessin(parent,couleur)
{
  this->signal=signal;
}

 MptkGuiSpectrogramDessin::~MptkGuiSpectrogramDessin(){}


void MptkGuiSpectrogramDessin::remplir_TF_map(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin)
{
  if (map!=NULL) {delete (map);}
  map= new MP_TF_Map_c (taille.GetWidth(), taille.GetHeight(), signal->numChans, tdeb, tfin, fdeb, ffin);
  //signal->add_to_tfmap(map,MP_TFMAP_PSEUDO_WIGNER,NULL);
}
