#ifndef MPTKGUIDESSIN_H
#define MPTKGUIDESSIN_H

#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "mptk.h"
#include "MptkGuiColormaps.h"

/***********************/
/* MptkGuiDessin CLASS */
/***********************/

/**
   \brief
   * MptkGuiDessin Draw inside the MptkGuiTFView, it is a super class that should not be
   * directly use.
   * MptkGuiDessin use the MP_TF_Map_c class to draw, first the MP_TF_Map_c object is fill with
   * the data you want to draw and then the MP_TF_Map_c object is draw on MptkGuiTFView
*/

class MptkGuiDessin : public wxBufferedPaintDC
{
public :
  /** \brief constructor 
   * \param parent the parent of the MptkGuiDessin it should be a MptkGuiTFView objet
   * \param couleur the colormaps that will be use to draw
   */
  MptkGuiDessin(wxWindow* parent , MptkGuiColormaps * couleur);
  /** \brief destructor */
  ~MptkGuiDessin();

  /** \brief dessine draw on MptkGuiTFView between the sample tdeb and tfin and between the stantardize frequency fdeb and ffin
   * \param tdeb the first sample
   * \param tfin the last sample
   * \param fdeb the minimum frequency
   * \param ffin the maximum frequency
   */
  void dessine(int tdeb, int tfin, MP_Real_t fdeb, MP_Real_t ffin);

  /** \brief remplir_TF_map is a virtual fonction that should be implement in all class that inherite MptkGuiDessin
   * \param tdeb the first sample
   * \param tfin the last sample
   * \param fdeb the minimum frequency
   * \param ffin the maximum frequency
   */
  virtual void remplir_TF_map(int WXUNUSED(tdeb), int WXUNUSED(tfin), MP_Real_t WXUNUSED(fdeb), MP_Real_t WXUNUSED(ffin)){}

  /** \brief dessine_TF_map draw the MP_TF_Map_c on MptkGuiTFView  */
  void dessine_TF_map();

  /** \brief setSelectedChannel is used to set the channel you want to be draw on MptkGuiTFView
   * \param chan the channel of the book that will be draw
   */
  void setSelectedChannel(int chan);

  /** \brief
   * MaxTotal is the maximum value you will find in the MP_TF_Map_c
   */
  float maxTotal;
protected:
  MP_TF_Map_c * map;
  MptkGuiColormaps * couleur;
  wxWindow * parent;
  wxSize taille;
  int selectedChannel;
};

#endif
