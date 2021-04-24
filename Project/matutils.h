#include <Python.h>

/* Allocate a matrix (of double with dim1 row and dim2 columns)*/
double** malloc_double_mat(int dim1, int dim2);

/* Free a matrix (of doubles with m rows) memory allocation */
int free_double_mat(double** mat, int m);

/* Get the product of two vectors */
double vecProd(int N, double* vecA, double* vecB);

/* Multiply two matrices - matA : M*N, matB: N*K, and place the results in matC of size M*K*/
void matMul(int N, double **matA, double **matB, double **matC);

/* Copy-paste values from one vector to another, in-place function*/
void vecAssign(double *from, double *to, int vecSize);

/* Initialize vector with zeros, in-place function*/
void initVectorZeros(double* vector, int vecSize);

/* Calculate the difference between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecDiff(double* a, double* b, double* result, int d);

/* Calculate the difference between two matrices (of doubles), (matA - matB)
 * and place the result into matC*/
void matDiff(double **matA, double **matB, double **matC, int N, int K);

/* Calculate the sum between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecSum(double* a, double* b, double* result, int d);

/* Divide every element of the vector by a const inplace */
void vecDiv(double* vector, int vecSize, int divisor);

/* Calculate the euclidean norm of a vector (of doubles) */
double norm2(double* x, int vecSize);

/* Save the normalized vectore srcVec into dstVec */
void normalize(double* srcVec, double* dstVec, int vecSize);

/* Calculate the diff between every element of two matrices, 
 * deciding whether it's larger than epsilon */
int isMatDiff(int e, double** matA, double** matB, int N);

/* Copy each value from mat A to mat B */
void matCopy(double** matA, double** matB, int N);

/* Copy srcMat tranposed into the dstMat (flipping i's and j's) */
void tranpose(double** srcMat, double** dstMat, int N);

/* Create a mat with only 1s on the diagonal */
void createImat(double **Imat, int N);

/* Create a mat with only 0s */
void createOmat(double **Omat, int N);

/* Convert a python matrix into a C object */
int py2CMat(PyObject* pyMat, double** mat);

/* Convert a C object matrix int oa python matrix */
PyObject* c2pyMat(double** mat, int n, int m);