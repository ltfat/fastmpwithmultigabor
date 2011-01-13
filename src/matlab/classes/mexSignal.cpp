#include "mptk4matlab.h"

mxArray *mp_create_mxSignal_from_signal(MP_Signal_c *signal) {
  const char *func = "mp_create_mxSignal_from_signal()";

  // Checking input
  if(NULL==signal) {
    mp_error_msg(func,"NULL input");
    return(NULL);
  }

  // Creating storage for output
  mxArray *mxSignal = mxCreateDoubleMatrix(signal->numSamples, signal->numChans, mxREAL);
  if (NULL==mxSignal) {
    mp_error_msg(func, "Can't allocate a new mxSignal.\n" );
    return(NULL);
  }

  // Copying content
  unsigned long int sample;
  int channel;
  // DEBUG
  // mp_info_msg( func, "filling signal vector with %d samples and %d channels\n",signal->numSamples,signal->numChans);

  for (channel=0; channel<signal->numChans; channel++) {
    for (sample=0; sample<signal->numSamples; sample++) {
      *( mxGetPr(mxSignal) + channel*signal->numSamples +  sample) = (double) (signal->channel[channel][sample]);
    }
  }

  //mp_info_msg(func, "filling succesfull of mxSignal = %p\n",mxSignal);
  return(mxSignal);
}

MP_Signal_c *mp_create_signal_from_mxSignal(const mxArray *mxSignal) {
  const char *func = "mp_create_signal_from_mxSignal()";

  // Checking input
  if(NULL==mxSignal) {
    mp_error_msg(func,"NULL input");
    return(NULL);
  }
  if(2!=mxGetNumberOfDimensions(mxSignal)) {
    mp_error_msg(func,"input signal should be a numSamples x numChans matrix");
    return(NULL);
  }
  unsigned long int numSamples = (unsigned long int)mxGetM(mxSignal);
  unsigned int      numChans   = (unsigned int)mxGetN(mxSignal);
  
  // Creating storage for output
  MP_Signal_c *signal = MP_Signal_c::init(numChans,numSamples,1);
  if (NULL==signal) {
    mp_error_msg(func, "Can't allocate a new signal.\n" );
    return(NULL);
  }

  // Copying content
  unsigned long int sample;
  int channel;
  //mp_info_msg( func, "filling signal vector with %d samples and %d channels\n",numSamples,numChans);

  for (channel=0; channel<signal->numChans; channel++) {
    for (sample=0; sample<signal->numSamples; sample++) {
      signal->channel[channel][sample] =  *( mxGetPr(mxSignal) + channel*signal->numSamples +  sample);
    }
  }
  signal->refresh_energy();

  //mp_info_msg(func, "filling succesfull of signal = %p\n",signal);
  return(signal);
}


