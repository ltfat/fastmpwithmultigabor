#ifndef MPTK_SIGNAL_VIEW_H_
#define MPTK_SIGNAL_VIEW_H_

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "mptk.h"
#include "MptkGuiZoomEvent.h"

/** \brief
 *  This class gives the oscillogram view of a signal.
 *  It load a MP_Signal_c signal representation and
 *  draw its oscillogram, allowing it to be zoomed.
 *  Zooms will be propagated to the other views.
 */

class MptkGuiSignalView: public wxPanel
{
public:
  MptkGuiSignalView(wxWindow *parent,int id);

    /** \brief Function called by wxWidgets event handler when readrawing is needed*/ 
    void OnPaint(wxPaintEvent &event);

    /** \brief Changes the signal to be displayed*/
    void setSignal(MP_Signal_c *newsignal);//Changement du signal a afficher
    
    /** \brief Reaction to left mouse button release.
         Performs a zoom in the rectangle given by the point where the left button 
         was pressed and the current mouse cursor point*/
    void reacLeftUp(wxMouseEvent & event);

    /** \brief Reaction to left mouse button down events*/
    void reacLeftDown(wxMouseEvent & event);

    /** \brief Reaction to right mouse button release.
         Performs a zoom in the rectangle of height the total height of the area
         and width the width of the rectangle given by the point where the right button 
         was pressed and the current mouse cursor point*/
    void reacRightUp(wxMouseEvent & event);

    /** \brief Reaction to right mouse button down event*/
    void reacRightDown(wxMouseEvent & event);
    
    /** \brief Reaction to middle mouse button release.Resets the zoom 
	(Makes the view as if the signal has just been loaded). */
    void reacMiddleUp(wxMouseEvent & event);

    /** \brief Reaction to middle mouse button down event*/
    void reacMiddleDown(wxMouseEvent & event);

    /** \brief Reaction to resize events*/
    void onResize(wxSizeEvent & event);
    
    /** \brief Reaction to mouse motion. If right or left button is pressed, the zoom rectangle will 
	be drawn*/
    void onMouseMove(wxMouseEvent &event);

    /** \brief Performs a zoom by readjusting bounds time and amplitude parameters*/
    void zoom(float tdeb,float tfin,float min_amp,float max_amp);

    /** \brief Performs a zoom by readjusting bounds time parameters only.*/
    void zoom(float tdeb,float tfin);

    /** \brief Resets the zoom.(Makes the view as if the signal has just been loaded).*/ 
    void resetZoom();

    /** \brief Gives the coordinates (in pixels) of the last click point*/
    wxPoint getLastClick();

    /** \brief Gives a pointer on the drawed signal*/
    MP_Signal_c * getSignal();
    
    /** \brief Selects the channel of the signal to be drawn*/
    void setSelectedChannel(int numchan);

    /** \brief Gives the index of current selected channel*/
    int getSelectedChannel();

    /** \brief Returns the current start time (the time corresponding to the left bound
	of the drawn area)*/
    float getStartTime();

    /** \brief Returns the current end time (the time corresponding to the right bound
	of the drawn area)*/
    float getEndTime();

    /** \brief Returns the current minimal amplitude (the amplitude corresponding to the 
	bottom bound of the drawn area)*/
    float getMinAmp();

    /** \brief Returns the current maximum amplitude (the amplitude corresponding to the 
	top bound of the drawn area)*/
    float getMaxAmp();

private:
    /** \brief Redraws the whole signal and axes*/
    void updateScreen();
    wxBufferedPaintDC *bufferedimage;
    wxBufferedPaintDC *backupbuffer;
    int WIDTH;
    int HEIGHT;
    int selected_channel;
    double current_max_amp;
    float current_step;//intervalle courant (en pixels) entre deux echantillons sur la vue
    float current_start_x;//current step*i+current_start_x=abscisse (en pixels) de l'echantillon i
    float current_scale;//current_scale*x+current_start_y=ordonne (en pixels) de la valeur d'amplitude x
    float current_start_y;
    MP_Signal_c * signal;
    wxPoint cornerNW;
    wxPoint origine;
    wxPoint lastclick;
    bool zooming_left,zooming_right;
    DECLARE_EVENT_TABLE()
};

#endif /*MPTK_SIGNAL_VIEW_H_*/

