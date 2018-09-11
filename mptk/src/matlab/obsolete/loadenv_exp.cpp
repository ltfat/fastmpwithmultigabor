#include "mex.h"
#include "mptk.h"

/* Define a function which displays error messages within Matlab */
void msgfunc(char *msge) {
	mexPrintf("%s",msge);
	mexEvalString("pause(.001);"); // to dump string and flush
}

void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {

	/* Declarations */
	char *filename = NULL;
	
	if (nlhs>0) {
	  mexErrMsgTxt("Bad number of output arguments");
	}
	/* Input */
	switch(nrhs) {
	case 0:
	  break;
	case 1:
	  filename = mxArrayToString(prhs[0]);
	  break;
	default:
	  mexErrMsgTxt("Bad number of arguments");
	}

  /* Register the display functions */
  /* We could set a flag variable to do it only once */
    MPTK_Server_c::get_msg_server()->register_display_function("info_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("error_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("warning_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("progress_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("debug_message_display", &msgfunc);


	if (!MPTK_Env_c::get_env()->load_environment_if_needed(filename)) {
	  mexPrintf("Could not load environment");
	  mxFree(filename);
	  mexErrMsgTxt("Aborting");
	} else {
	  mxFree(filename);
	}
	
}
