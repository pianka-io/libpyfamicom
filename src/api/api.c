#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>

#include "api/api.h"

static PyObject* create_numpy_array(PyObject* self, PyObject* args) {
    (void)self;
    (void)args;

    npy_intp dims[1] = {3};
    PyObject* numpy_array = PyArray_SimpleNew(1, dims, NPY_DOUBLE);

    if (!numpy_array) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create NumPy array");
        return NULL;
    }

    double* data = (double*)PyArray_DATA((PyArrayObject*)numpy_array);
    data[0] = 1.0;
    data[1] = 2.0;
    data[2] = 3.0;

    return numpy_array;
}

static PyMethodDef PytendoMethods[] = {
    {"create_numpy_array", create_numpy_array, METH_NOARGS, "Create and return a NumPy array."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef pytendo_module = {
    PyModuleDef_HEAD_INIT,
    "pytendo",
    NULL,
    -1,
    PytendoMethods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_pytendo(void) {
    import_array();
    return PyModule_Create(&pytendo_module);
}
