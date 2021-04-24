#include <Python.h>
#include <math.h>

#define TRUE 1
#define FALSE 0


double** malloc_double_mat(int dim1, int dim2){
    int i;
    double** mat;
    mat = (double **)malloc(dim1 * sizeof(double*));
    if (mat == NULL) {
        return NULL;
    }
    for(i = 0; i < dim1; i++){
        mat[i] = (double *)malloc(dim2 * sizeof(double));
        if (mat[i] == NULL){
            return NULL;
        }
    }
    return mat;
}

int free_double_mat(double** mat, int m){
    int i;
    for (i=0; i<m; i++){
            free(mat[i]);
        }
    free(mat);
    return 1;
}



double vecProd(int N, double* vecA, double* vecB){
    int i;
    double prod = 0;
    for (i = 0; i < N; i++){
        prod += vecA[i]*vecB[i];
    }
    return prod;
}


void matMul(int N, double **matA, double **matB, double **matC){
    int i, j, k;
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            matC[i][j] = 0;
            for(k = 0; k < N; k++){
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
}

void vecAssign(double *from, double *to, int vecSize) {
    int i;
    for (i=0; i<vecSize; i++){
        to[i] = from[i];
    }
    return;
}

void initVectorZeros(double* vector, int vecSize){
    int i;
    for (i = 0; i < vecSize; i++) {
        vector[i]=0;
    }
}

void vecDiff(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i < d; i++){
        result[i] = a[i] - b[i];
    }
    return;
}

void matDiff(double **matA, double **matB, double **matC, int N, int K){
    int i;
    for (i = 0; i < N; i++){
        vecDiff(matA[i], matB[i], matC[i], K);
    }
    return;
}

void vecSum(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i < d ; i++){
        result[i] = a[i] + b[i];
    }
    return;
}

void vecDiv(double* vector, int vecSize, int divisor) {
    int i;
    for (i = 0; i < vecSize; i++) {
        vector[i] = vector[i] / divisor;
    }
}

double norm2(double* x, int vecSize){
    int i;
    double sumOfSquares = 0;
    for (i=0; i < vecSize; i++){
        sumOfSquares += x[i] * x[i];
    }
    return sumOfSquares;
}

void normalize(double* srcVec, double* dstVec, int vecSize){
    int i;
    double norm;

    norm = sqrt(norm2(srcVec, vecSize));
    for (i = 0; i < vecSize; i++){
        dstVec[i] = srcVec[i] / norm;
    }

}

int isMatDiff(int e, double** matA, double** matB, int N){
    int i, j;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (abs(matA[i][j] - matB[i][j]) > e){
                return TRUE;
            }
        }
    }
    return FALSE;
}


void matCopy(double** matA, double** matB, int N){
    int i,j;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            matB[i][j] = matA[i][j];
        }
    }
}

void tranpose(double** srcMat, double** dstMat, int N){
    int i, j;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            dstMat[j][i] = srcMat[i][j];
        }
    }
}

void createImat(double **Imat, int N){
    int i, j;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            Imat[i][j] = (i == j);
        }
    }
    return;
}

void createOmat(double **Omat, int N){
    int i, j;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            Omat[i][j] = 0;
        }
    }
    return;
}

int py2CMat(PyObject* pyMat, double** mat){
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

PyObject* c2pyMat(double** mat, int n, int m){
    int i, j;
    PyObject *pyItem, *pyVec, *pyMat;
    pyMat = PyList_New(0);
    for (i=0; i < n; i++){
        pyVec = PyList_New(0);
        for (j = 0; j < m; j++){
            pyItem = Py_BuildValue("d", mat[i][j]);
            PyList_Append(pyVec, pyItem);
        }

        PyList_Append(pyMat, pyVec);
    }
    return pyMat;
}