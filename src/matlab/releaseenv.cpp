#include "mex.h"
#include "mptk.h"


void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
  /* Release Mptk environnement */
  MPTK_Env_c::get_env()->release_environment();
}

