// NB this define must come before the includes -- see http://mail.scipy.org/pipermail/numpy-discussion/2001-July/013060.html
#define PY_ARRAY_UNIQUE_SYMBOL Py_Array_API_pymptk
#include "pyMPTK.h"

/**
* Author: Dan Stowell
* Description: This file defines the initialisation for the module etc, as well as the bits which get data in/out of PyArray format.
*/

// The following two lists must be kept in sync, please
static const char *mptk_decompose_kwlist[] = {"signal", "dictpath", "samplerate", "numiters", "snr", "method", "decaypath", "bookpath", "reportevery",
		"cmpd_maxnum_cycles", "cpmd_min_cycleimprovedb", "cpmd_maxnum_aug_beforecycle", 
		"cpmd_maxnum_aug_beforecycle_db", "cpmd_max_aud_stopcycle", "cpmd_max_db_stopcycle", "cpmd_hold",
			NULL};
static const char *mptk_decompose_kwstring = "decompose a signal into a 'book' and residual, using Matching Pursuit or related methods.\nThe first three args are compulsory, the rest optional:"
		"\n(book, residual) = mptk.decompose("
                                              "signal, dictpath, samplerate\n[, numiters, snr, method, decaypath, bookpath, reportevery,\n"
		"cmpd_maxnum_cycles, cpmd_min_cycleimprovedb, cpmd_maxnum_aug_beforecycle,\n"
		"cpmd_maxnum_aug_beforecycle_db, cpmd_max_aud_stopcycle, cpmd_max_db_stopcycle, cpmd_hold"
		"])";

static PyMethodDef module_methods[] = {
	{"loadconfig", mptk_loadconfig, METH_VARARGS, "load MPTK config file from a specific path. do this BEFORE running decompose() etc."},
	{"decompose" , (PyCFunction) mptk_decompose,  METH_VARARGS | METH_KEYWORDS, mptk_decompose_kwstring },
	{"reconstruct" , (PyCFunction) mptk_reconstruct,  METH_VARARGS, "mptk.reconstruct(book, dictpath) -- turn a book back into a signal"},
	{"anywave_encode" , (PyCFunction) mptk_anywave_encode,  METH_VARARGS, "mptk.anywave_encode(signal) -- encode a signal as base64, for use in an xml dictionary"},
	{NULL}  /* Sentinel */
};

PyTypeObject bookType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "mptk.book",               /*tp_name*/
    sizeof(BookObject),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)book_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "A 'book' in MPTK is a list of atoms, the result of performing a sparse decomposition on a signal.\nIt is produced by mptk.decompose(), and used by mptk.reconstruct().\nThe main data structure in this object is the 'atoms' list.",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    book_methods,             /* tp_methods */
    book_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)book_init,      /* tp_init */
    0,                         /* tp_alloc */
    book_new,                 /* tp_new */
};

PyObject *
mptk_loadconfig(PyObject *self, PyObject *args)
{
	const char *cfgpath;
	if (!PyArg_ParseTuple(args, "s:mptk_loadconfig", &cfgpath))
		return NULL;
	//printf("mptk_loadconfig: parsed args\n");
	int result = MPTK_Env_c::get_env()->load_environment_if_needed(cfgpath);
	//printf("mptk_loadconfig: done, result %i, about to return\n", result);
	return Py_BuildValue("i", result);
}


// This method needs to stay in the same file as initmptk(), because of the use of PyArray_FromDims() interacts with import_array()
PyArrayObject* mp_create_numpyarray_from_signal(MP_Signal_c *signal){
	unsigned long int nspls = signal->numSamples;
	unsigned      int nchans= signal->numChans;
	//printf("mp_create_numpyarray_from_signal(): %i samples, %i channels\n", nspls, nchans);
	int dims[2];
	dims[0] = nspls;
	dims[1] = nchans;
	PyArrayObject* nparray = (PyArrayObject *) PyArray_FromDims(2, dims, NPY_FLOAT);
	float *signal_data = (float *)nparray->data;
	// There may be a faster way. Can probably memcpy each individual channel, though possibly not the whole block
	for (unsigned int channel=0; channel < nchans; ++channel) {
		for (unsigned long int sample=0; sample < nspls; ++sample) {
			signal_data[channel*nspls + sample] = (float) signal->channel[channel][sample];
		}
	}
	return nparray;
}



// This method needs to stay in the same file as initmptk(), because of the use of import_array()
PyObject *
mptk_decompose(PyObject *self, PyObject *args, PyObject *keywds)
{
	// book, residual, decay = mptk.decompose(sig, dictpath, samplerate, [ snr=0.5, numiters=10, ... ])
	PyObject *pysignal; // note: do not touch the referencecount for this
	PyArrayObject *numpysignal;
	const char *dictpath;
	float samplerate;
	unsigned long int numiters=0; // 0 is flag to use SNR not numiters
	float snr=0.5f;
	const char *method="mp";
	const char *decaypath = "";
	const char *bookpath="";
	unsigned long int reportevery=10;

	// CMP's special options
	unsigned long int cmpd_maxnum_cycles             = CMPD_DEFAULT_MAXNUM_CYCLES;
	double            cpmd_min_cycleimprovedb        = CMPD_DEFAULT_MIN_CYCLEIMPROVEDB;
	unsigned long int cpmd_maxnum_aug_beforecycle    = CMPD_DEFAULT_MAXNUM_AUG_BEFORECYCLE;
	double            cpmd_maxnum_aug_beforecycle_db = CMPD_DEFAULT_MAXNUM_AUG_BEFORECYCLE_DB;
	unsigned long int cpmd_max_aud_stopcycle         = CMPD_DEFAULT_MAX_AUG_STOPCYCLE;
	double            cpmd_max_db_stopcycle          = CMPD_DEFAULT_MAX_DB_STOPCYCLE;
	int               cmpd_hold = 0;

	if (!PyArg_ParseTupleAndKeywords(args, keywds, "Osf|kfsssiifififi", (char**)mptk_decompose_kwlist,
		&pysignal, &dictpath, &samplerate,
		&numiters, &snr, &method, &decaypath, &bookpath, &reportevery,
		&cmpd_maxnum_cycles, &cpmd_min_cycleimprovedb, &cpmd_maxnum_aug_beforecycle,
		&cpmd_maxnum_aug_beforecycle_db, &cpmd_max_aud_stopcycle, &cpmd_max_db_stopcycle, &cmpd_hold
		))
		return NULL;
	//printf("mptk_decompose: parsed args\n");

	// Now to get a usable numpy array from the opaque obj
	numpysignal = (PyArrayObject*) PyArray_ContiguousFromObject(pysignal, PyArray_DOUBLE, 1, 2); // 1D or 2D
	if(numpysignal==NULL){
		PyErr_SetString(PyExc_RuntimeError, "mptk_decompose failed to get numpy array object\n");
		return NULL;
	}
	// From this point on: remember to do Py_DECREF(numpysignal) if terminating early

	// Here's where we call the heavy stuff
	mptk_decompose_result result;
	int intresult = mptk_decompose_body(numpysignal, dictpath, (int)samplerate, numiters, snr, method, decaypath, bookpath, reportevery,
		cmpd_maxnum_cycles, cpmd_min_cycleimprovedb, cpmd_maxnum_aug_beforecycle,
		cpmd_maxnum_aug_beforecycle_db, cpmd_max_aud_stopcycle, cpmd_max_db_stopcycle, cmpd_hold,
		result);

	//printf("mptk_decompose: about to return\n");
	Py_DECREF(numpysignal); // destroy the contig array
	return Py_BuildValue("OO", result.thebook, result.residual);
}

// This method needs to stay in the same file as initmptk(), because of the use of import_array()
PyObject *
mptk_reconstruct(PyObject *self, PyObject *args)
{
	PyObject *pybookobj;
	const char *dictpath;
	if (!PyArg_ParseTuple(args, "Os", &pybookobj, &dictpath))
		return NULL;
	BookObject *pybook = (BookObject*)pybookobj;
	MP_Signal_c *sig;

	//printf("pybook stats: numChans %i, numSamples %i, sampleRate %i.\n", ((BookObject*)pybook)->numChans, ((BookObject*)pybook)->numSamples, ((BookObject*)pybook)->sampleRate);

	// get dict in mem in appropriate format - we'll do it from file - easy
	MP_Dict_c* dict = MP_Dict_c::read_from_xml_file(dictpath);
	if(NULL==dict) {
		PyErr_SetString(PyExc_RuntimeError, "Failed to read dict from file.\n");
		return NULL;
	}

	// reconstruct the mpbook from our pybook
	MP_Book_c *mpbook = MP_Book_c::create(pybook->numChans, pybook->numSamples, pybook->sampleRate);
	if ( NULL == mpbook )  {
	    PyErr_SetString(PyExc_RuntimeError, "Failed to create a book object.\n" );
	    return NULL;
	}
	//printf("mpbook stats before mpbook_from_pybook: numChans %i, numSamples %i, sampleRate %i, numAtoms %i.\n", mpbook->numChans, mpbook->numSamples, mpbook->sampleRate, mpbook->numAtoms);
	int res = mpbook_from_pybook(mpbook, pybook, dict);
	if ( res != 0 )  {
	    PyErr_SetString(PyExc_RuntimeError, "Failed to complete mpbook object from pybook.\n" );
	    return NULL;
	}
	//printf("mpbook stats after mpbook_from_pybook: numChans %i, numSamples %i, sampleRate %i, numAtoms %i.\n", mpbook->numChans, mpbook->numSamples, mpbook->sampleRate, mpbook->numAtoms);

	// initialise an empty signal
	sig = MP_Signal_c::init( mpbook->numChans, mpbook->numSamples, mpbook->sampleRate );
	if ( sig == NULL ){
		PyErr_SetString(PyExc_RuntimeError, "Can't make a new signal" );
		return NULL;
	}

	// add all the atoms on:
	if ( mpbook->substract_add( NULL, sig, NULL ) == 0 )
	{
		PyErr_SetString(PyExc_RuntimeError, "No atoms were found in the book to rebuild the signal" );
		delete sig;
		return NULL;
	}

	PyArrayObject* sigarray = mp_create_numpyarray_from_signal(sig);
	return Py_BuildValue("O", sigarray);
}

// This method needs to stay in the same file as initmptk(), because of the use of import_array()
PyObject *
mptk_anywave_encode(PyObject *self, PyObject *args)
{
	PyObject *pysigobj;
	if (!PyArg_ParseTuple(args, "O", &pysigobj))
		return NULL;
	if (!(PyArray_Check(pysigobj) || PyList_Check(pysigobj))){
		PyErr_SetString(PyExc_RuntimeError, "mptk_anywave_encode: must provide a list or ndarray\n");
		return NULL;
	}

	// NOTE: here we demand data in double format (PyArray_DOUBLE) because this is the required format of data to be base64-encoded
	PyArrayObject* numpysignal = (PyArrayObject*) PyArray_ContiguousFromObject(pysigobj, PyArray_DOUBLE, 1, 2); // 1D or 2D
	if(numpysignal==NULL){
		PyErr_SetString(PyExc_RuntimeError, "mptk_anywave_encode failed to get numpy array object\n");
		return NULL;
	}
	// From this point on: remember to do Py_DECREF(numpysignal) if terminating early
	size_t numvals = numpysignal->dimensions[0];
	if(numpysignal->nd == 2){
		numvals *= numpysignal->dimensions[1];
	}
	// TODO: I have not checked if multichannel data gives the right ordering, or if it needs to be transposed.
	// TODO if the encoder doesn't care about (double*) we could avoid a cast
	double* doubles = (double *)(numpysignal->data);

	// to consider: we shouldn't need to create a new "table" every time, it might be handy to have a cached obj
	MP_Anywave_Table_c *table = new MP_Anywave_Table_c();
	string encodedstr = table->encodeBase64((char *)doubles, numvals * sizeof(double));
	delete table;

	PyObject* pyencodedstr = PyString_FromString(encodedstr.c_str());

	Py_DECREF(numpysignal); // destroy the contig array
	return Py_BuildValue("O", pyencodedstr);
}


#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initmptk(void)
{
    PyObject* m;

    if (PyType_Ready(&bookType) < 0){
        printf("not PyType_Ready(&bookType)\n");
        return;
   }

    m = Py_InitModule3("mptk", module_methods,
                       "MPTK - Matching Pursuit ToolKit - decompose and analyse signals.");

    if (m == NULL){
        printf("Py_InitModule3(mptk) failed\n");
      return;
    }

    Py_INCREF(&bookType);
    PyModule_AddObject(m, "book", (PyObject *)&bookType);

    import_array(); // numpy init -- must be called after InitModule
    //printf("Py_InitModule3(mptk) OK, and done import_array()\n");
}
