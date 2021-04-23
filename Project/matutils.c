#include <Python.h>

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
    for (i=0;i<n;i++){
        pyVec = PyList_New(0);
        for (j=0;j<m;j++){
            pyItem = Py_BuildValue("d", mat[i][j]);
            PyList_Append(pyVec, pyItem);
        }

        PyList_Append(pyMat, pyVec);
    }
    return pyMat;
}