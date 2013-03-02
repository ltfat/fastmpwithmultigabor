#include "pyMPTK.h"

void
book_dealloc(BookObject* self)
{
	self->ob_type->tp_free((PyObject*)self);
}

PyObject *
book_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	MPTK_Env_c::get_env()->load_environment_if_needed("");
	BookObject *self;

	self = (BookObject *)type->tp_alloc(type, 0);
	self->mpbook = MP_Book_c::create();
	// NB the values below, book_append_atoms_from_mpbook() will overwrite them iff 0
	self->numChans = 0;
	self->numSamples = 0;
	self->sampleRate = 0;
	self->atoms = PyList_New(0);

	return (PyObject *)self;
}

int
book_init(BookObject *self, PyObject *args, PyObject *kwds)
{
	return 0;
}

PyObject *
book_read(BookObject* self, PyObject *args)
{
	char *filename;

	if (!PyArg_ParseTuple(args, "s", &filename))
		return NULL;

	self->mpbook->load( filename );

	int result = book_append_atoms_from_mpbook(self, self->mpbook);
	return Py_BuildValue("i", result);
}

// This is intended only to be used by the internal functions which read from file or from memory. It doesn't ensure the self->mpbook and mpbook are in sync.
int
book_append_atoms_from_mpbook(BookObject* self, MP_Book_c *mpbook)
{
	int numAtoms = mpbook->numAtoms;
	int n,m;
	PyObject *tmp;
	if(self->numChans==0){
		self->numChans = mpbook->numChans;
	}else if(self->numChans != mpbook->numChans ){
		return 1;
	}
	if(self->numSamples==0){
		self->numSamples = mpbook->numSamples;
	}else if(self->numSamples != mpbook->numSamples ){
		return 2;
	}
	if(self->sampleRate==0){
		self->sampleRate = mpbook->sampleRate;
	}else if(self->sampleRate != mpbook->sampleRate ){
		return 3;
	}
	for ( n=0 ; n<numAtoms ; ++n ) {

		// Create a dict representing one atom, containing all its properties
		PyObject* atom = PyDict_New();
		/////////////////////////////////
		// Mono properties:
		PyDict_SetItemString(atom, "type", Py_BuildValue("s", mpbook->atom[n]->type_name()));
		// freq
		if ( mpbook->atom[n]->has_field(MP_FREQ_PROP) ) {
			PyDict_SetItemString(atom, "freq", Py_BuildValue("d", mpbook->atom[n]->get_field(MP_FREQ_PROP, 0)));
		}
		// chirp
		if ( mpbook->atom[n]->has_field(MP_CHIRP_PROP) ) {
			PyDict_SetItemString(atom, "chirp", Py_BuildValue("d", mpbook->atom[n]->get_field(MP_CHIRP_PROP, 0)));
		}
		/////////////////////////////////
		// Multichannel properties:
		// len
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( mpbook->atom[n]->has_field(MP_LEN_PROP) ) {
				PyList_Append( tmp, Py_BuildValue("i", (int)mpbook->atom[n]->get_field(MP_LEN_PROP, m)));
			}
		}
		PyDict_SetItemString(atom, "len", tmp);
		// pos
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( mpbook->atom[n]->has_field(MP_POS_PROP) ) {
				PyList_Append( tmp, Py_BuildValue("i", (int)mpbook->atom[n]->get_field(MP_POS_PROP, m)));
			}
		}
		PyDict_SetItemString(atom, "pos", tmp);
		// amp
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( mpbook->atom[n]->has_field(MP_AMP_PROP) ) {
				PyList_Append( tmp, Py_BuildValue("d", mpbook->atom[n]->get_field(MP_AMP_PROP, m)));
			}
		}
		PyDict_SetItemString(atom, "amp", tmp);
		// phase
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( mpbook->atom[n]->has_field(MP_PHASE_PROP) ) {
				PyList_Append( tmp, Py_BuildValue("d", mpbook->atom[n]->get_field(MP_PHASE_PROP, m)));
			}
		}
		PyDict_SetItemString(atom, "phase", tmp);
		// and finally, append our atom to the list
		PyList_Append(self->atoms, atom);
	}
	return 0;
}

PyObject *
book_short_info(BookObject* self)
{
	self->mpbook->short_info();
	return Py_BuildValue("i", 0);
}

