#include "mptk.h"
#include "wx/wx.h"
#include "MptkGuiConsoleView.h"

/**
 * \brief Errror Handler, just shows a message box with the error message 
 */
void mp_gui_error_handler( void );

/**
 * \brief  Appends the message in the console
 */
void mp_gui_handler( void );

/** 
 * \ brief Appends message in the console and refresh the MptkGuiFrame
 */
void mp_gui_all_handler( void );
