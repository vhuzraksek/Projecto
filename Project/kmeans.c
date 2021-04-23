#include <Python.h>
#include "matutils.h"

#define TRUE 1
#define FALSE 0

/* Init input2CentMapping to a their respective clusters
 * and the others will be assigned to cluster>K, thus non existent clusters,
 * so first time there will be a noticed change for them,
 * while first ones remain unchanged, as expected.
 * In-place function*/

void init_input2CentMapping(int* input2CentMapping, int N){
    int i;
    for (i = 0; i < N; ++i){
        input2CentMapping[i] = i;
    }
    return;
}

int mapInputCentroid(int K, int N, int d, double **input, double **centroids, int* input2CentMapping) {
    /*Iteration Variables*/
    int i, j;

    /*Temp vars 2A*/
    double minDistance,currentDistance;
    int minCentroidIndex=0;
    int changeHappened=FALSE;
    double* diffResult;

    /*1. malloc & init*/
    diffResult = (double *)malloc(N * sizeof(double));
    assert(diffResult!=NULL);

    /*A) Assign inputs to centroids*/
    for (i = 0; i < N; ++i) {
        for (j = 0; j < K; ++j) {
            vecDiff(input[i], centroids[j], diffResult, d);
            currentDistance = norm2(diffResult, d);
            if (j==0 || currentDistance < minDistance){
                minDistance = currentDistance;
                minCentroidIndex = j;
            }
        }
        if (input2CentMapping[i]!=minCentroidIndex){
            changeHappened=TRUE;
            input2CentMapping[i]=minCentroidIndex;
        }
    }
    /*Free all local memory*/
    free(diffResult);

    return changeHappened;
}

void updateCentroids(int K, int N, int d, double **input, double **centroids, int* input2CentMapping) {
    /*Iteration Variables*/
    int i, j;

    /*Temp vars for 2B*/
    int clusterSize;
    double* newCentroidVec;

    /*1. malloc & init*/
    newCentroidVec = (double *)malloc(d * sizeof(double));
    assert(newCentroidVec!=NULL);

    for (j = 0; j < K; ++j) {
        clusterSize=0;
        initVectorZeros(newCentroidVec, d);
        for (i = 0; i < N; ++i) {
            if (input2CentMapping[i]==j){
                clusterSize+=1;
                vecSum(newCentroidVec, input[i], newCentroidVec, d);
            }
        }
        vecDiv(newCentroidVec, d, clusterSize);
        vecAssign(newCentroidVec,centroids[j], d);
    }
    free(newCentroidVec);
}

/* Logic of the kmeans calculation */
void kmeans(int K, int N, int d, int MAX_ITER, double** input, double** centroids, int* input2CentMapping){
    int iter;
    int changeHappened=TRUE;

    /*1. malloc & init*/
    init_input2CentMapping(input2CentMapping, N);

    /*2. Iterations*/
    for (iter = 0; iter < MAX_ITER && changeHappened; ++iter) {
        changeHappened = mapInputCentroid(K, N, d, input, centroids, input2CentMapping);
        updateCentroids(K, N, d, input, centroids, input2CentMapping);
    }
    /*Free all local memory*/
}

static PyObject * kmeans_capi(PyObject* self, PyObject* args){
	int exitcode = 1;
    int i;
    int K, N, d, MAX_ITER;
    int* input2CentMapping;
    double** input;
    double** centroids;
    PyObject * tempItem;
    PyObject * pyInput;
    PyObject * pyCentroids;
    PyObject * pyResultCentroids;
    PyObject * pyInput2CentMapping;
    PyObject * pyReturn = PyList_New(0);

    /*Get args*/
    if (!PyArg_ParseTuple(args,"iOO", &MAX_ITER, &pyInput, &pyCentroids)){
        return NULL;
    }
    /*Get sizes*/
    if (!PyList_Check(pyInput)) {
        PyErr_BadInternalCall();
        return NULL;
    }

    if (!PyList_Check(pyCentroids)) {
        PyErr_BadInternalCall();
        return NULL;
    }

    N = PyList_Size(pyInput);
    K = PyList_Size(pyCentroids);
    tempItem = PyList_GetItem(pyInput, 0); /*Item for getting the dimension*/
    d = PyList_Size(tempItem);

    /*malloc & init*/
    input = malloc_double_mat(N, d);
    centroids = malloc_double_mat(K, d);
    input2CentMapping = (int *)malloc(N * sizeof(int));
    if (input == NULL || centroids == NULL || input2CentMapping == NULL){
        exitcode = -1;
        goto cleanup;
    }

    /*Copy Python matrices to C Matrices*/
    py2CMat(pyInput,input);
    py2CMat(pyCentroids,centroids);
    
    /*Actual execution*/
    kmeans(K, N, d, MAX_ITER, input, centroids, input2CentMapping);
    pyResultCentroids = c2pyMat(centroids, K, d);

    // pyInput2CentMapping = c2pyMat(input2CentMapping, N, 1);
    pyInput2CentMapping = PyList_New(0);
    for (i = 0; i < N; i++){
    	PyList_Append(pyInput2CentMapping, Py_BuildValue("i", input2CentMapping[i]));
    }

    PyList_Append(pyReturn, pyResultCentroids);
    PyList_Append(pyReturn, pyInput2CentMapping);
    
    cleanup:
        PyList_Append(pyReturn, Py_BuildValue("i", exitcode));
	    free(input2CentMapping);
		free_double_mat(input, N);   
	    free_double_mat(centroids, K);
	    return pyReturn;
}

static PyMethodDef capiMethods[] = {
        {"kmeans",
                (PyCFunction) kmeans_capi,
                METH_VARARGS,
                PyDoc_STR("Solve K-Means with initial Centroids pre-set."),
            },
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "mykmeanssp", /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        capiMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
