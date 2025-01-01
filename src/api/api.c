#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>

#include "ines/pal.h"
#include "ines/rom.h"
#include "emu/emu.h"

void emu_capsule_destructor(PyObject* capsule) {
    struct emu_t* emu = (struct emu_t*)PyCapsule_GetPointer(capsule, "emu_t");
    if (emu) {
        emu_destroy(emu);
    }
}

static PyObject* create_emu(PyObject* self, PyObject* args) {
    (void)self;
    const char* pal_filename;
    const char* rom_filename;
    if (!PyArg_ParseTuple(args, "ss", &pal_filename, &rom_filename)) {
        return NULL;
    }

    //TODO memory leaking for pal and rom
    struct pal_t* pal = pal_load(pal_filename);
    struct rom_t* rom = rom_load(rom_filename);
    struct emu_t* emu = emu_create(pal, rom);

    return PyCapsule_New(emu, "emu_t", emu_capsule_destructor);
}

static PyObject* tick_emu(PyObject* self, PyObject* args) {
    (void)self;
    PyObject* capsule;

    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected a capsule argument");
        return NULL;
    }

    struct emu_t* emu = (struct emu_t*)PyCapsule_GetPointer(capsule, "emu_t");
    if (!emu) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid or missing emulator capsule");
        return NULL;
    }

    emu_tick(emu);

    Py_RETURN_NONE;
}

static PyObject* frame_emu(PyObject* self, PyObject* args) {
    (void)self;
    PyObject* capsule;

    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected a capsule argument");
        return NULL;
    }

    struct emu_t* emu = (struct emu_t*)PyCapsule_GetPointer(capsule, "emu_t");
    if (!emu) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid or missing emulator capsule");
        return NULL;
    }

    Py_INCREF(emu->ppu->state.frame_array);
    return emu->ppu->state.frame_array;
}

static PyMethodDef PytendoMethods[] = {
    {"create_emu", create_emu, METH_VARARGS, "Create a pytendo emulator."},
    {"tick_emu", tick_emu, METH_VARARGS, "Tick a pytendo emulator."},
    {"frame_emu", frame_emu, METH_VARARGS, "Get the latest frame from a pytendo emulator."},
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
    ppu_initialize_numpy();
    return PyModule_Create(&pytendo_module);
}
