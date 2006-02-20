#ifndef MPTKGUITFVIEW_H
#define MPTKGUITFVIEW_H

#include "wx/wx.h"
#include "mptk.h"
#include "MptkGuiDessin.h"
#include "MptkGuiZoomEvent.h"


/***********************/
/* MptkGuiTFView CLASS */
/***********************/

/**
   \brief
   * This class is the panel where we will draw the time/frequency data it is a super class that should not be use directly
*/

class MptkGuiTFView : public wxPanel
{
public :
/** \brief constructor 
   * \param parent the parent of the MptkGuiTFView it should be a MptkGuiExtendedTFView object
   * \param id this is the identificator of the MptkGuiTFView it is used to send event
   * \param couleur the colormaps that will be use to draw
   * \param rate it is the sample rate of the data that will be draw
   */
  MptkGuiTFView(wxWindow* parent, int id, MptkGuiColormaps * couleur, int rate);

/** \brief destructor */
  virtual ~MptkGuiTFView();

/** \brief OnPaint refresh the image when it get a paint event
 * \param evt paint event that is handle
 */
  void OnPaint(wxPaintEvent& evt);

/** \brief OnResize resize the image when needed
 * \param evt resize event that is handle
 */
  virtual void OnResize(wxSizeEvent& WXUNUSED(evt)){}

/** \brief reset the View to its original time/frequency */
  virtual void resetZoom(){}

/** \brief Ondown is called when the mouse button is pressed
 * \param evt mouse event that is handle
 */
  void OnDown(wxMouseEvent& evt);

/** \brief OnUp is called when the mouse button is no longer pressed
 * \param evt mouse event that is handle
 */
  void OnUp(wxMouseEvent& evt);

/** \brief OnMotion is called when you move the  mouse above the panel, 
 * if the left button is pressed it will draw a rectangle between the point where you have pressed the button and the actual point.
 * \param evt mouse event that is handle
 */
  virtual void OnMotion(wxMouseEvent& evt);

/** \brief OnCenterClick is called when you click the center button of the mouse, its effect is to reset the zoom 
 * \param evt mouse event that is handle
 */
  void OnCenterClick(wxMouseEvent& evt);

/** \brief This function zoom between the sample td and tf and between the standard frequency fd and ff
   * \param td the first sample
   * \param tf the last sample
   * \param fd the minimum frequency
   * \param ff the maximum frequency
   */
  void zoom(int td,int tf,float fd,float ff);

/** \brief This function zoom between the time td and tf and the frequency fd and ff
 * \param td the begining time
   * \param tf the ending time
   * \param fd the minimum frequency
   * \param ff the maximum frequency
 */
  void zoom(float td,float tf,float fd,float ff);

/** \brief This function zoom between the time td and tf
 * \param tdeb the begining time
   * \param tfin the ending time
 */
  void zoom(float td,float tf);

/** \brief Function that send a MptkGuiZoomEvent to its parent, it is use to synchronize all view*/
  void sendZoom();

/** \brief Procedure use to change the channel that will be draw
 * \param chan number of the channel to draw
 */
  void setSelectedChannel(int chan);
/** \brief refresh the image when  we change the colormap
 */
  void refreshColor();


  /** \brief Maximum amplitude you will find in the TF_map */
  float maxTotal;
/** \brief The first sample */
  int tdeb;
/** \brief The last sample */
  int tfin;
/** \brief The minimum frequency */
  double fdeb;
/** \brief The maximum frequency */
  double ffin;
  
  protected:
  MptkGuiDessin * dessin;
  MptkGuiColormaps * colormap;
  int selectedChannel;
  int ttemp;
  double ftemp;
  int coordtempX;
  int coordtempY;
  bool zooming;
  int sampleRate;
  DECLARE_EVENT_TABLE()
    };

#endif
