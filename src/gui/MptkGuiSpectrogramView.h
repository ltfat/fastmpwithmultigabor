#ifndef MPTKGUISPECTROGRAMVIEW_H
#define MPTKGUISPECTROGRAMVIEW_H

#include "MptkGuiTFView.h"
#include "MptkGuiSpectrogramDessin.h"
#include "MptkGuiResizeTFMapEvent.h"
/** \brief In this class we will take in charge the interaction with the spectrogram view */
class MptkGuiSpectrogramView : public MptkGuiTFView
{
public :

   MptkGuiSpectrogramView(wxWindow* parent, int id, MP_Signal_c * signal, MptkGuiColormaps * couleur);
  ~ MptkGuiSpectrogramView();

  void OnResize(wxSizeEvent& evt);
  void resetZoom();

protected:
  MP_Signal_c * signal;
    };

#endif
