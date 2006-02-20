#ifndef MPTKGUIEXTENDEDVIEW_H
#define MPTKGUIEXTENDEDVIEW_H

#include "MptkGuiDeleteViewEvent.h"
#include "MptkGuiUpDownPanel.h"

/**
 * \brief Super type for MptkGuiExtendedSignalView and MptkGuiExtendedTFView
 * Defines some virtual fonctions for the child class
 */

class MptkGuiExtendedView
{
protected :
  bool signalView;

public :

  virtual ~MptkGuiExtendedView(){};
  
  bool isSignalView(){return signalView;}
  bool isTFView(){return !signalView;}
  virtual int getId(){return -1;};
  virtual void zoom(float WXUNUSED(tdeb),float WXUNUSED(tfin), float WXUNUSED(fdeb), float WXUNUSED(ffin)){};
  virtual void zoom(float WXUNUSED(tdeb),float WXUNUSED(tfin)){};

protected :
  // Panel for the buttons Up and Down
  MptkGuiUpDownPanel *panelUpDown;
};

#endif

