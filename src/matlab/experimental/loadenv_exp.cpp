#include "mex.h"
#include "mptk.h"


void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {

	/* Declarations */
	int tmpcharlen;
	char filename[1000];

	/* Input */
	tmpcharlen = mxGetN(prhs[0])+1;
	mxGetString(prhs[0],&filename[0],tmpcharlen);
  if (!MPTK_Env_c::get_env()->get_environment_loaded()) MPTK_Env_c::get_env()->load_environment(filename);

}
