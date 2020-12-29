#include <Python.h>

#define TRUE 1
#define FALSE 0

/*Free a matrix (of doubles with m rows) memory allocation */
int free_double_mat(double** mat, int m){
    int i;
    for (i=0; i<m; i++){
            free(mat[i]);
        }
    free(mat);
    return 1;
}

/* Copy-paste values from one vector to another, in-place function*/
void vecAssign(double *from, double *to, int vecSize) {
    int i;
    for (i=0; i<vecSize; i++){
        to[i] = from[i];
    }
    return;
}

/*Initialize vector with zeros, in-place function*/
void initVectorZeros(double* vector, int vecSize){
    int i;
    for (i = 0; i < vecSize; ++i) {
        vector[i]=0;
    }
}

    /*Calculate the difference between two vectors (of doubles), c=(a-b) of length d,
     * in place into result vector */
void vecDiff(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i<d; i++){
        result[i] = a[i]-b[i];
    }
    return;
}

/*Calculate the sum between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecSum(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i<d; i++){
        result[i] = a[i]+b[i];
    }
    return;
}

void vecDiv(double* vector, int vecSize, int divisor) {
    int i;
    for (i = 0; i < vecSize; ++i) {
        vector[i] = vector[i] / divisor;
    }
}

/* Calculate the euclidean norm of a vector (of doubles) */
double norm2(double* x, int vecSize){
    int i;
    int sumOfSquares = 0;
    for (i=0; i < vecSize; i++){
        sumOfSquares += x[i]*x[i];
    }
    return sumOfSquares;
}

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
void kmeans(int K, int N, int d, int MAX_ITER, double** input, double** centroids){
    int iter;
    int changeHappened=TRUE;
    int* input2CentMapping;

    /*1. malloc & init*/
    input2CentMapping = (int *)malloc(N * sizeof(int));
    assert(input2CentMapping!=NULL);
    init_input2CentMapping(input2CentMapping, N);

    /*2. Iterations*/
    for (iter = 0; iter < MAX_ITER && changeHappened; ++iter) {
        changeHappened = mapInputCentroid(K, N, d, input, centroids, input2CentMapping);
        updateCentroids(K, N, d, input, centroids, input2CentMapping);
    }
    /*Free all local memory*/
    free(input2CentMapping);
}

static int py2CMat(PyObject* pyMat, double** mat){
    int i,j,m,n;
    PyObject* pyVec;
    PyObject* pyItem;
    /* Is it a list? */
    if (!PyList_Check(pyMat))
        return FALSE;
    /* Get the size of it and build the output list */
    n = PyList_Size(pyMat);  /*  Same as in Python len(_list)  */
    /* Go over each item of the list and reduce it */
    for (i = 0; i < n; i++) {
        pyVec = PyList_GetItem(pyMat, i);
        if (!PyList_Check(pyVec)){  /* We only print lists */
            return FALSE;
        }
        m = PyList_Size(pyVec);
        for (j = 0; j < m; j++) {
            pyItem = PyList_GetItem(pyVec, j);
            mat[i][j] = PyFloat_AsDouble(pyItem);
    
            if (mat[i][j]  == -1 && PyErr_Occurred()){
                /* float too big to fit in a C double, bail out */
                puts("Error parsing a list to C matrix");
                return FALSE;
            }
        }
    }
    return TRUE;
}

static PyObject* c2pyMat(double** mat, int n, int m){
    int i, j;
    PyObject *pyItem, *pyVec, *pyMat;
    pyMat = PyList_New(n);
    for (i=0;i<n;i++){
        pyVec = PyList_New(m);
        for (j=0;j<m;j++){
    
            pyItem = Py_BuildValue("d", mat[i][j]);
            PyList_Append(pyVec, pyItem);
        }

        PyList_Append(pyMat, pyVec);
    }
    return pyMat;
}

static PyObject * kmeans_capi(PyObject* self, PyObject* args){
    int i;
    int K, N, d, MAX_ITER;
    double** input;
    double** centroids;
    PyObject * tempItem;
    PyObject * pyInput;
    PyObject * pyCentroids;
    PyObject * pyResultCentroids;

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
    input = (double **)malloc(N * sizeof(double*));
    assert(input!=NULL);
    for(i = 0; i < N; i++){
        input[i] = (double *)malloc(d * sizeof(double));
        assert(input[i]!=NULL);
    }
    centroids = (double **)malloc(K * sizeof(double*));
    assert(centroids!=NULL);
    for(i = 0; i < K; i++){
        centroids[i] = (double *)malloc(d * sizeof(double));
        assert(centroids[i]!=NULL);
    }
    /*Copy Python matrices to C Matrices*/
    py2CMat(pyInput,input);
    py2CMat(pyCentroids,centroids);

    /*Actual execution*/
    kmeans(K, N, d, MAX_ITER, input, centroids);
    pyResultCentroids = c2pyMat(centroids, K, d);
    free_double_mat(input, N);   
    free_double_mat(centroids, K);
    return pyResultCentroids;
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
