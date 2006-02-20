#include "MptkGuiHandlers.h"
#include "MptkGuiFrame.h"

void mp_gui_error_handler( void ) {
  wxMessageBox(MP_GLOBAL_MSG_SERVER.stdBuff, wxT("Error"), wxICON_ERROR | wxOK);
}

void mp_gui_handler(){
  MPTK_GUI_CONSOLE->appendText(MP_GLOBAL_MSG_SERVER.stdBuff);
}

void mp_gui_all_handler(){
  MPTK_GUI_CONSOLE->appendTextAndRefreshView(MP_GLOBAL_MSG_SERVER.stdBuff);
}
