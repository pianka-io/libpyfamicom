#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>

#include "ines/pal.h"
#include "ines/rom.h"
#include "cpu/memory.h"
#include "emu/emu.h"
#include "ppu/memory.h"

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

static PyObject* dbg_prg_rom(PyObject* self, PyObject* args) {
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

    if (!emu->cpu || !emu->cpu->memory) {
        PyErr_SetString(PyExc_RuntimeError, "CPU or memory is not initialized");
        return NULL;
    }

    struct cpu_memory_t* memory = emu->cpu->memory;
    npy_intp dims[1] = {(npy_intp)memory->prg_rom_size};
    if (dims[0] <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "PRG ROM size is invalid");
        return NULL;
    }

    PyObject* array = PyArray_SimpleNew(1, dims, NPY_UINT8);
    if (!array) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create numpy array");
        return NULL;
    }

    memcpy(PyArray_DATA((PyArrayObject*)array), memory->data + PRG_ROM_OFFSET, memory->prg_rom_size);

    return array;
}

static PyObject* dbg_tick_cpu(PyObject* self, PyObject* args) {
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

    dbg_cpu_tick(emu);

    Py_RETURN_NONE;
}

static PyObject* dbg_state(PyObject* self, PyObject* args) {
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

    PyObject* result = PyDict_New();
    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create dictionary");
        return NULL;
    }

    // not exactly pretty
    PyDict_SetItemString(result, "cycles_cpu", PyLong_FromUnsignedLong((unsigned long)emu->clock->cpu_cycles));
    PyDict_SetItemString(result, "cycles_ppu", PyLong_FromUnsignedLong((unsigned long)emu->clock->ppu_cycles));
    PyDict_SetItemString(result, "cpu_register_a", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.a));
    PyDict_SetItemString(result, "cpu_register_x", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.x));
    PyDict_SetItemString(result, "cpu_register_y", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.y));
    PyDict_SetItemString(result, "cpu_register_p", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.p));
    PyDict_SetItemString(result, "cpu_register_sp", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.sp));
    PyDict_SetItemString(result, "cpu_register_pc", PyLong_FromUnsignedLong((unsigned long)emu->cpu->registers.pc));
    PyDict_SetItemString(result, "ppu_register_ppuctrl", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppuctrl));
    PyDict_SetItemString(result, "ppu_register_ppumask", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppumask));
    PyDict_SetItemString(result, "ppu_register_ppustatus", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppustatus));
    PyDict_SetItemString(result, "ppu_register_oamaddr", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.oamaddr));
    PyDict_SetItemString(result, "ppu_register_oamdata", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.oamdata));
    PyDict_SetItemString(result, "ppu_register_ppuscroll", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppuscroll));
    PyDict_SetItemString(result, "ppu_register_ppuaddr", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppuaddr));
    PyDict_SetItemString(result, "ppu_register_ppudata", PyLong_FromUnsignedLong((unsigned long)emu->ppu->registers.ppudata));

    return result;
}

static PyObject* dbg_cpu_ram(PyObject* self, PyObject* args) {
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

    if (!emu->cpu || !emu->cpu->memory) {
        PyErr_SetString(PyExc_RuntimeError, "CPU or memory is not initialized");
        return NULL;
    }

    struct cpu_memory_t* memory = emu->cpu->memory;
    npy_intp dims[1] = {(npy_intp)CPU_MEMORY_SIZE};
    if (dims[0] <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "PRG ROM size is invalid");
        return NULL;
    }

    PyObject* array = PyArray_SimpleNew(1, dims, NPY_UINT8);
    if (!array) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create numpy array");
        return NULL;
    }

    memcpy(PyArray_DATA((PyArrayObject*)array), memory->data, CPU_MEMORY_SIZE);

    return array;
}

static PyObject* dbg_ppu_ram(PyObject* self, PyObject* args) {
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

    if (!emu->cpu || !emu->ppu->memory) {
        PyErr_SetString(PyExc_RuntimeError, "CPU or memory is not initialized");
        return NULL;
    }

    struct ppu_memory_t* memory = emu->ppu->memory;
    npy_intp dims[1] = {(npy_intp)PPU_MEMORY_SIZE};
    if (dims[0] <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "PRG ROM size is invalid");
        return NULL;
    }

    PyObject* array = PyArray_SimpleNew(1, dims, NPY_UINT8);
    if (!array) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create numpy array");
        return NULL;
    }

    memcpy(PyArray_DATA((PyArrayObject*)array), memory->data, PPU_MEMORY_SIZE);

    return array;
}

static PyObject* emu_controller_press(PyObject* self, PyObject* args) {
    (void)self;
    PyObject* capsule;
    byte button;

    if (!PyArg_ParseTuple(args, "Ob", &capsule, &button)) {
        PyErr_SetString(PyExc_TypeError, "Expected a capsule and a byte as arguments");
        return NULL;
    }

    struct emu_t* emu = (struct emu_t*)PyCapsule_GetPointer(capsule, "emu_t");
    if (!emu) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid or missing emulator capsule");
        return NULL;
    }

    emu->controller.state = controller_press(emu->controller.state, button);
    Py_RETURN_NONE;
}

static PyObject* emu_controller_release(PyObject* self, PyObject* args) {
    (void)self;
    PyObject* capsule;
    byte button;

    if (!PyArg_ParseTuple(args, "Ob", &capsule, &button)) {
        PyErr_SetString(PyExc_TypeError, "Expected a capsule and a byte as arguments");
        return NULL;
    }

    struct emu_t* emu = (struct emu_t*)PyCapsule_GetPointer(capsule, "emu_t");
    if (!emu) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid or missing emulator capsule");
        return NULL;
    }

    emu->controller.state = controller_release(emu->controller.state, button);
    Py_RETURN_NONE;
}

static PyMethodDef PytendoMethods[] = {
    {"emu_create", create_emu, METH_VARARGS, "Create a pytendo emulator."},
    {"emu_tick", tick_emu, METH_VARARGS, "Tick a pytendo emulator by a frame."},
    {"emu_frame", frame_emu, METH_VARARGS, "Get the latest frame from a pytendo emulator."},
    {"dbg_state", dbg_state, METH_VARARGS, "Get the CPU and PPU state."},
    {"dbg_prg_rom", dbg_prg_rom, METH_VARARGS, "Get the PRG ROM section of the emulator's memory as a numpy array."},
    {"dbg_cpu_tick", dbg_tick_cpu, METH_VARARGS, "Tick a pytendo emulator by a CPU instruction."},
    {"dbg_cpu_ram", dbg_cpu_ram, METH_VARARGS, "Get the CPU memory as a numpy array."},
    {"dbg_ppu_ram", dbg_ppu_ram, METH_VARARGS, "Get the PPU memory as a numpy array."},
    {"emu_controller_press", emu_controller_press, METH_VARARGS, "Press a button on the controller."},
    {"emu_controller_release", emu_controller_release, METH_VARARGS, "Release a button on the controller."},
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
    import_array()
    emu_initialize_numpy();
    ppu_initialize_numpy();
    return PyModule_Create(&pytendo_module);
}
