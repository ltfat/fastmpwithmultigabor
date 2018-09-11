#include "pyMPTK.h"

#include "../plugin/base/anywave_atom_plugin.h"
#include "../plugin/base/anywave_hilbert_atom_plugin.h"
#include "../plugin/base/constant_atom_plugin.h"
#include "../plugin/base/dirac_atom_plugin.h"
#include "../plugin/base/gabor_atom_plugin.h"
#include "../plugin/base/harmonic_atom_plugin.h"
#include "../plugin/base/nyquist_atom_plugin.h"
#include "../plugin/contrib/lam/mclt_atom_plugin.h"
#include "../plugin/contrib/lam/mdct_atom_plugin.h"
#include "../plugin/contrib/lam/mdst_atom_plugin.h"

// macro used in mpatom_from_pyatom()
#define PYATOMOBJ_GETITEM_NONEWATOM(keystr, objvarname, checktype) \
		keyobj = PyString_FromString(keystr); \
		PyObject* objvarname = PyObject_GetItem(pyatomobj, keyobj); \
		Py_DECREF(keyobj); \
		if(objvarname==NULL){ \
			printf("'%s' gotobj is null\n", keystr); \
			return NULL; \
		} \
		if(!checktype##_Check(objvarname)){ \
			printf("'%s' gotobj is not of correct type\n", keystr); \
			return NULL; \
		}
#define PYATOMOBJ_GETITEM(keystr, objvarname, checktype) \
		keyobj = PyString_FromString(keystr); \
		PyObject* objvarname = PyObject_GetItem(pyatomobj, keyobj); \
		Py_DECREF(keyobj); \
		if(objvarname==NULL){ \
			printf("'%s' gotobj is null\n", keystr); \
			delete newAtom; \
			return NULL; \
		} \
		if(!checktype##_Check(objvarname)){ \
			printf("'%s' gotobj is not of correct type\n", keystr); \
			delete newAtom; \
			return NULL; \
		}
#define PYATOMOBJ_GETITEM_DOUBLE_NULLOK(keystr, objvarname, doublename, nullmeans) \
		keyobj = PyString_FromString(keystr); \
		PyObject* objvarname = PyObject_GetItem(pyatomobj, keyobj); \
		Py_DECREF(keyobj); \
		double doublename; \
		if(objvarname==NULL){ \
			doublename = nullmeans; \
		} \
		else if(!PyFloat_Check(objvarname)){ \
			printf("'%s' gotobj is not of correct type\n", keystr); \
			delete newAtom; \
			return NULL; \
		} else \
		{ \
			doublename = PyFloat_AsDouble(objvarname); \
		}

// Method to create an atom in mptk data structure, from a python specification in memory.
// Based on the matlab wrapper: MP_Atom_c *GetMP_Atom(const mxArray *mxBook,MP_Chan_t numChans,unsigned long int atomIdx)
MP_Atom_c* mpatom_from_pyatom(PyDictObject* pyatom, MP_Chan_t numChans, MP_Dict_c* dict) {
	const char *func = "mpatom_from_pyatom";
	PyObject* pyatomobj = (PyObject*)pyatom;
	unsigned long int c; //MP_Chan_t c;

	PyObject* keyobj; // used in PYATOMOBJ_GETITEM
	char* keystr;

	PYATOMOBJ_GETITEM_NONEWATOM("type", typeobj, PyString)
	const char* typestr = PyString_AsString(typeobj);


	// Get Atom creator method 
	MP_Atom_c* (*emptyAtomCreator)( MP_Dict_c* dict) = MP_Atom_Factory_c::get_empty_atom_creator(typestr);
	if (NULL == emptyAtomCreator)	{
			printf("-- unknown	MP_Atom_Factory_c method for atomType:%s\n", typestr);
			return( NULL );
	}
	
	// Create empty atom 
	MP_Atom_c *newAtom = (*emptyAtomCreator)(dict);
	if ( NULL==newAtom ) {
		mp_error_msg(func,"-- could not create empty atom of type %s\n", typestr);
		return( NULL );
	}
	// Allocate main fields
	if ( newAtom->alloc_atom_param(numChans) ) {
		mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n", typestr);
		delete newAtom;
		return( NULL );
	}
	mp_debug_msg(MP_DEBUG_SPARSE,func," -- atom [%s]\n", typestr);

	// retrieve pointers to arrays of parameters that are common to every atom type (pos, len, amp)
	PYATOMOBJ_GETITEM("pos", listobj_pos, PyList)
	PYATOMOBJ_GETITEM("len", listobj_len, PyList)
	PYATOMOBJ_GETITEM("amp", listobj_amp, PyList)

	// Fill the main fields
	for (c=0; c<numChans; ++c) { // loop on channels
		newAtom->support[c].pos = (unsigned long int) PyInt_AsLong(PyList_GetItem(listobj_pos, c));
		newAtom->support[c].len = (unsigned long int) PyInt_AsLong(PyList_GetItem(listobj_len, c));
		newAtom->totalChanLen += newAtom->support[c].len;
		newAtom->amp[c]         = (MP_Real_t)     PyFloat_AsDouble(PyList_GetItem(listobj_amp, c));
	}
	
	// and fill it with the params field
	// CONSTANT / DIRAC / NYQUIST / 
	if (strcmp(typestr, "constant")==0 || strcmp(typestr, "dirac")==0 || strcmp(typestr, "nyquist")==0) {
		 return (newAtom);
	} 

	// GABOR / HARMONIC
	else if (strcmp(typestr, "gabor")==0 || strcmp(typestr, "harmonic")==0) {
		MP_Gabor_Atom_Plugin_c* gaborAtom =	(MP_Gabor_Atom_Plugin_c*)newAtom;
		if ( gaborAtom->alloc_gabor_atom_param(numChans) )	{
			mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n", typestr);
			delete newAtom;
			return( NULL );
		}

		PYATOMOBJ_GETITEM("chirp",   chirpobj,   PyFloat)
		PYATOMOBJ_GETITEM("freq",    freqobj,    PyFloat)
		PYATOMOBJ_GETITEM("phase",   phaseobj,   PyList )
		PYATOMOBJ_GETITEM("wintype", wintypeobj, PyString)

		const char* wintypestr = PyString_AsString(wintypeobj);
		gaborAtom->windowType = window_type(wintypestr);

		PYATOMOBJ_GETITEM_DOUBLE_NULLOK("winopt", winoptobj, winopt, 0.)
		gaborAtom->windowOption = winopt;
		gaborAtom->freq	= (MP_Real_t) PyFloat_AsDouble(freqobj);
		gaborAtom->chirp = (MP_Real_t) PyFloat_AsDouble(chirpobj);
		for (c=0;c<numChans;c++) { // loop on channels
			gaborAtom->phase[c] = (MP_Real_t) PyFloat_AsDouble(PyList_GetItem(phaseobj, c));
		}
			
		if(strcmp(typestr, "gabor")==0) {
			return (newAtom);
		}
		else if (strcmp(typestr, "harmonic")==0)	{
			MP_Harmonic_Atom_Plugin_c* harmonicAtom =	(MP_Harmonic_Atom_Plugin_c*)newAtom;


			PYATOMOBJ_GETITEM("numPartials",   numpartialsobj,   PyInt)

			unsigned int numPartials = (unsigned int) PyInt_AsLong(numpartialsobj);
			unsigned int p;
			if ( harmonicAtom->alloc_harmonic_atom_param( numChans, numPartials ) ) {
				mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n", typestr);
				delete newAtom;
				return( NULL );
			}


			// set atoms params 
			PYATOMOBJ_GETITEM("harmonicity",  harmonicityobj,  PyList)
			PYATOMOBJ_GETITEM("partialAmp",   partialampobj,   PyList)
			PYATOMOBJ_GETITEM("partialPhase", partialphaseobj, PyList)

			harmonicAtom->numPartials = numPartials;
			
			for (p=0;p<numPartials;p++) { // loop on partials
				harmonicAtom->harmonicity[p] =	(MP_Real_t) PyFloat_AsDouble(PyList_GetItem(harmonicityobj, p));
			}
			for (c=0;c<numChans;c++) { // loop on channels
				for (p=0;p<numPartials;p++) { // loop on partials
					harmonicAtom->partialAmp[c][p] = (MP_Real_t)   PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(partialampobj, c), p));
					harmonicAtom->partialPhase[c][p] = (MP_Real_t) PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(partialphaseobj, c), p));
				}
			}
			return (newAtom);
		}
		else {
			mp_error_msg(func,"This code should never be reached!\n");
			delete newAtom;
			return(NULL);
		}
	}
	// ANYWAVE / ANYWAVE_HILBERT
	else if (strcmp(typestr, "anywave")==0 || strcmp(typestr, "anywavehilbert")==0) {	
		MP_Anywave_Atom_Plugin_c* anywaveAtom =	(MP_Anywave_Atom_Plugin_c*)newAtom;

		PYATOMOBJ_GETITEM("anywaveIdx", anywaveidxobj, PyInt)
		anywaveAtom->anywaveIdx =	(unsigned long int) PyInt_AsLong(anywaveidxobj);

		PYATOMOBJ_GETITEM("tableIdx", tableidxobj, PyInt)
		anywaveAtom->tableIdx =	(unsigned long int) PyInt_AsLong(tableidxobj);
		anywaveAtom->anywaveTable = MPTK_Server_c::get_anywave_server()->tables[anywaveAtom->tableIdx];
		if(NULL==anywaveAtom->anywaveTable) {
			mp_error_msg(func,"Failed to retrieve anywaveTable number %d from server\n",anywaveAtom->tableIdx);
			delete newAtom;
			return(NULL);
		}

		if(strcmp(typestr, "anywave")==0) {
				return (newAtom);
		} else if (strcmp(typestr, "anywavehilbert")==0) {
			MP_Anywave_Hilbert_Atom_Plugin_c* anywaveHilbertAtom =	(MP_Anywave_Hilbert_Atom_Plugin_c*)newAtom;
			if ( anywaveHilbertAtom->alloc_hilbert_atom_param(numChans) ) {
				mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n", typestr);
				delete newAtom;
				return( NULL );
			}			
			// Channel independent fields
			PYATOMOBJ_GETITEM("realTableIdx", realtableidxobj, PyInt)
			anywaveHilbertAtom->realTableIdx =	(unsigned long int) PyInt_AsLong(realtableidxobj);
			anywaveHilbertAtom->anywaveRealTable = MPTK_Server_c::get_anywave_server()->tables[anywaveHilbertAtom->realTableIdx];
			if(NULL==anywaveHilbertAtom->anywaveRealTable) {
				mp_error_msg(func,"Failed to retrieve anywaveRealTable number %d from server\n",anywaveHilbertAtom->realTableIdx);
				delete newAtom;
				return(NULL);
			}
	 
			PYATOMOBJ_GETITEM("hilbertTableIdx", hilberttableidxobj, PyInt)
			anywaveHilbertAtom->hilbertTableIdx =	(unsigned long int) PyInt_AsLong(hilberttableidxobj);
			anywaveHilbertAtom->anywaveHilbertTable = MPTK_Server_c::get_anywave_server()->tables[anywaveHilbertAtom->hilbertTableIdx];
			if(NULL==anywaveHilbertAtom->anywaveHilbertTable) {
				mp_error_msg(func,"Failed to retrieve anywaveHilbertTable number %d from server\n",anywaveHilbertAtom->hilbertTableIdx);
				delete newAtom;
				return(NULL);
			}

			// Channel dependent fields
			PYATOMOBJ_GETITEM("realPart", realpartobj, PyList)
			PYATOMOBJ_GETITEM("hilbertPart", hilbertpartobj, PyList)
			for (c=0; c<numChans; ++c) { // loop on channels
				// Real/Hilbert part
				anywaveHilbertAtom->realPart[c]    = (MP_Real_t) PyFloat_AsDouble(PyList_GetItem(   realpartobj, c));
				anywaveHilbertAtom->hilbertPart[c] = (MP_Real_t) PyFloat_AsDouble(PyList_GetItem(hilbertpartobj, c));
			}
		 
			return(newAtom);
		} else {
			mp_error_msg(func,"This code should never be reached!\n");
			delete newAtom;
			return(NULL);
		}
	}
	 // MCLT ATOM
	else if (strcmp(typestr, "mclt")==0)	{
		MP_Mclt_Atom_Plugin_c* mcltAtom =	(MP_Mclt_Atom_Plugin_c*)newAtom;
		if ( mcltAtom->alloc_mclt_atom_param(numChans) ) {
			mp_error_msg(func,"Failed to allocate some vectors in the atom %s. Returning a NULL atom.\n", typestr);
			return( NULL );
		}

		PYATOMOBJ_GETITEM("chirp",   chirpobj,   PyFloat)
		PYATOMOBJ_GETITEM("freq",    freqobj,    PyFloat)
		PYATOMOBJ_GETITEM("phase",   phaseobj,   PyList )
		PYATOMOBJ_GETITEM("wintype", wintypeobj, PyString)

		const char* wintypestr = PyString_AsString(wintypeobj);
		mcltAtom->windowType = window_type(wintypestr);

		PYATOMOBJ_GETITEM_DOUBLE_NULLOK("winopt", winoptobj, winopt, 0.)
		mcltAtom->windowOption = winopt;
		mcltAtom->freq	= (MP_Real_t) PyFloat_AsDouble(freqobj);
		mcltAtom->chirp = (MP_Real_t) PyFloat_AsDouble(chirpobj);
		for (c=0;c<numChans;c++) { // loop on channels
			mcltAtom->phase[c] = (MP_Real_t) PyFloat_AsDouble(PyList_GetItem(phaseobj, c));
		}
		return (newAtom);
	}
	// MDCT ATOM
	else if (strcmp(typestr, "mdct")==0) {
		MP_Mdct_Atom_Plugin_c* mdctAtom =	(MP_Mdct_Atom_Plugin_c*)newAtom;

		PYATOMOBJ_GETITEM("freq",    freqobj,    PyFloat)
		PYATOMOBJ_GETITEM("wintype", wintypeobj, PyString)
			
		const char* wintypestr = PyString_AsString(wintypeobj);
		mdctAtom->windowType = window_type(wintypestr);
		PYATOMOBJ_GETITEM_DOUBLE_NULLOK("winopt", winoptobj, winopt, 0.)
		mdctAtom->windowOption = winopt;
		mdctAtom->freq	= (MP_Real_t) PyFloat_AsDouble(freqobj);
		return(newAtom);
	}
	// MDST ATOM
	else if (strcmp(typestr, "mdst")==0) {
		MP_Mdst_Atom_Plugin_c* mdstAtom =	(MP_Mdst_Atom_Plugin_c*)newAtom;

		PYATOMOBJ_GETITEM("freq",    freqobj,    PyFloat)
		PYATOMOBJ_GETITEM("wintype", wintypeobj, PyString)
			
		const char* wintypestr = PyString_AsString(wintypeobj);
		mdstAtom->windowType = window_type(wintypestr);
		PYATOMOBJ_GETITEM_DOUBLE_NULLOK("winopt", winoptobj, winopt, 0.)
		mdstAtom->windowOption = winopt;
		mdstAtom->freq	= (MP_Real_t) PyFloat_AsDouble(freqobj);
		return(newAtom);
	}
	else {
		mp_error_msg(func,"Atom type [%s] unknown, consider adding its information in pyMPTK_atom.cpp\n", typestr);
		return (NULL);
	} 
}


PyObject*  pyatom_from_mpatom(MP_Atom_c* mpatom, MP_Chan_t numChans){
	int m;
	PyObject *tmp;
	// Create a dict representing one atom, containing all its properties
	PyObject* atom = PyDict_New();
	/////////////////////////////////
	// Mono properties:
	PyDict_SetItemString(atom, "type", Py_BuildValue("s", mpatom->type_name()));
	// freq
	if ( mpatom->has_field(MP_FREQ_PROP) ) {
		PyDict_SetItemString(atom, "freq", Py_BuildValue("d", mpatom->get_field(MP_FREQ_PROP, 0)));
	}
	// chirp
	if ( mpatom->has_field(MP_CHIRP_PROP) ) {
		PyDict_SetItemString(atom, "chirp", Py_BuildValue("d", mpatom->get_field(MP_CHIRP_PROP, 0)));
	}
	// wintype
	if ( mpatom->has_field(MP_WINDOW_TYPE_PROP) ) {
		const char* winname = window_name(mpatom->get_field(MP_WINDOW_TYPE_PROP, 0));
		//printf("got winname %s\n", winname);
		PyDict_SetItemString(atom, "wintype", Py_BuildValue("s", winname));
	}
	// winopt
	if ( mpatom->has_field(MP_WINDOW_OPTION_PROP) ) {
		PyDict_SetItemString(atom, "winopt", Py_BuildValue("d", mpatom->get_field(MP_WINDOW_OPTION_PROP, 0)));
	}
	// tableIdx
	if ( mpatom->has_field(MP_TABLE_IDX_PROP) ) {
		int val = mpatom->get_field(MP_TABLE_IDX_PROP, 0); // note - using this "val" intermediate because compiler does a bad optimisation otherwise
		PyDict_SetItemString(atom, "tableIdx", Py_BuildValue("i", val));
	}
	// anywaveIdx
	if ( mpatom->has_field(MP_ANYWAVE_IDX_PROP) ) {
		int val = mpatom->get_field(MP_ANYWAVE_IDX_PROP, 0);
		PyDict_SetItemString(atom, "anywaveIdx", Py_BuildValue("i", val));
	}
	// realTableIdx
	if ( mpatom->has_field(MP_REAL_TABLE_IDX_PROP) ) {
		int val = mpatom->get_field(MP_REAL_TABLE_IDX_PROP, 0);
		PyDict_SetItemString(atom, "realTableIdx", Py_BuildValue("i", val));
	}
	// hilbertTableIdx
	if ( mpatom->has_field(MP_HILBERT_TABLE_IDX_PROP) ) {
		int val = mpatom->get_field(MP_HILBERT_TABLE_IDX_PROP, 0);
		PyDict_SetItemString(atom, "hilbertTableIdx", Py_BuildValue("i", val));
	}

	/////////////////////////////////
	// Multichannel properties:
	// len
	tmp = PyList_New(0);
	for ( m=0 ; m< numChans ; ++m ) {
		if ( mpatom->has_field(MP_LEN_PROP) ) {
			PyList_Append( tmp, Py_BuildValue("i", (int)mpatom->get_field(MP_LEN_PROP, m)));
		}
	}
	PyDict_SetItemString(atom, "len", tmp);
	// pos
	tmp = PyList_New(0);
	for ( m=0 ; m< numChans ; ++m ) {
		if ( mpatom->has_field(MP_POS_PROP) ) {
			PyList_Append( tmp, Py_BuildValue("i", (int)mpatom->get_field(MP_POS_PROP, m)));
		}
	}
	PyDict_SetItemString(atom, "pos", tmp);
	// amp
	tmp = PyList_New(0);
	for ( m=0 ; m< numChans ; ++m ) {
		if ( mpatom->has_field(MP_AMP_PROP) ) {
			PyList_Append( tmp, Py_BuildValue("d", mpatom->get_field(MP_AMP_PROP, m)));
		}
	}
	PyDict_SetItemString(atom, "amp", tmp);
	// phase
	tmp = PyList_New(0);
	for ( m=0 ; m< numChans ; ++m ) {
		if ( mpatom->has_field(MP_PHASE_PROP) ) {
			PyList_Append( tmp, Py_BuildValue("d", mpatom->get_field(MP_PHASE_PROP, m)));
		}
	}
	PyDict_SetItemString(atom, "phase", tmp);
	// and finally
	return atom;
}

