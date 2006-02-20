#ifndef __MPTKGUICOLORMAPVIEW_H
#define __MPTKGUICOLORMAPVIEW_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "mptk.h"
#include "MptkGuiColormaps.h"
#include "MptkGuiCMapZoomEvent.h"

/** \brief This is a colorbar representation of a colormap to allow changing
     its bound values. It draws  the colorbar corresponding to a given colormap,
     and two cursors just behind the user can drag to select the minimum and maximum 
     intensity values (in dB) of a book visualized in the associated  MptkGuiTFView
 */

class MptkGuiColorMapView: public wxPanel
{
 public:
  /** \brief Constructor
      \param cmap the colormap used to draw the colorbar and the associated MptkGuiTFView 
      \param dBmin the lower bound for intensity (corresponding to the left bound of the colorbar) 
      \param dBmax the upper bound for intensity (corresponding to the right bound of the colorbar) */
  MptkGuiColorMapView(wxWindow *parent,MptkGuiColormaps *cmap,float dBmin,float dBmax);

  /** \brief Function called by wxWidgets event handler when readrawing is needed*/ 
  void OnPaint(wxPaintEvent & event);
  
   /** \brief Reaction to resize events*/
  void onResize(wxSizeEvent & event);

   /** \brief Reaction to left mouse button down event
       if the mouse cursor is above the min or max cursor, it will be selected and dragged
       with mouse motion until the button is released. Notice that if min min and max 
       cursors are overlapping, the priority (choice between the two) will change each time*/ 
  void reacLeftDown(wxMouseEvent & event);

   /** \brief Reaction to left mouse button release.If the mouse was dragging a cursor, it is dropped
       and  and event is generated to make the associated MptkGuiTFView adjust to the new parameters*/
  void reacLeftUp(wxMouseEvent & event);

  /** \brief Reaction to middle mouse button release. Positions both cursors to bound values, and 
      generates an event for the associated MptkGuiTFview to adjust*/  
  void reacMiddleUp(wxMouseEvent & event);

  /** \brief Reaction to mouse cursor motion. Drags a cursor if one is selected*/
  void reacMouseMotion(wxMouseEvent & event);

  /** \brief Returns the value pointed by the minimum amplitude cursor*/
  float getCurrentMin();

  /** \brief Returns the value pointed by the maximum amplitude cursor*/
  float getCurrentMax();

  /** \brief Sets the minimum amplitude bound*/
  void setMinBound(float new_minbound);
  
  /** \brief Sets the maximum amplitude bound*/
  void setMaxBound(float new_maxbound);

  /** \brief Returns the minimum amplitude bound*/
  float getMinBound();

  /** \brief Returns the maximum amplitude bound*/
  float getMaxBound();

  /** \brief Sets the colormap used for the colorbar*/
  void setColorMap(MptkGuiColormaps * new_cmap);

  /** \brief Sets the type of the colormap used
      \param new_colormap_type Must be one of the colormap type
       constants defined in MptkGuiColormaps (GRAY,JET,HOT,BONE,PINK,COPPER,COOL) 
   */
  void setColorMapType(int new_colormap_type);
  
 private:
  void drawColorMap();
  void drawSizers();
  void updateScreen();
  MptkGuiColormaps * colormap;
  wxBufferedPaintDC *bufferedimage;
  int WIDTH;
  int HEIGHT;
  float minsizerpos;
  float maxsizerpos;
  float bornemin;
  float bornemax;
  bool minsizermoving;
  bool maxsizermoving;
  bool prio;
  DECLARE_EVENT_TABLE();
};

#endif /*MPTKGUICOLORMAPVIEW*/



