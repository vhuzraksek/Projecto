#include <Python.h>
#include <math.h>
#include "matutils.h"

#define TRUE 1
#define FALSE 0

/* Create the weighted adj matrix */
void createAdjMat(int N, int d, double **input, double **adj) {
    int i, j;
    double w = 0;
    double* diffResult;    
    diffResult = (double *)malloc(N * sizeof(double));
    assert(diffResult!=NULL);
    for (i = 0; i < N; ++i){
        for (j = 0; j < d; ++j){
            if (i == j){
                w = 0;
            } else {
                vecDiff(input[i], input[j], diffResult, d);
                w = exp(-(norm2(diffResult, d)) / 2.0);
            }
            adj[i][j] = w;
        }
    }
    return;
}

static PyObject * spectral_capi(PyObject* self, PyObject* args){
    int exitcode = 1;
	int N, d, k;
    double** input;
    double** adj;
    PyObject * pyInput;
    PyObject * pyAdj;
    PyObject * pyReturn = PyList_New(0);

    /*Get args*/
    if (!PyArg_ParseTuple(args,"iiO", &N, &d, &pyInput)){
        return NULL;
    }
    /*Get sizes*/
    if (!PyList_Check(pyInput)) {
        PyErr_BadInternalCall();
        return NULL;
    }

    input = malloc_double_mat(N, d);
    adj = malloc_double_mat(N, d);
    if (input == NULL || adj == NULL){
        exitcode = -1;
        goto cleanup;
    }

    py2CMat(pyInput,input);
    createAdjMat(N, d, input, adj);
    
    pyAdj = c2pyMat(adj, N, d);
    PyList_Append(pyReturn, pyAdj);
    k = 3;
    PyList_Append(pyReturn, Py_BuildValue("i", k));

    cleanup:
        PyList_Append(pyReturn, Py_BuildValue("i", exitcode));
        free_double_mat(input, N);
        free_double_mat(adj, N);
        return pyReturn;
}

static PyMethodDef capiMethods[] = {
        {"spectral",
                (PyCFunction) spectral_capi,
                METH_VARARGS,
                PyDoc_STR("Solve spectral clustering with initial Centroids pre-set."),
            },
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "myspectral", /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        capiMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC
PyInit_myspectral(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
