#ifndef MPTKGUIATOMVIEW_H
#define MPTKGUIATOMVIEW_H

#include "MptkGuiTFView.h"
#include "MptkGuiAtomDessin.h"
#include "MptkGuiResizeTFMapEvent.h"

/** \brief In this class we will take in charge the interaction with the atom view */
class MptkGuiAtomView : public MptkGuiTFView
{
public :

  MptkGuiAtomView(wxWindow* parent, int id, MP_Book_c * signal, MptkGuiColormaps * couleur);
  ~MptkGuiAtomView();

  void OnResize(wxSizeEvent& evt);
  void OnMotion(wxMouseEvent& evt);
  void resetZoom(); 

protected:
  MP_Book_c * book;
    };

#endif
