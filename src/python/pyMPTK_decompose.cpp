#include "pyMPTK.h"
#include "mpd_core.h"

extern PyTypeObject bookType;

MP_Signal_c* mp_create_signal_from_numpyarray(const PyArrayObject *nparray){
	unsigned long int nspls = nparray->dimensions[0];
	unsigned      int nchans= nparray->dimensions[1];
	float *signal_data = (float *)nparray->data;
	const char *func = "mp_create_signal_from_numpyarray()";
	printf("%s - numpy array has %d channels, %d samples\n", func, (int)nchans, (int)nspls);

	if(2 != nparray->nd) {
		mp_error_msg(func,"input signal should be a numSamples x numChans matrix");
		return(NULL);
	}

	// Creating storage for output
	MP_Signal_c *signal = MP_Signal_c::init(nchans, nspls, 1);
	if (NULL==signal) {
		mp_error_msg(func, "Can't allocate a new signal.\n" );
		return(NULL);
	}

	// Copying content. NB I'm quite confident in-place operation is not a good idea, since mptk modifies the signal in-place
	for (unsigned int channel=0; channel < nchans; ++channel) {
		for (unsigned long int sample=0; sample < nspls; ++sample) {
			signal->channel[channel][sample] =  signal_data[channel*nspls +  sample];
		}
	}
	signal->refresh_energy();

	return(signal);
}


// Moved mptk_decompose into the main .cpp, since I think it needs to be in the same compiled object file in order not to crash on import_array() issues.
// The implementation of the main number-crunching calls goes here though.

int
mptk_decompose_body(const PyArrayObject *numpysignal, const char *dictpath, const int samplerate, const unsigned long int numiters, const char *method, const bool getdecay, const char* bookpath, mptk_decompose_result& result){
	// book, residual, decay = mptk.decompose(sig, dictpath, samplerate, [ numiters=10, method='mp', getdecay=False ])

	////////////////////////////////////////////////////////////
	// get signal in mem in appropriate format
	// Q: can we do it in-place? to do this, we'd need to check if it was a numpy array, with the right float format, and the right matrix-ordering, error if not.
	// TODO: check with MPTK crew, that in-place operation will not mangle the signal (it is not const)
	MP_Signal_c *signal = mp_create_signal_from_numpyarray(numpysignal);
	if(NULL==signal) {
		printf("Failed to convert a signal from python to MPTK.\n");
		return 1;
	}
	signal->sampleRate = samplerate;

	////////////////////////////////////////////////////////////
	// get dict in mem in appropriate format - we'll do it from file - easy
	MP_Dict_c* dict = MP_Dict_c::init(dictpath);
	if(NULL==dict) {
		printf("Failed to convert a dict from Matlab to MPTK or to read it from file.\n");
		delete signal;
		return 2;
	}

	////////////////////////////////////////////////////////////
	// Configure and run

	// Set up core and book obj
	MP_Book_c *mpbook = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );
	if ( NULL == mpbook )  {
	    printf("Failed to create a book object.\n" );
	    delete signal;
	    delete dict;
	    return 3;
	}
	MP_Mpd_Core_c *mpdCore =  MP_Mpd_Core_c::create( signal, mpbook, dict );
	if ( NULL == mpdCore )  {
	    printf("Failed to create a MPD core object.\n" );
	    delete signal;
	    delete dict;
	    delete mpbook;
	    return 4;
	}

	unsigned long int reportHit = 10;  // To parameterize
	// Set stopping condition
	mpdCore->set_iter_condition( numiters );
	mpdCore->set_save_hit(ULONG_MAX, bookpath, NULL, NULL); // OR we could let the user specify paths to save to?
	mpdCore->set_report_hit(reportHit);
	if(getdecay) mpdCore->set_use_decay();

	mpdCore->set_verbose();
	// Display some information
	printf("The dictionary contains %d blocks\n",dict->numBlocks);
	printf("The signal has:\n");
	signal->info();
	mpdCore->info_conditions();
	printf("Initial signal energy is %g\n",mpdCore->get_initial_energy());
	printf("Starting to iterate\n");

	mpdCore->run();
	mpdCore->info_state();
	mpdCore->info_result();


	// Get results - book, residual, decay. as a first pass, maybe we should write the book to disk, return the residual, ignore the decay (useDecay=false).

	// write book XML to disk.
	if(bookpath!=NULL && bookpath[0] != NULL){
		mpdCore->save_result();
	}

	// create python book object, which will be returned
	BookObject* thebook = (BookObject*)PyObject_CallObject((PyObject *) &bookType, NULL);
	book_append_atoms_from_mpbook(thebook, mpbook);

	result.thebook = thebook;
	result.residual = mp_create_numpyarray_from_signal(signal); // residual is in here (i.e. the "signal" is updated in-place)

	delete signal;
	delete dict;
	delete mpbook;
	delete mpdCore;

	return 0;
}

