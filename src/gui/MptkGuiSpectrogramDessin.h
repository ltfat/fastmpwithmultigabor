#ifndef MPTKGUISPECTROGRAMDESSIN_H
#define MPTKGUISPECTROGRAMDESSIN_H

#include "MptkGuiDessin.h"

/** \brief In this class we will draw in the spectrogram view */
class MptkGuiSpectrogramDessin : public MptkGuiDessin
{
public :

  MptkGuiSpectrogramDessin(wxWindow* parent, MP_Signal_c * Signal, MptkGuiColormaps * couleur);
  ~MptkGuiSpectrogramDessin();
 
  virtual void remplir_TF_map(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin);


private:
  MP_Signal_c * signal;
};

#endif
