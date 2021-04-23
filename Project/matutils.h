#include <Python.h>

/*Allocate a matrix (of double with dim1 row and dim2 columns)*/
double** malloc_double_mat(int dim1, int dim2);

/*Free a matrix (of doubles with m rows) memory allocation */
int free_double_mat(double** mat, int m);

/* Copy-paste values from one vector to another, in-place function*/
void vecAssign(double *from, double *to, int vecSize);

/*Initialize vector with zeros, in-place function*/
void initVectorZeros(double* vector, int vecSize);

/*Calculate the difference between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecDiff(double* a, double* b, double* result, int d);

/*Calculate the sum between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecSum(double* a, double* b, double* result, int d);

/*Divide every element of the vector by a const inplace */
void vecDiv(double* vector, int vecSize, int divisor);

/* Calculate the euclidean norm of a vector (of doubles) */
double norm2(double* x, int vecSize);

/* Convert a python matrix into a C object */
int py2CMat(PyObject* pyMat, double** mat);

/* Convert a C object matrix int oa python matrix */
PyObject* c2pyMat(double** mat, int n, int m);