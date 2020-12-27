#include <Python.h>

#define TRUE 1
#define FALSE 0

/*Print a matrix (of doubles) of size mxn */
void print_double_mat(double** mat, int m, int n){
    int i, j;
    for (i=0; i<m; i++){
        for (j=0; j<n; j++){
            printf("%.2f", mat[i][j]);
            if (j<n-1){
                /*If this is not the last item in line, print a comma*/
                printf(",");
            }
        }
        printf("\n");
    }
}
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
    int minCentroidIndex;
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
double** kmeans(int K, int N, int d, int MAX_ITER, double** input){
    int iter, i;
    int changeHappened=TRUE;
    int* input2CentMapping;
    double** centroids;

    /*1. malloc & init*/
    input2CentMapping = (int *)malloc(N * sizeof(int));
    assert(input2CentMapping!=NULL);
    centroids = (double **)malloc(K * sizeof(double*));
    assert(centroids!=NULL);
    for(i = 0; i < K; i++){
        centroids[i] = (double *)malloc(d * sizeof(double));
        assert(centroids[i]!=NULL);
    }
    init_centroids(centroids, input, K, d);
    init_input2CentMapping(input2CentMapping, N);

    /*2. Iterations*/
    for (iter = 0; iter < MAX_ITER || changeHappened; ++iter) {
        changeHappened = mapInputCentroid(K, N, d, input, centroids, input2CentMapping);
        updateCentroids(K, N, d, input, centroids, input2CentMapping);
    }
    /*Free all local memory*/
    free(input2CentMapping);

    return centroids;
}

static PyObject * kmeans_capi(PyObject* self, PyObject* args){
    int K, N, d, MAX_ITER;
    double** input;
    double** initialCentroids;
    double** centroids;
    if (PyArg_ParseTuple(args,"iiiiOOO",&K, &N, &d, &MAX_ITER, &input, &initialCentroids)){
        return NULL;
    }

    centroids = kmeans(K, N, d, MAX_ITER, input, initialCentroids);

    return Py_BuildValue("O",centroids);
}
static PyMethodDef capiMethods[] = {
        {"kmeans",
                (PyCFunction) kmeans_capi,
                METHVARARGS,
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
