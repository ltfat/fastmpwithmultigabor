#include <Python.h>
#include <structmember.h>

#define HAVE_FFTW3 1

#include "mptk.h"

typedef struct {
	PyObject_HEAD
	MP_Book_c *book;
	int numAtoms;
	int numChans;
	int numSamples;
	int sampleRate;
	PyObject *atomType;
	PyObject *atomParams;
} book;

static void
book_dealloc(book* self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
book_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	/* Load the MPTK environment if not loaded */
  if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
	book *self;

	self = (book *)type->tp_alloc(type, 0);
	self->book = MP_Book_c::create();
	self->numAtoms = 0;
	self->numChans = 0;
	self->numSamples = 0;
	self->sampleRate = 0;
	self->atomType = PyList_New(0);
	self->atomParams = PyDict_New();

	return (PyObject *)self;
}

static int
book_init(book *self, PyObject *args, PyObject *kwds)
{
	return 0;
}

static PyObject *
book_read(book* self, PyObject *args)
{
	char *filename;
	int n,m;
	PyObject *tmp,*len,*pos,*freq,*amp,*phase,*chirp;

	if (!PyArg_ParseTuple(args, "s", &filename))
		return NULL;

	self->book->load( filename );

	self->numAtoms = self->book->numAtoms;
	self->numChans = self->book->numChans;
	self->numSamples = self->book->numSamples;
	self->sampleRate = self->book->sampleRate;
	len = PyList_New(0);
	pos = PyList_New(0);
	freq = PyList_New(0);
	amp = PyList_New(0);
	phase = PyList_New(0);
	chirp = PyList_New(0);
	for ( n=0 ; n<self->numAtoms ; n++ ) {
		PyList_Append( self->atomType, Py_BuildValue("s", self->book->atom[n]->type_name()));
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(0) ) {
				PyList_Append( tmp, Py_BuildValue("i", (int)self->book->atom[n]->get_field(0,m)));
			}
		}
		PyList_Append( len, tmp);
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(1) ) {
				PyList_Append( tmp, Py_BuildValue("i", (int)self->book->atom[n]->get_field(1,m)));
			}
		}
		PyList_Append( pos, tmp);
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(2) ) {
				PyList_Append( tmp, Py_BuildValue("d", self->book->atom[n]->get_field(2,m)));
			}
		}
		PyList_Append( freq, tmp);
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(3) ) {
				PyList_Append( tmp, Py_BuildValue("d", self->book->atom[n]->get_field(3,m)));
			}
		}
		PyList_Append( amp, tmp);
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(4) ) {
				PyList_Append( tmp, Py_BuildValue("d", self->book->atom[n]->get_field(4,m)));
			}
		}
		PyList_Append( phase, tmp);
		tmp = PyList_New(0);
		for ( m=0 ; m<self->numChans ; m++ ) {
			if ( self->book->atom[n]->has_field(5) ) {
				PyList_Append( tmp, Py_BuildValue("d", self->book->atom[n]->get_field(5,m)));
			}
		}
		PyList_Append( chirp, tmp);
	}
	PyDict_SetItemString(self->atomParams, "len", len);
	PyDict_SetItemString(self->atomParams, "pos", pos);
	PyDict_SetItemString(self->atomParams, "freq", freq);
	PyDict_SetItemString(self->atomParams, "amp", amp);
	PyDict_SetItemString(self->atomParams, "phase", phase);
	PyDict_SetItemString(self->atomParams, "chirp", chirp);

	return Py_BuildValue("i", 0);
}

static PyObject *
book_short_info(book* self)
{
	self->book->short_info();

	return Py_BuildValue("i", 0);
}

static PyMethodDef book_methods[] = {
	{"read", (PyCFunction)book_read, METH_VARARGS,
		"read a book file"
		},
	{"short_info", (PyCFunction)book_short_info, METH_NOARGS,
		"print short info"
		},
	{NULL}  /* Sentinel */
};

static PyMemberDef book_members[] = {
	{"numAtoms", T_INT, offsetof(book, numAtoms), 0,
		"number of atoms"},
	{"numChans", T_INT, offsetof(book, numChans), 0,
		"number of chanels"},
	{"numSamples", T_INT, offsetof(book, numSamples), 0,
		"number of samples"},
	{"sampleRate", T_INT, offsetof(book, sampleRate), 0,
		"sample rate"},
	{"atomType", T_OBJECT_EX, offsetof(book, atomType), 0,
		"atom type"},
	{"atomParams", T_OBJECT_EX, offsetof(book, atomParams), 0,
		"atom parameters"},
	{NULL}  /* Sentinel */
};

static PyTypeObject bookType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyMPTK.book",             /*tp_name*/
    sizeof(book),             /*tp_basicsize*/
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
    "book objects",           /* tp_doc */
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

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpyMPTK(void) 
{
    PyObject* m;

    if (PyType_Ready(&bookType) < 0)
        return;

    m = Py_InitModule3("pyMPTK", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
      return;

    Py_INCREF(&bookType);
    PyModule_AddObject(m, "book", (PyObject *)&bookType);
}
