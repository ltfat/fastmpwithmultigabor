#ifndef MPTKGUIATOMDESSIN_H
#define MPTKGUIATOMDESSIN_H

#include "MptkGuiDessin.h"

/** \brief In this class we will draw in the atom view */
class MptkGuiAtomDessin : public MptkGuiDessin
{
public :

  MptkGuiAtomDessin(wxWindow* parent, MP_Book_c * book, MptkGuiColormaps * couleur);
  ~MptkGuiAtomDessin();
 
  virtual void remplir_TF_map(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin);


private:
  MP_Book_c * book;
};

#endif
