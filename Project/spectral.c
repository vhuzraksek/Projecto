#include <Python.h>
#include <math.h>
#include "matutils.h"
#include <stdio.h>

#define EPSILON 0.0001

/* Create the weighted adj matrix */
int createAdjMat(int N, int d, double **input, double **adj) {
    int i, j;
    double w = 0;
    double* diffResult;    
    diffResult = (double *)malloc(N * sizeof(double));
    if (diffResult == NULL){
        return -1;
    }
    for (i = 0; i < N; ++i){
        for (j = 0; j < N; ++j){
            if (i == j){
                w = 0;
            } else {
                vecDiff(input[i], input[j], diffResult, d);
                w = exp(-(norm2(diffResult, d) / 2.0));
            }
            adj[i][j] = w;
        }
    }
    free(diffResult);
    return 0;
}

/* Create the Diagonal Degree Matrix (to the power of minus half)*/
int createDiagMat(int N, double **adj, double **diag){
    int i, j;
    double element;
    for (i = 0; i < N; ++i){
        element = 0;
        for (j = 0; j < N; j++){
            diag[i][j] = 0;
            element += adj[i][j];
        }
        diag[i][i] = 1.0 / sqrt(element);
    }
    return 0;
}

/* Create the Lnorm matrix*/
int createLnorm(double** input, double** adj, double** diag, double** Imat, double** Lnorm, int N, int d){
    int retval = 0;
    double** res1 = NULL;
    double** res2 = NULL;

    /* Malloc and init */
    res1 = malloc_double_mat(N, N);
    res2 = malloc_double_mat(N, N);
    if (res1 == NULL || res2 == NULL){
        retval = -1;
        goto cleanup;
    }
    createImat(Imat, N);

    /* Create adjacency matrix */
    if (createAdjMat(N, d, input, adj) == -1){
        return -1;
    }

    /* Create diagonal matrix to the power of -1/2*/
    createDiagMat(N, adj, diag);


    // Calculate D^(-1/2) * W and put it into res1
    matMul(N, diag, adj, res1);
    // Using res1 = D^(-1/2) * W calculate Lnorm * D^(-1/2) and put it into Lnorm 
    matMul(N, res1, diag, res2);
    // Using Lnorm = D^(-1/2) * W  * D^(-1/2) calculate I - Lnorm and put it into Lnorm
    matDiff(Imat, res2, Lnorm, N, N);
    // Lnorm should now be D^(-1/2) * W  * D^(-1/2)

    cleanup:
        free_double_mat(res1, N);
        free_double_mat(res2, N);
        return retval;
}

void getCol(int N, int idx, double** aMat, double* resVec){
    int i;
    for (i = 0; i < N; i++){
        resVec[i] = aMat[i][idx];
    }
}

/* Calculate the grahm schmidt result (q and r) from mat A*/
int modified_GS(int N, double** aMat, double** qMat, double** rMat){
    int retval = 0;
    int i, j, k;
    double** uMat = NULL;
    double** tempMat = NULL;
    double* tempVec = NULL;
    double* tempVec2 = NULL;

    /* Malloc and init */
    uMat = malloc_double_mat(N, N);
    tempMat = malloc_double_mat(N, N);
    tempVec = (double *)malloc(N * sizeof(double));
    tempVec2 = (double *)malloc(N * sizeof(double));
    if (uMat == NULL || tempMat == NULL || tempVec == NULL){
        retval = -1;
        goto cleanup;
    }
    matCopy(aMat, uMat, N);

    /* Main GS logic */
    for (i = 0; i < N; i++){
        getCol(N,  i, uMat, tempVec);
        rMat[i][i] = sqrt(norm2(tempVec, N));
        for (j = 0; j < N; j++){
            qMat[j][i] = uMat[j][i] / rMat[i][i];
        }
        for (j = i + 1; j < N; j++){
            getCol(N,  i, qMat, tempVec);
            getCol(N,  j, aMat, tempVec2);
            rMat[i][j] = vecProd(N, tempVec, tempVec2);
            for (k = 0; k < N; k++){
                tempVec[k] = rMat[i][j] * qMat[k][i];
            }
            getCol(N,  j, uMat, tempVec2);
            vecDiff(tempVec2, tempVec, tempVec2, N);
            for (k = 0; k < N; k++){
                uMat[k][j] = tempVec2[k];
            }
        }
    }
    cleanup:
        free(tempVec);
        free(tempVec2);
        free_double_mat(uMat, N);
        free_double_mat(tempMat, N);
        return retval;
}

/* use QR iteration to get the eigenvalues and eigenvectors from vector Lnorm*/
int QR_iteration(int N, double** Lnorm, double** aHatMat, double** qHatMat){
    int retval = 0;
    int i;
    double** qMat = NULL;
    double** rMat = NULL;
    double** tempMat = NULL;
    /* Malloc and init */
    qMat = malloc_double_mat(N, N);
    rMat = malloc_double_mat(N, N);
    tempMat = malloc_double_mat(N, N);
    if (qMat == NULL || rMat == NULL || tempMat == NULL){
        retval = -1;
        goto cleanup;
    }
    matCopy(Lnorm, aHatMat, N);
    createImat(qHatMat, N);

    /* Main QR iteration */
    for (i = 0; i < N; i ++){
        createOmat(qMat, N);
        createOmat(rMat, N);
        if (modified_GS(N, aHatMat, qMat, rMat) == -1){
            retval = -1;
            goto cleanup;
        }
        matMul(N, rMat, qMat, aHatMat);
        matMul(N, qHatMat, qMat, tempMat);
        if (isMatDiff(EPSILON, qHatMat, tempMat, N)){
            goto cleanup;
        }
        matCopy(tempMat, qHatMat, N);
    }


    cleanup:
        free_double_mat(qMat, N);
        free_double_mat(rMat, N);
        return retval;
}

static PyObject * spectral_capi(PyObject* self, PyObject* args){
    int exitcode = 1;
	int N, d;
    double** input = NULL;

    // For Lnorm creation
    double** adj = NULL;
    double** diag = NULL;
    double** Imat = NULL;
    double** Lnorm = NULL; 

    // For Grahm Schmidt   
    double** aHatMat = NULL;
    double** qHatMat = NULL;
    PyObject * pyInput;
    PyObject * pyqHatMat;
    PyObject * pyaHatMat;
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

    /* malloc and init */
    input = malloc_double_mat(N, d);
    adj = malloc_double_mat(N, N);
    diag = malloc_double_mat(N, N);
    Lnorm = malloc_double_mat(N, N);
    Imat = malloc_double_mat(N, N);

    aHatMat = malloc_double_mat(N, N);
    qHatMat = malloc_double_mat(N, N);

    if (input == NULL || 
        adj == NULL || diag == NULL || Lnorm == NULL || Imat == NULL || 
        aHatMat == NULL || qHatMat == NULL){
        exitcode = -1;
        goto cleanup;
    }

    py2CMat(pyInput,input);

    if (createLnorm(input, adj, diag, Imat, Lnorm, N, d) == -1){
        exitcode = -1;
        goto cleanup;
    }

    if (QR_iteration(N, Lnorm, aHatMat, qHatMat) == -1){
        exitcode = -1;
        goto cleanup;
    }

    pyqHatMat = c2pyMat(qHatMat, N, N);
    pyaHatMat = c2pyMat(aHatMat, N, N);
    PyList_Append(pyReturn, pyqHatMat);
    PyList_Append(pyReturn, pyaHatMat);

    cleanup:
        PyList_Append(pyReturn, Py_BuildValue("i", exitcode));
        free_double_mat(input, N);
        free_double_mat(adj, N);
        free_double_mat(diag, N);
        free_double_mat(Lnorm ,N);
        free_double_mat(Imat, N);
        free_double_mat(aHatMat, N);
        free_double_mat(qHatMat, N);
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
