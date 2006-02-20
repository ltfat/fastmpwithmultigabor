#ifndef MPTKGUIAPP_H
#define MPTKGUIAPP_H

#include "MptkGuiFrame.h"

/**
 * \brief MptkGuiApp creates the main frame (MptkGuiFrame) and shows it 
 */

class MptkGuiApp : public wxApp
{
 public :
  MptkGuiFrame *frame;

  virtual bool OnInit();
};

#endif
